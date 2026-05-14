#include "Move_Control.h"
#include "myPCA9685.h"
#include "mymux.h"

/**
  ******************************************************************************
  * @file    Move_Control.c
  * @brief   触觉反馈运动控制模块
  *          核心逻辑：
  *            1. Motion_Init()      — 数据结构初始化，加载标定参数
  *            2. Move_Coding()     — 上位机目标角度 → 各触点目标 Hall 值
  *            3. Actual_Num_Get()  — MUX 传感器读数 → 触点当前 Hall 值
  *            4. Acutation_PCA9685() — 目标值 vs 当前值 → PWM 加热决策
  *          SMA 对拉规则：
  *            Hall=0 表示回缩状态，Hall=1 表示下压状态
  *            每个触点一对 SMA 互斥工作（一方加热收缩时另一方冷却放松）
  *            addr_n   对应"挤压"侧 SMA → 加热后 Hall 从 0→1
  *            addr_n+1 对应"回缩"侧 SMA → 加热后 Hall 从 1→0
  ******************************************************************************
  */

Motion_Coding_struct  Move_Aim[MAX_JOINTS];
Motion_Control_struct Point_Motion[MAX_JOINTS][MAX_POINTS];

/* 每组关节的霍尔传感器标定范围（上/下限） */
static const float   max_HallValue[MAX_JOINTS] = {790,790,790,790,790,790,790,790,790,790,790,790,790,790,790,790,790,790,790};
static const float   min_HallValue[MAX_JOINTS] = {550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550};
/* 分区参数（越大越不灵敏） */
static const int     Pre_Part[MAX_JOINTS]      = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
/* 每组有效触点数（实际只使用前 2 组含触点的，其余为 0） */
static const uint8_t I_length[MAX_JOINTS]      = {4,3,1,3,5,5,3,2,3,2,2,3,2,2,3,2,0,0,0};

/**
  * @brief  运动控制数据结构初始化
  *         加载标定参数，清零所有触点当前值和目标值
  */
void Motion_Init(void)
{
    for (int i = 0; i < MAX_JOINTS; i++)
    {
        Move_Aim[i].Data_Set      = 0;
        Move_Aim[i].Indi_Num      = 0;
        Move_Aim[i].Part          = Pre_Part[i];
        Move_Aim[i].max_HallValue = max_HallValue[i];
        Move_Aim[i].min_HallValue = min_HallValue[i];
        Move_Aim[i].I_length      = I_length[i];
    }

    for (int i = 0; i < MAX_JOINTS; i++)
        for (int j = 0; j < MAX_POINTS; j++)
        {
            Point_Motion[i][j].HallValue_Actual = 0;
            Point_Motion[i][j].HallValue_Set    = 0;
        }
}

/**
  * @brief  运动编码：将上位机目标值转换为每个触点的目标 Hall 状态
  *         Move_Aim[j].Data_Set 是上位机下发的关节目标值
  *         计算公式：Indi_Num = (Data_Set - min_HallValue) / Part
  *         前 Indi_Num 个触点设为目标 1（下压），其余设为目标 0（回缩）
  * @note   目前只处理 j=0 和 j=1 两组（即前 2 组触觉反馈）
  */
void Move_Coding(void)
{
    for (int j = 0; j < 2; j++)
    {
        /* 计算需要下压的触点数 */
        Move_Aim[j].Indi_Num = (int)((Move_Aim[j].Data_Set - Move_Aim[j].min_HallValue) / Move_Aim[j].Part);
        for (int i = 0; i < Move_Aim[j].I_length; i++)
        {
            Point_Motion[j][i].HallValue_Set = (i < Move_Aim[j].Indi_Num) ? 1 : 0;
        }
    }
}

/**
  * @brief  将 MUX 传感器原始数据映射到 Point_Motion 结构体
  *         从 ADC_MUX_NUM[48] 中按硬件接线顺序读取
  */
void Actual_Num_Get(void)
{
    Point_Motion[0][0].HallValue_Actual = ADC_MUX_NUM[0];
    Point_Motion[0][1].HallValue_Actual = ADC_MUX_NUM[3];
    Point_Motion[0][2].HallValue_Actual = ADC_MUX_NUM[6];
    Point_Motion[0][3].HallValue_Actual = ADC_MUX_NUM[9];
    Point_Motion[0][4].HallValue_Actual = ADC_MUX_NUM[12];

    Point_Motion[1][0].HallValue_Actual = ADC_MUX_NUM[15];
    Point_Motion[1][1].HallValue_Actual = ADC_MUX_NUM[18];
    Point_Motion[1][2].HallValue_Actual = ADC_MUX_NUM[21];
}

/**
  * @brief  驱动单个触点的一对 SMA
  * @param  j  关节/分组索引
  * @param  i  组内触点索引
  * @note   判决逻辑：
  *          目标==当前 → 两路 PWM 均为 0（保持不动）
  *          目标> 当前 → 加热挤压侧（addr_n），Hall 从 0→1
  *          目标< 当前 → 加热回缩侧（addr_n+1），Hall 从 1→0
  *          PWM=4000 对应满功率加热（12 位分辨率 0~4095）
  */
void Acutation_PCA9685(uint8_t j, uint8_t i)
{
    uint8_t set    = Point_Motion[j][i].HallValue_Set;
    uint8_t actual = Point_Motion[j][i].HallValue_Actual;
    uint8_t addr   = Point_Motion[j][i].PCA9685_n;
    uint8_t ch     = Point_Motion[j][i].addr_n;

    if (set == actual)                      /* 状态一致，保持不动 */
    {
        setPWM(addr, ch,     0);
        setPWM(addr, ch + 1, 0);
    }
    else if (set > actual)                  /* 需要下压：Hall 0→1，加热挤压侧 SMA */
    {
        setPWM(addr, ch,     4000);
        setPWM(addr, ch + 1, 0);
    }
    else                                    /* 需要回缩：Hall 1→0，加热回缩侧 SMA */
    {
        setPWM(addr, ch,     0);
        setPWM(addr, ch + 1, 4000);
    }
}

/**
  * @brief  批量驱动全部触点的 SMA
  *         遍历前 2 组关节的前 5 个触点（共 10 个）
  */
void Motion_Control(void)
{
    for (int l = 0; l < 2; l++)
        for (int m = 0; m < 5; m++)
            Acutation_PCA9685(l, m);
}
