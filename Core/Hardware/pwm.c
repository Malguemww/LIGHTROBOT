#include "pwm.h"

/**
  * @brief  启动全部 6 路舵机 PWM 输出
  * @note   TIM5 和 TIM9 由 CubeMX 配置（MX_TIM5_Init、MX_TIM9_Init）
  * @retval 无
  */
void PWM_Servo_Init(void)
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
  * @brief  设置舵机1 角度（TIM5_CH1: PA0）
  * @param  pulse: 脉宽值（500 ~ 2500 对应 -90° ~ +90°）
  * @retval 无
  */
void PWM_SetServo1(uint16_t pulse)
{
    __HAL_TIM_SET_COMPARE(SERVO1_TIM, SERVO1_CHANNEL, pulse);
}

/**
  * @brief  设置舵机2 角度（TIM5_CH2: PA1）
  * @note   PA1 与灰度传感器 ADC1_CH1 共用，不可同时使用
  * @param  pulse: 脉宽值（500 ~ 2500 对应 -90° ~ +90°）
  * @retval 无
  */
void PWM_SetServo2(uint16_t pulse)
{
    __HAL_TIM_SET_COMPARE(SERVO2_TIM, SERVO2_CHANNEL, pulse);
}

/**
  * @brief  设置舵机3 角度（TIM5_CH3: PA2）
  * @param  pulse: 脉宽值（500 ~ 2500 对应 -90° ~ +90°）
  * @retval 无
  */
void PWM_SetServo3(uint16_t pulse)
{
    __HAL_TIM_SET_COMPARE(SERVO3_TIM, SERVO3_CHANNEL, pulse);
}

/**
  * @brief  设置舵机4 角度（TIM5_CH4: PA3）
  * @param  pulse: 脉宽值（500 ~ 2500 对应 -90° ~ +90°）
  * @retval 无
  */
void PWM_SetServo4(uint16_t pulse)
{
    __HAL_TIM_SET_COMPARE(SERVO4_TIM, SERVO4_CHANNEL, pulse);
}

/**
  * @brief  设置舵机5 角度（TIM9_CH1: PE5）
  * @param  pulse: 脉宽值（500 ~ 2500 对应 -90° ~ +90°）
  * @retval 无
  */
void PWM_SetServo5(uint16_t pulse)
{
    __HAL_TIM_SET_COMPARE(SERVO5_TIM, SERVO5_CHANNEL, pulse);
}

/**
  * @brief  设置舵机6 角度（TIM9_CH2: PE6）
  * @param  pulse: 脉宽值（500 ~ 2500 对应 -90° ~ +90°）
  * @retval 无
  */
void PWM_SetServo6(uint16_t pulse)
{
    __HAL_TIM_SET_COMPARE(SERVO6_TIM, SERVO6_CHANNEL, pulse);
}