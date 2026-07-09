#ifndef __TCS34725_H
#define __TCS34725_H

#include "main.h"
#include "stm32f4xx_hal.h"

/* I2C3 句柄（PA8: SCL, PC9: SDA）-------------------------------------------*/
#define TCS34725_I2C            hi2c3

/* TCS34725 I2C 7位地址 -----------------------------------------------------*/
#define TCS34725_ADDR           0x29

/* TCS34725 寄存器 -----------------------------------------------------------*/
#define TCS34725_ENABLE         0x00
#define TCS34725_ATIME          0x01
#define TCS34725_CONTROL        0x0F
#define TCS34725_ID             0x12
#define TCS34725_CDATAL         0x14
#define TCS34725_RDATAL         0x16
#define TCS34725_GDATAL         0x18
#define TCS34725_BDATAL         0x1A

/* TCS34725 配置命令 ---------------------------------------------------------*/
#define TCS34725_CMD_AUTO_INC   0xA0

/* 颜色数据结构体 ------------------------------------------------------------*/
typedef struct
{
    uint16_t clear;
    uint16_t red;
    uint16_t green;
    uint16_t blue;
} TCS34725_Color;

/* 白平衡增益结构体 ----------------------------------------------------------*/
typedef struct
{
    float r_gain;       /* 红色通道增益 */
    float g_gain;       /* 绿色通道增益 */
    float b_gain;       /* 蓝色通道增益 */
} TCS34725_WhiteBalance;

/* 颜色名称枚举 --------------------------------------------------------------*/
typedef enum
{
    COLOR_BLACK = 0,
    COLOR_WHITE,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_UNKNOWN
} TCS34725_ColorName;

/* 颜色名称字符串（供外部 OLED 显示用）----------------------------------------*/
extern const char *TCS34725_color_str[];

/* 公共函数声明 --------------------------------------------------------------*/
uint8_t TCS34725_Init(void);                                /* 初始化颜色传感器（I2C3，增益1x） */
uint8_t TCS34725_ReadColor(TCS34725_Color *color);          /* 读取 RGBC 原始值 */
void    TCS34725_WhiteBalance(void);                        /* 白平衡校准（对准白色物体调用） */
void    TCS34725_ApplyWhiteBalance(TCS34725_Color *raw, TCS34725_Color *corrected); /* 白平衡校正 */
TCS34725_ColorName TCS34725_ClassifyColor(TCS34725_Color *c); /* 颜色分类识别 */
TCS34725_WhiteBalance TCS34725_GetWhiteBalance(void);       /* 获取当前白平衡增益（OLED显示用） */
void    TCS34725_SetWhiteBalance(TCS34725_WhiteBalance gains); /* 手动设置白平衡增益 */

#endif /* __TCS34725_H */