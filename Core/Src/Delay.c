#include "Delay.h"

/**
  ******************************************************************************
  * @file    Delay.c
  * @brief   基于 DWT（Data Watchpoint and Trace）硬件周期计数器的延时函数
  *          使用 DWT->CYCCNT 而不占用 SysTick，避免了与 HAL 库时基的冲突
  *          HAL 库时基已配置为 TIM4，SysTick 空闲可供用户使用
  ******************************************************************************
  */

/**
  * @brief  延时模块初始化，使能 DWT 周期计数器
  * @param  无
  * @retval 无
  */
void Delay_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;   /* 使能 DWT 调试模块 */
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;              /* 使能周期计数器 */
    DWT->CYCCNT = 0;                                   /* 清零计数器 */
}

/**
  * @brief  微秒级延时
  * @param  us  延时时长（微秒），在 72MHz 下最大约 59 秒
  * @retval 无
  */
void Delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000); /* CPU频率每微秒的计数值 */
    while ((DWT->CYCCNT - start) < ticks);             /* 等待计数器达到目标值 */
}

/**
  * @brief  毫秒级延时
  * @param  ms  延时时长（毫秒）
  * @retval 无
  */
void Delay_ms(uint32_t ms)
{
    while (ms--)
        Delay_us(1000);
}

/**
  * @brief  秒级延时
  * @param  s  延时时长（秒）
  * @retval 无
  */
void Delay_s(uint32_t s)
{
    while (s--)
        Delay_ms(1000);
}
