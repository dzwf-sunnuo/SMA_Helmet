#ifndef __MYUSART_H
#define __MYUSART_H
#include <stdio.h>

//extern uint8_t Serial_RxPacket;
extern uint8_t Serial_RxPacket[];
extern uint8_t Serial_RxFlag;
extern float Sim_Data[];	
extern uint8_t Cmd[];
extern uint8_t Lock[];

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);

uint8_t Serial_GetRxFlag(void);
uint8_t Serial_GetRxData(void);

void Get_Sim_Data(void);


#endif
