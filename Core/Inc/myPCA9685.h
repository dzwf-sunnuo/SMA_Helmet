#ifndef _MYPCA9685_H
#define _MYPCA9685_H

/**
  ******************************************************************************
  * @file    myPCA9685.h
  * @brief   PCA9685 16 通道 PWM 驱动器头文件（I2C 接口）
  *          共 6 片，可输出 96 路 PWM，频率 100Hz
  *          每路 PWM 占空比范围 0~4095（12 位分辨率）
  *          I2C 从机地址为 7 位左对齐格式
  ******************************************************************************
  */

#include "main.h"
#include "I2Csml.h"
#include <math.h>

/* PCA9685 I2C 从机地址（7 位左对齐） */
enum {
    PCA9685_ADDR_1 = 0x80,   /* 硬件地址 1000 000 0 */
    PCA9685_ADDR_2 = 0x82,   /* 硬件地址 1000 001 0 */
    PCA9685_ADDR_3 = 0x84,   /* 硬件地址 1000 010 0 */
    PCA9685_ADDR_4 = 0x88,   /* 硬件地址 1000 100 0 */
    PCA9685_ADDR_5 = 0x90,   /* 硬件地址 1001 000 0 */
    PCA9685_ADDR_6 = 0xA0,   /* 硬件地址 1010 000 0 */
    PCA9685_COUNT = 6,       /* PCA9685 总片数 */
};

extern const uint8_t PCA9685_ADDR[PCA9685_COUNT];

/* PCA9685 内部寄存器地址 */
#define PCA9685_MODE1      0x00       /* 模式寄存器 1 */
#define LEDn_ON_L(n)       (0x06 + (n) * 4)  /* 通道 n 亮起低 8 位 */
#define LEDn_ON_H(n)       (0x07 + (n) * 4)  /* 通道 n 亮起高 8 位 */
#define LEDn_OFF_L(n)      (0x08 + (n) * 4)  /* 通道 n 熄灭低 8 位 */
#define LEDn_OFF_H(n)      (0x09 + (n) * 4)  /* 通道 n 熄灭高 8 位 */
#define PCA9685_PRE_SCALE  0xFE       /* 预分频寄存器 */

/* MODE1 寄存器位定义 */
#define Sleep_ON  0x10               /* 休眠使能：写 1 进入休眠，此时可修改 PRE_SCALE */
#define AI_ON     0x20               /* 自动增量：写 1 寄存器地址自动递增 */

/* 函数声明 */
void PCA9685_InitAll(uint16_t freq_hz);                  /* 初始化全部 PCA9685 */
void PrescaleConfig_PCA9685(uint8_t addr, uint8_t prescale); /* 配置 PWM 频率 */
void setPWM(uint8_t addr, uint8_t channel, uint16_t led_off); /* 设置单路 PWM 占空比 */
uint8_t Prescale_Calculate(uint16_t frequence);           /* 计算预分频值 */

#endif
