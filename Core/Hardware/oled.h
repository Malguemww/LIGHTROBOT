#ifndef __OLED_H
#define __OLED_H

#include "main.h"
#include "stm32f4xx_hal.h"

/* OLED 接口配置（I2C2: PB10 SCL, PB11 SDA）-----------------------------*/
#define OLED_I2C                hi2c2
#define OLED_ADDR               0x78    /* 7位地址左移1位 = 0x3C << 1 */

/* OLED 尺寸 ----------------------------------------------------------------*/
#define OLED_WIDTH              128
#define OLED_HEIGHT             64

/* 公共函数声明 --------------------------------------------------------------*/
void OLED_Init(void);                /* 初始化 OLED 显示屏（I2C2） */
void OLED_Clear(void);               /* 清空屏幕缓冲区 */
void OLED_ShowString(uint8_t x, uint8_t y, const char *str, uint8_t size); /* 显示字符串（size: 12/16） */
void OLED_ShowNum(uint8_t x, uint8_t y, int32_t num, uint8_t len, uint8_t size); /* 显示整数 */
void OLED_ShowFloat(uint8_t x, uint8_t y, float num, uint8_t int_len, uint8_t dec_len, uint8_t size); /* 显示浮点数 */
void OLED_Refresh(void);             /* 全屏刷新（将缓冲区写入 OLED） */
void OLED_SetCursor(uint8_t x, uint8_t y); /* 设置光标位置 */
void OLED_Printf(const char *fmt, ...); /* 格式化打印（类似 printf） */

#endif /* __OLED_H */