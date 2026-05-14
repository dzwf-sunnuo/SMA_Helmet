#include "I2Csml.h"
#include "Delay.h"

/**
  ******************************************************************************
  * @file    I2Csml.c
  * @brief   GPIO 模拟 I2C 总线（位操作实现）
  *          引脚：PA3 = I2C_SDA（数据线），PA4 = I2C_SCL（时钟线）
  *          SDA 在发送/接收时动态切换 GPIO 方向（开漏输出 ↔ 上拉输入）
  *          用于驱动 6 片 PCA9685 的寄存器读写
  ******************************************************************************
  */

/* 宏定义：简化 HAL GPIO 操作 */
#define I2C_SCL_H  HAL_GPIO_WritePin(I2C_SCL_GPIO_Port, I2C_SCL_Pin, GPIO_PIN_SET)
#define I2C_SCL_L  HAL_GPIO_WritePin(I2C_SCL_GPIO_Port, I2C_SCL_Pin, GPIO_PIN_RESET)
#define I2C_SDA_H  HAL_GPIO_WritePin(I2C_SDA_GPIO_Port, I2C_SDA_Pin, GPIO_PIN_SET)
#define I2C_SDA_L  HAL_GPIO_WritePin(I2C_SDA_GPIO_Port, I2C_SDA_Pin, GPIO_PIN_RESET)
#define I2C_SDA_IN  HAL_GPIO_ReadPin(I2C_SDA_GPIO_Port, I2C_SDA_Pin)

/** 将 SDA 切换为输出模式（开漏） */
static void SDA_Out(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = I2C_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(I2C_SDA_GPIO_Port, &GPIO_InitStruct);
}

/** 将 SDA 切换为输入模式（上拉） */
static void SDA_In(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = I2C_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(I2C_SDA_GPIO_Port, &GPIO_InitStruct);
}

/**
  * @brief  I2C 初始化：将 SCL 和 SDA 拉高（空闲状态）
  */
void I2C_INIT(void)
{
    I2C_SCL_H;
    I2C_SDA_H;
}

/**
  * @brief  发送 I2C 起始信号
  *         SCL=H 时 SDA 由 H→L
  */
void I2C_Start(void)
{
    SDA_Out();
    I2C_SDA_H;
    I2C_SCL_H;
    Delay_us(5);
    I2C_SDA_L;
    Delay_us(6);
    I2C_SCL_L;
}

/**
  * @brief  发送 I2C 停止信号
  *         SCL=H 时 SDA 由 L→H
  */
void I2C_Stop(void)
{
    SDA_Out();
    I2C_SCL_L;
    I2C_SDA_L;
    I2C_SCL_H;
    Delay_us(6);
    I2C_SDA_H;
    Delay_us(6);
}

/** 主机发送应答信号（ACK） */
void I2C_Ack(void)
{
    I2C_SCL_L;
    SDA_Out();
    I2C_SDA_L;
    Delay_us(2);
    I2C_SCL_H;
    Delay_us(5);
    I2C_SCL_L;
}

/** 主机发送非应答信号（NACK） */
void I2C_NAck(void)
{
    I2C_SCL_L;
    SDA_Out();
    I2C_SDA_H;
    Delay_us(2);
    I2C_SCL_H;
    Delay_us(5);
    I2C_SCL_L;
}

/**
  * @brief  等待从机应答
  * @retval 0  收到 ACK
  * @retval 1  超时（从机无应答），已自动发送停止信号
  */
uint8_t I2C_Wait_Ack(void)
{
    uint8_t tempTime = 0;
    SDA_In();
    I2C_SDA_H;
    Delay_us(1);
    I2C_SCL_H;
    Delay_us(1);
    while (I2C_SDA_IN)                    /* 等待 SDA 被从机拉低 */
    {
        tempTime++;
        if (tempTime > 250)               /* 超时退出 */
        {
            I2C_Stop();
            return 1;
        }
    }
    I2C_SCL_L;
    return 0;
}

/**
  * @brief  通过 I2C 发送一个 8 位字节
  * @param  txd  待发送数据（MSB 优先）
  */
void I2C_Send_Byte(uint8_t txd)
{
    uint8_t i;
    SDA_Out();
    I2C_SCL_L;                            /* 拉低时钟，准备数据 */
    for (i = 0; i < 8; i++)
    {
        if ((txd & 0x80) > 0)             /* 0x80 = 1000 0000，取最高位 */
            I2C_SDA_H;
        else
            I2C_SDA_L;
        txd <<= 1;                        /* 左移一位 */
        I2C_SCL_H;                        /* 时钟上升沿，从机采样 */
        Delay_us(2);
        I2C_SCL_L;
        Delay_us(2);
    }
}

/**
  * @brief  通过 I2C 读取一个 8 位字节
  * @param  ack  0=回复 NACK，非 0=回复 ACK
  * @retval 读取到的字节
  */
uint8_t I2C_Read_Byte(uint8_t ack)
{
    uint8_t i, receive = 0;
    SDA_In();
    for (i = 0; i < 8; i++)
    {
        I2C_SCL_L;
        Delay_us(2);
        I2C_SCL_H;
        receive <<= 1;
        if (I2C_SDA_IN)
            receive++;
        Delay_us(1);
    }
    if (ack == 0)
        I2C_NAck();
    else
        I2C_Ack();
    return receive;
}
