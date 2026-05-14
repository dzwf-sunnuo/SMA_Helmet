#ifndef __MOVE_CONTROL_H
#define __MOVE_CONTROL_H

/**
  ******************************************************************************
  * @file    Move_Control.h
  * @brief   触觉反馈运动控制模块头文件
  *          定义触点数据结构（当前值/目标值/PWM 通道映射）
  *          实现运动编码（Move_Coding）和驱动执行（Acutation_PCA9685）
  *          数据结构从 SMA 灵巧手项目改造而来：
  *            MAX_JOINTS=19（原为 19 个假肢关节）→ 保留以兼容
  *            MAX_POINTS=10（每个关节最多 10 个触点槽位）
  *            实际使用 8 组 × 5 触点 = 40 个触觉反馈点
  ******************************************************************************
  */

#include "main.h"

#define MAX_JOINTS       19    /* 关节/分组最大数（历史兼容） */
#define MAX_POINTS       10    /* 每组最大触点数 */
#define MAX_GROUPS       8     /* 实际使用的触觉组数 */
#define POINTS_PER_GROUP 5     /* 每组触点数 */

/** 运动编码结构体：用于将关节角度转换为触点目标值 */
typedef struct
{
    float   Data_Set;       /* 上位机下发的目标值 */
    int     Indi_Num;       /* 计算出的中间指标数 */
    int     Part;           /* 分区参数（灵敏度） */
    uint8_t I_length;       /* 本组有效触点数 */
    float   max_HallValue;  /* 霍尔传感器最大值 */
    float   min_HallValue;  /* 霍尔传感器最小值 */
} Motion_Coding_struct;

/** 触点运动控制结构体：每个触点的完整控制信息 */
typedef struct
{
    uint8_t HallValue_Actual;  /* 霍尔传感器当前值（0=回缩, 1=下压） */
    uint8_t HallValue_Set;     /* 目标状态值 */
    uint8_t addr_n;            /* PWM 通道号（挤压侧 SMA） */
    uint8_t PCA9685_n;         /* 所属 PCA9685 的 I2C 地址 */
} Motion_Control_struct;

/* 全局数据 */
extern Motion_Coding_struct  Move_Aim[MAX_JOINTS];
extern Motion_Control_struct Point_Motion[MAX_JOINTS][MAX_POINTS];

/* 运动控制函数 */
void Motion_Init(void);                           /* 数据结构初始化 */
void Move_Coding(void);                           /* 关节角度 → 触点目标值 */
void Actual_Num_Get(void);                        /* MUX 读数 → Point_Motion 当前值 */
void Acutation_PCA9685(uint8_t j, uint8_t i);     /* 驱动单个触点 SMA */
void Motion_Control(void);                        /* 批量驱动全部触点 */

/* 由 singlemove.c 实现，此处声明以供外部调用 */
void PWM_Rearrange_Init(void);                    /* SMA → PWM 通道映射配置 */
void AD_Rearrange(void);                          /* MUX 传感器 → Point_Motion 映射 */

#endif
