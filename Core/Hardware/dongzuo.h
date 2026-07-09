#ifndef __DONGZUO_H
#define __DONGZUO_H

#include "main.h"

/* 旋转参数配置 --------------------------------------------------------------*/
#define CAR_TURN_BASE_SPEED     400     /* 旋转基础 PWM 速度（0~1000） */
#define CAR_TURN_TOLERANCE_DEG  1.5f    /* 角度容差（°） */
#define CAR_TURN_TIMEOUT_MS     5000    /* 旋转超时（ms） */

/* 小车旋转动作（比例减速 + MPU6050 陀螺仪闭环）-----------------------------*/
void car_turn_s_45(void);     /* 顺时针旋转 45° */
void car_turn_s_90(void);     /* 顺时针旋转 90° */
void car_turn_s_135(void);    /* 顺时针旋转 135° */
void car_turn_n_45(void);     /* 逆时针旋转 45° */
void car_turn_n_90(void);     /* 逆时针旋转 90° */
void car_turn_n_135(void);    /* 逆时针旋转 135° */
void car_turn_180(void);      /* 旋转 180°（半圈，默认顺时针） */

/* 通用旋转函数 --------------------------------------------------------------*/
void car_turn_to(float target_angle_deg, uint16_t base_speed);

#endif /* __DONGZUO_H */