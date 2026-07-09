#ifndef __SERVO_H
#define __SERVO_H

#include "main.h"
#include "stm32f4xx_hal.h"

/* 舵机 PWM 通道定义 ---------------------------------------------------------*/
/* 舵机1: PA0 - TIM5_CH1 */
#define SERVO1_TIM              TIM5
#define SERVO1_CHANNEL          TIM_CHANNEL_1

/* 舵机2: PA1 - TIM5_CH2 */
#define SERVO2_TIM              TIM5
#define SERVO2_CHANNEL          TIM_CHANNEL_2

/* 舵机3: PA2 - TIM5_CH3 */
#define SERVO3_TIM              TIM5
#define SERVO3_CHANNEL          TIM_CHANNEL_3

/* 舵机4: PA3 - TIM5_CH4 */
#define SERVO4_TIM              TIM5
#define SERVO4_CHANNEL          TIM_CHANNEL_4

/* 舵机5: PE5 - TIM9_CH1 */
#define SERVO5_TIM              TIM9
#define SERVO5_CHANNEL          TIM_CHANNEL_1

/* 舵机6: PE6 - TIM9_CH2 */
#define SERVO6_TIM              TIM9
#define SERVO6_CHANNEL          TIM_CHANNEL_2

/* 舵机角度范围定义 ----------------------------------------------------------*/
#define SERVO_ANGLE_MIN         0       /* 最小角度 */
#define SERVO_ANGLE_MAX         180     /* 最大角度 */
#define SERVO_PULSE_MIN         500     /* 最小角度对应脉宽（us） */
#define SERVO_PULSE_MAX         2500    /* 最大角度对应脉宽（us） */

/* 公共函数声明 --------------------------------------------------------------*/
void Servo_Init(void);              /* 初始化 6 路舵机 PWM */
void Servo1_SetAngle(uint8_t angle);/* 设置舵机1角度（0° ~ 180°） */
void Servo2_SetAngle(uint8_t angle);/* 设置舵机2角度（0° ~ 180°） */
void Servo3_SetAngle(uint8_t angle);/* 设置舵机3角度（0° ~ 180°） */
void Servo4_SetAngle(uint8_t angle);/* 设置舵机4角度（0° ~ 180°） */
void Servo5_SetAngle(uint8_t angle);/* 设置舵机5角度（0° ~ 180°） */
void Servo6_SetAngle(uint8_t angle);/* 设置舵机6角度（0° ~ 180°） */

#endif /* __SERVO_H */