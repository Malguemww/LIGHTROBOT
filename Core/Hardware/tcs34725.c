#include "tcs34725.h"
#include <math.h>

/* 白平衡增益（校准后写死）--------------------------------------------------*/
static TCS34725_WhiteBalance wb = {
    .r_gain = 1.0f,     /* 红色通道增益 → 校准OLED显示的值填到这里 */
    .g_gain = 1.0f,     /* 绿色通道增益 */
    .b_gain = 1.0f,     /* 蓝色通道增益 → 校准OLED显示的值填到这里 */
};

/* 颜色名称字符串表 ----------------------------------------------------------*/
const char *TCS34725_color_str[] = {
    "BLACK", "WHITE", "RED", "GREEN", "BLUE", "UNKNOWN"
};

/**
  * @brief  通过 I2C3 读取 TCS34725 寄存器
  */
static HAL_StatusTypeDef TCS34725_Read(uint8_t reg, uint8_t *buf, uint8_t len)
{
    return HAL_I2C_Mem_Read(&TCS34725_I2C, TCS34725_ADDR << 1,
                            reg | TCS34725_CMD_AUTO_INC,
                            I2C_MEMADD_SIZE_8BIT, buf, len, 100);
}

/**
  * @brief  通过 I2C3 写入 TCS34725 寄存器
  */
static HAL_StatusTypeDef TCS34725_Write(uint8_t reg, uint8_t data)
{
    return HAL_I2C_Mem_Write(&TCS34725_I2C, TCS34725_ADDR << 1,
                             reg | TCS34725_CMD_AUTO_INC,
                             I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
}

/**
  * @brief  初始化 TCS34725 颜色传感器（I2C3: PA8 + PC9）
  * @retval 0=成功, 1=失败
  */
uint8_t TCS34725_Init(void)
{
    uint8_t id;

    /* 读取芯片 ID（应为 0x44） */
    if (TCS34725_Read(TCS34725_ID, &id, 1) != HAL_OK)
        return 1;
    if (id != 0x44)
        return 1;

    /* 使能传感器（PON + AEN） */
    TCS34725_Write(TCS34725_ENABLE, 0x03);

    /* 设置积分时间（约 154ms） */
    TCS34725_Write(TCS34725_ATIME, 0x00);

    /* 设置增益为 1x */
    TCS34725_Write(TCS34725_CONTROL, 0x00);

    return 0;
}

/**
  * @brief  读取 TCS34725 颜色数据
  * @param  color: 颜色数据结构体指针
  * @retval 0=成功, 1=失败
  */
uint8_t TCS34725_ReadColor(TCS34725_Color *color)
{
    uint8_t buf[8];

    if (TCS34725_Read(TCS34725_CDATAL, buf, 8) != HAL_OK)
        return 1;

    color->clear = ((uint16_t)buf[1] << 8) | buf[0];
    color->red   = ((uint16_t)buf[3] << 8) | buf[2];
    color->green = ((uint16_t)buf[5] << 8) | buf[4];
    color->blue  = ((uint16_t)buf[7] << 8) | buf[6];

    return 0;
}

/**
  * @brief  白平衡校准：对准白色物体，自动计算各通道增益
  * @note   10 次采样取平均，以绿色为基准计算红蓝色增益
  */
void TCS34725_WhiteBalance(void)
{
    TCS34725_Color c;
    uint8_t i;
    uint32_t sum_r = 0, sum_g = 0, sum_b = 0;

    for (i = 0; i < 10; i++)
    {
        TCS34725_ReadColor(&c);
        sum_r += c.red;
        sum_g += c.green;
        sum_b += c.blue;
        HAL_Delay(50);
    }

    float avg_r = (float)sum_r / 10.0f;
    float avg_g = (float)sum_g / 10.0f;
    float avg_b = (float)sum_b / 10.0f;

    /* 以绿色为基准，计算红蓝增益 */
    if (avg_g > 0)
    {
        wb.r_gain = avg_g / avg_r;
        wb.g_gain = 1.0f;
        wb.b_gain = avg_g / avg_b;
    }
}

/**
  * @brief  白平衡校正后的颜色数据
  * @param  raw: 原始颜色数据
  * @param  corrected: 输出校正后的数据
  */
void TCS34725_ApplyWhiteBalance(TCS34725_Color *raw, TCS34725_Color *corrected)
{
    corrected->clear = raw->clear;
    corrected->red   = (uint16_t)((float)raw->red   * wb.r_gain);
    corrected->green = (uint16_t)((float)raw->green * wb.g_gain);
    corrected->blue  = (uint16_t)((float)raw->blue  * wb.b_gain);
}

/**
  * @brief  获取当前白平衡增益（调试时 OLED 显示用）
  */
TCS34725_WhiteBalance TCS34725_GetWhiteBalance(void)
{
    return wb;
}

/**
  * @brief  手动设置白平衡增益（校准后写死用）
  */
void TCS34725_SetWhiteBalance(TCS34725_WhiteBalance gains)
{
    wb = gains;
}

TCS34725_ColorName TCS34725_ClassifyColor(TCS34725_Color *c)
{
    uint16_t r = c->red;
    uint16_t g = c->green;
    uint16_t b = c->blue;
    uint32_t sum = (uint32_t)r + g + b;

    /* 极低亮度 → 黑色 */
    if (c->clear < 50 || sum < 100)
        return COLOR_BLACK;

    /* 归一化到 0~1000 */
    uint16_t r_n = (uint16_t)((uint32_t)r * 1000 / sum);
    uint16_t g_n = (uint16_t)((uint32_t)g * 1000 / sum);
    uint16_t b_n = (uint16_t)((uint32_t)b * 1000 / sum);

    /* 白色：三通道接近均衡 */
    if (r_n > 280 && g_n > 280 && b_n > 280 && r_n < 400 && g_n < 400 && b_n < 400)
        return COLOR_WHITE;

    /* 红色：R 主导 */
    if (r_n > 380 && r_n > g_n + 100 && r_n > b_n + 100)
        return COLOR_RED;

    /* 绿色：G 主导 */
    if (g_n > 380 && g_n > r_n + 100 && g_n > b_n + 100)
        return COLOR_GREEN;

    /* 蓝色：B 主导 */
    if (b_n > 380 && b_n > r_n + 100 && b_n > g_n + 100)
        return COLOR_BLUE;

    return COLOR_UNKNOWN;
}
