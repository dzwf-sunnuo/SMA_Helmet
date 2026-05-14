/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Delay.h"           /* DWT 微秒延时 */
#include "I2Csml.h"          /* 软件模拟 I2C */
#include "myPCA9685.h"       /* PCA9685 PWM 驱动器 */
#include "mymux.h"           /* CD74HCx4067 多路复用器 */
#include "Move_Control.h"    /* 运动控制数据结构 */
#include "singlemove.h"      /* SMA 运动控制 */
#include "myusart.h"         /* 串口通讯协议 */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint16_t loop_cnt = 0;   /* 主循环计数器，每 20 次循环执行一次触点控制 */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void All_PWM_Off(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
//11111111
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
  /* ---- 用户自定义初始化 ---- */
  Delay_Init();                   /* DWT 延时模块初始化 */
  Motion_Init();                  /* 运动控制数据结构初始化 */
  I2C_INIT();                     /* 软件 I2C 引脚拉高（空闲状态） */
  PCA9685_InitAll(100);           /* 6 片 PCA9685 初始化，PWM 频率 100Hz */

  /* 所有 PWM 通道清零，防止上电瞬间误触发 */
  All_PWM_Off();

  /* 启动 TIM2（20Hz：串口接收处理 + 状态上报） */
  HAL_TIM_Base_Start_IT(&htim2);

  /* 启动 TIM3（20Hz：MUX 传感器扫描） */
  Mux_Start();

  /* 启动串口中断接收（逐字节接收，状态机解析帧） */
  USART_Start_Rx();

  Delay_ms(10);
  Serial_SendByte(0x01);          /* 通知上位机：初始化完成，就绪 */

  /* SMA → PCA9685 通道映射配置 */
  PWM_Rearrange_Init();

  /* 颈动触点抖动测试：确认通讯与控制链路正常 */
  setPWM(PCA9685_ADDR_1, 1, 4000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    loop_cnt++;                                   /* 主循环计数器自增 */
    if (loop_cnt >= 20)                           /* 每 20 次循环执行一次触点控制 */
    {
        Move_5_control();                          /* 8 组 × 5 触点完整控制循环 */
        loop_cnt = 0;
    }
    HAL_IWDG_Refresh(&hiwdg);                     /* 喂狗，防止看门狗复位 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
  * @brief  关闭全部 6 片 PCA9685 的所有 PWM 通道
  *         用于初始化时清零和紧急停止
  */
static void All_PWM_Off(void)
{
    for (int a = 0; a < PCA9685_COUNT; a++)
        for (int ch = 0; ch < 16; ch++)
            setPWM(PCA9685_ADDR[a], ch, 0);
}

/**
  * @brief  HAL 串口接收完成回调
  *         每收到一个字节就交给 myusart 模块的状态机处理
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        UART_RxByte_Handler(rx_byte_buf);  /* 逐字节帧解析 */
    }
}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  if (htim->Instance == TIM2)                    /* TIM2: 20Hz 串口处理与状态上报 */
  {
      Get_Sim_Data();                            /* 解析上位机指令帧 → Cmd/Lock */
      AD_Rearrange();                            /* MUX 数据 → Point_Motion 当前值 */

      /* 发送 40 字节霍尔状态 + \r\n（42 字节/帧） */
      for (int i = 0; i < MAX_GROUPS; i++)
          for (int j = 0; j < POINTS_PER_GROUP; j++)
              Serial_SendByte(Point_Motion[i][j].HallValue_Actual);
      Serial_SendByte(0x0D);                     /* \r */
      Serial_SendByte(0x0A);                     /* \n */
  }
  else if (htim->Instance == TIM3)               /* TIM3: 20Hz MUX 传感器扫描 */
  {
      Mux_Scan();                                /* 16 通道 × 3 线 = 48 个值 */
  }
  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
