#ifndef __DELAY_H
#define __DELAY_H

/**
  ******************************************************************************
  * @file    Delay.h
  * @brief   微秒/毫秒/秒级延时函数头文件（DWT 硬件周期计数器）
  ******************************************************************************
  */

#include "main.h"

void Delay_Init(void);
void Delay_us(uint32_t us);
void Delay_ms(uint32_t ms);
void Delay_s(uint32_t s);

#endif
