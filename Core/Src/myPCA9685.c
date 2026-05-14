#include "myPCA9685.h"
#include "Delay.h"

/**
  ******************************************************************************
  * @file    myPCA9685.c
  * @brief   PCA9685 16 通道 PWM 驱动器
  *          通过软件 I2C 与 6 片 PCA9685 通信，产生 100Hz PWM 信号
  *          驱动 SMA（形状记忆合金）触觉反馈阵列
  *          PWM 计算公式：PWM 频率 = 25MHz / (4096 × prescale × 0.915)
  *          默认 prescale 使 PWM 频率 = 100Hz
  *          setPWM(addr, ch, 0)    → 占空比 0%（停止加热）
  *          setPWM(addr, ch, 4000) → 占空比 ~98%（满功率加热）
  ******************************************************************************
  */

const uint8_t PCA9685_ADDR[PCA9685_COUNT] = {
    PCA9685_ADDR_1, PCA9685_ADDR_2, PCA9685_ADDR_3,
    PCA9685_ADDR_4, PCA9685_ADDR_5, PCA9685_ADDR_6,
};

/**
  * @brief  计算 PCA9685 预分频值
  * @param  frequence  目标 PWM 频率（Hz），范围 4~1000
  * @retval 预分频寄存器值
  */
uint8_t Prescale_Calculate(uint16_t frequence)
{
    float prescaleval = 25000000.0f / (4096.0f * (float)frequence * 0.915f);
    return (uint8_t)floorf(prescaleval + 0.5f) - 1;
}

/**
  * @brief  配置单个 PCA9685 的 PWM 刷新频率
  * @param  addr     PCA9685 的 I2C 地址
  * @param  prescale  预分频值（调用 Prescale_Calculate 计算）
  * @note   需先进入 SLEEP 模式才能修改 PRE_SCALE，改完后退出 SLEEP
  */
void PrescaleConfig_PCA9685(uint8_t addr, uint8_t prescale)
{
    /* 进入 SLEEP 模式 */
    I2C_Start();
    I2C_Send_Byte(addr);
    I2C_Wait_Ack();
    I2C_Send_Byte(PCA9685_MODE1);
    I2C_Wait_Ack();
    I2C_Send_Byte(Sleep_ON);
    I2C_Wait_Ack();

    /* 写入预分频值 */
    I2C_Start();
    I2C_Send_Byte(addr);
    I2C_Wait_Ack();
    I2C_Send_Byte(PCA9685_PRE_SCALE);
    I2C_Wait_Ack();
    I2C_Send_Byte(prescale);
    I2C_Wait_Ack();

    /* 退出 SLEEP，使能自动增量 */
    I2C_Start();
    I2C_Send_Byte(addr);
    I2C_Wait_Ack();
    I2C_Send_Byte(PCA9685_MODE1);
    I2C_Wait_Ack();
    I2C_Send_Byte(AI_ON);
    I2C_Wait_Ack();
    I2C_Stop();
}

/**
  * @brief  一次性初始化全部 6 片 PCA9685
  * @param  freq_hz  PWM 频率（建议 100Hz）
  */
void PCA9685_InitAll(uint16_t freq_hz)
{
    uint8_t prescale = Prescale_Calculate(freq_hz);
    for (int i = 0; i < PCA9685_COUNT; i++)
        PrescaleConfig_PCA9685(PCA9685_ADDR[i], prescale);
}

/**
  * @brief  设置单个 PWM 通道的占空比
  * @param  addr     PCA9685 的 I2C 地址
  * @param  channel  PWM 通道号（0~15）
  * @param  led_off  熄灭计数值（0~4095），0=常灭，4095=常亮
  * @note   LED_ON 始终为 0，通过 LED_OFF 控制占空比
  */
void setPWM(uint8_t addr, uint8_t channel, uint16_t led_off)
{
    uint8_t led_on_h = 0;
    uint8_t led_on_l = 0;
    uint8_t led_off_h = led_off >> 8;
    uint8_t led_off_l = led_off & 0xFF;

    I2C_Start();
    I2C_Send_Byte(addr);
    I2C_Wait_Ack();
    I2C_Send_Byte(LEDn_ON_L(channel));  /* 从 LEDn_ON_L 开始，利用 AI_ON 自动增量 */
    I2C_Wait_Ack();
    I2C_Send_Byte(led_on_l);
    I2C_Wait_Ack();
    I2C_Send_Byte(led_on_h);
    I2C_Wait_Ack();
    I2C_Send_Byte(led_off_l);
    I2C_Wait_Ack();
    I2C_Send_Byte(led_off_h);
    I2C_Wait_Ack();
    I2C_Stop();
}
