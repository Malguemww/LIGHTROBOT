#ifndef __MOTOR_H
#define __MOTOR_H

#include "main.h"
#include "stm32f4xx_hal.h"

/* 电机1 控制引脚（TIM2_CH1: PA5, Dir: PA7）----------------------------------*/
#define MOTOR1_PWM_PORT         GPIOA
#define MOTOR1_PWM_PIN          GPIO_PIN_5
#define MOTOR1_PWM_TIM          TIM2
#define MOTOR1_PWM_CHANNEL      TIM_CHANNEL_1

#define MOTOR1_DIR_PORT         GPIOA
#define MOTOR1_DIR_PIN          GPIO_PIN_7

/* 电机2 控制引脚（TIM3_CH1: PA6, Dir: PC4）----------------------------------*/
#define MOTOR2_PWM_PORT         GPIOA
#define MOTOR2_PWM_PIN          GPIO_PIN_6
#define MOTOR2_PWM_TIM          TIM3
#define MOTOR2_PWM_CHANNEL      TIM_CHANNEL_1

#define MOTOR2_DIR_PORT         GPIOC
#define MOTOR2_DIR_PIN          GPIO_PIN_4

/* 电机3 控制引脚（STEP: PD12, DIR: PD11）-----------------------------------*/
#define MOTOR3_STEP_PORT        GPIOD
#define MOTOR3_STEP_PIN         GPIO_PIN_12

#define MOTOR3_DIR_PORT         GPIOD
#define MOTOR3_DIR_PIN          GPIO_PIN_11

/* PWM 最大速度定义 ----------------------------------------------------------*/
#define MOTOR_MAX_SPEED         1000    /* |speed| 最大值 */

/* 丝杆滑台物理常量（Motor3）--------------------------------------------------*/
#define LEAD_SCREW_PITCH_MM     10.0f       /* 导程 10mm/圈（1610 丝杆） */
#define MOTOR3_STEPS_PER_REV    200         /* 42步进 200步/圈（全步 1.8°） */
#define MOTOR3_MICROSTEP        1           /* 微步细分（1/2/4/8/16，1=全步） */
#define MOTOR3_STEPS_PER_MM     (MOTOR3_STEPS_PER_REV * MOTOR3_MICROSTEP / LEAD_SCREW_PITCH_MM)
                                            /* 步/mm = 200×1/10 = 20 */
#define MOTOR3_MM_PER_STEP      (LEAD_SCREW_PITCH_MM / (MOTOR3_STEPS_PER_REV * MOTOR3_MICROSTEP))
                                            /* mm/步 = 10/(200×1) = 0.05 */

/* 步进脉冲最小宽度（μs），大多数驱动器 ≥ 2.5μs ------------------------------*/
#define STEP_PULSE_WIDTH_US     10

/* 公共函数声明 --------------------------------------------------------------*/
void Motor_Init(void);                  /* 初始化所有电机 GPIO 和 PWM */
void Motor1_SetSpeed(int16_t speed);    /* 设置左轮速度（正=前进，负=后退，±1000） */
void Motor2_SetSpeed(int16_t speed);    /* 设置右轮速度（正=前进，负=后退，±1000） */
void Motor3_SetSpeed(int16_t speed);    /* 设置步进电机方向和脉冲频率 */

/* 丝杆滑台步进距离控制 ------------------------------------------------------*/
void   Motor3_StepMove(int32_t steps, uint16_t step_period_us);   /* 步数移动（阻塞） */
void   Motor3_MoveDistance(float mm, uint16_t step_period_us);    /* 相对距离移动 mm（阻塞） */
void   Motor3_MoveToMM(float target_mm, uint16_t step_period_us); /* 绝对定位到目标 mm */
void   Motor3_Home(uint16_t step_period_us);                       /* 回零（上升 100mm） */
void   Motor3_Stop(void);                                          /* 急停步进电机 */
int32_t Motor3_GetPosition(void);                                  /* 获取当前绝对位置（步） */
float   Motor3_GetPositionMM(void);                                /* 获取当前绝对位置（mm） */

#endif /* __MOTOR_H */