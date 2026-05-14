#include "myusart.h"
#include "usart.h"
#include "singlemove.h"
#include <string.h>

/**
  ******************************************************************************
  * @file    myusart.c
  * @brief   串口通讯模块（USART1 → 串口转蓝牙 → 上位机）
  *
  *          ── 接收帧格式（上位机 → 头盔）──
  *          0xCC + 40 字节 + 0xFE
  *          使用字节中断（HAL_UART_Receive_IT）逐字节接收
  *          状态机解析帧头/数据/帧尾
  *
  *          ── 发送（头盔 → 上位机）──
  *          40 字节 Hall 传感器当前值 + \r\n
  *          由 TIM2 中断（20Hz）触发发送
  *
  *          ── 指令解析（Get_Sim_Data）──
  *          Cmd[i] 存入指令值，Lock[i] 根据指令生成锁状态：
  *            Cmd=0x01 → Lock=0x01（锁止，触点不动）
  *            Cmd≠0x01 → Lock=0x02（解锁，允许运动）
  *          Lock 值在 mutilmove_40 中再 -1：
  *            0x01-0x01=0x00 → 停止
  *            0x02-0x01=0x01 → 运动
  ******************************************************************************
  */

uint8_t Serial_RxPacket[UART_FRAME_LEN + 20];  /* 接收缓冲区 */
uint8_t Serial_RxFlag;                          /* 帧接收完成标志 */

/* 帧解析状态机 */
static uint8_t  RxState   = 0;    /* 0=等待帧头, 1=接收数据, 2=等待帧尾 */
static uint8_t  pRxPacket = 0;    /* 当前数据字节指针 */
uint8_t         rx_byte_buf;       /* HAL 单字节接收缓冲（供回调使用） */

/**
  * @brief  启动中断方式接收（单字节）
  *         每收到一个字节，HAL 会调用 HAL_UART_RxCpltCallback
  *         在其中处理该字节并重新启动下一字节接收
  */
void USART_Start_Rx(void)
{
    HAL_UART_Receive_IT(&huart1, &rx_byte_buf, 1);
}

/**
  * @brief  串口接收字节处理（由 HAL_UART_RxCpltCallback 调用）
  * @param  RxData  当前接收到的字节
  * @note   状态机流程：
  *          状态 0：等待 0xCC 帧头 → 转到状态 1
  *          状态 1：接收 40 字节数据 → 存满后转到状态 2
  *          状态 2：等待 0xFE 帧尾 → 置 Serial_RxFlag=1，回到状态 0
  *         每处理完一个字节都重新启动接收（环回）
  */
void UART_RxByte_Handler(uint8_t RxData)
{
    if (RxState == 0)                               /* 等待帧头 */
    {
        if (RxData == 0xCC)
        {
            RxState   = 1;
            pRxPacket = 0;
        }
    }
    else if (RxState == 1)                          /* 接收数据 */
    {
        Serial_RxPacket[pRxPacket] = RxData;
        pRxPacket++;
        if (pRxPacket >= UART_FRAME_LEN)            /* 收满 40 字节 */
        {
            RxState = 2;
        }
    }
    else if (RxState == 2)                          /* 等待帧尾 */
    {
        if (RxData == 0xFE)
        {
            RxState       = 0;                      /* 帧完成，回到初始状态 */
            Serial_RxFlag = 1;                      /* 通知主循环有新数据 */
        }
    }

    /* 重新启动下一字节接收 */
    HAL_UART_Receive_IT(&huart1, &rx_byte_buf, 1);
}

/* ─── 发送函数 ─── */

/** 发送单字节（阻塞，超时 10ms） */
void Serial_SendByte(uint8_t Byte)
{
    HAL_UART_Transmit(&huart1, &Byte, 1, 10);
}

/** 发送字节数组 */
void Serial_SendArray(const uint8_t *Array, uint16_t Length)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)Array, Length, 100);
}

/** 发送字符串 */
void Serial_SendString(const char *String)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)String, strlen(String), 100);
}

/* ─── 接收帧解析 ─── */

/**
  * @brief  解析接收到的完整帧，更新 Cmd 和 Lock 数组
  *          由 TIM2 中断（20Hz）在 Get_Sim_Data 标志有效时调用
  * @note   二次校验帧头确保数据完整性
  *          生成 Lock 数组供 Move_5_control 使用
  */
void Get_Sim_Data(void)
{
    if (Serial_RxFlag == 1)
    {
        if (strlen((char *)Serial_RxPacket) == UART_FRAME_LEN)
        {
            if (Serial_RxPacket[0] != 0xCC)         /* 二次校验：第一字节不应为帧头 */
            {
                for (int i = 0; i < UART_FRAME_LEN; i++)
                {
                    Cmd[i]  = Serial_RxPacket[i];
                    Lock[i] = (Cmd[i] == 0x01) ? 0x01 : 0x02;
                    /* Cmd=0x01 → Lock=0x01（锁止不动）
                       Cmd≠0x01 → Lock=0x02（允许运动） */
                }
            }
        }
        Serial_RxFlag = 0;                          /* 清除标志，等待下一帧 */
    }
}
