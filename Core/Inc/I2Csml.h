#ifndef _I2CSML_H
#define _I2CSML_H

/**
  ******************************************************************************
  * @file    I2Csml.h
  * @brief   GPIO 模拟 I2C（位操作）头文件
  *          由于 PA3/PA4 在 STM32F103C8 上没有硬件 I2C 功能，通过软件模拟实现
  *          用于与 6 片 PCA9685 PWM 驱动器通信
  ******************************************************************************
  */

#include "main.h"

void I2C_INIT(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Ack(void);
void I2C_NAck(void);
uint8_t I2C_Wait_Ack(void);
void I2C_Send_Byte(uint8_t txd);
uint8_t I2C_Read_Byte(uint8_t ack);

#endif
