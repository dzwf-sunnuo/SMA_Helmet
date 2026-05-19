#ifndef __MYUSART_H
#define __MYUSART_H

/**
  ******************************************************************************
  * @file    myusart.h
  * @brief   串口发送模块（USART1 → 上位机）
  *          发送使用阻塞 HAL_UART_Transmit，每秒约 40 字节 × 20 次 = 800 字节
  *          接收由 ringbuffer 模块（DMA + IDLE 中断）独立处理
  ******************************************************************************
  */

#include "main.h"

void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(const uint8_t *Array, uint16_t Length);
void Serial_SendString(const char *String);

#endif
