#ifndef __HUIDU_H
#define __HUIDU_H

#include "main.h"
#include "stm32f4xx_hal.h"
#include "mpu6050.h"

/* 灰度传感器串口（USART6: PC6_TX, PC7_RX）-----------------------------*/
#define HUIDU_UART              huart6
#define HUIDU_RX_BUF_SIZE       64
#define HUIDU_CH_NUM            8           /* 8 路灰度传感器 */

/* 权重算法输出结构体 --------------------------------------------------------*/
typedef struct
{
    float   position;          /* 线位置（0.0 ~ 7.0），中心 = 3.5 */
    float   error;             /* 位置偏差（-3.5 ~ +3.5），0 = 居中 */
    int16_t left_speed;        /* 左轮目标速度 */
    int16_t right_speed;       /* 右轮目标速度 */
    uint8_t sensor[HUIDU_CH_NUM];  /* 原始灰度值（0=黑, 1=白） */
    uint32_t loss_start;       /* 丢线开始时刻（ms），0=未丢线 */
} HUIDU_Result;

/* 接收缓冲区结构体 ----------------------------------------------------------*/
typedef struct
{
    uint8_t data[HUIDU_RX_BUF_SIZE];    /* 接收缓冲区 */
    uint8_t rx_index;                   /* 接收索引 */
    uint8_t rx_complete;                /* 接收完成标志 */
} HUIDU_Handle;

/* 外部变量声明 --------------------------------------------------------------*/
extern HUIDU_Handle huidu;
extern HUIDU_Result huidu_result;

/* 公共函数声明 --------------------------------------------------------------*/
void    HUIDU_Init(void);              /* 初始化灰度传感器串口（USART6） */
void    HUIDU_RxCallback(uint8_t data);/* 串口接收回调（放入缓冲区） */
uint8_t HUIDU_Parse(void);             /* 解析一帧灰度数据并存入 sensor[] */
void    HUIDU_LineFollow(void);        /* 比例 + 陀螺仪融合循迹主循环 */

#endif /* __HUIDU_H */