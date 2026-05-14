#ifndef __MYMUX_H
#define __MYMUX_H

/**
  ******************************************************************************
  * @file    mymux.h
  * @brief   CD74HCx4067 16 通道模拟多路复用器驱动头文件
  *          3 片 MUX × 16 通道 × 3 数据线 = 48 个霍尔传感器值
  *          地址选择：PA1(S3), PA5(S0), PA6(S1), PA7(S2)
  *          数据读取：PB11(数据线1), PB10(数据线2), PB1(数据线3)
  *          TIM3 以 20Hz 频率触发扫描
  ******************************************************************************
  */

#include "main.h"

/* 多路复用器读取数据结构体 */
typedef struct
{
    uint8_t Data_1;    /* PB11 读取值（第 1 路） */
    uint8_t Data_2;    /* PB10 读取值（第 2 路） */
    uint8_t Data_3;    /* PB1  读取值（第 3 路） */
} Mux_Data_struct;

#define MUX_CHANNELS  16   /* 每片 MUX 的通道数 */
#define MUX_LINES     3    /* 每通道读取的数据线数 */
#define MUX_TOTAL     (MUX_CHANNELS * MUX_LINES)  /* 共 48 个数值 */

extern uint8_t ADC_MUX_NUM[MUX_TOTAL];   /* 全局传感器数值数组 */
extern Mux_Data_struct Mux_Data;         /* 当前通道的三路数据 */

void Mux_choose(uint8_t chs);   /* 选择 MUX 通道并读取数据 */
void Mux_Scan(void);            /* 扫描全部 16 通道，更新 ADC_MUX_NUM */
void Mux_Start(void);           /* 启动 TIM3 定时扫描 */

#endif
