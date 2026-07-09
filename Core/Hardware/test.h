#ifndef __TEST_H
#define __TEST_H

#include "main.h"
#include "oled.h"
#include "tcs34725.h"

/**
  * 全部测试函数的统一入口
  * 在 main.c 的 while(1) 中调用，由宏开关控制启用/禁用
  */

/* 测试功能开关：1 = 启用，0 = 禁用 ------------------------------------------*/
#define TEST_TCS34725_ENABLE    1       /* 颜色传感器识别 + OLED 显示 */
#define TCS34725_CALIBRATE      0       /* 白平衡校准模式：1=OLED显示增益值（写死到tcs34725.c后改为0） */

/* 公共函数声明 --------------------------------------------------------------*/
void Test_TCS34725_ColorRecognize(void); /* TCS34725 颜色识别 + OLED 显示 */
void Test_All(void);                     /* 全部测试统一调度入口 */

#endif /* __TEST_H */