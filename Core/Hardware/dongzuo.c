#include "dongzuo.h"
#include "motor.h"
#include "mpu6050.h"
#include <math.h>

/* ==========================================================================
 *                           小车原地旋转动作
 *  原理：左右轮差速原地转向
 *    顺时针（CW） = 左轮前进 + 右轮后退
 *    逆时针（CCW）= 左轮后退 + 右轮前进
 *  传感器：MPU6050 Z轴陀螺仪积分 → 当前已转角度
 *  控制：比例衰减 → 剩余角度越小速度越慢，到达容差后停止
 * ========================================================================== */

/**
  * @brief  小车原地旋转到目标角度（比例减速 + 陀螺仪闭环，阻塞模式）
  * @param  target_angle_deg: 目标旋转角度（°）
  *           >0 = 顺时针, <0 = 逆时针
  * @param  base_speed: 基础 PWM 速度（0~1000，建议 300~500）
  * @retval 无
  *
  * 控制流程：
  *   1. 每 10ms 读取 MPU6050 Z 轴角速度
  *   2. 积分得到已转角度（CW 为正方向）
  *   3. 速度 = min(base_speed, Kp × 剩余角度)，角度越小越慢
  *   4. 到达容差范围或超时后停止
  */
void car_turn_to(float target_angle_deg, uint16_t base_speed)
{
    if (fabsf(target_angle_deg) < 0.1f)
        return;

    float   abs_target = fabsf(target_angle_deg);
    int8_t  direction  = (target_angle_deg > 0.0f) ? 1 : -1;

    float    accumulated  = 0.0f;
    uint32_t last_tick    = HAL_GetTick();
    uint32_t start_tick   = last_tick;
    float    kp_slowdown  = 15.0f;  /* 比例减速系数：剩余角度 × kp → 速度 */

    while (1)
    {
        uint32_t now = HAL_GetTick();
        float    dt  = (float)(now - last_tick) / 1000.0f;
        if (dt < 0.001f) dt = 0.001f;
        last_tick = now;

        /* 读取陀螺仪 */
        MPU6050_ReadAll();

        /* 积分角度：MPU6050 Z+ = 逆时针，取负后 CW 为正 */
        accumulated += (-mpu6050.gyro_z_dps) * dt;

        /* 到达目标 → 退出 */
        float remaining = abs_target - fabsf(accumulated);
        if (remaining <= CAR_TURN_TOLERANCE_DEG)
            break;

        /* 超时保护 */
        if (now - start_tick > CAR_TURN_TIMEOUT_MS)
            break;

        /* 比例减速：剩余角度越小速度越低 */
        int16_t speed = (int16_t)(remaining * kp_slowdown);
        if (speed > (int16_t)base_speed)
            speed = (int16_t)base_speed;
        if (speed < 80)
            speed = 80;  /* 最低速度保证转动 */

        /* 差速驱动 */
        if (direction > 0)
        {
            Motor1_SetSpeed(speed);
            Motor2_SetSpeed(-speed);
        }
        else
        {
            Motor1_SetSpeed(-speed);
            Motor2_SetSpeed(speed);
        }

        HAL_Delay(10);
    }

    /* 停止 */
    Motor1_SetSpeed(0);
    Motor2_SetSpeed(0);
}

/* ========================== 顺时针旋转 ==================================== */

void car_turn_s_45(void)  { car_turn_to(45.0f,  CAR_TURN_BASE_SPEED); }
void car_turn_s_90(void)  { car_turn_to(90.0f,  CAR_TURN_BASE_SPEED); }
void car_turn_s_135(void) { car_turn_to(135.0f, CAR_TURN_BASE_SPEED); }

/* ========================== 逆时针旋转 ==================================== */

void car_turn_n_45(void)  { car_turn_to(-45.0f,  CAR_TURN_BASE_SPEED); }
void car_turn_n_90(void)  { car_turn_to(-90.0f,  CAR_TURN_BASE_SPEED); }
void car_turn_n_135(void) { car_turn_to(-135.0f, CAR_TURN_BASE_SPEED); }

/* ========================== 旋转 180° ===================================== */

void car_turn_180(void)   { car_turn_to(180.0f,  CAR_TURN_BASE_SPEED); }