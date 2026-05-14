#ifndef __SINGLE_MOVE_H
#define __SINGLE_MOVE_H

/**
  ******************************************************************************
  * @file    singlemove.h
  * @brief   SMA 单点/多点运动控制及完整触点驱动头文件
  *          包含：
  *            - SMA → PCA9685 通道映射（PWM_Rearrange_Init）
  *            - MUX 传感器 → Point_Motion 映射（AD_Rearrange）
  *            - 8×5=40 触点完整控制循环（Move_5_control）
  *            - 单点/多点/40 点运动判决函数
  ******************************************************************************
  */

#include "main.h"

extern uint8_t Cmd[40];    /* 上位机指令数组（40 个触点的目标指令） */
extern uint8_t Lock[40];   /* 运动锁数组（0x01=锁止，0x02=允许运动） */

void stopPWM(void);        /* 停止所有 PWM 输出 */
void singlemove(uint8_t cmd, uint8_t lock);
void mutilmove(uint8_t cmd, uint8_t target, uint8_t n, uint8_t lock);
void mutilmove_40(uint8_t cmd, uint8_t target, uint8_t pwm, uint8_t n1, uint8_t n2, uint8_t lock);
void Move_5_control(void); /* 主控制循环：8 组 × 5 触点 = 40 点 */

#endif
