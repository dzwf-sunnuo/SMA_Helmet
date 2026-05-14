#include "Pointmove.h"
#include "myPCA9685.h"
#include "mymux.h"

/**
  ******************************************************************************
  * @file    Pointmove.c
  * @brief   单个触点上下运动原子操作
  *          每个触点由一对 SMA（形状记忆合金）组成对拉机构：
  *            channel    → 挤压 SMA（加热收缩使触点下压）
  *            channel+1  → 回缩 SMA（加热收缩使触点抬起）
  *          同一时刻仅允许一侧 SMA 加热，另一侧冷却
  *          状态由双稳态霍尔传感器判断：0=回缩, 1=下压
  ******************************************************************************
  */

/**
  * @brief  触点回缩（上抬），Hall 目标值 = 0
  * @param  channel  挤出 SMA 对应的 PWM 通道号
  * @note   当前 Hall=0（已回缩）→ 不动作
  *         当前 Hall=1（已下压）→ 加热回缩侧 SMA（channel+1）
  */
void Motion_up(uint8_t channel)
{
    if (ADC_MUX_NUM[0] == 0)                    /* 已回缩，不动作 */
    {
        setPWM(PCA9685_ADDR_1, channel,     2000);
        setPWM(PCA9685_ADDR_1, channel + 1, 0);
    }
    else if (ADC_MUX_NUM[0] == 1)               /* 当前下压，需回缩 */
    {
        setPWM(PCA9685_ADDR_1, channel,     0);
        setPWM(PCA9685_ADDR_1, channel + 1, 0);
    }
}

/**
  * @brief  触点下压（下扎），Hall 目标值 = 1
  * @param  channel  挤出 SMA 对应的 PWM 通道号
  * @note   当前 Hall=1（已下压）→ 不动作
  *         当前 Hall=0（已回缩）→ 加热挤压侧 SMA（channel）
  */
void Motion_down(uint8_t channel)
{
    if (ADC_MUX_NUM[0] == 1)                    /* 当前下压，不动作 */
    {
        setPWM(PCA9685_ADDR_1, channel,     0);
        setPWM(PCA9685_ADDR_1, channel + 1, 2000);
    }
    else if (ADC_MUX_NUM[0] == 0)               /* 当前回缩，需下压 */
    {
        setPWM(PCA9685_ADDR_1, channel,     0);
        setPWM(PCA9685_ADDR_1, channel + 1, 0);
    }
}
