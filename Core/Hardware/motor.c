#include "motor.h"

/* ========================== 私有变量 ====================================== */
static volatile int32_t motor3_position = 0; /* Motor3 当前位置（步） */

/* ========================== 微秒延时 ====================================== */

/**
  * @brief  微秒级阻塞延时（基于 DWT 周期计数器）
  * @param  us: 微秒数
  * @retval 无
  */
static void DWT_Delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000U);
    while ((DWT->CYCCNT - start) < ticks);
}

/* ========================== 初始化 ======================================== */

/**
  * @brief  初始化 3 路电机的 GPIO 方向引脚 + DWT
  * @note   Motor1(TIM2_CH1/PA5), Motor2(TIM3_CH1/PA6), Motor3(PD12/PD11)
  * @retval 无
  */
void Motor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 使能 DWT 用于微秒延时 */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    /* 使能 GPIO 时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /* 配置电机1 方向引脚 (PA7) 为推挽输出 */
    GPIO_InitStruct.Pin   = MOTOR1_DIR_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(MOTOR1_DIR_PORT, &GPIO_InitStruct);

    /* 配置电机2 方向引脚 (PC4) 为推挽输出 */
    GPIO_InitStruct.Pin   = MOTOR2_DIR_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(MOTOR2_DIR_PORT, &GPIO_InitStruct);

    /* 配置电机3 STEP 引脚 (PD12) 为推挽输出 */
    GPIO_InitStruct.Pin   = MOTOR3_STEP_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; /* 脉冲需要高速 */
    HAL_GPIO_Init(MOTOR3_STEP_PORT, &GPIO_InitStruct);

    /* 配置电机3 DIR 引脚 (PD11) 为推挽输出 */
    GPIO_InitStruct.Pin   = MOTOR3_DIR_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(MOTOR3_DIR_PORT, &GPIO_InitStruct);

    /* 初始状态：低电平 */
    HAL_GPIO_WritePin(MOTOR3_STEP_PORT, MOTOR3_STEP_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR3_DIR_PORT, MOTOR3_DIR_PIN, GPIO_PIN_RESET);
}

/* ========================== 电机1 (PWM) =================================== */

/**
  * @brief  设置电机1 的转速和方向（PA5: TIM2_CH1, Dir: PA7）
  * @param  speed: 速度值，正 = 正转，负 = 反转，0 = 停止
  * @retval 无
  */
void Motor1_SetSpeed(int16_t speed)
{
    if (speed > 0)
    {
        HAL_GPIO_WritePin(MOTOR1_DIR_PORT, MOTOR1_DIR_PIN, GPIO_PIN_SET);
        __HAL_TIM_SET_COMPARE(MOTOR1_PWM_TIM, MOTOR1_PWM_CHANNEL, speed);
    }
    else if (speed < 0)
    {
        HAL_GPIO_WritePin(MOTOR1_DIR_PORT, MOTOR1_DIR_PIN, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(MOTOR1_PWM_TIM, MOTOR1_PWM_CHANNEL, -speed);
    }
    else
    {
        __HAL_TIM_SET_COMPARE(MOTOR1_PWM_TIM, MOTOR1_PWM_CHANNEL, 0);
    }
}

/* ========================== 电机2 (PWM) =================================== */

/**
  * @brief  设置电机2 的转速和方向（PA6: TIM3_CH1, Dir: PC4）
  * @param  speed: 速度值，正 = 正转，负 = 反转，0 = 停止
  * @retval 无
  */
void Motor2_SetSpeed(int16_t speed)
{
    if (speed > 0)
    {
        HAL_GPIO_WritePin(MOTOR2_DIR_PORT, MOTOR2_DIR_PIN, GPIO_PIN_SET);
        __HAL_TIM_SET_COMPARE(MOTOR2_PWM_TIM, MOTOR2_PWM_CHANNEL, speed);
    }
    else if (speed < 0)
    {
        HAL_GPIO_WritePin(MOTOR2_DIR_PORT, MOTOR2_DIR_PIN, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(MOTOR2_PWM_TIM, MOTOR2_PWM_CHANNEL, -speed);
    }
    else
    {
        __HAL_TIM_SET_COMPARE(MOTOR2_PWM_TIM, MOTOR2_PWM_CHANNEL, 0);
    }
}

/* ========================== 电机3 (步进脉冲) ============================== */

/**
  * @brief  设置电机3 转速（软件脉冲方式）
  * @param  speed: 速度值，正=正转，负=反转，0=停止
  * @note   此函数仅保留兼容性，步进电机应使用 Motor3_StepMove 系列
  *         持续运行请自行用定时器调用 Motor3_StepPulse(&dir)
  */
void Motor3_SetSpeed(int16_t speed)
{
    (void)speed;
    /* 步进电机不应使用此函数，请使用 Motor3_StepMove / Motor3_MoveDistance */
}

/**
  * @brief  丝杆滑台步进移动（阻塞模式，核心函数）
  * @param  steps: 移动步数，>0 = 正转（下降），<0 = 反转（上升）
  * @param  step_period_us: 脉冲周期（μs），决定速度
  *                         200μs = 5KHz（快），2000μs = 100Hz（慢）
  * @retval 无
  *
  * 换算关系：
  *   steps = mm × STEPS_PER_MM（20 步/mm，全步）
  *   speed(mm/s) = 1e6 / step_period_us / STEPS_PER_MM
  *   例：step_period_us=250 → 速度 = 1e6/250/20 = 200mm/s
  */
void Motor3_StepMove(int32_t steps, uint16_t step_period_us)
{
    if (steps == 0 || step_period_us < STEP_PULSE_WIDTH_US * 2)
        return;

    int8_t dir = (steps > 0) ? 1 : 0;
    uint32_t count = (steps > 0) ? (uint32_t)steps : (uint32_t)(-steps);

    /* 设置方向 */
    if (dir)
        MOTOR3_DIR_PORT->BSRR = MOTOR3_DIR_PIN;               /* DIR = HIGH */
    else
        MOTOR3_DIR_PORT->BSRR = (uint32_t)MOTOR3_DIR_PIN << 16U; /* DIR = LOW */

    /* 方向建立时间（多数驱动器 ≥ 1μs） */
    DWT_Delay_us(2);

    uint32_t half_period = step_period_us / 2;
    uint32_t pulse_high  = STEP_PULSE_WIDTH_US;
    uint32_t pulse_low   = (half_period > pulse_high) ? (half_period - pulse_high) : 1;

    for (uint32_t i = 0; i < count; i++)
    {
        MOTOR3_STEP_PORT->BSRR = MOTOR3_STEP_PIN;               /* STEP = HIGH */
        DWT_Delay_us(pulse_high);
        MOTOR3_STEP_PORT->BSRR = (uint32_t)MOTOR3_STEP_PIN << 16U; /* STEP = LOW */
        DWT_Delay_us(pulse_low);

        /* 更新位置 */
        if (dir)
            motor3_position++;
        else
            motor3_position--;
    }
}

/**
  * @brief  丝杆滑台移动指定距离（阻塞模式）
  * @param  mm: 移动距离，>0 = 下降，<0 = 上升
  * @param  step_period_us: 脉冲周期（μs），决定速度
  * @retval 无
  *
  * 使用示例：
  *   Motor3_MoveDistance(50.0f, 250);   // 下降 50mm，周期 250μs → 200mm/s
  *   Motor3_MoveDistance(-30.0f, 500);  // 上升 30mm，周期 500μs → 100mm/s
  */
void Motor3_MoveDistance(float mm, uint16_t step_period_us)
{
    if (mm == 0.0f || step_period_us < STEP_PULSE_WIDTH_US * 2)
        return;

    /* mm → steps */
    int32_t steps = (int32_t)(mm * MOTOR3_STEPS_PER_MM);

    /* 方向：+mm=下降，-mm=上升，steps 自带正负 */
    Motor3_StepMove(steps, step_period_us);
}

/**
  * @brief  丝杆滑台绝对定位（移动到目标 mm 位置）
  * @param  target_mm: 目标绝对位置（mm），0 = 顶部原点
  * @param  step_period_us: 脉冲周期（μs）
  * @retval 无
  *
  * 注意：需要在开机时调用 Motor3_Home() 建立原点（当前位置 = 0mm）
  */
void Motor3_MoveToMM(float target_mm, uint16_t step_period_us)
{
    float current_mm = Motor3_GetPositionMM();
    float delta_mm = target_mm - current_mm;

    if (delta_mm == 0.0f)
        return;

    Motor3_MoveDistance(delta_mm, step_period_us);
}

/**
  * @brief  丝杆滑台回零（上升，阻塞）
  * @param  step_period_us: 脉冲周期（μs），建议 500~1000
  * @retval 无
  *
  * 原理：假设滑台当前在底部最大行程位置（~100mm），执行上行 100mm 回到顶部
  *       实际使用时建议配合限位开关实现真正的回零
  */
void Motor3_Home(uint16_t step_period_us)
{
    /* 上升 100mm（最大行程）回到顶部 */
    int32_t steps_up = (int32_t)(-100.0f * MOTOR3_STEPS_PER_MM);
    Motor3_StepMove(steps_up, step_period_us);

    /* 重置位置为 0 */
    motor3_position = 0;
}

/**
  * @brief  紧急停止
  * @retval 无
  */
void Motor3_Stop(void)
{
    MOTOR3_STEP_PORT->BSRR = (uint32_t)MOTOR3_STEP_PIN << 16U; /* STEP = LOW */
}

/**
  * @brief  获取当前位置（步）
  * @retval 步数（0 = 顶部原点）
  */
int32_t Motor3_GetPosition(void)
{
    return motor3_position;
}

/**
  * @brief  获取当前位置（mm）
  * @retval 距离顶部原点的毫米数
  */
float Motor3_GetPositionMM(void)
{
    return (float)motor3_position * MOTOR3_MM_PER_STEP;
}