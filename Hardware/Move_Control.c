#include "stm32f10x.h"                  // Device header
#include "Move_Control.h"
#include "myPCA9685.h"
#include "myusart.h"
#include "mymux.h"

Motion_Coding_struct Move_Aim[19];
Motion_Control_struct Point_Motion[19][10];
//初始值
//系统规定状态0触点在上方，状态1触点在下方
float max_HallValue[19] = {790,790,790,790,790,790,790,790,790,790,790,790,790,790,790,790,790,790,790};
float min_HallValue[19] = {550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550};
int   Pre_Part[19]      = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
uint8_t   I_length[19]      = {4,3,1,3,5,5,3,2,3,2,2,3,2,2,3,2,0,0,0};
	
void Motion_Init(void)
{
	uint8_t i,j = 0;
	for(i = 0; i < 19; i++)
	{
     Move_Aim[i].Data_Set = 0;
		 Move_Aim[i].Indi_Num = 0;
		 Move_Aim[i].Part = Pre_Part[i];
 		 Move_Aim[i].max_HallValue = max_HallValue[i];
		 Move_Aim[i].min_HallValue = min_HallValue[i];
		 Move_Aim[i].I_length = I_length[i];
 	}
	
	for(i = 0; i < 19; i++)
	{
		for(j = 0; j < 5; j++)
		{
			Point_Motion[i][j].HallValue_Actual = 0;
      Point_Motion[i][j].HallValue_Set = 0;
		}
 	}
   //PCA9685_1
//   Point_Motion[0][0].addr_n=0;
//	 Point_Motion[0][1].addr_n=2;
//	 Point_Motion[0][2].addr_n=4;
//	 Point_Motion[0][3].addr_n=6;
//	
//	 Point_Motion[0][4].addr_n=8;
//	 Point_Motion[1][0].addr_n=10;
//	 Point_Motion[1][1].addr_n=12;
//	 Point_Motion[1][2].addr_n=14;
//	 Point_Motion[1][1].addr_n=8;
//	 Point_Motion[1][2].addr_n=10;
//	 Point_Motion[1][3].addr_n=12;
//	 Point_Motion[1][4].addr_n=14;
	 
//	 Point_Motion[0][0].PCA9685_n=PCA9685_Addr_1;
//	 Point_Motion[0][1].PCA9685_n=PCA9685_Addr_1;
//	 Point_Motion[0][2].PCA9685_n=PCA9685_Addr_1;
//	 Point_Motion[0][3].PCA9685_n=PCA9685_Addr_1;
//	 Point_Motion[0][4].PCA9685_n=PCA9685_Addr_1;
//	 Point_Motion[1][0].PCA9685_n=PCA9685_Addr_1;
//	 Point_Motion[1][1].PCA9685_n=PCA9685_Addr_1;
//	 Point_Motion[1][2].PCA9685_n=PCA9685_Addr_1;
//	 Point_Motion[1][1].PCA9685_n=PCA9685_Addr_1;
//	 Point_Motion[1][2].PCA9685_n=PCA9685_Addr_1;
//	 Point_Motion[1][3].PCA9685_n=PCA9685_Addr_1;
//	 Point_Motion[1][4].PCA9685_n=PCA9685_Addr_1;
	 
//   //PCA9685_2	
//   Point_Motion[2][0].addr_n=0;
//	 Point_Motion[2][1].addr_n=2;
//	 Point_Motion[2][2].addr_n=4;
//	 Point_Motion[3][0].addr_n=6;
//	 Point_Motion[3][1].addr_n=8;
//	 Point_Motion[3][2].addr_n=10;
//	 Point_Motion[4][0].addr_n=12;
//	 Point_Motion[4][1].addr_n=14;	
//	 
//	 Point_Motion[2][0].PCA9685_n=PCA9685_Addr_2;
//	 Point_Motion[2][1].PCA9685_n=PCA9685_Addr_2;
//	 Point_Motion[2][2].PCA9685_n=PCA9685_Addr_2;
//	 Point_Motion[3][0].PCA9685_n=PCA9685_Addr_2;
//	 Point_Motion[3][1].PCA9685_n=PCA9685_Addr_2;
//	 Point_Motion[3][2].PCA9685_n=PCA9685_Addr_2;
//	 Point_Motion[4][0].PCA9685_n=PCA9685_Addr_2;
//	 Point_Motion[4][1].PCA9685_n=PCA9685_Addr_2;
//	 
//	 //PCA9685_3	
//	 Point_Motion[4][2].addr_n=0;
//	 Point_Motion[4][3].addr_n=2;
//	 Point_Motion[4][4].addr_n=4;
//	 Point_Motion[5][0].addr_n=6;
//	 Point_Motion[5][1].addr_n=8;
//	 Point_Motion[5][2].addr_n=10;
//	 Point_Motion[5][3].addr_n=12;
//	 Point_Motion[5][4].addr_n=14;
//	
//	 Point_Motion[4][2].PCA9685_n=PCA9685_Addr_3;
//	 Point_Motion[4][3].PCA9685_n=PCA9685_Addr_3;
//	 Point_Motion[4][4].PCA9685_n=PCA9685_Addr_3;
//	 Point_Motion[5][0].PCA9685_n=PCA9685_Addr_3;
//	 Point_Motion[5][1].PCA9685_n=PCA9685_Addr_3;
//	 Point_Motion[5][2].PCA9685_n=PCA9685_Addr_3;
//	 Point_Motion[5][3].PCA9685_n=PCA9685_Addr_3;
//	 Point_Motion[5][4].PCA9685_n=PCA9685_Addr_3;	 
//	
//	 //PCA9685_4
//	 Point_Motion[6][0].addr_n=0;
//	 Point_Motion[6][1].addr_n=2;
//	 Point_Motion[6][2].addr_n=4;
//	 Point_Motion[7][0].addr_n=6;
//	 Point_Motion[7][1].addr_n=8;
//	 Point_Motion[8][0].addr_n=10;
//	 Point_Motion[8][1].addr_n=12;
//	 Point_Motion[8][2].addr_n=14;
//	 
//	 Point_Motion[6][0].PCA9685_n=PCA9685_Addr_4;
//	 Point_Motion[6][1].PCA9685_n=PCA9685_Addr_4;
//	 Point_Motion[6][2].PCA9685_n=PCA9685_Addr_4;
//	 Point_Motion[7][0].PCA9685_n=PCA9685_Addr_4;
//	 Point_Motion[7][1].PCA9685_n=PCA9685_Addr_4;
//	 Point_Motion[8][0].PCA9685_n=PCA9685_Addr_4;
//	 Point_Motion[8][1].PCA9685_n=PCA9685_Addr_4;
//	 Point_Motion[8][2].PCA9685_n=PCA9685_Addr_4;	
//	 
//	 //PCA9685_5
//	 Point_Motion[9][0].addr_n=0;
//	 Point_Motion[9][1].addr_n=2;
//	 Point_Motion[10][0].addr_n=4;
//	 Point_Motion[10][1].addr_n=6;
//	 Point_Motion[11][0].addr_n=8;
//	 Point_Motion[11][1].addr_n=10;
//	 Point_Motion[11][2].addr_n=12;
//	 Point_Motion[12][0].addr_n=14;
//	 
//	 Point_Motion[9][0].PCA9685_n=PCA9685_Addr_5;
//	 Point_Motion[9][1].PCA9685_n=PCA9685_Addr_5;
//	 Point_Motion[10][0].PCA9685_n=PCA9685_Addr_5;
//	 Point_Motion[10][1].PCA9685_n=PCA9685_Addr_5;
//	 Point_Motion[11][0].PCA9685_n=PCA9685_Addr_5;
//	 Point_Motion[11][1].PCA9685_n=PCA9685_Addr_5;
//	 Point_Motion[11][2].PCA9685_n=PCA9685_Addr_5;
//	 Point_Motion[12][0].PCA9685_n=PCA9685_Addr_5;	
//	 
//	 //PCA9685_6
//	 Point_Motion[12][1].addr_n=0;
//	 Point_Motion[13][0].addr_n=2;
//	 Point_Motion[13][1].addr_n=4;
//	 Point_Motion[14][0].addr_n=6;
//	 Point_Motion[14][1].addr_n=8;
//	 Point_Motion[14][2].addr_n=10;
//	 Point_Motion[15][0].addr_n=12;
//	 Point_Motion[15][1].addr_n=14;
//	 
//	 Point_Motion[12][1].PCA9685_n=PCA9685_Addr_6;
//	 Point_Motion[13][0].PCA9685_n=PCA9685_Addr_6;
//	 Point_Motion[13][1].PCA9685_n=PCA9685_Addr_6;
//	 Point_Motion[14][0].PCA9685_n=PCA9685_Addr_6;
//	 Point_Motion[14][1].PCA9685_n=PCA9685_Addr_6;
//	 Point_Motion[14][2].PCA9685_n=PCA9685_Addr_6;
//	 Point_Motion[15][0].PCA9685_n=PCA9685_Addr_6;
//	 Point_Motion[15][1].PCA9685_n=PCA9685_Addr_6;
}




//解析代码。把当前值减去最小值，该偏差除以步长，得到指数。该指数每个关节都不一样。

//Move_Aim[j].Data_Set是假肢发过来的关节角度，Move_Aim[j].min_HallValue是假肢关节角度最小值，Move_Aim[j].Part是分区步长，决定分区数
//Move_Aim[j].Indi_Num是分区数，根据当前关节角度与最小角度之差，除以分区步长，决定分区内有几个触点要动

//这里j代表假肢手19个关节的序号，而每个关节都对应几个触点。可能是5个或者3个。这个对应数量就是Move_Aim[j].I_length
//在Move_Aim[j].I_length长度的里面，根据前面计算出来的Move_Aim[j].Indi_Num分区数，决定在长度内部哪几个触点要动

//i遍历关节j对应的所有触点，根据判断条件Move_Aim[j].Indi_Num，确定哪几个触点要运动
//也就是给每个触点Point_Motion[j][i].HallValue_Set运动的目标值

//j遍历全部的关节传感器数据
void Move_Coding(void)
{
	uint8_t i,j;
	
	for(j=0;j<2;j++)
	{
		Move_Aim[j].Indi_Num  = (Move_Aim[j].Data_Set - Move_Aim[j].min_HallValue) / Move_Aim[j].Part; 
		for(i=0;i<Move_Aim[j].I_length;i++)
		{
			if(i<Move_Aim[j].Indi_Num)
			{
				Point_Motion[j][i].HallValue_Set=1;
			}
			else
			{
				Point_Motion[j][i].HallValue_Set=0;
			}
		}
  }
    //	Serial_SendByte(Point_Motion[0][0].HallValue_Set);
}




 //把读取到的数值分别传递给每一个触点的结构体。一共最多48个数
 //每个手指大概有9-10个数。目前只能一个一个赋值。

 //Point_Motion[i][j].HallValue_Actual是每个触点的实际状态，也就是朝下还是朝上，触点的当前值
 //Point_Motion[i][j]，i代表的是哪个关节，j代表的是这个关节对应的几个触点中的哪一个
 //Point_Motion[i][j]，j的最大值是5 一个关节最多对应5个触点。而且不同关节对应的数量不一样

 //ADC_MUX_NUM[k]是霍尔传感器读取的实际值，但是这个传感器数值因为采用多路选择模块读取，每个数值和触点的对应关系比较复杂
 //k与0模3，代表第一个板子读取的数值，k与1模3，代表第二个板子读取的数值，k与2模3，代表第三个板子读取的数值
 //k除3，结果就是哪一个点位，k对3取余，结果就是哪一个传感器板子

 //明确传感器板子读取数据的值就能明确触点当前值。触点目标值是计算出来的，跟物理连接没有关系。
void Actual_Num_Get(void)
{
//	uint8_t i,j;
   Point_Motion[0][0].HallValue_Actual=ADC_MUX_NUM[0];
	 Point_Motion[0][1].HallValue_Actual=ADC_MUX_NUM[3];
	 Point_Motion[0][2].HallValue_Actual=ADC_MUX_NUM[6];
	 Point_Motion[0][3].HallValue_Actual=ADC_MUX_NUM[9];
   Point_Motion[0][4].HallValue_Actual=ADC_MUX_NUM[12];
	 
	 Point_Motion[1][0].HallValue_Actual=ADC_MUX_NUM[15];
	 Point_Motion[1][1].HallValue_Actual=ADC_MUX_NUM[18];
	 Point_Motion[1][2].HallValue_Actual=ADC_MUX_NUM[21];
	 
}


//0和1代表状态，0是收回状态，1是挤压状态
//目标值-当前值=1，目标值为1当前值为0，应该挤压
//当前值-目标值=1，目标值为0当前值为1，应该收回
//addr_n连接挤压的SMA，addr_n+1连接收回的SMA
//挤压发生在当前值为0，也就是0值对应的SMA
//收回发生在当前值为1，也就是1值对应的SMA
//j代表关节名称，i代表对应的触点
//Point_Motion[j][i].PCA9685_n为触点所在的PCA9685板子，Point_Motion[j][i].addr_n为触点连接的PWM端口，这个依赖于物理连接
void Acutation_PCA9685(uint8_t j,uint8_t i)
{
	if(Point_Motion[j][i].HallValue_Set==Point_Motion[j][i].HallValue_Actual)
	{
		setPWM(Point_Motion[j][i].PCA9685_n, Point_Motion[j][i].addr_n, 0); 
	  setPWM(Point_Motion[j][i].PCA9685_n, Point_Motion[j][i].addr_n+1, 0); 
	}
	if(Point_Motion[j][i].HallValue_Set - Point_Motion[j][i].HallValue_Actual==1)
	{
		setPWM(Point_Motion[j][i].PCA9685_n, Point_Motion[j][i].addr_n, 4000); 
	  setPWM(Point_Motion[j][i].PCA9685_n, Point_Motion[j][i].addr_n+1, 0); 
	}
	if(Point_Motion[j][i].HallValue_Actual - Point_Motion[j][i].HallValue_Set==1)
	{
		setPWM(Point_Motion[j][i].PCA9685_n, Point_Motion[j][i].addr_n, 0); 
	  setPWM(Point_Motion[j][i].PCA9685_n, Point_Motion[j][i].addr_n+1, 4000);
	}
}



//遍历上面的单个触点驱动程序，l是关节名称，m是对应触点
//给全部的触点进行驱动。与上面的程序是配套的
void Motion_Control(void)
{
	 uint8_t l,m;
		for(l=0;l<2;l++)
		{
			for(m=0;m<5;m++)
			{
			    Acutation_PCA9685(l,m);
			}
		}
}

