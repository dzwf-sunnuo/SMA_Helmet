#include "myusart.h"
#include "usart.h"
#include <string.h>

/**
  ******************************************************************************
  * @file    myusart.c
  * @brief   串口发送函数（USART1 → 蓝牙 → 上位机）
  *          参数：115200 bps，8 数据位，1 停止位，无校验
  *          发送内容：40 字节霍尔状态 + \r\n（由 TIM2 中断 20Hz 触发）
  ******************************************************************************
  */

/**
  * @brief  发送单字节（阻塞，超时 10ms）
  */
void Serial_SendByte(uint8_t Byte)
{
    HAL_UART_Transmit(&huart1, &Byte, 1, 10);
}

/**
  * @brief  发送字节数组（阻塞，超时 100ms）
  */
void Serial_SendArray(const uint8_t *Array, uint16_t Length)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)Array, Length, 100);
}

/**
  * @brief  发送字符串（阻塞）
  */
void Serial_SendString(const char *String)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)String, strlen(String), 100);
}
