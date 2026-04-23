#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "Key.h"
#include "myusart.h"
#include "mytimer.h"
#include "mymux.h"
#include "myPCA9685.h"
#include "I2Csml.h"
#include "Pointmove.h"
#include "Move_Control.h"
#include "singlemove.h"   

int main(void)
{
	static uint16_t i,Num=0;
//	LED_Init();  //初始化
	Key_Init();
	Serial_Init();                            //串口初始化，PA9 PA10
	Timer_Init();                             //定时器2初始化，定时读取串口数据（目标位置）
	Mux_Init();                               //多路选择传感器读取，包括选择引脚配置和，用定时器3，定时读取
	My_GPIO_Input_Init();                     //多路选择传感器读取，读取引脚配置
	Motion_Init();                            //触点初始化，包括位置和驱动，传感器数值目标位置，触点对应的传感和驱动口等等
	I2C_INIT(GPIOA, RCC_APB2Periph_GPIOA);		// I2C初始化
	PrescaleConfig_PCA9685(PCA9685_Addr_1, Prescale_Calculate(100));	// 第一块PCA9685初始化，PWM刷新频率100HZ
	PrescaleConfig_PCA9685(PCA9685_Addr_2, Prescale_Calculate(100));	// 第二块PCA9685初始化，PWM刷新频率100HZ
	PrescaleConfig_PCA9685(PCA9685_Addr_3, Prescale_Calculate(100));	// 第三块PCA9685初始化，PWM刷新频率100HZ
	PrescaleConfig_PCA9685(PCA9685_Addr_4, Prescale_Calculate(100));	// 第四块PCA9685初始化，PWM刷新频率100HZ
	PrescaleConfig_PCA9685(PCA9685_Addr_5, Prescale_Calculate(100));	// 第五块PCA9685初始化，PWM刷新频率100HZ
	PrescaleConfig_PCA9685(PCA9685_Addr_6, Prescale_Calculate(100));	// 第六块PCA9685初始化，PWM刷新频率100HZ
	Delay_ms(10);
	Serial_SendByte(0x01);
  PWM_Rearrange_Init();//初始化触点与驱动板对应关系
//	setPWM(PCA9685_Addr_1, 4, 2000);
		for(i=0;i<16;i++)	{setPWM(PCA9685_Addr_1, i, 0);}//自己画的PCA9685板子需要置零，否则就会出错
		for(i=0;i<16;i++)	{setPWM(PCA9685_Addr_2, i, 0);}
		for(i=0;i<16;i++)	{setPWM(PCA9685_Addr_3, i, 0);}
		for(i=0;i<16;i++)	{setPWM(PCA9685_Addr_4, i, 0);}
		for(i=0;i<16;i++)	{setPWM(PCA9685_Addr_5, i, 0);}
		for(i=0;i<16;i++)	{setPWM(PCA9685_Addr_6, i, 0);}
		setPWM(PCA9685_Addr_1, 1, 4000);//左-右 第五个振动电机 现在是风扇
	
	while (1)
	{ 
		Num++;
//		uint16_t i = 2000;
//		setPWM(PCA9685_Addr_1, 0, i);
//		Delay_ms(100);            
//		setPWM(PCA9685_Addr_1, 1, i);
//		Delay_ms(100);            
//		setPWM(PCA9685_Addr_1, 2, i);
//		Delay_ms(100);            
//		setPWM(PCA9685_Addr_1, 3, i);
//		Delay_ms(100);            
//		setPWM(PCA9685_Addr_1, 4, i);
//		Delay_ms(100);            
//		setPWM(PCA9685_Addr_1, 5, i);
//		Delay_ms(100);            
//		setPWM(PCA9685_Addr_1, 6, i);
//		Delay_ms(100);            
//		setPWM(PCA9685_Addr_1, 7, i);
//		Delay_ms(100);            
//		setPWM(PCA9685_Addr_1, 8, i);
//		Delay_ms(100);            
//		setPWM(PCA9685_Addr_1, 9, i);
//		Delay_ms(100);
//		i = 0;
//		setPWM(PCA9685_Addr_1, 0, i);
//		Delay_ms(100);            
//		setPWM(PCA9685_Addr_1, 1, i);
//		Delay_ms(100);            
//		setPWM(PCA9685_Addr_1, 2, i);
//		Delay_ms(100);            
//		setPWM(PCA9685_Addr_1, 3, i);
//		Delay_ms(100);            
//		setPWM(PCA9685_Addr_1, 4, i);
//		Delay_ms(100);            
//		setPWM(PCA9685_Addr_1, 5, i);
//		Delay_ms(100);            
//		setPWM(PCA9685_Addr_1, 6, i);
//		Delay_ms(100);            
//		setPWM(PCA9685_Addr_1, 7, i);
//		Delay_ms(100);            
//		setPWM(PCA9685_Addr_1, 8, i);
//		Delay_ms(100);            
//		setPWM(PCA9685_Addr_1, 9, i);
//		Delay_ms(100);
		
    if(Num==20)  
		{	


//					for(i=0;i<16;i++)	{setPWM(PCA9685_Addr_1, i, 4000);}
//					for(i=0;i<16;i++)	{setPWM(PCA9685_Addr_2, i, 500);}
//					for(i=0;i<16;i++)	{setPWM(PCA9685_Addr_3, i, 1000);}
//					for(i=0;i<16;i++)	{setPWM(PCA9685_Addr_4, i, 2000);}
//					for(i=0;i<16;i++)	{setPWM(PCA9685_Addr_5, i, 2500);}
//					for(i=0;i<16;i++)	{setPWM(PCA9685_Addr_6, i, 4000);}
//				for(i=0;i<5;i++)
//				{
//					setPWM(PWM_source[i],PWM_number[i],2000);
//					setPWM(PWM_source[i],PWM_number[i]+1,2000);
//				}
//					setPWM(PWM_source[0],PWM_number[0],3500);
//					setPWM(PWM_source[0],PWM_number[0]+1,3000);
//					setPWM(PWM_source[1],PWM_number[1],2550);
//					setPWM(PWM_source[1],PWM_number[1]+1,2000);
//					setPWM(PWM_source[2],PWM_number[2],1000);
//					setPWM(PWM_source[2],PWM_number[2]+1,2000);
//					setPWM(PWM_source[3],PWM_number[3],2500);
//					setPWM(PWM_source[3],PWM_number[3]+1,3000);
//					setPWM(PWM_source[4],PWM_number[4],3500);
//					setPWM(PWM_source[4],PWM_number[4]+1,4000);
				Move_5_control(); 
//			Move_Coding();                       //对于每个触点，根据当前值计算需要运动的触点名，给每个触点目标霍尔值。核心程序
//			Actual_Num_Get();                    //读取每个触点的当前霍尔值
////			Serial_SendByte(0xBB);
////			Serial_SendByte(Point_Motion[0][0].HallValue_Set);
////			Serial_SendByte(Point_Motion[0][0].HallValue_Actual);
////			Serial_SendByte(0xAA);
////			Motion_Control();                    //根据当前霍尔值和目标霍尔值，驱动每个触点
////			Touch_Point_Control(); 
			Num=0;
			
			
		}
//					Actual_Num_Get(); 
//					singlemove(cmd,lock); 
	}
}


//定时器2中断，以20Hz的频率，读取串口接收的假肢关节传感器数据
void TIM2_IRQHandler(void)  
{
	static uint8_t i=0,j=0;
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
    Get_Sim_Data();
//		AD_change();
		AD_Rearrange();
//		Move_5_control();
//		for(i=0;i<5;i++)
//		{
////			Serial_SendByte(Cmd[i]);
////			Serial_SendByte(Lock[i]);
////			Serial_SendByte(AD_data[i]);
//		}
		for(i=0;i<8;i++)
		{
			for(j=0;j<5;j++)
			{
				 Serial_SendByte(Point_Motion[i][j].HallValue_Actual);				
			}
		}
//		Serial_SendByte(PWM_source[1]);
		Serial_SendByte(0x0D);Serial_SendByte(0x0A);
//		my_getstate();
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
