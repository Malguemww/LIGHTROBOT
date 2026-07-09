#include "mpu6050.h"

MPU6050_Data mpu6050;

/* 当前量程灵敏度（由 Init 时设置）-------------------------------------------*/
static float accel_sensitivity = 16384.0f;   /* 默认 ±2g */
static float gyro_sensitivity  = 131.0f;     /* 默认 ±250°/s */

/* I2C 写寄存器 --------------------------------------------------------------*/
static uint8_t MPU6050_WriteReg(uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = {reg, value};
    return HAL_I2C_Master_Transmit(&MPU6050_I2C, MPU6050_ADDR, buf, 2, 10);
}

/* I2C 读多字节 --------------------------------------------------------------*/
static uint8_t MPU6050_ReadMulti(uint8_t reg, uint8_t *buf, uint8_t len)
{
    return HAL_I2C_Master_Transmit(&MPU6050_I2C, MPU6050_ADDR, &reg, 1, 10)
        || HAL_I2C_Master_Receive(&MPU6050_I2C, MPU6050_ADDR, buf, len, 10);
}

/**
  * @brief  初始化 MPU6050（I2C1: PB6/PB7）
  * @retval 0 = 成功, 1 = WHO_AM_I 校验失败
  */
uint8_t MPU6050_Init(void)
{
    uint8_t who_am_i;

    /* 检查 WHO_AM_I（应为 0x68） */
    MPU6050_ReadMulti(MPU6050_REG_WHO_AM_I, &who_am_i, 1);
    if (who_am_i != 0x68)
        return 1;

    /* 唤醒 MPU6050，时钟源 = 内部 8MHz */
    MPU6050_WriteReg(MPU6050_REG_PWR_MGMT_1, 0x00);
    HAL_Delay(50);

    /* 采样率分频：1kHz / (1 + 4) = 200Hz */
    MPU6050_WriteReg(MPU6050_REG_SMPLRT_DIV, 0x04);

    /* 数字低通滤波器：DLPF = 3（陀螺 41Hz, 加速度 44Hz） */
    MPU6050_WriteReg(MPU6050_REG_CONFIG, 0x03);

    /* 加速度计量程：±2g */
    MPU6050_WriteReg(MPU6050_REG_ACCEL_CONFIG, ACCEL_RANGE_2G);
    accel_sensitivity = 16384.0f;

    /* 陀螺仪量程：±250°/s */
    MPU6050_WriteReg(MPU6050_REG_GYRO_CONFIG, GYRO_RANGE_250);
    gyro_sensitivity = 131.0f;

    return 0;
}

/**
  * @brief  读取加速度 + 陀螺仪 + 温度，换算为物理单位
  * @retval 0 = 成功
  * @note   数据存入全局变量 mpu6050
  */
uint8_t MPU6050_ReadAll(void)
{
    uint8_t raw[14];

    if (MPU6050_ReadMulti(MPU6050_REG_ACCEL_XOUT_H, raw, 14) != HAL_OK)
        return 1;

    /* 加速度原始值（高字节在前） */
    mpu6050.accel_x_raw = (int16_t)((raw[0] << 8) | raw[1]);
    mpu6050.accel_y_raw = (int16_t)((raw[2] << 8) | raw[3]);
    mpu6050.accel_z_raw = (int16_t)((raw[4] << 8) | raw[5]);

    /* 温度原始值 */
    mpu6050.temp_raw = (int16_t)((raw[6] << 8) | raw[7]);

    /* 陀螺仪原始值（高字节在前） */
    mpu6050.gyro_x_raw  = (int16_t)((raw[8]  << 8) | raw[9]);
    mpu6050.gyro_y_raw  = (int16_t)((raw[10] << 8) | raw[11]);
    mpu6050.gyro_z_raw  = (int16_t)((raw[12] << 8) | raw[13]);

    /* 换算为物理单位 */
    mpu6050.accel_x_g  = (float)mpu6050.accel_x_raw / accel_sensitivity;
    mpu6050.accel_y_g  = (float)mpu6050.accel_y_raw / accel_sensitivity;
    mpu6050.accel_z_g  = (float)mpu6050.accel_z_raw / accel_sensitivity;

    mpu6050.gyro_x_dps = (float)mpu6050.gyro_x_raw  / gyro_sensitivity;
    mpu6050.gyro_y_dps = (float)mpu6050.gyro_y_raw  / gyro_sensitivity;
    mpu6050.gyro_z_dps = (float)mpu6050.gyro_z_raw  / gyro_sensitivity;

    /* 温度公式：T(°C) = TEMP_OUT / 340 + 36.53 */
    mpu6050.temp_c = (float)mpu6050.temp_raw / 340.0f + 36.53f;

    return 0;
}