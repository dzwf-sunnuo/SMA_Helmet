#include "mymux.h"
#include "tim.h"

/**
  ******************************************************************************
  * @file    mymux.c
  * @brief   CD74HCx4067 多路复用器驱动
  *          3 片 16 通道 MUX 的地址线并联（S0-S3 共用），
  *          每片 MUX 的公共输出端分别连接到 PB11/PB10/PB1，
  *          每次切换地址后同时读取 3 路数据，存入 ADC_MUX_NUM[48]
  *          TIM3 以 20Hz 频率触发 Mux_Scan 完成一轮 48 个传感器的读取
  ******************************************************************************
  */

uint8_t ADC_MUX_NUM[MUX_TOTAL];   /* 48 个霍尔传感器原始值（0 或 1） */
Mux_Data_struct Mux_Data;         /* 单次读取的 3 路数据 */

/**
  * @brief  设置 MUX 的 4 位地址并读取 3 路数据
  * @param  chs  通道号（0~15），对应 4 位地址 S3-S0
  * @note   地址建立后需短暂延时等待 MUX 切换稳定（原代码的硬件延时）
  *         读取结果存入全局结构体 Mux_Data
  */
void Mux_choose(uint8_t chs)
{
    /* S3-S0 对应 PA1, PA7, PA6, PA5 */
    HAL_GPIO_WritePin(MUX_S0_GPIO_Port, MUX_S0_Pin, (chs & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MUX_S1_GPIO_Port, MUX_S1_Pin, (chs & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MUX_S2_GPIO_Port, MUX_S2_Pin, (chs & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MUX_S3_GPIO_Port, MUX_S3_Pin, (chs & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    /* 短暂延时等待 MUX 切换稳定 */
    for (volatile uint8_t m = 0; m < 50; m++);

    /* 读取三路 MUX 数据 */
    Mux_Data.Data_1 = HAL_GPIO_ReadPin(MUX_DATA1_GPIO_Port, MUX_DATA1_Pin);  /* PB11 */
    Mux_Data.Data_2 = HAL_GPIO_ReadPin(MUX_DATA2_GPIO_Port, MUX_DATA2_Pin);  /* PB10 */
    Mux_Data.Data_3 = HAL_GPIO_ReadPin(MUX_DATA3_GPIO_Port, MUX_DATA3_Pin);  /* PB1  */

    for (volatile uint8_t m = 0; m < 50; m++);
}

/**
  * @brief  扫描全部 16 个 MUX 通道，读取 48 个传感器值
  * @note   每次 MUX 地址循环 i=0~15，每通道读出 3 个值
  *          存储格式：ADC_MUX_NUM[3*i+0]=Data1, [3*i+1]=Data2, [3*i+2]=Data3
  *          由 TIM3 中断（20Hz）调用
  */
void Mux_Scan(void)
{
    uint8_t temp1, temp2, temp3;

    for (uint8_t i = 0; i < MUX_CHANNELS; i++)
    {
        Mux_choose(i);
        temp1 = Mux_Data.Data_1;
        temp2 = Mux_Data.Data_2;
        temp3 = Mux_Data.Data_3;
        ADC_MUX_NUM[3 * i + 0] = temp1;
        ADC_MUX_NUM[3 * i + 1] = temp2;
        ADC_MUX_NUM[3 * i + 2] = temp3;
    }
}

/**
  * @brief  启动 TIM3，开始定时 MUX 扫描
  *         TIM3 配置为 20Hz（PSC=1000-1, ARR=3600-1）
  */
void Mux_Start(void)
{
    HAL_TIM_Base_Start_IT(&htim3);
}
