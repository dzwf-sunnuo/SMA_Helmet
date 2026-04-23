#include "stm32f10x.h"                  // Device header
#include "Delay.h" 
#include "Key.h" 
#include "myusart.h"
#include "mymux.h"

uint8_t ADC_MUX_NUM[48]={0};//全局变量，存储读取到的传感器值
Mux_Data_struct Mux_Data;

//配置传感器读取的3个引脚PB11 PB10 PB1为上拉输入。因为是高低电平不用AD
//PB11读第一个板子，PB10读第二个板子，PB1读第三个
void My_GPIO_Input_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}


//多路选择芯片配置及中断配置。3个16选一芯片，选择引脚都连接在PA5 PA6 PA7 PA1上，对应S0 S1 S2 S3
//定时器3负责以 72M/(3600*1000)=20Hz的频率，读取触点霍尔值。优先级2 2较低
void Mux_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

//PA5,6,7,1是供电引脚，接多路复用芯片地址选择脚S0,S1,S2,S3	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

//PB1是读取引脚，接多路复用芯片输出引脚z
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	TIM_InternalClockConfig(TIM3);

//配置定时器3频率为20Hz
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 3600 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1000 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM3, ENABLE);
}

//多路复用模块，选取不同的地址以选择不同的传感器，在同三个引脚读取
//用for循环手动延时，不能用delay，这样会导致程序冲突。等待程序稳定
//输入为通道号，按照通道号确定四个引脚哪几个接通，进而读取对应3个板子上的数值
//PB11读第一个板子，PB10读第二个板子，PB1读第三个
//通过与的运算读取目标引脚号
void Mux_choose(uint8_t chs)
{
	static uint8_t m=0;
	GPIO_WriteBit(GPIOA, GPIO_Pin_5, (BitAction)(chs & 0x01));
	GPIO_WriteBit(GPIOA, GPIO_Pin_6, (BitAction)(chs & 0x02));
	GPIO_WriteBit(GPIOA, GPIO_Pin_7, (BitAction)(chs & 0x04));
	GPIO_WriteBit(GPIOA, GPIO_Pin_1, (BitAction)(chs & 0x08));
	for(m=0;m<50;m++){ }
	Mux_Data.Data_1= GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);
	Mux_Data.Data_2= GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);
	Mux_Data.Data_3= GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);
	for(m=0;m<50;m++){ }
}


//定时器3中断，以20Hz的频率，循环给多路复用器供电，读取每个通道的值并存储
//i负责遍历16个引脚。同时j为16进制通道号，跟随i一起变化。j遍历选择引脚，配合上面的程序读取全部传感器数值
//每次读取的数发给临时变量temp1 temp2 temp3，再通过临时变量发给ADC_MUX_NUM[k]，作为读取到的霍尔当前值。清空临时变量，清除中断位
void TIM3_IRQHandler(void)
{
	static uint8_t i=0,j=0x00;
	static uint8_t temp1,temp2,temp3;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		for(i=0;i<16;i++)
		{
			Mux_choose(j);
			temp1 = Mux_Data.Data_1;
			temp2 = Mux_Data.Data_2; 
		  temp3 = Mux_Data.Data_3;
			ADC_MUX_NUM[3*i+0]=	temp1;
      ADC_MUX_NUM[3*i+1]=	temp2;
		  ADC_MUX_NUM[3*i+2]=	temp3;
//			Serial_SendByte(ADC_MUX_NUM[3*i+0]);//显示第一块16选3板子的读数
//			Serial_SendByte(ADC_MUX_NUM[3*i+1]);//显示第二块16选3板子的读数
//			Serial_SendByte(ADC_MUX_NUM[3*i+2]);//显示第三块16选3板子的读数
			j=j+0x01;
			temp1=0;temp2=0;temp3=0;
		}
//		Serial_SendByte(0x0D);Serial_SendByte(0x0A);
		j=0x00;
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}
