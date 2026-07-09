#include "test.h"
#include "motor.h"

/**
  * @brief  白平衡校准模式：OLED 显示计算出的增益值，供用户抄录后写死
  * @note   TCS34725_CALIBRATE = 1 时执行，显示后进入死循环
  */
static void Test_CalibrateWhiteBalance(void)
{
    TCS34725_WhiteBalance wb_gains;

    /* 提示对准白色物体 */
    OLED_Clear();
    OLED_ShowString(0, 0, "Calibrate WB", 12);
    OLED_ShowString(0, 20, "Point to WHITE", 12);
    OLED_ShowString(0, 36, "Wait 3s...", 12);
    OLED_Refresh();
    HAL_Delay(3000);

    /* 执行白平衡校准 */
    TCS34725_WhiteBalance();

    /* 读取计算出的增益值 */
    wb_gains = TCS34725_GetWhiteBalance();

    /* OLED 显示增益值 ---------------------------------------------------*/
    OLED_Clear();
    OLED_ShowString(0, 0, "=== WB GAINS ===", 8);

    /* R_Gain（乘以1000取整显示，如 1.234 → 1234） */
    OLED_ShowString(0, 16, "R:", 12);
    OLED_ShowNum(12, 16, (uint16_t)(wb_gains.r_gain * 1000), 4, 12);

    /* G_Gain */
    OLED_ShowString(48, 16, "G:", 12);
    OLED_ShowNum(60, 16, (uint16_t)(wb_gains.g_gain * 1000), 4, 12);

    /* B_Gain */
    OLED_ShowString(96, 16, "B:", 12);
    OLED_ShowNum(108, 16, (uint16_t)(wb_gains.b_gain * 1000), 4, 12);

    /* 提示：抄下数值更新 tcs34725.c */
    OLED_ShowString(0, 40, "Copy to", 8);
    OLED_ShowString(0, 48, "tcs34725.c", 8);

    OLED_Refresh();

    /* 死循环，用户记下数值后断电 */
    while (1)
    {
        HAL_Delay(100);
    }
}

/**
  * @brief  颜色传感器识别 + OLED 显示
  * @note   每 200ms 采集一次，显示识别结果和白平衡值
  */
void Test_TCS34725_ColorRecognize(void)
{
    TCS34725_Color raw, corrected;
    TCS34725_ColorName name;

    /* 读取原始数据 */
    if (TCS34725_ReadColor(&raw) != 0)
    {
        OLED_Clear();
        OLED_ShowString(0, 20, "TCS34725 ERR", 16);
        OLED_Refresh();
        return;
    }

    /* 白平衡校正 */
    TCS34725_ApplyWhiteBalance(&raw, &corrected);

    /* 颜色识别 */
    name = TCS34725_ClassifyColor(&corrected);

    /* OLED 显示 --------------------------------------------------------*/
    OLED_Clear();

    /* 第一行：颜色名称（大字体） */
    OLED_ShowString(0, 0, "Color:", 12);
    OLED_ShowString(48, 0, TCS34725_color_str[name], 16);

    /* 第二行：白平衡后 RGB */
    OLED_ShowString(0, 18, "R:", 12);
    OLED_ShowNum(12, 18, corrected.red, 4, 12);
    OLED_ShowString(48, 18, "G:", 12);
    OLED_ShowNum(60, 18, corrected.green, 4, 12);
    OLED_ShowString(96, 18, "B:", 12);
    OLED_ShowNum(108, 18, corrected.blue, 4, 12);

    /* 第三行：原始 RGB */
    OLED_ShowString(0, 30, "raw:", 12);
    OLED_ShowNum(28, 30, raw.red, 4, 12);
    OLED_ShowNum(64, 30, raw.green, 4, 12);
    OLED_ShowNum(100, 30, raw.blue, 4, 12);

    /* 第四行：Clear 通道 */
    OLED_ShowString(0, 44, "C:", 12);
    OLED_ShowNum(12, 44, raw.clear, 4, 12);

    OLED_Refresh();
}

/**
  * @brief  全部测试调度
  */
void Test_All(void)
{
#if TCS34725_CALIBRATE
    /* 白平衡校准模式：显示增益值后死循环，记下数值填入 tcs34725.c */
    Test_CalibrateWhiteBalance();
#endif

#if TEST_TCS34725_ENABLE
    Test_TCS34725_ColorRecognize();
    HAL_Delay(200);     /* 200ms 刷新间隔 */
#endif
}
