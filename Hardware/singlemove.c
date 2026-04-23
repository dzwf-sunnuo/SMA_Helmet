#include "singlemove.h"                  // Device header
#include "myPCA9685.h"
#include "myusart.h"
#include "mymux.h"
#include "Move_Control.h"

//extern uint8_t move_state=0;
char cmd=0,lock=0;
uint8_t AD_data[40]={0};
uint8_t PWM_source[40]={0};
uint8_t PWM_number[40]={0};
// **向上运动**
void Move_Up(void) 
{
    stopPWM(); // 先停止当前运动
    setPWM(PCA9685_Addr_1, 1, 2000); // 触点向上
	    setPWM(PCA9685_Addr_1, 0, 0); // 触点向上
}

// **向下运动**
void Move_Down(void) 
{
    stopPWM(); // 先停止当前运动
    setPWM(PCA9685_Addr_1, 0, 2000); // 触点向下
	    setPWM(PCA9685_Addr_1, 1, 0); // 触点向下
}


// **停止 PWM 输出**
void stopPWM(void) 
{
  uint8_t i=0;
	for(i=0;i<15;i++)
	{
		setPWM(PCA9685_Addr_1, i, 0); // 停止通道 0-15
		setPWM(PCA9685_Addr_2, i, 0); // 停止通道 0-15
		setPWM(PCA9685_Addr_3, i, 0); // 停止通道 0-15
		setPWM(PCA9685_Addr_4, i, 0); // 停止通道 0-15
		setPWM(PCA9685_Addr_5, i, 0); // 停止通道 0-15
		setPWM(PCA9685_Addr_6, i, 0); // 停止通道 0-15
	}  
}

//传感器与触点的对应,需要实时更新
void AD_change(void)
{
	AD_data[0]=ADC_MUX_NUM[0];//4-1触点
  AD_data[1]=ADC_MUX_NUM[46];//4-2触点	
	AD_data[2]=ADC_MUX_NUM[43];//4-3触点
	AD_data[3]=ADC_MUX_NUM[40];//4-4触点
	AD_data[4]=ADC_MUX_NUM[37];//4-5触点
	
}

//封装40个传感器与触点的对应,需要实时更新
void AD_Rearrange(void)
{
	 Point_Motion[0][0].HallValue_Actual=ADC_MUX_NUM[45];
	 Point_Motion[0][1].HallValue_Actual=ADC_MUX_NUM[42];
	 Point_Motion[0][2].HallValue_Actual=ADC_MUX_NUM[39];
	 Point_Motion[0][3].HallValue_Actual=ADC_MUX_NUM[36];
   Point_Motion[0][4].HallValue_Actual=ADC_MUX_NUM[33];
	 
	 Point_Motion[1][0].HallValue_Actual=ADC_MUX_NUM[30];
	 Point_Motion[1][1].HallValue_Actual=ADC_MUX_NUM[27];
	 Point_Motion[1][2].HallValue_Actual=ADC_MUX_NUM[24];
	 Point_Motion[1][3].HallValue_Actual=ADC_MUX_NUM[21];
	 Point_Motion[1][4].HallValue_Actual=ADC_MUX_NUM[18];

	 Point_Motion[2][0].HallValue_Actual=ADC_MUX_NUM[15];
	 Point_Motion[2][1].HallValue_Actual=ADC_MUX_NUM[12];
	 Point_Motion[2][2].HallValue_Actual=ADC_MUX_NUM[9];
	 Point_Motion[2][3].HallValue_Actual=ADC_MUX_NUM[6];
	 Point_Motion[2][4].HallValue_Actual=ADC_MUX_NUM[3];

	 Point_Motion[3][0].HallValue_Actual=ADC_MUX_NUM[0];
	 Point_Motion[3][1].HallValue_Actual=ADC_MUX_NUM[46];
	 Point_Motion[3][2].HallValue_Actual=ADC_MUX_NUM[43];
	 Point_Motion[3][3].HallValue_Actual=ADC_MUX_NUM[40];
	 Point_Motion[3][4].HallValue_Actual=ADC_MUX_NUM[37];
	 
	 Point_Motion[4][0].HallValue_Actual=ADC_MUX_NUM[34];
	 Point_Motion[4][1].HallValue_Actual=ADC_MUX_NUM[31];
	 Point_Motion[4][2].HallValue_Actual=ADC_MUX_NUM[28];
	 Point_Motion[4][3].HallValue_Actual=ADC_MUX_NUM[25];
	 Point_Motion[4][4].HallValue_Actual=ADC_MUX_NUM[22];

	 Point_Motion[5][0].HallValue_Actual=ADC_MUX_NUM[19];
	 Point_Motion[5][1].HallValue_Actual=ADC_MUX_NUM[16];
	 Point_Motion[5][2].HallValue_Actual=ADC_MUX_NUM[13];
	 Point_Motion[5][3].HallValue_Actual=ADC_MUX_NUM[10];
	 Point_Motion[5][4].HallValue_Actual=ADC_MUX_NUM[7];
	 
	 Point_Motion[6][0].HallValue_Actual=ADC_MUX_NUM[4];
	 Point_Motion[6][1].HallValue_Actual=ADC_MUX_NUM[1];
	 Point_Motion[6][2].HallValue_Actual=ADC_MUX_NUM[47];
	 Point_Motion[6][3].HallValue_Actual=ADC_MUX_NUM[44];
	 Point_Motion[6][4].HallValue_Actual=ADC_MUX_NUM[41];

	 Point_Motion[7][0].HallValue_Actual=ADC_MUX_NUM[38];
	 Point_Motion[7][1].HallValue_Actual=ADC_MUX_NUM[35];
	 Point_Motion[7][2].HallValue_Actual=ADC_MUX_NUM[32];
	 Point_Motion[7][3].HallValue_Actual=ADC_MUX_NUM[29];
	 Point_Motion[7][4].HallValue_Actual=ADC_MUX_NUM[26];
	
}

//PWM驱动与触点的对应，不需要实时更新
void PWM_change_Init(void)
{
	PWM_source[0]=PCA9685_Addr_2;//4-1触点
  PWM_source[1]=PCA9685_Addr_3;//4-2触点	
	PWM_source[2]=PCA9685_Addr_3;//4-3触点
	PWM_source[3]=PCA9685_Addr_3;//4-4触点
	PWM_source[4]=PCA9685_Addr_3;//4-5触点
	PWM_number[0]=14;
	PWM_number[1]=0;
	PWM_number[2]=2;
	PWM_number[3]=4;
	PWM_number[4]=6;
	
}

//PWM驱动与触点的对应，不需要实时更新
//封装了包括SMA驱动 振动电机驱动和引出口的全部对应关系
//以端口来看的封装方法.由于存在反序，端口和序号没有必然联系，因此必须做彻底的封装。
//从第五块板开始 SMA驱动板反接 对应关系全部修改
void PWM_Rearrange_Init(void)
{
	//第一块SMA驱动板，正向接线PWM口1 2个口对应一个SMA
	 Point_Motion[0][0].PCA9685_n=PCA9685_Addr_1;
	 Point_Motion[0][1].PCA9685_n=PCA9685_Addr_1;
	 Point_Motion[0][2].PCA9685_n=PCA9685_Addr_1;
	 Point_Motion[0][3].PCA9685_n=PCA9685_Addr_1;
	 Point_Motion[0][4].PCA9685_n=PCA9685_Addr_1;
	 Point_Motion[0][5].PCA9685_n=PCA9685_Addr_1;
	 Point_Motion[0][6].PCA9685_n=PCA9685_Addr_1;
	 Point_Motion[0][7].PCA9685_n=PCA9685_Addr_1;
	 Point_Motion[0][8].PCA9685_n=PCA9685_Addr_1;
	 Point_Motion[0][9].PCA9685_n=PCA9685_Addr_1;
   Point_Motion[0][0].addr_n=0;
	 Point_Motion[0][1].addr_n=1;
	 Point_Motion[0][2].addr_n=2;
	 Point_Motion[0][3].addr_n=3;
	 Point_Motion[0][4].addr_n=4;
   Point_Motion[0][5].addr_n=5;
	 Point_Motion[0][6].addr_n=6;
	 Point_Motion[0][7].addr_n=7;
	 Point_Motion[0][8].addr_n=8;
	 Point_Motion[0][9].addr_n=9;
	 
	//第二块SMA驱动板，正向接线PWM口1和2
	 Point_Motion[1][0].PCA9685_n=PCA9685_Addr_1;
	 Point_Motion[1][1].PCA9685_n=PCA9685_Addr_1;
	 Point_Motion[1][2].PCA9685_n=PCA9685_Addr_1;
	 Point_Motion[1][3].PCA9685_n=PCA9685_Addr_1;
	 Point_Motion[1][4].PCA9685_n=PCA9685_Addr_1;
	 Point_Motion[1][5].PCA9685_n=PCA9685_Addr_1;	 
	 Point_Motion[1][6].PCA9685_n=PCA9685_Addr_2;
	 Point_Motion[1][7].PCA9685_n=PCA9685_Addr_2;
	 Point_Motion[1][8].PCA9685_n=PCA9685_Addr_2;
	 Point_Motion[1][9].PCA9685_n=PCA9685_Addr_2;
   Point_Motion[1][0].addr_n=10;
	 Point_Motion[1][1].addr_n=11;
	 Point_Motion[1][2].addr_n=12;
	 Point_Motion[1][3].addr_n=13;
	 Point_Motion[1][4].addr_n=14;
   Point_Motion[1][5].addr_n=15;	 
	 Point_Motion[1][6].addr_n=0;
	 Point_Motion[1][7].addr_n=1;
	 Point_Motion[1][8].addr_n=2;
	 Point_Motion[1][9].addr_n=3;

	//第三块SMA驱动板，正向接线PWM口2
	 Point_Motion[2][0].PCA9685_n=PCA9685_Addr_2;
	 Point_Motion[2][1].PCA9685_n=PCA9685_Addr_2;
	 Point_Motion[2][2].PCA9685_n=PCA9685_Addr_2;
	 Point_Motion[2][3].PCA9685_n=PCA9685_Addr_2;
	 Point_Motion[2][4].PCA9685_n=PCA9685_Addr_2;
	 Point_Motion[2][5].PCA9685_n=PCA9685_Addr_2;
	 Point_Motion[2][6].PCA9685_n=PCA9685_Addr_2;
	 Point_Motion[2][7].PCA9685_n=PCA9685_Addr_2;
	 Point_Motion[2][8].PCA9685_n=PCA9685_Addr_2;
	 Point_Motion[2][9].PCA9685_n=PCA9685_Addr_2;
   Point_Motion[2][0].addr_n=4;
	 Point_Motion[2][1].addr_n=5;
	 Point_Motion[2][2].addr_n=6;
	 Point_Motion[2][3].addr_n=7;
	 Point_Motion[2][4].addr_n=8;
   Point_Motion[2][5].addr_n=9;
	 Point_Motion[2][6].addr_n=10;
	 Point_Motion[2][7].addr_n=11;
	 Point_Motion[2][8].addr_n=12;
	 Point_Motion[2][9].addr_n=13;

	//第四块SMA驱动板，正向接线PWM口2和3
	 Point_Motion[3][0].PCA9685_n=PCA9685_Addr_2;
	 Point_Motion[3][1].PCA9685_n=PCA9685_Addr_2;
	 Point_Motion[3][2].PCA9685_n=PCA9685_Addr_3;
	 Point_Motion[3][3].PCA9685_n=PCA9685_Addr_3;
	 Point_Motion[3][4].PCA9685_n=PCA9685_Addr_3;
	 Point_Motion[3][5].PCA9685_n=PCA9685_Addr_3;
	 Point_Motion[3][6].PCA9685_n=PCA9685_Addr_3;
	 Point_Motion[3][7].PCA9685_n=PCA9685_Addr_3;
	 Point_Motion[3][8].PCA9685_n=PCA9685_Addr_3;
	 Point_Motion[3][9].PCA9685_n=PCA9685_Addr_3;
   Point_Motion[3][0].addr_n=14;
	 Point_Motion[3][1].addr_n=15;
	 Point_Motion[3][2].addr_n=0;
	 Point_Motion[3][3].addr_n=1;
	 Point_Motion[3][4].addr_n=2;
   Point_Motion[3][5].addr_n=3;
	 Point_Motion[3][6].addr_n=4;
	 Point_Motion[3][7].addr_n=5;
	 Point_Motion[3][8].addr_n=6;
	 Point_Motion[3][9].addr_n=7;

	//从第五块板开始 SMA驱动板反接 对应关系全部修改
	//第五块SMA驱动板，正向接线PWM口3，反向接线PWM口4
	 Point_Motion[4][0].PCA9685_n=PCA9685_Addr_4;
	 Point_Motion[4][1].PCA9685_n=PCA9685_Addr_4;
	 Point_Motion[4][2].PCA9685_n=PCA9685_Addr_3;
	 Point_Motion[4][3].PCA9685_n=PCA9685_Addr_3;
	 Point_Motion[4][4].PCA9685_n=PCA9685_Addr_3;
	 Point_Motion[4][5].PCA9685_n=PCA9685_Addr_3;
	 Point_Motion[4][6].PCA9685_n=PCA9685_Addr_3;
	 Point_Motion[4][7].PCA9685_n=PCA9685_Addr_3;
	 Point_Motion[4][8].PCA9685_n=PCA9685_Addr_3;
	 Point_Motion[4][9].PCA9685_n=PCA9685_Addr_3;
   Point_Motion[4][0].addr_n=14;
	 Point_Motion[4][1].addr_n=15;
	 Point_Motion[4][2].addr_n=15;
	 Point_Motion[4][3].addr_n=14;
	 Point_Motion[4][4].addr_n=13;
   Point_Motion[4][5].addr_n=12;
	 Point_Motion[4][6].addr_n=11;
	 Point_Motion[4][7].addr_n=10;
	 Point_Motion[4][8].addr_n=9;
	 Point_Motion[4][9].addr_n=8;

	//第六块SMA驱动板，反向接线PWM口4
	 Point_Motion[5][0].PCA9685_n=PCA9685_Addr_4;
	 Point_Motion[5][1].PCA9685_n=PCA9685_Addr_4;
	 Point_Motion[5][2].PCA9685_n=PCA9685_Addr_4;
	 Point_Motion[5][3].PCA9685_n=PCA9685_Addr_4;
	 Point_Motion[5][4].PCA9685_n=PCA9685_Addr_4;
	 Point_Motion[5][5].PCA9685_n=PCA9685_Addr_4;
	 Point_Motion[5][6].PCA9685_n=PCA9685_Addr_4;
	 Point_Motion[5][7].PCA9685_n=PCA9685_Addr_4;
	 Point_Motion[5][8].PCA9685_n=PCA9685_Addr_4;
	 Point_Motion[5][9].PCA9685_n=PCA9685_Addr_4;
   Point_Motion[5][0].addr_n=4;
	 Point_Motion[5][1].addr_n=5;
	 Point_Motion[5][2].addr_n=6;
	 Point_Motion[5][3].addr_n=7;
	 Point_Motion[5][4].addr_n=8;
   Point_Motion[5][5].addr_n=9;
	 Point_Motion[5][6].addr_n=10;
	 Point_Motion[5][7].addr_n=11;
	 Point_Motion[5][8].addr_n=12;
	 Point_Motion[5][9].addr_n=13;

	//第七块SMA驱动板，反向接线PWM口4和5
	 Point_Motion[6][0].PCA9685_n=PCA9685_Addr_5;
	 Point_Motion[6][1].PCA9685_n=PCA9685_Addr_5;
	 Point_Motion[6][2].PCA9685_n=PCA9685_Addr_5;
	 Point_Motion[6][3].PCA9685_n=PCA9685_Addr_5;
	 Point_Motion[6][4].PCA9685_n=PCA9685_Addr_5;
	 Point_Motion[6][5].PCA9685_n=PCA9685_Addr_5;
	 Point_Motion[6][6].PCA9685_n=PCA9685_Addr_4;
	 Point_Motion[6][7].PCA9685_n=PCA9685_Addr_4;
	 Point_Motion[6][8].PCA9685_n=PCA9685_Addr_4;
	 Point_Motion[6][9].PCA9685_n=PCA9685_Addr_4;
   Point_Motion[6][0].addr_n=10;
	 Point_Motion[6][1].addr_n=11;
	 Point_Motion[6][2].addr_n=12;
	 Point_Motion[6][3].addr_n=13;
	 Point_Motion[6][4].addr_n=14;
   Point_Motion[6][5].addr_n=15;
	 Point_Motion[6][6].addr_n=0;
	 Point_Motion[6][7].addr_n=1;
	 Point_Motion[6][8].addr_n=2;
	 Point_Motion[6][9].addr_n=3;

	//第八块SMA驱动板，反向接线PWM口5
	 Point_Motion[7][0].PCA9685_n=PCA9685_Addr_5;
	 Point_Motion[7][1].PCA9685_n=PCA9685_Addr_5;
	 Point_Motion[7][2].PCA9685_n=PCA9685_Addr_5;
	 Point_Motion[7][3].PCA9685_n=PCA9685_Addr_5;
	 Point_Motion[7][4].PCA9685_n=PCA9685_Addr_5;
	 Point_Motion[7][5].PCA9685_n=PCA9685_Addr_5;
	 Point_Motion[7][6].PCA9685_n=PCA9685_Addr_5;
	 Point_Motion[7][7].PCA9685_n=PCA9685_Addr_5;
	 Point_Motion[7][8].PCA9685_n=PCA9685_Addr_5;
	 Point_Motion[7][9].PCA9685_n=PCA9685_Addr_5;
   Point_Motion[7][0].addr_n=0;
	 Point_Motion[7][1].addr_n=1;
	 Point_Motion[7][2].addr_n=2;
	 Point_Motion[7][3].addr_n=3;
	 Point_Motion[7][4].addr_n=4;
   Point_Motion[7][5].addr_n=5;
	 Point_Motion[7][6].addr_n=6;
	 Point_Motion[7][7].addr_n=7;
	 Point_Motion[7][8].addr_n=8;
	 Point_Motion[7][9].addr_n=9;

	//振动电机驱动0-4以及引出5-8，正向接线PWM口6
	 Point_Motion[8][0].PCA9685_n=PCA9685_Addr_6;
	 Point_Motion[8][1].PCA9685_n=PCA9685_Addr_6;
	 Point_Motion[8][2].PCA9685_n=PCA9685_Addr_6;
	 Point_Motion[8][3].PCA9685_n=PCA9685_Addr_6;
	 Point_Motion[8][4].PCA9685_n=PCA9685_Addr_6;
	 Point_Motion[8][5].PCA9685_n=PCA9685_Addr_6;
	 Point_Motion[8][6].PCA9685_n=PCA9685_Addr_6;
	 Point_Motion[8][7].PCA9685_n=PCA9685_Addr_6;
	 Point_Motion[8][8].PCA9685_n=PCA9685_Addr_6;
   Point_Motion[8][0].addr_n=15;
	 Point_Motion[8][1].addr_n=14;
	 Point_Motion[8][2].addr_n=13;
	 Point_Motion[8][3].addr_n=12;
	 Point_Motion[8][4].addr_n=11;
   Point_Motion[8][5].addr_n=0;
	 Point_Motion[8][6].addr_n=1;
	 Point_Motion[8][7].addr_n=2;
	 Point_Motion[8][8].addr_n=3;

	
}


// **单步运动控制（仅使用简单的 if 判断）**
void singlemove(uint8_t cmd,uint8_t lock) {
   if(lock==1){
			if (cmd == GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)) {
					stopPWM();  // **状态相同，不动**
			} 
			else if (cmd > GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)) {
					setPWM(PCA9685_Addr_1, 0, 4000);  // **cmd=1, 传感器=0，向下运动**
					setPWM(PCA9685_Addr_1, 1, 0);  // **cmd=1, 传感器=0，向下运动**
			} 
			else if (cmd < GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)){
					setPWM(PCA9685_Addr_1, 1, 4000);  // **cmd=0, 传感器=1，向上运动**
					setPWM(PCA9685_Addr_1, 0, 0);  // **cmd=1, 传感器=0，向下运动**
			}
	 }
	 else if(lock==0){
		 stopPWM();
	 }
}

// **多步运动控制（仅使用简单的 if 判断）**
void mutilmove(uint8_t cmd,uint8_t target,uint8_t n,uint8_t lock) {
   if(lock==2){
			if (cmd == target) {
					stopPWM();  // **状态相同，不动**
			} 
			else if (cmd >target) {
					setPWM(PCA9685_Addr_1, n, 4000);  // **cmd=1, 传感器=0，向下运动**
					setPWM(PCA9685_Addr_1, n+1, 0);  // **cmd=1, 传感器=0，向下运动**
			} 
			else if (cmd < target){
					setPWM(PCA9685_Addr_1, n+1, 4000);  // **cmd=0, 传感器=1，向上运动**
					setPWM(PCA9685_Addr_1, n, 0);  // **cmd=1, 传感器=0，向下运动**
			}
	 }
	 else if(lock==1){
		 stopPWM();
	 }
}

//// **多步运动控制（仅使用简单的 if 判断）**
//void mutilmove_5(uint8_t cmd,uint8_t target,uint8_t n,uint8_t lock) {
//  cmd=cmd-0x02;
//	lock=lock-0x01;
//	if(lock==0x01){
//			if (cmd == target) {
//					stopPWM();  // **状态相同，不动**
//			} 
//			else if (cmd >target) {
//					setPWM(PCA9685_Addr_1, n, 4000);  // **cmd=1, 传感器=0，向下运动**
//					setPWM(PCA9685_Addr_1, n+1, 0);  // **cmd=1, 传感器=0，向下运动**
//			} 
//			else if (cmd < target){
//					setPWM(PCA9685_Addr_1, n+1, 4000);  // **cmd=0, 传感器=1，向上运动**
//					setPWM(PCA9685_Addr_1, n, 0);  // **cmd=1, 传感器=0，向下运动**
//			}
//	 }
//	 else if(lock==0x00){
//		 stopPWM();
//	 }
//}

//// **多步运动控制（仅使用简单的 if 判断）**
//void mutilmove_5(uint8_t cmd,uint8_t target,uint8_t pwm,uint8_t n,uint8_t lock) {
//  cmd=cmd-0x02;
//	lock=lock-0x01;
//	if(lock==0x01){
//			if (cmd == target) {
////					stopPWM();  // **状态相同，不动**
//				//不能把所有的PWM一起stop 那样会导致无法驱动。应该是把自己停止。
//					setPWM(pwm, n, 0);  
//					setPWM(pwm, n+1, 0); 
//			} 
//			else if (cmd >target) {
//					setPWM(pwm, n, 4000);  // **cmd=1, 传感器=0，向下运动**
//					setPWM(pwm, n+1, 0);  // **cmd=1, 传感器=0，向下运动**
////				Serial_SendByte(0x11);
//			} 
//			else if (cmd < target){
//					setPWM(pwm, n+1, 4000);  // **cmd=0, 传感器=1，向上运动**
//					setPWM(pwm, n, 0);  // **cmd=1, 传感器=0，向下运动**
////					Serial_SendByte(0x00);
//			}
//	 }
//	 else if(lock==0x00){
//					setPWM(pwm, n, 0);  // 自己停止
//					setPWM(pwm, n+1, 0); 
//	 }
//}
// **多步运动控制（仅使用简单的 if 判断），同时针对40触点的情况进行优化**
void mutilmove_40(uint8_t cmd,uint8_t target,uint8_t pwm,uint8_t n1, uint8_t n2,uint8_t lock) {
  cmd=cmd-0x02;
	lock=lock-0x01;
	if(lock==0x01){
			if (cmd == target) {
//					stopPWM();  // **状态相同，不动**
				//不能把所有的PWM一起stop 那样会导致无法驱动。应该是把自己停止。
					setPWM(pwm, n1, 0);  
					setPWM(pwm, n2, 0); 
			} 
			else if (cmd >target) {
					setPWM(pwm, n1, 4000);  // **cmd=1, 传感器=0，向下运动**
					setPWM(pwm, n2, 0);  // **cmd=1, 传感器=0，向下运动**
//				Serial_SendByte(0x11);
			} 
			else if (cmd < target){
					setPWM(pwm, n1, 0);  // **cmd=0, 传感器=1，向上运动**
					setPWM(pwm, n2, 4000);  // **cmd=1, 传感器=0，向下运动**
//					Serial_SendByte(0x00);
			}
	 }
	 else if(lock==0x00){
					setPWM(pwm, n1, 0);  // 自己停止
					setPWM(pwm, n2, 0); 
	 }
}

//把5个数据目标值与当前值作对比，一一进行驱动.一次最多驱动一列5个的数据。
//目前程序已经修改为8*5=40个触点的驱动方法
//需要把Cmd[j]进行修改，具体应该是8*i+j,以及Lock
void Move_5_control(void)
{
	uint8_t i=0,j=0;
//  AD_change();
  AD_Rearrange();
	for(i=0;i<8;i++)
	{
		for(j=0;j<5;j++)
		{
			mutilmove_40(Cmd[5*i+j],Point_Motion[i][j].HallValue_Actual, Point_Motion[i][2*j].PCA9685_n, Point_Motion[i][2*j].addr_n, Point_Motion[i][2*j+1].addr_n, Lock[5*i+j]);
		}
	}
}

