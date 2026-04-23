#include "myPCA9685.h"
#include "mymux.h"
#include "Pointmove.h"
//需要2个稳定的状态，一个双极性霍尔和两个磁铁或者直接用线性霍尔，但是要有阈值
void Motion_up(uint8_t channel)
{
	if(ADC_MUX_NUM[0]==0)
	{
	  setPWM(PCA9685_Addr_1, channel, 2000);
		setPWM(PCA9685_Addr_1, channel+1, 0);
	}
	else if(ADC_MUX_NUM[0]==1)
	{
		setPWM(PCA9685_Addr_1, channel, 0);
		setPWM(PCA9685_Addr_1, channel+1, 0);
	}
}

void Motion_down(uint8_t channel)
{
	if(ADC_MUX_NUM[0]==1)
	{
	  setPWM(PCA9685_Addr_1, channel, 0);
		setPWM(PCA9685_Addr_1, channel+1, 2000);
	}
	else if(ADC_MUX_NUM[0]==0)
	{
		setPWM(PCA9685_Addr_1, channel, 0);
		setPWM(PCA9685_Addr_1, channel+1, 0);
	}
}
