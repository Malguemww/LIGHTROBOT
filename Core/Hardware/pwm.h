#ifndef __PWM_H
#define __PWM_H

#include "main.h"
#include "stm32f4xx_hal.h"

/* 6路舵机 PWM 通道定义 ------------------------------------------------------*/
/* TIM5 CH1~CH4 + TIM9 CH1~CH2，均由 CubeMX 配置 */

/* 舵机1: TIM5_CH1 (PA0) */
#define SERVO1_TIM              TIM5
#define SERVO1_CHANNEL          TIM_CHANNEL_1

/* 舵机2: TIM5_CH2 (PA1) — 与灰度传感器 ADC1_CH1 共用 PA1，不可同时使用 */
#define SERVO2_TIM              TIM5
#define SERVO2_CHANNEL          TIM_CHANNEL_2

/* 舵机3: TIM5_CH3 (PA2) */
#define SERVO3_TIM              TIM5
#define SERVO3_CHANNEL          TIM_CHANNEL_3

/* 舵机4: TIM5_CH4 (PA3) */
#define SERVO4_TIM              TIM5
#define SERVO4_CHANNEL          TIM_CHANNEL_4

/* 舵机5: TIM9_CH1 (PE5) */
#define SERVO5_TIM              TIM9
#define SERVO5_CHANNEL          TIM_CHANNEL_1

/* 舵机6: TIM9_CH2 (PE6) */
#define SERVO6_TIM              TIM9
#define SERVO6_CHANNEL          TIM_CHANNEL_2

/* 公共函数声明 --------------------------------------------------------------*/
void PWM_Servo_Init(void);            /* 初始化 6 路舵机 PWM 定时器 */
void PWM_SetServo1(uint16_t pulse);   /* 设置舵机1脉宽（500us ~ 2500us） */
void PWM_SetServo2(uint16_t pulse);   /* 设置舵机2脉宽（500us ~ 2500us） */
void PWM_SetServo3(uint16_t pulse);   /* 设置舵机3脉宽（500us ~ 2500us） */
void PWM_SetServo4(uint16_t pulse);   /* 设置舵机4脉宽（500us ~ 2500us） */
void PWM_SetServo5(uint16_t pulse);   /* 设置舵机5脉宽（500us ~ 2500us） */
void PWM_SetServo6(uint16_t pulse);   /* 设置舵机6脉宽（500us ~ 2500us） */

#endif /* __PWM_H */