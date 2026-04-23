#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "Move_Control.h"
#include "singlemove.h" 

char Serial_RxData[51];
uint8_t Serial_RxFlag;
uint8_t Serial_RxPacket[57+20];				//"@MSG\r\n"
//uint8_t Serial_RxPacket[20];	
float Sim_Data[19]={0};				
uint8_t Cmd[40]={0};
uint8_t Lock[40]={0};
int uart_length=40;//要驱动40个触点 帧总长度为40

//串口初始化波特率115200，优先级给0 0最高，防止因为打断导致数据出错
void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);

  
}

void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}

void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length)
{


	Serial_SendByte(Number / 100 % 10 + '0');
	Serial_SendByte(Number / 10 % 10 + '0');
	Serial_SendByte(Number / 1 % 10 + '0');
}

int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}

void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}

uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}

//从串口处获得所有假肢手关节的目标位置。
//首先进行：Serial_RxFlag 接收完毕符号检验，包长检验（19×3），包头检验是否为@，第一个数是不是在范围内
//检验完成后，把串口数据帧转化为float型数字，再把数字通过中转赋值给关节目标值
//遍历整个过程，把string字符串化为float数组，进行赋值，发给Move_Aim[i].Data_Set
//Move_Aim[i].Data_Set在程序move_coding里面参与运算，确定每个触点的目标值
//赋值完毕后Serial_RxFlag置零，表示已结束，可进行下一轮接收

//目前程序中对应5个触点的挤压状态以及使能
//void Get_Sim_Data(void)
//{
//		if (Serial_RxFlag == 1)
//		{
//			  uint8_t i=0;
//			if(strlen(Serial_RxPacket)==6){
//			    if(Serial_RxPacket[0]!='@')
//					{
//					    Cmd[0]=(Serial_RxPacket[0]-'0');
//							Cmd[1]=(Serial_RxPacket[1]-'0');
//							Cmd[2]=(Serial_RxPacket[2]-'0');
//							Cmd[3]=(Serial_RxPacket[3]-'0');
//							Cmd[4]=(Serial_RxPacket[4]-'0');
//							Cmd[5]=(Serial_RxPacket[5]-'0');
////						  lock=(Serial_RxPacket[1]-'0');
////						Serial_SendString(Cmd);
////            Serial_SendByte(Cmd[5]);
////            Serial_SendByte(lock);
////						Serial_SendString(&cmd);
////						Serial_SendString(&lock);
////						Serial_SendString("hello\r\n");

//				  }
//			}
//        Serial_RxFlag = 0;
//		}
//}

//16进制数据获取
void Get_Sim_Data(void)
{
		uint8_t i=0;
	  if (Serial_RxFlag == 1)
		{
			if(strlen(Serial_RxPacket)==uart_length){
			    if(Serial_RxPacket[0]!=0xCC)
					{
					    for(i=0;i<uart_length;i++)
							{
								Cmd[i]=Serial_RxPacket[i];
								if(Cmd[i]==0x01) {Lock[i]=0x01;} else if(Cmd[i]!=0x01) {Lock[i]=0x02;}
							}
//						  Cmd[0]=Serial_RxPacket[0];
//						  if(Cmd[0]==0x01) {Lock[0]=0x01;} else if(Cmd[0]!=0x01) {Lock[0]=0x02;}
//					    Cmd[1]=Serial_RxPacket[1];
//						  if(Cmd[1]==0x01) {Lock[1]=0x01;} else if(Cmd[1]!=0x01) {Lock[1]=0x02;}
//					    Cmd[2]=Serial_RxPacket[2];
//						  if(Cmd[2]==0x01) {Lock[2]=0x01;} else if(Cmd[2]!=0x01) {Lock[2]=0x02;}			
//					    Cmd[3]=Serial_RxPacket[3];
//						  if(Cmd[3]==0x01) {Lock[3]=0x01;} else if(Cmd[3]!=0x01) {Lock[3]=0x02;}	
//					    Cmd[4]=Serial_RxPacket[4];
//						  if(Cmd[4]==0x01) {Lock[4]=0x01;} else if(Cmd[4]!=0x01) {Lock[4]=0x02;}								
//						  cmd=Serial_RxPacket[0];
//						  lock=Serial_RxPacket[1];
//						Serial_SendString(Serial_RxPacket);
//            Serial_SendByte(cmd);
//            Serial_SendByte(lock);
//						Serial_SendByte(0xaa);
//						Serial_SendArray(Serial_RxPacket,2);
//						Serial_SendString(&cmd);
//						Serial_SendString(&lock);
//						Serial_SendString("hello\r\n");

				  }
			}
        Serial_RxFlag = 0;
		}
}



//串口接收中断服务函数。每次串口接收寄存器非空时，进入该中断。串口中断的优先级最高，是为了防止别的任务打断串口导致数据出错
//用于处理接收到的数据，并合成一个数据帧
//串口数据帧采用ASCII码作为数据类型，实际上还是char型或者uint8_t型数据，只是含义跟数值不一样
//数据帧以@作为包头，每个关节的霍尔传感器数值为关节角度值，19个关节每个3个数字，共57位
//以换行\r\n作为停止标志。整个数据帧读取程序是基于状态机的3个状态设计。
//每次进入中断，读取当前接收的值，都会进行判断，处于状态0，说明还没读取，判断当前是不是包头，是就下一次进状态1并初始化指针pRxPacket
//当前为状态1，如果包头和开关标志符Serial_RxFlag正常，则把当前接收的数挪进数组Serial_RxPacket中，数组指针pRxPacket后移一位
//当检测到读取的数字为\r时，或者指针移到57位，则数据帧结束，进入状态2
//在状态2，如果检查到这次读取的是\n，则状态进入0，同时在string数组的末端补上\0，Serial_RxFlag置1
//清除中断标志位

//多触点程序 需要通过串口发送整个的5个点的目标值。比如@100111，前五位代表5个点的状态，第6代表使能
//void USART1_IRQHandler(void)
//{
//	static uint8_t RxState = 0;
//	static uint8_t pRxPacket = 0;
////	static uint8_t temp = 0;
////	uint8_t i;
//	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
//	{
//		uint8_t RxData = USART_ReceiveData(USART1);
////		Serial_RxPacket=RxData;
////		Serial_RxFlag = 1;
////		Serial_SendByte(RxData);
////		Serial_SendString("\r\n");
////		Serial_SendString("\r\n");
//		if (RxState == 0)
//		{
//			if (RxData == '@' && Serial_RxFlag == 0)
//			{
////				for(i=0;i<70;i++) Serial_RxPacket[i]=0;
//				RxState = 1;
//				pRxPacket = 0;
//			}
//		}
//		else if (RxState == 1)
//		{
//			if (RxData == '\r')
//			{
//				RxState = 2;
//			}
//			else
//			{
//				Serial_RxPacket[pRxPacket] = RxData;
//				pRxPacket ++;
//				if (pRxPacket == 6)
//			  {
//				    RxState = 2;
//			  }
//			}
//		}
//		else if (RxState == 2)
//		{
//			if (RxData == '\n')
//			{
////				if(pRxPacket >= 40)
////				{
//					RxState = 0;
//					Serial_RxPacket[pRxPacket] = '\0';
//					Serial_RxFlag = 1;
////					Serial_SendByte(pRxPacket);
////	        Serial_SendByte(pRxPacket / 10 % 10 + '0');
////					Serial_SendString("hello");
////					Serial_SendString("\r\n");
//				
////				}
//			}
//		}
////		temp=RxData;
////		Get_Sim_Data();
//		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
//	}
//}

//16进制发送的串口解析程序
//CC 02 00 01 FE 每一位的位置代表第几个电机，00代表失能，02向下运动，01向上运动
void USART1_IRQHandler(void)
{
	static uint8_t RxState = 0;
	static uint8_t pRxPacket = 0;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		uint8_t RxData = USART_ReceiveData(USART1);
		
		if (RxState == 0)
		{
			if (RxData == 0xCC)
			{
				RxState = 1;
				pRxPacket = 0;
			}
		}
		else if (RxState == 1)
		{
			Serial_RxPacket[pRxPacket] = RxData;
			pRxPacket ++;
			if (pRxPacket >= uart_length)
			{
				RxState = 2;
			}
		}
		else if (RxState == 2)
		{
			if (RxData == 0xFE)
			{
				RxState = 0;
				Serial_RxFlag = 1;
			}
		}
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

