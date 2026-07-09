#include "servo.h"

/**
  * @brief  启动全部 6 路舵机 PWM 输出
  * @note   TIM5 和 TIM9 已由 CubeMX 配置，需确保 Prescaler 使定时器计数频率为 1MHz
  * @retval 无
  */
void Servo_Init(void)
{
    /* 启动 TIM5 四路 PWM */
    HAL_TIM_PWM_Start(SERVO1_TIM, SERVO1_CHANNEL);
    HAL_TIM_PWM_Start(SERVO2_TIM, SERVO2_CHANNEL);
    HAL_TIM_PWM_Start(SERVO3_TIM, SERVO3_CHANNEL);
    HAL_TIM_PWM_Start(SERVO4_TIM, SERVO4_CHANNEL);

    /* 启动 TIM9 两路 PWM */
    HAL_TIM_PWM_Start(SERVO5_TIM, SERVO5_CHANNEL);
    HAL_TIM_PWM_Start(SERVO6_TIM, SERVO6_CHANNEL);
}

/**
  * @brief  角度值转脉宽值（us）
  * @param  angle: 目标角度（0 ~ 180）
  * @retval 脉宽值（500 ~ 2500 us）
  * @note   线性映射：0°=500us, 180°=2500us
  */
static uint32_t Angle_To_Pulse(uint8_t angle)
{
    if (angle > SERVO_ANGLE_MAX)
        angle = SERVO_ANGLE_MAX;
    return SERVO_PULSE_MIN + (uint32_t)angle * (SERVO_PULSE_MAX - SERVO_PULSE_MIN) / SERVO_ANGLE_MAX;
}

/**
  * @brief  设置舵机1 角度（PA0: TIM5_CH1）
  * @param  angle: 目标角度（0 ~ 180）
  * @retval 无
  */
void Servo1_SetAngle(uint8_t angle)
{
    __HAL_TIM_SET_COMPARE(SERVO1_TIM, SERVO1_CHANNEL, Angle_To_Pulse(angle));
}

/**
  * @brief  设置舵机2 角度（PA1: TIM5_CH2）
  * @param  angle: 目标角度（0 ~ 180）
  * @retval 无
  */
void Servo2_SetAngle(uint8_t angle)
{
    __HAL_TIM_SET_COMPARE(SERVO2_TIM, SERVO2_CHANNEL, Angle_To_Pulse(angle));
}

/**
  * @brief  设置舵机3 角度（PA2: TIM5_CH3）
  * @param  angle: 目标角度（0 ~ 180）
  * @retval 无
  */
void Servo3_SetAngle(uint8_t angle)
{
    __HAL_TIM_SET_COMPARE(SERVO3_TIM, SERVO3_CHANNEL, Angle_To_Pulse(angle));
}

/**
  * @brief  设置舵机4 角度（PA3: TIM5_CH4）
  * @param  angle: 目标角度（0 ~ 180）
  * @retval 无
  */
void Servo4_SetAngle(uint8_t angle)
{
    __HAL_TIM_SET_COMPARE(SERVO4_TIM, SERVO4_CHANNEL, Angle_To_Pulse(angle));
}

/**
  * @brief  设置舵机5 角度（PE5: TIM9_CH1）
  * @param  angle: 目标角度（0 ~ 180）
  * @retval 无
  */
void Servo5_SetAngle(uint8_t angle)
{
    __HAL_TIM_SET_COMPARE(SERVO5_TIM, SERVO5_CHANNEL, Angle_To_Pulse(angle));
}

/**
  * @brief  设置舵机6 角度（PE6: TIM9_CH2）
  * @param  angle: 目标角度（0 ~ 180）
  * @retval 无
  */
void Servo6_SetAngle(uint8_t angle)
{
    __HAL_TIM_SET_COMPARE(SERVO6_TIM, SERVO6_CHANNEL, Angle_To_Pulse(angle));
}