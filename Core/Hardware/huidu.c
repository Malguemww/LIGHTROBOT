#include "huidu.h"
#include "motor.h"

HUIDU_Handle huidu;
HUIDU_Result huidu_result;

/* HAL 中断接收单字节缓冲区 */
static uint8_t huidu_rx_byte;

/* ===== 可调参数 ===== */
#define BASE_SPEED          600       /* 基础速度 */
#define TARGET_POSITION     3.5f      /* 线目标位置（传感器中心） */
#define MIN_SPEED           100       /* 最低速度 */
#define MAX_SPEED           1000      /* 最高速度 */

/* 比例控制 */
#define KP_LINE             150.0f    /* 线偏离比例系数（error × KP → 差速） */
#define MAX_DIFF            500       /* 最大差速 */

/* 陀螺仪辅助 */
#define GYRO_KP             3.0f      /* 陀螺弯道前馈系数 */
#define GYRO_LOSS_THRESH    150.0f    /* 急弯丢线阈值（°/s） */
#define LOSS_TIMEOUT_MS     300       /* 丢线超时（ms） */

/* 权重数组：传感器 0 ~ 7 对应位置权重 0 ~ 7000（单位 0.001） */
static const int32_t weight[HUIDU_CH_NUM] = {
        0,  1000,  2000,  3000,  4000,  5000,  6000,  7000
};

/**
  * @brief  初始化灰度传感器（USART6 中断接收）
  * @retval 无
  */
void HUIDU_Init(void)
{
    huidu.rx_index    = 0;
    huidu.rx_complete = 0;

    huidu_result.position     = TARGET_POSITION;
    huidu_result.error        = 0.0f;
    huidu_result.left_speed   = BASE_SPEED;
    huidu_result.right_speed  = BASE_SPEED;
    huidu_result.loss_start   = 0;

    /* 启动 USART6 中断接收（单字节） */
    HAL_UART_Receive_IT(&HUIDU_UART, &huidu_rx_byte, 1);
}

/**
  * @brief  USART6 中断接收回调（逐字节存入缓冲区）
  * @param  data: 接收到的字节
  * @retval 无
  */
void HUIDU_RxCallback(uint8_t data)
{
    if (huidu.rx_complete)
        return;

    if (huidu.rx_index < HUIDU_RX_BUF_SIZE - 1)
    {
        huidu.data[huidu.rx_index++] = data;
    }

    if (data == 0x0A || data == 0x0D)
    {
        huidu.data[huidu.rx_index] = '\0';
        huidu.rx_complete = 1;
    }
}

/**
  * @brief  解析灰度传感器数据帧
  * @retval 0=成功, 1=无数据
  */
uint8_t HUIDU_Parse(void)
{
    uint8_t i;

    if (!huidu.rx_complete)
        return 1;

    for (i = 0; i < HUIDU_CH_NUM && i < huidu.rx_index; i++)
    {
        huidu_result.sensor[i] = (huidu.data[i] == '1') ? 1 : 0;
    }

    huidu.rx_index    = 0;
    huidu.rx_complete = 0;

    return 0;
}

/**
  * @brief  计算线位置（权重质心法）
  * @retval 0=有数据, 1=丢线
  */
static uint8_t HUIDU_CalcPosition(void)
{
    int32_t sum_weighted = 0;
    int32_t sum_active   = 0;
    uint8_t i;

    for (i = 0; i < HUIDU_CH_NUM; i++)
    {
        if (huidu_result.sensor[i])
        {
            sum_weighted += weight[i];
            sum_active   += 1000;
        }
    }

    if (sum_active == 0)
        return 1;

    huidu_result.position = (float)sum_weighted / (float)sum_active;
    huidu_result.error    = huidu_result.position - TARGET_POSITION;

    return 0;
}

/**
  * @brief  计算并输出左右轮速度
  * @param  diff: 差速值（正=偏右需右轮加速）
  */
static void HUIDU_ApplySpeed(float diff)
{
    int32_t left, right;

    if (diff >  MAX_DIFF) diff =  MAX_DIFF;
    if (diff < -MAX_DIFF) diff = -MAX_DIFF;

    left  = (int32_t)(BASE_SPEED - diff);
    right = (int32_t)(BASE_SPEED + diff);

    if (left  < MIN_SPEED) left  = MIN_SPEED;
    if (right < MIN_SPEED) right = MIN_SPEED;
    if (left  > MAX_SPEED) left  = MAX_SPEED;
    if (right > MAX_SPEED) right = MAX_SPEED;

    huidu_result.left_speed  = (int16_t)left;
    huidu_result.right_speed = (int16_t)right;

    Motor1_SetSpeed(huidu_result.left_speed);
    Motor2_SetSpeed(huidu_result.right_speed);
}

/**
  * @brief  比例 + 陀螺仪融合循迹主循环
  * @note   纯比例控制 + 陀螺仪前馈，无 PID
  *         丢线时用陀螺推测方向，超时停车
  */
void HUIDU_LineFollow(void)
{
    float diff;
    float gyro_z;
    uint32_t now = HAL_GetTick();

    /* 解析传感器 */
    if (HUIDU_Parse() != 0)
        return;

    /* 读取陀螺仪 */
    MPU6050_ReadAll();
    gyro_z = mpu6050.gyro_z_dps;

    /* 计算线位置 */
    if (HUIDU_CalcPosition() == 0)
    {
        /* 有线 → 比例控制 */
        huidu_result.loss_start = 0;

        diff = huidu_result.error * KP_LINE;
        diff += GYRO_KP * gyro_z;  /* 陀螺弯道前馈 */
    }
    else
    {
        /* 丢线 → 陀螺推测 */
        if (huidu_result.loss_start == 0)
        {
            huidu_result.loss_start = now;
        }
        else if (now - huidu_result.loss_start > LOSS_TIMEOUT_MS)
        {
            /* 超时停车 */
            huidu_result.left_speed  = 0;
            huidu_result.right_speed = 0;
            Motor1_SetSpeed(0);
            Motor2_SetSpeed(0);
            return;
        }

        diff = GYRO_KP * 2.0f * gyro_z;
    }

    HUIDU_ApplySpeed(diff);
}