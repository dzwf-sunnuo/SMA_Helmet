#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include <stdio.h>
#include <stdarg.h>
#include "myusart.h"
#include "mymux.h"

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//根据传感器读取到的数据，转化为ASCII码发送到串口
void my_getstate(void)
{
//	Serial_SendString("红外对管");
//	if(ADC_MUX_NUM[0] == 0)
//	{
//		Serial_SendString("-00");
//	}
//	else if(ADC_MUX_NUM[0] == 1)
//	{
//		Serial_SendString("-01");
//	}
//	Serial_SendString("\r\n");
//	
//	Serial_SendString("光敏电阻");
//	if(ADC_MUX_NUM[1] == 0)
//	{
//		Serial_SendString("-00");
//	}
//	else if(ADC_MUX_NUM[1] == 1)
//	{
//		Serial_SendString("-01");
//	}
//	Serial_SendString("\r\n");

//	Serial_SendString("霍尔传感器");
//	if(ADC_MUX_NUM[2] == 0)
//	{
//		Serial_SendString("-00");
//	}
//	else if(ADC_MUX_NUM[2] == 1)
//	{
//		Serial_SendString("-01");
//	}
//  Serial_SendString("传感器读数");
//	Serial_SendString("\r\n");
//	Serial_SendByte(ADC_MUX_NUM[12]);
	Serial_SendArray(ADC_MUX_NUM, 48);
	Serial_SendByte(0x0D);	Serial_SendByte(0x0A);	
//	Serial_SendString("\r\n");
}
