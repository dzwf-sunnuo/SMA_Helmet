#ifndef _MYPCA9685_H
#define _MYPCA9685_H
#include "stm32f10x.h"
#include "I2Csml.h"
#include "math.h"

/****** 硬件地址 ******/
#define PCA9685_Addr_1 		0x80					//硬件地址1000 000 0
#define PCA9685_Addr_2 		0x82					//硬件地址1000 001 0
#define PCA9685_Addr_3    0x84					//硬件地址1000 010 0
#define PCA9685_Addr_4    0x88					//硬件地址1000 100 0
#define PCA9685_Addr_5    0x90					//硬件地址1001 000 0
#define PCA9685_Addr_6    0xA0					//硬件地址1010 000 0

/****** 寄存器地址 ******/
#define LEDn_ON_L(n)		(0x06 + (n)*4)					//引脚n高电位低八位寄存器地址
#define LEDn_ON_H(n)		(0x07 + (n)*4)					//引脚n高电位高八位寄存器地址
#define LEDn_OFF_L(n)		(0x08 + (n)*4)					//引脚n低电位低八位寄存器地址
#define LEDn_OFF_H(n)		(0x09 + (n)*4)					//引脚n低电位低八位寄存器地址
// n取值：0~15
#define PCA9685_MODE1            0x00
#define PCA9685_PRE_SCALE      	 0xfe						//PRE_SCALE周期寄存器

/****** MODE1模式选择(使用时需要进行‘与’运算) ******/
#define Restart					 0x80						//写1复位
#define ExtClock_ON				 0x40						//写0使用内部25MHz时钟，写1使用外部时钟
#define AI_ON					 0x20						//写0内部地址读写后不自动增加，写1内部地址自动增加
#define Sleep_ON 				 0x10						//写0退出SLEEP模式，写1进入SLEEP模式（修改周期寄存器，时钟寄存器）
#define AllCall_ON               0x01						//写0不响应0x07IIC通用地址，写1响应0x07通用地址
#define INIT                     0x00

/****** 函数声明 ******/
void PrescaleConfig_PCA9685(u8 SCALE_Addr, u8 prescale);
void setPWM(u8 SCALE_Addr, u8 n, u16 led_off);				//默认从0开始，通过控制led_off来控制占空比
u8 Prescale_Calculate(u16 frequence);
#endif






















