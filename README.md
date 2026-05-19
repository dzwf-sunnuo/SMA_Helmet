# helmet_hal — 触觉反馈头盔固件（HAL 库版）

基于 STM32F103C8Tx + HAL 库的触觉反馈头盔固件，使用 CubeMX 生成框架，CMake + GCC 构建。

## 快速开始

### 环境

- `arm-none-eabi-gcc` 在 PATH 中
- CMake ≥ 3.22
- MinGW Make 或 Ninja（Windows）；make（Linux）

### 构建

```bash
cd helmet_hal
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

产物在 `build/helmet_hal.elf`。

### 烧录

```bash
# ST-Link
arm-none-eabi-objcopy -O binary build/helmet_hal.elf build/helmet_hal.bin
STM32_Programmer_CLI -c port=SWD -w build/helmet_hal.bin 0x08000000

# 或 OpenOCD
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program build/helmet_hal.elf verify reset exit"
```

### CubeMX 重新生成

用 STM32CubeMX 打开 `helmet_hal.ioc`，修改后重新生成。用户代码在 `USER CODE BEGIN/END` 标记内，不会被覆盖。重新生成后需将自定义源文件路径重新加到根 `CMakeLists.txt` 的 `target_sources` 中。

## 硬件连接

| MCU 引脚 | 功能 | 方向 |
|----------|------|------|
| PA9 | USART1_TX → 蓝牙模块 | AF PP |
| PA10 | USART1_RX ← 蓝牙模块 | 上拉输入 |
| PA3 | I2C_SDA → 6× PCA9685 | 开漏输出 |
| PA4 | I2C_SCL → 6× PCA9685 | 开漏输出 |
| PA1 | MUX 地址 S3 | 推挽输出 |
| PA5 | MUX 地址 S0 | 推挽输出 |
| PA6 | MUX 地址 S1 | 推挽输出 |
| PA7 | MUX 地址 S2 | 推挽输出 |
| PB11 | MUX 数据线 1 ← | 上拉输入 |
| PB10 | MUX 数据线 2 ← | 上拉输入 |
| PB1 | MUX 数据线 3 ← | 上拉输入 |
| PA13 | SWDIO | 调试 |
| PA14 | SWCLK | 调试 |

## 项目结构

```
helmet_hal/
├── Core/
│   ├── Inc/                    # 头文件
│   │   ├── main.h              # HAL 框架头文件（引脚宏定义）
│   │   ├── gpio.h / tim.h / usart.h / dma.h / iwdg.h  # CubeMX 生成
│   │   └── Delay.h / I2Csml.h / myPCA9685.h / mymux.h / ...   # 用户模块
│   └── Src/
│       ├── main.c              # 主程序（CubeMX 生成框架 + 用户代码）
│       ├── gpio.c / tim.c / usart.c / dma.c / iwdg.c  # CubeMX 生成
│       ├── stm32f1xx_it.c      # 中断服务函数
│       └── *.c                 # 用户模块（8 个自定义模块）
├── Drivers/                    # HAL 库 & CMSIS
├── cmake/                      # 工具链和 CubeMX 构建脚本
├── CMakeLists.txt              # 顶层 CMake（用户源文件在此注册）
├── helmet_hal.ioc              # CubeMX 项目文件
└── STM32F103XX_FLASH.ld       # 链接脚本
```

## 自定义模块

| 模块 | 文件 | 功能 |
|------|------|------|
| Delay | `Delay.c/h` | DWT 周期计数器微秒延时（不占 SysTick） |
| I2Csml | `I2Csml.c/h` | GPIO 模拟 I2C（PA3/PA4） |
| myPCA9685 | `myPCA9685.c/h` | PCA9685 PWM 驱动（共 6 片） |
| mymux | `mymux.c/h` | CD74HCx4067 多路复用器扫描（48 霍尔传感器） |
| Move_Control | `Move_Control.c/h` | 运动控制数据结构与编码 |
| singlemove | `singlemove.c/h` | SMA 映射表 + 8×5 触点控制循环 |
| Pointmove | `Pointmove.c/h` | 单触点 SMA 上下运动 |
| myusart | `myusart.c/h` | 串口发送 |
| ringbuffer | `ringbuffer.c/h` | DMA+IDLE 环形缓冲区帧接收 |

## 通讯协议

上位机 ↔ 头盔通过蓝牙串口（USART1, 115200 8N1）通讯。

**上位机 → 头盔**：`0xCC` + 40 字节 + `0xFE`

| 指令值 | 含义 |
|--------|------|
| 0x01 | 触点不动 |
| 0x02 | 触点回缩（抬起） |
| 0x03 | 触点下压（挤压） |

**头盔 → 上位机**：40 字节霍尔状态 + `\r\n`（每 50ms 发送一次）

## 中断优先级

| 中断 | 抢占 | 响应 | 频率 |
|------|------|------|------|
| USART1 (IDLE) | 0 | 0 | ~20Hz |
| TIM2 | 1 | 1 | 20Hz |
| TIM3 | 2 | 2 | 20Hz |
| TIM4 (HAL 时基) | — | — | 1kHz |
| SysTick | — | — | 空闲 |

## 其他文档

- `../helmet_og_overview.md` — 原 SPL 版项目完整文档
- `../helmet_hal_cubemx_guide.md` — CubeMX 配置详解

