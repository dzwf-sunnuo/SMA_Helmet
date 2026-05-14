#ifndef __POINTMOVE_H
#define __POINTMOVE_H

/**
  ******************************************************************************
  * @file    Pointmove.h
  * @brief   单个触点上下运动原子操作头文件
  *          通过霍尔传感器检测当前状态，驱动 SMA 对拉机构
  *          Hall=0 → 回缩状态，Hall=1 → 下压状态
  ******************************************************************************
  */

#include "main.h"

void Motion_up(uint8_t channel);    /* 触点回缩（上抬） */
void Motion_down(uint8_t channel);  /* 触点下压（下扎） */

#endif
