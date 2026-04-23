#ifndef __MYMUX_H
#define __MYMUX_H

typedef struct 
	{
		uint8_t Data_1;											
   	uint8_t Data_2; 
		uint8_t Data_3;
	}Mux_Data_struct;
	
extern 	Mux_Data_struct Mux_Data;
void Mux_Init(void);
void Mux_choose(uint8_t chs);
void My_GPIO_Input_Init(void);
extern uint8_t ADC_MUX_NUM[48];
void My_GPIO_Input_Init(void);

	
#endif
