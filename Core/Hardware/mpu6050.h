#ifndef __MPU6050_H
#define __MPU6050_H

#include "main.h"
#include "stm32f4xx_hal.h"

/* MPU6050 接口配置（I2C1: PB6 SCL, PB7 SDA）-----------------------------*/
#define MPU6050_I2C             hi2c1
#define MPU6050_ADDR            0xD0    /* 7位地址 0x68 << 1 */

/* MPU6050 寄存器地址 --------------------------------------------------------*/
#define MPU6050_REG_WHO_AM_I    0x75
#define MPU6050_REG_PWR_MGMT_1  0x6B
#define MPU6050_REG_SMPLRT_DIV  0x19
#define MPU6050_REG_CONFIG      0x1A
#define MPU6050_REG_GYRO_CONFIG 0x1B
#define MPU6050_REG_ACCEL_CONFIG 0x1C
#define MPU6050_REG_ACCEL_XOUT_H 0x3B
#define MPU6050_REG_TEMP_OUT_H   0x41
#define MPU6050_REG_GYRO_XOUT_H  0x43

/* 加速度量程选择 ------------------------------------------------------------*/
typedef enum
{
    ACCEL_RANGE_2G  = 0x00,     /* ±2g,  灵敏度 16384 LSB/g */
    ACCEL_RANGE_4G  = 0x08,     /* ±4g,  灵敏度  8192 LSB/g */
    ACCEL_RANGE_8G  = 0x10,     /* ±8g,  灵敏度  4096 LSB/g */
    ACCEL_RANGE_16G = 0x18      /* ±16g, 灵敏度  2048 LSB/g */
} MPU6050_AccelRange;

/* 陀螺仪量程选择 ------------------------------------------------------------*/
typedef enum
{
    GYRO_RANGE_250  = 0x00,     /* ±250°/s,  灵敏度 131  LSB/(°/s) */
    GYRO_RANGE_500  = 0x08,     /* ±500°/s,  灵敏度 65.5 LSB/(°/s) */
    GYRO_RANGE_1000 = 0x10,     /* ±1000°/s, 灵敏度 32.8 LSB/(°/s) */
    GYRO_RANGE_2000 = 0x18      /* ±2000°/s, 灵敏度 16.4 LSB/(°/s) */
} MPU6050_GyroRange;

/* 传感器数据结构体 ----------------------------------------------------------*/
typedef struct
{
    int16_t accel_x_raw;
    int16_t accel_y_raw;
    int16_t accel_z_raw;
    int16_t gyro_x_raw;
    int16_t gyro_y_raw;
    int16_t gyro_z_raw;
    int16_t temp_raw;

    float accel_x_g;            /* 加速度（g） */
    float accel_y_g;
    float accel_z_g;
    float gyro_x_dps;           /* 角速度（°/s） */
    float gyro_y_dps;
    float gyro_z_dps;
    float temp_c;               /* 温度（°C） */
} MPU6050_Data;

/* 外部变量声明 --------------------------------------------------------------*/
extern MPU6050_Data mpu6050;

/* 公共函数声明 --------------------------------------------------------------*/
uint8_t MPU6050_Init(void);     /* 初始化 MPU6050（I2C1，±2000°/s，±2g） */
uint8_t MPU6050_ReadAll(void);  /* 读取加速度+角速度+温度，换算后存入 mpu6050 全局变量 */

#endif /* __MPU6050_H */