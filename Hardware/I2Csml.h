#ifndef _I2Csml_H
#define _I2Csml_H
#include "stm32f10x.h"
#include "Delay.h"

#define I2C_SDA GPIO_Pin_3									// PA6
#define I2C_SCL GPIO_Pin_4									// PA7
#define GPIO_I2C GPIOA

#define I2C_SDA_H GPIO_SetBits(GPIO_I2C, I2C_SDA)			// SDA置高
#define I2C_SDA_L GPIO_ResetBits(GPIO_I2C, I2C_SDA)			// SDA置低
#define I2C_SCL_H GPIO_SetBits(GPIO_I2C, I2C_SCL)			// SCL置高
#define I2C_SCL_L GPIO_ResetBits(GPIO_I2C, I2C_SCL)			// SCL置低

/* 声明全局函数 */
void I2C_INIT(GPIO_TypeDef* GPIOx, u32 RCC_APB2Periph);		// 初始化I2C模拟引脚
void I2C_SDA_OUT(void);
void I2C_SDA_IN(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Ack(void);
void I2C_NAck(void);
u8   I2C_Wait_Ack(void);
void I2C_Send_Byte(u8 txd);
u8   I2C_Read_Byte(u8 ack);

#endif
