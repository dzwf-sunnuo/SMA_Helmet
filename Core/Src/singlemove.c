#include "singlemove.h"
#include "myPCA9685.h"
#include "mymux.h"
#include "Move_Control.h"

/**
  ******************************************************************************
  * @file    singlemove.c
  * @brief   SMA 单点/多点运动控制
  *
  *          ── SMA → PCA9685 通道映射（PWM_Rearrange_Init）──
  *          8 组触觉 + 1 组颈动 = 9 组 SMA，共 89 个 SMA 单元
  *          每个 SMA 单元通过查表确定所属 PCA9685 芯片和 PWM 通道
  *          表结构：{ 组索引, 组内序号, PCA9685 芯片索引, 通道号 }
  *
  *          ── 运动判决逻辑（mutilmove_40）──
  *          上位机指令偏移：cmd = cmd - 0x02
  *              0x01 → lock=0x00 → 停止（不动）
  *              0x02 → cmd=0x00 → 目标 Hall=0（回缩/抬起）
  *              0x03 → cmd=0x01 → 目标 Hall=1（下压/挤压）
  *          比较目标值与当前 Hall 状态，决定加热哪一侧 SMA
  *
  *          ── 主控制循环（Move_5_control）──
  *          8 组 × 5 触点 = 40 个触觉反馈点
  *          每个触点使用相邻 2 路 PWM：[2j]=挤压, [2j+1]=回缩
  ******************************************************************************
  */

uint8_t Cmd[40];     /* 上位机指令（0x01=不动, 0x02=回缩, 0x03=下压） */
uint8_t Lock[40];    /* 运动锁（0x01=锁止不动, 0x02=允许运动） */

/* ─── SMA 物理接线 → PCA9685 PWM 通道映射表 ─── */
typedef struct {
    uint8_t group;  /* Point_Motion 第一维索引（分组） */
    uint8_t idx;    /* Point_Motion 第二维索引（组内序号） */
    uint8_t pca;    /* PCA9685_ADDR[] 数组索引（0~5） */
    uint8_t ch;     /* PWM 通道号（addr_n，即挤压侧 SMA 所在通道） */
} sma_map_t;

static const sma_map_t sma_map[] = {
    /* 第一块 SMA — PCA9685_1，通道 0~9 */
    {0,0, 0,0},{0,1, 0,1},{0,2, 0,2},{0,3, 0,3},{0,4, 0,4},
    {0,5, 0,5},{0,6, 0,6},{0,7, 0,7},{0,8, 0,8},{0,9, 0,9},
    /* 第二块 SMA — PCA9685_1 通道 10~15，PCA9685_2 通道 0~3 */
    {1,0, 0,10},{1,1, 0,11},{1,2, 0,12},{1,3, 0,13},{1,4, 0,14},
    {1,5, 0,15},{1,6, 1,0}, {1,7, 1,1}, {1,8, 1,2}, {1,9, 1,3},
    /* 第三块 SMA — PCA9685_2 通道 4~13 */
    {2,0, 1,4},{2,1, 1,5},{2,2, 1,6},{2,3, 1,7},{2,4, 1,8},
    {2,5, 1,9},{2,6, 1,10},{2,7, 1,11},{2,8, 1,12},{2,9, 1,13},
    /* 第四块 SMA — PCA9685_2 通道 14~15，PCA9685_3 通道 0~7 */
    {3,0, 1,14},{3,1, 1,15},{3,2, 2,0},{3,3, 2,1},{3,4, 2,2},
    {3,5, 2,3},{3,6, 2,4},{3,7, 2,5},{3,8, 2,6},{3,9, 2,7},
    /* 第五块 SMA — PCA9685_4 通道 14~15，PCA9685_3 通道 8~15（反转：集体反了） */
    {4,0, 3,14},{4,1, 3,15},{4,2, 2,15},{4,3, 2,14},{4,4, 2,13},
    {4,5, 2,12},{4,6, 2,11},{4,7, 2,10},{4,8, 2,9}, {4,9, 2,8},
    /* 第六块 SMA — PCA9685_4 通道 4~13 */
    {5,0, 3,4},{5,1, 3,5},{5,2, 3,6},{5,3, 3,7},{5,4, 3,8},
    {5,5, 3,9},{5,6, 3,10},{5,7, 3,11},{5,8, 3,12},{5,9, 3,13},
    /* 第七块 SMA — PCA9685_5 通道 10~15，PCA9685_4 通道 0~3 */
    {6,0, 4,10},{6,1, 4,11},{6,2, 4,12},{6,3, 4,13},{6,4, 4,14},
    {6,5, 4,15},{6,6, 3,0}, {6,7, 3,1}, {6,8, 3,2}, {6,9, 3,3},
    /* 第八块 SMA — PCA9685_5 通道 0~9 */
    {7,0, 4,0},{7,1, 4,1},{7,2, 4,2},{7,3, 4,3},{7,4, 4,4},
    {7,5, 4,5},{7,6, 4,6},{7,7, 4,7},{7,8, 4,8},{7,9, 4,9},
    /* 颈动控制 — PCA9685_6（通道号递减 + 跳跃） */
    {8,0, 5,15},{8,1, 5,14},{8,2, 5,13},{8,3, 5,12},{8,4, 5,11},
    {8,5, 5,0}, {8,6, 5,1}, {8,7, 5,2}, {8,8, 5,3},
};
#define SMA_MAP_COUNT (sizeof(sma_map) / sizeof(sma_map[0]))

/**
  * @brief  PWM 通道重排初始化：将 SMA 物理接线映射到 Point_Motion 结构体
  *         查表填充每个触点的 PCA9685 地址和 PWM 通道号
  *         替代原代码 200+ 行手动赋值，修改配置只需改上表
  */
void PWM_Rearrange_Init(void)
{
    for (uint32_t k = 0; k < SMA_MAP_COUNT; k++)
    {
        uint8_t g = sma_map[k].group;
        uint8_t i = sma_map[k].idx;
        Point_Motion[g][i].PCA9685_n = PCA9685_ADDR[sma_map[k].pca];
        Point_Motion[g][i].addr_n     = sma_map[k].ch;
    }
}

/* ─── MUX 传感器 → Point_Motion 映射（硬件接线顺序，已验证） ─── */
/**
  * @brief  将 ADC_MUX_NUM[48] 中的传感器读数按接线顺序填入 Point_Motion
  *          8 组 × 5 触点 = 40 个值
  *          MUX 通道索引由 0 到 47，每 3 个一组（三路数据线同时采样）
  */
void AD_Rearrange(void)
{
    Point_Motion[0][0].HallValue_Actual = ADC_MUX_NUM[45];
    Point_Motion[0][1].HallValue_Actual = ADC_MUX_NUM[42];
    Point_Motion[0][2].HallValue_Actual = ADC_MUX_NUM[39];
    Point_Motion[0][3].HallValue_Actual = ADC_MUX_NUM[36];
    Point_Motion[0][4].HallValue_Actual = ADC_MUX_NUM[33];

    Point_Motion[1][0].HallValue_Actual = ADC_MUX_NUM[30];
    Point_Motion[1][1].HallValue_Actual = ADC_MUX_NUM[27];
    Point_Motion[1][2].HallValue_Actual = ADC_MUX_NUM[24];
    Point_Motion[1][3].HallValue_Actual = ADC_MUX_NUM[21];
    Point_Motion[1][4].HallValue_Actual = ADC_MUX_NUM[18];

    Point_Motion[2][0].HallValue_Actual = ADC_MUX_NUM[15];
    Point_Motion[2][1].HallValue_Actual = ADC_MUX_NUM[12];
    Point_Motion[2][2].HallValue_Actual = ADC_MUX_NUM[9];
    Point_Motion[2][3].HallValue_Actual = ADC_MUX_NUM[6];
    Point_Motion[2][4].HallValue_Actual = ADC_MUX_NUM[3];

    Point_Motion[3][0].HallValue_Actual = ADC_MUX_NUM[0];
    Point_Motion[3][1].HallValue_Actual = ADC_MUX_NUM[46];
    Point_Motion[3][2].HallValue_Actual = ADC_MUX_NUM[43];
    Point_Motion[3][3].HallValue_Actual = ADC_MUX_NUM[40];
    Point_Motion[3][4].HallValue_Actual = ADC_MUX_NUM[37];

    Point_Motion[4][0].HallValue_Actual = ADC_MUX_NUM[34];
    Point_Motion[4][1].HallValue_Actual = ADC_MUX_NUM[31];
    Point_Motion[4][2].HallValue_Actual = ADC_MUX_NUM[28];
    Point_Motion[4][3].HallValue_Actual = ADC_MUX_NUM[25];
    Point_Motion[4][4].HallValue_Actual = ADC_MUX_NUM[22];

    Point_Motion[5][0].HallValue_Actual = ADC_MUX_NUM[19];
    Point_Motion[5][1].HallValue_Actual = ADC_MUX_NUM[16];
    Point_Motion[5][2].HallValue_Actual = ADC_MUX_NUM[13];
    Point_Motion[5][3].HallValue_Actual = ADC_MUX_NUM[10];
    Point_Motion[5][4].HallValue_Actual = ADC_MUX_NUM[7];

    Point_Motion[6][0].HallValue_Actual = ADC_MUX_NUM[4];
    Point_Motion[6][1].HallValue_Actual = ADC_MUX_NUM[1];
    Point_Motion[6][2].HallValue_Actual = ADC_MUX_NUM[47];
    Point_Motion[6][3].HallValue_Actual = ADC_MUX_NUM[44];
    Point_Motion[6][4].HallValue_Actual = ADC_MUX_NUM[41];

    Point_Motion[7][0].HallValue_Actual = ADC_MUX_NUM[38];
    Point_Motion[7][1].HallValue_Actual = ADC_MUX_NUM[35];
    Point_Motion[7][2].HallValue_Actual = ADC_MUX_NUM[32];
    Point_Motion[7][3].HallValue_Actual = ADC_MUX_NUM[29];
    Point_Motion[7][4].HallValue_Actual = ADC_MUX_NUM[26];
}

/* ─── 停止全部 6 片 PCA9685 的所有 PWM 输出 ─── */
/**
  * @brief  紧急停止：所有 PWM 通道输出 0，全部 SMA 停止加热
  */
void stopPWM(void)
{
    for (int a = 0; a < PCA9685_COUNT; a++)
        for (int ch = 0; ch < 16; ch++)
            setPWM(PCA9685_ADDR[a], ch, 0);
}

/* ─── 单个 SMA 触点运动控制（调试用） ─── */
void singlemove(uint8_t cmd, uint8_t lock)
{
    if (lock == 1)
    {
        uint8_t state = HAL_GPIO_ReadPin(MUX_DATA1_GPIO_Port, MUX_DATA1_Pin);
        if (cmd == state)                           /* 状态相同，不动 */
            stopPWM();
        else if (cmd > state)                       /* cmd=1, Hall=0 → 下压 */
        {
            setPWM(PCA9685_ADDR_1, 0, 4000);
            setPWM(PCA9685_ADDR_1, 1, 0);
        }
        else                                        /* cmd=0, Hall=1 → 回缩 */
        {
            setPWM(PCA9685_ADDR_1, 1, 4000);
            setPWM(PCA9685_ADDR_1, 0, 0);
        }
    }
    else
        stopPWM();
}

/* ─── 多点运动控制（固定使用 PCA9685_1） ─── */
void mutilmove(uint8_t cmd, uint8_t target, uint8_t n, uint8_t lock)
{
    if (lock == 2)
    {
        if (cmd == target)
            stopPWM();
        else if (cmd > target)
        {
            setPWM(PCA9685_ADDR_1, n,     4000);
            setPWM(PCA9685_ADDR_1, n + 1, 0);
        }
        else
        {
            setPWM(PCA9685_ADDR_1, n + 1, 4000);
            setPWM(PCA9685_ADDR_1, n,     0);
        }
    }
    else
        stopPWM();
}

/* ─── 40 触点通用运动控制（可指定任意 PCA9685 芯片和通道对） ─── */
/**
  * @brief  单触点运动判决与执行（通用版，支持 40 点同时控制）
  * @param  cmd    上位机指令（0x01=不动, 0x02=回缩, 0x03=下压）
  * @param  target 当前霍尔传感器实际值（0 或 1）
  * @param  pwm    所属 PCA9685 的 I2C 地址
  * @param  n1     挤压侧 SMA 的 PWM 通道号（addr_n）
  * @param  n2     回缩侧 SMA 的 PWM 通道号（addr_n+1）
  * @param  lock   运动锁（0x01=锁止, 0x02=允许运动）
  * @note   内部偏移：cmd=cmd-0x02, lock=lock-0x01
  *          判决表：
  *            lock=0x00（原 0x01）→ 两路 PWM=0，触点不动
  *            lock=0x01（原 0x02）→ 比较 cmd 与 target
  *              cmd=0x00（原 0x02，目标回缩） vs target
  *              cmd=0x01（原 0x03，目标下压） vs target
  */
void mutilmove_40(uint8_t cmd, uint8_t target, uint8_t pwm, uint8_t n1, uint8_t n2, uint8_t lock)
{
    cmd  = cmd  - 0x02;     /* 0x02→0x00（回缩），0x03→0x01（下压） */
    lock = lock - 0x01;     /* 0x01→0x00（锁止），0x02→0x01（解锁） */

    if (lock == 0x01)                           /* 允许运动 */
    {
        if (cmd == target)                      /* 已是目标状态，停止 */
        {
            setPWM(pwm, n1, 0);
            setPWM(pwm, n2, 0);
        }
        else if (cmd > target)                  /* 需下压：加热挤压侧 SMA */
        {
            setPWM(pwm, n1, 4000);
            setPWM(pwm, n2, 0);
        }
        else                                    /* 需回缩：加热回缩侧 SMA */
        {
            setPWM(pwm, n1, 0);
            setPWM(pwm, n2, 4000);
        }
    }
    else                                        /* 锁止状态，停止 */
    {
        setPWM(pwm, n1, 0);
        setPWM(pwm, n2, 0);
    }
}

/* ─── 40 触点主控制循环 ─── */
/**
  * @brief  8 组 × 5 触点 = 40 触点完整控制循环
  *         先调用 AD_Rearrange 更新传感器当前值
  *         然后逐触点比较 Cmd 目标与 Hall 当前值，驱动 SMA
  *         i 为组索引（0~7），j 为组内触点索引（0~4）
  *         每个触点占用相邻 2 路 PWM：[2j]=挤压侧, [2j+1]=回缩侧
  */
void Move_5_control(void)
{
    AD_Rearrange();
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            mutilmove_40(Cmd[5 * i + j],
                         Point_Motion[i][j].HallValue_Actual,
                         Point_Motion[i][2 * j].PCA9685_n,
                         Point_Motion[i][2 * j].addr_n,
                         Point_Motion[i][2 * j + 1].addr_n,
                         Lock[5 * i + j]);
        }
    }
}
