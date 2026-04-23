#ifndef __MOVE_CONTROL_H
#define __MOVE_CONTROL_H

#define PWM_Rate 2000;

//****** 定义结构体数据类型 ******//
typedef struct 
	{
		float Data_Set;											// 霍尔值设定数值
   	int Indi_Num; 
		int Part;
		uint8_t I_length;	
		float max_HallValue;   //max value
		float min_HallValue;   //min value
	}Motion_Coding_struct;
	
typedef struct 
	{
		uint8_t HallValue_Actual;											// 霍尔值实际数值
		uint8_t HallValue_Set;											// 霍尔值设定数值
		uint8_t addr_n;
		uint8_t PCA9685_n;
	}Motion_Control_struct;

extern Motion_Coding_struct Move_Aim[19];
extern Motion_Control_struct Point_Motion[19][10];
	
void Motion_Init(void);
void Move_Coding(void);
void Actual_Num_Get(void);
void Acutation_PCA9685(uint8_t j,uint8_t i);
void Motion_Control(void);

#endif
	