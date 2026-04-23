#ifndef __SINGLE_MOVE_H
#define __SINGLE_MOVE_H
#include "stm32f10x.h"                  // Device header
void Move_Up(void);
void Move_Down(void);
void Touch_Point_Control(void);
void stopPWM(void);
void singlemove(uint8_t cmd,uint8_t lock);
void mutilmove(uint8_t cmd,uint8_t target,uint8_t n,uint8_t lock);
//void mutilmove_5(uint8_t cmd,uint8_t target,uint8_t n,uint8_t lock);
void mutilmove_5(uint8_t cmd,uint8_t target,uint8_t pwm,uint8_t n,uint8_t lock);
void mutilmove_40(uint8_t cmd,uint8_t target,uint8_t pwm,uint8_t n1, uint8_t n2,uint8_t lock); 
void Move_5_control(void);
void AD_change(void);
void AD_Rearrange(void);
void PWM_Rearrange_Init(void);
void PWM_change_Init(void);
extern char move_state, cmd,lock;
extern uint8_t AD_data[];
extern uint8_t PWM_source[];
extern uint8_t PWM_number[];

#endif