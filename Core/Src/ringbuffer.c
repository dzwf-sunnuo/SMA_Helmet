#include "ringbuffer.h"
#include "usart.h"
#include "dma.h"
#include "singlemove.h"
#include <string.h>

/**
  ******************************************************************************
  * @file    ringbuffer.c
  * @brief   DMA 环形缓冲区 + UART IDLE 中断帧接收
  *
  *          工作原理：
  *            DMA1 CH5 以 Circular 模式持续将 USART1 RX 数据写入 rbuf.buf
  *            上位机每 50ms 发送一帧：0xCC + 40 字节 + 0xFE = 42 字节
  *            帧发送完成后线路空闲 → UART IDLE 中断触发
  *            HAL_UARTEx_RxEventCallback → RingBuf_RxCallback 提取帧
  *
  *          帧定位算法：
  *            利用 DMA NDTR 寄存器计算当前写入位置
  *            NDTR = DMA 剩余传输次数（从 BUF_SIZE 递减）
  *            wr_pos = RINGBUF_SIZE - NDTR（已写入的总字节数）
  *            最近一帧的结尾 = wr_pos（最后一个字节为 0xFE 或刚好在线路空闲前）
  *            帧长固定 42 字节，直接回卷拷贝
  *
  *          防重复机制：
  *            记录上次 NDTR 值，仅当 NDTR 变化超过 42 字节时才解析新帧
  *            避免 HAL 重复触发 IDLE 回调导致解析空帧
  ******************************************************************************
  */

static RingBuf_t rbuf;

/* 将 buf 中从 start 开始的 length 字节拷贝到 dst（处理回卷） */
static void buf_copyout(uint8_t *dst, uint16_t start, uint16_t length)
{
    for (uint16_t i = 0; i < length; i++)
    {
        dst[i] = rbuf.buf[(start + i) & (RINGBUF_SIZE - 1)];
    }
}

/**
  * @brief  启动 DMA + IDLE 接收
  * @param  huart  UART 句柄（&huart1）
  * @note   调用 HAL_UARTEx_ReceiveToIdle_DMA 启动 Circular DMA 接收
  *         同时使能 USART1 IDLE 中断
  */
void RingBuf_Start(UART_HandleTypeDef *huart)
{
    memset(rbuf.buf, 0, sizeof(rbuf.buf));
    rbuf.last_ndtr  = RINGBUF_SIZE;
    rbuf.frame_ready = 0;

    HAL_UARTEx_ReceiveToIdle_DMA(huart, rbuf.buf, RINGBUF_SIZE);
}

/**
  * @brief  DMA 接收回调（由 main.c 中的 HAL_UARTEx_RxEventCallback 调用）
  * @param  huart  UART 句柄
  * @param  Size  自上次回调以来接收的字节数
  * @note   通过 DMA NDTR 寄存器精确定位帧的起始和结束位置
  */
void RingBuf_RxCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance != USART1 || Size == 0)
        return;

    /* 读取 DMA 当前剩余计数 */
    uint16_t ndtr = (uint16_t)__HAL_DMA_GET_COUNTER(huart->hdmarx);
    uint16_t wr_pos = RINGBUF_SIZE - ndtr;   /* 当前 DMA 写入指针（取模后） */

    /* 防重复：NDTR 没变说明是同一帧的重复回调（HAL 内部行为） */
    if (ndtr == rbuf.last_ndtr && rbuf.last_ndtr != RINGBUF_SIZE)
        return;
    rbuf.last_ndtr = ndtr;

    /*
     * 帧边界计算：
     *   wr_pos = DMA 已写入的总字节（非模值）
     *   最新一帧的结尾 = wr_pos（刚好在线路空闲前的最后一个字节位置）
     *   帧长度 = 42 字节（0xCC + 40 + 0xFE）
     *
     *   取模后得到缓冲区中的起始位置
     */
    if (Size < 42)
    {
        /* 帧不完整，忽略，等待下一帧 */
        return;
    }

    /* 帧起始位置 = 当前写入位置 - 帧长度，取模回卷 */
    uint16_t frame_start = (wr_pos + RINGBUF_SIZE - 42) & (RINGBUF_SIZE - 1);

    /* 验证帧头帧尾 */
    if (rbuf.buf[frame_start] == 0xCC)
    {
        uint16_t frame_end = (frame_start + 41) & (RINGBUF_SIZE - 1);
        if (rbuf.buf[frame_end] == 0xFE)
        {
            rbuf.frame_ready = 1;
        }
    }
}

/**
  * @brief  检查是否收到完整帧，并解析到 Cmd[]/Lock[] 数组
  *         由 TIM2 ISR（20Hz）中的 Get_Sim_Data 调用
  * @note   解析逻辑与原 Get_Sim_Data 一致：
  *           Cmd[i] = 帧数据字节
  *           Lock[i] = (Cmd[i]==0x01) ? 0x01 : 0x02
  */
void RingBuf_Process(void)
{
    if (!rbuf.frame_ready)
        return;

    rbuf.frame_ready = 0;

    /* 从环形缓冲区提取帧数据 */
    uint8_t frame[42];
    uint16_t ndtr = (uint16_t)__HAL_DMA_GET_COUNTER(huart1.hdmarx);
    uint16_t wr_pos = RINGBUF_SIZE - ndtr;
    uint16_t frame_start = (wr_pos + RINGBUF_SIZE - 42) & (RINGBUF_SIZE - 1);
    buf_copyout(frame, frame_start, 42);

    /* 解析 40 字节有效载荷 */
    for (int i = 0; i < 40; i++)
    {
        Cmd[i]  = frame[i + 1];                   /* 跳过 0xCC 帧头 */
        Lock[i] = (Cmd[i] == 0x01) ? 0x01 : 0x02;
    }
}

/*
工作原理详解
整个方案可以拆成三层来看：

底层搬运：DMA 循环模式

HAL_UARTEx_ReceiveToIdle_DMA 将 DMA 配置为 Circular 模式，数据从 USART 接收寄存器源源不断地搬运到 rbuf.buf（大小 RINGBUF_SIZE，必须为 2 的幂，因为代码里用了 & (RINGBUF_SIZE - 1) 掩码回卷）。

DMA 写完整个缓冲区后会自动回到起始地址，形成首尾相接的环形缓冲区，无需 CPU 干预。

帧边界检测：IDLE 中断 + NDTR 推算写指针

上位机每 50 ms 发送一帧 42 字节（0xCC + 40 数据 + 0xFE），帧结束后线路上会出现一个字节时长的空闲，触发 USART IDLE 中断。

在 IDLE 回调 RingBuf_RxCallback 中，通过读取 DMA 的 NDTR 寄存器（剩余传输次数）计算当前写入位置：
wr_pos = RINGBUF_SIZE - ndtr
这代表 DMA 下一笔将要写入的缓冲区索引，因此最后一帧的结尾就在 wr_pos - 1 处。

帧长固定 42 字节，直接反推帧起始位置：
frame_start = (wr_pos - 42) mod RINGBUF_SIZE
再校验帧头 0xCC 和帧尾 0xFE，校验通过后置 frame_ready = 1。

帧提取与解析：定时器中断中的消费者

TIM2 每 50 ms 触发一次中断，调用 RingBuf_Process，看到 frame_ready 后再次用 NDTR 计算帧位置，调用 buf_copyout 把 42 字节完整拷贝出来，避免回卷问题。

之后按原逻辑将 40 字节有效载荷转换为 Cmd[] 和 Lock[]。

防重复机制
HAL 库在某些情况下可能多次触发 IDLE 回调，代码记录 last_ndtr，仅当本次 ndtr 与上次不同（或为初始值）时才处理，过滤重复回调。

*/

/*
NDTR 是 DMA 数据传输数量寄存器（Number of Data Transfer Register）的缩写，在 STM32 中，它记录了 DMA 传输中剩余还未完成的数据单元个数。

在代码中通过 __HAL_DMA_GET_COUNTER(huart->hdmarx) 读取到的就是这个寄存器值。

它如何与环形缓冲区结合？
你的 DMA 是这么配置的：

模式：Circular（循环）

缓冲区大小：RINGBUF_SIZE（假设为 256）

数据宽度：字节

启动时，DMA 会把 NDTR 自动加载为 RINGBUF_SIZE（256）。

工作过程：

每接收一个字节，DMA 把它写入缓冲区，然后 NDTR 减 1。

当缓冲区写满（256 字节都写完），NDTR 减到 0，硬件会自动把它重置为 RINGBUF_SIZE，并回到缓冲区首地址继续写。

因此，任何时候你读取 NDTR，都能立刻算出 DMA 已经写了多少字节：
已写入字节数 = RINGBUF_SIZE - NDTR

而 下一个即将被写入的位置（即写指针）就是这个已写入字节数对 RINGBUF_SIZE 取模（实际上因为缓冲区是 2 的幂，用掩码即可）。
*/

/* 核心优点总结
无动态内存分配：固定大小数组，运行期间内存占用确定，对嵌入式系统极其友好。

时间效率极高：读写操作都只需移动指针和简单的边界判断，O(1)复杂度，特别适合在中断中使用。

天然的数据隔离：中断只操作写指针，主循环只操作读指针，只要设计得当，可以做到无锁（单生产者单消费者模型），避免竞态条件。

防止旧数据被覆盖：满时不写入新数据，保证了已接收但未处理的数据的完整性。

所以，在串口接收中使用环形缓冲区，本质是用最小的开销和确定的资源，在异步到来的数据流和处理速度不定的接收端之间架起一座流畅的、不会溢出的缓存桥。*/