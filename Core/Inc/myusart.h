#ifndef __MYUSART_H
#define __MYUSART_H

/**
  ******************************************************************************
  * @file    myusart.h
  * @brief   串口通讯模块头文件（USART1 → 蓝牙 → 上位机）
  *          UART 参数：115200 bps，8 数据位，1 停止位，无校验
  *          帧格式（上位机 → 头盔）：
  *            0xCC + 40 字节 Cmd 数据 + 0xFE
  *          Cmd[i] 含义：
  *            0x01 = 触点不动（Lock=0x01，解析为锁止）
  *            0x02 = 触点回缩/抬起（Lock=0x02）
  *            0x03 = 触点下压/挤压（Lock=0x02）
  *          头盔 → 上位机：40 字节 Hall 状态 + \r\n（20Hz 定时发送）
  ******************************************************************************
  */

#include "main.h"

#define UART_FRAME_LEN  40    /* 有效数据长度（字节） */

extern uint8_t Serial_RxPacket[UART_FRAME_LEN + 20];  /* 接收缓冲 */
extern uint8_t Serial_RxFlag;                          /* 接收完成标志 */
extern uint8_t rx_byte_buf;   /* HAL 单字节接收缓冲（在回调中使用） */

void USART_Start_Rx(void);    /* 启动中断方式接收 */
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(const uint8_t *Array, uint16_t Length);
void Serial_SendString(const char *String);
void Get_Sim_Data(void);      /* 解析接收帧 → Cmd/Lock 数组 */

/* HAL 回调处理函数（由 main.c 中的 HAL_UART_RxCpltCallback 调用） */
void UART_RxByte_Handler(uint8_t rx_byte);

#endif
