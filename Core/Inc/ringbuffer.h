#ifndef __RINGBUFFER_H
#define __RINGBUFFER_H

/**
  ******************************************************************************
  * @file    ringbuffer.h
  * @brief   DMA 环形缓冲区 + UART IDLE 中断帧接收模块
  *          DMA1 CH5 (Circular) 持续将 USART1 数据写入环形缓冲
  *          UART IDLE 中断检测帧间空闲，触发帧解析
  *          替代原逐字节中断方案，中断频率从 840 次/秒降至 20 次/秒
  ******************************************************************************
  */

#include "main.h"

#define RINGBUF_SIZE  256    /* 环形缓冲容量（可容纳 5+ 帧），须为 2^n */

/* 环形缓冲区控制结构 */
typedef struct {
    uint8_t  buf[RINGBUF_SIZE];  /* 数据缓冲区（DMA 循环写入） */
    uint16_t last_ndtr;          /* 上次处理时的 NDTR 值（用于防重复） */
    volatile uint8_t  frame_ready;        /* 完整帧就绪标志 */
} RingBuf_t;

/* 由 main.c 中的 HAL_UARTEx_RxEventCallback 调用 */
void RingBuf_RxCallback(UART_HandleTypeDef *huart, uint16_t Size);

/* 启动 DMA + IDLE 接收（初始化时调用一次） */
void RingBuf_Start(UART_HandleTypeDef *huart);

/* 检查是否有完整帧就绪，如有则解析到 Cmd/Lock */
void RingBuf_Process(void);

#endif
