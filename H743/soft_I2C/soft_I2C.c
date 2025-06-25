/*
 * soft_I2C.c
 *
 *  Created on: Jun 25, 2025
 *      Author: 12114
 */

#include "soft_I2C.h"
//#include "math.h"

#define I2C_SOFT_SCL_HIGH(hi2c)   (hi2c)->SCL_Port->BSRR = (hi2c)->SCL_Pin
#define I2C_SOFT_SCL_LOW(hi2c)    (hi2c)->SCL_Port->BSRR = (uint32_t)(hi2c)->SCL_Pin << 16
#define I2C_SOFT_SDA_HIGH(hi2c)   (hi2c)->SDA_Port->BSRR = (hi2c)->SDA_Pin
#define I2C_SOFT_SDA_LOW(hi2c)    (hi2c)->SDA_Port->BSRR = (uint32_t)(hi2c)->SDA_Pin << 16

#define I2C_SOFT_SDA_READ(hi2c)   (((hi2c)->SDA_Port->IDR & (hi2c)->SDA_Pin) ? 1 : 0)
#define I2C_SOFT_SCL_READ(hi2c)   (((hi2c)->SCL_Port->IDR & (hi2c)->SCL_Pin) ? 1 : 0)


#define I2C_SOFT_DELAY_US(hi2c, us) do { \
    uint32_t cycles = (uint32_t)((us) * (hi2c)->delay_multiplier); \
    while (cycles--) { \
        __NOP(); \
    } \
} while(0)

/* 可精确控制每个延时
 * 一般配置如下(实际须看从机手册要求):
 * 参数		 说明					标准模式(100kHz)	快速模式(400kHz)
 * tSCL_low	 SCL低电平时间(μs)		4.7				1.3
 * tSCL_high SCL高电平时间(μs)		4.0				0.6
 * tSU_STA	 起始条件建立时间(μs)	4.7				0.6
 * tHD_STA	 起始条件保持时间(μs)	4.0				0.6
 * tSU_STO	 停止条件建立时间(μs)	4.0				0.6
 * tSU_DAT	 数据建立时间(μs)		0.25			0.1
 * tHD_DAT	 数据保持时间(μs)		0				0
 * tBUF		 总线空闲时间(μs)		4.7				1.3
 *
 */
static const I2C_Soft_TimingConfig STANDARD_TIMING = {
    .tSCL_low = 4.7f,
    .tSCL_high = 4.0f,
    .tSU_STA = 4.7f,
    .tHD_STA = 4.0f,
    .tSU_STO = 4.0f,
    .tSU_DAT = 0.25f,
    .tHD_DAT = 0.0f,
    .tBUF = 4.7f
};

// 快速模式时序配置 (400kHz)
static const I2C_Soft_TimingConfig FAST_TIMING = {
    .tSCL_low = 1.3f,
    .tSCL_high = 0.6f,
    .tSU_STA = 0.6f,
    .tHD_STA = 0.6f,
    .tSU_STO = 0.6f,
    .tSU_DAT = 0.1f,
    .tHD_DAT = 0.0f,
    .tBUF = 1.3f
};

// 初始化I2C接口
void I2C_Soft_Init(I2C_Soft_HandleTypeDef *hi2c, uint32_t sys_clk_freq) {
    // 配置SCL和SDA为开漏输出模式
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // SCL引脚配置
    GPIO_InitStruct.Pin = hi2c->SCL_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(hi2c->SCL_Port, &GPIO_InitStruct);

    // SDA引脚配置
    GPIO_InitStruct.Pin = hi2c->SDA_Pin;
    HAL_GPIO_Init(hi2c->SDA_Port, &GPIO_InitStruct);

    // 释放总线
    I2C_SOFT_SCL_HIGH(hi2c);
    I2C_SOFT_SDA_HIGH(hi2c);

    // 计算延时乘数 (基于系统时钟频率)
    // 假设每个NOP指令消耗1个时钟周期
    hi2c->delay_multiplier = sys_clk_freq / 1000000.0f; // 每微秒的周期数

    // 应用默认时序配置 (如果用户未配置)
    if (hi2c->timing.tSCL_low == 0) {
        I2C_Soft_SetTiming(hi2c, &STANDARD_TIMING);
    }

    hi2c->isBusy = 0;
}

// 设置时序配置
void I2C_Soft_SetTiming(I2C_Soft_HandleTypeDef *hi2c,
                       const I2C_Soft_TimingConfig *timing) {
    hi2c->timing = *timing;
}

/* ================================================以下是底层操作======================================================== */

// 生成起始条件
static void I2C_Soft_Start(I2C_Soft_HandleTypeDef *hi2c) {
    I2C_SOFT_SDA_HIGH(hi2c);
    I2C_SOFT_SCL_HIGH(hi2c);
    I2C_SOFT_DELAY_US(hi2c, hi2c->timing.tBUF); // 总线空闲时间

    I2C_SOFT_SDA_LOW(hi2c);
    I2C_SOFT_DELAY_US(hi2c, hi2c->timing.tSU_STA); // 起始条件建立时间

    I2C_SOFT_SCL_LOW(hi2c);
    I2C_SOFT_DELAY_US(hi2c, hi2c->timing.tHD_STA); // 起始条件保持时间
}

// 生成停止条件
static void I2C_Soft_Stop(I2C_Soft_HandleTypeDef *hi2c) {
    I2C_SOFT_SDA_LOW(hi2c);
    I2C_SOFT_DELAY_US(hi2c, hi2c->timing.tSU_DAT); // 数据建立时间

    I2C_SOFT_SCL_HIGH(hi2c);
    I2C_SOFT_DELAY_US(hi2c, hi2c->timing.tSU_STO); // 停止条件建立时间

    I2C_SOFT_SDA_HIGH(hi2c);
    I2C_SOFT_DELAY_US(hi2c, hi2c->timing.tBUF); // 总线空闲时间
}

// 发送应答位
static void I2C_Soft_Ack(I2C_Soft_HandleTypeDef *hi2c) {
    I2C_SOFT_SDA_LOW(hi2c);
    I2C_SOFT_DELAY_US(hi2c, hi2c->timing.tSU_DAT); // 数据建立时间

    I2C_SOFT_SCL_HIGH(hi2c);
    I2C_SOFT_DELAY_US(hi2c, hi2c->timing.tSCL_high); // SCL高电平时间

    I2C_SOFT_SCL_LOW(hi2c);
    I2C_SOFT_DELAY_US(hi2c, hi2c->timing.tSCL_low); // SCL低电平时间

    I2C_SOFT_SDA_HIGH(hi2c); // 释放SDA
}

// 发送非应答位
static void I2C_Soft_NAck(I2C_Soft_HandleTypeDef *hi2c) {
    I2C_SOFT_SDA_HIGH(hi2c);
    I2C_SOFT_DELAY_US(hi2c, hi2c->timing.tSU_DAT); // 数据建立时间

    I2C_SOFT_SCL_HIGH(hi2c);
    I2C_SOFT_DELAY_US(hi2c, hi2c->timing.tSCL_high); // SCL高电平时间

    I2C_SOFT_SCL_LOW(hi2c);
    I2C_SOFT_DELAY_US(hi2c, hi2c->timing.tSCL_low); // SCL低电平时间
}

// 等待应答
static uint8_t I2C_Soft_Wait_Ack(I2C_Soft_HandleTypeDef *hi2c, uint32_t timeout) {
    uint32_t start = HAL_GetTick();
    I2C_SOFT_SDA_HIGH(hi2c); // 释放SDA

    I2C_SOFT_DELAY_US(hi2c, hi2c->timing.tSU_DAT); // 数据建立时间
    I2C_SOFT_SCL_HIGH(hi2c);

    while (I2C_SOFT_SDA_READ(hi2c)) {
        if ((HAL_GetTick() - start) > timeout) {
            I2C_SOFT_SCL_LOW(hi2c);
            return 1; // 超时无应答
        }
        I2C_SOFT_DELAY_US(hi2c, 1); // 微秒级检查
    }

    I2C_SOFT_DELAY_US(hi2c, hi2c->timing.tSCL_high / 2); // 半个SCL高电平时间
    I2C_SOFT_SCL_LOW(hi2c);
    I2C_SOFT_DELAY_US(hi2c, hi2c->timing.tSCL_low); // SCL低电平时间
    return 0;
}

// 发送一个字节
static void I2C_Soft_SendByte(I2C_Soft_HandleTypeDef *hi2c, uint8_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        (data & 0x80) ? (I2C_SOFT_SDA_HIGH(hi2c)) : (I2C_SOFT_SDA_LOW(hi2c));
        data <<= 1;
        I2C_SOFT_DELAY_US(hi2c, hi2c->timing.tSU_DAT); // 数据建立时间

        I2C_SOFT_SCL_HIGH(hi2c);
        I2C_SOFT_DELAY_US(hi2c, hi2c->timing.tSCL_high); // SCL高电平时间

        I2C_SOFT_SCL_LOW(hi2c);
        I2C_SOFT_DELAY_US(hi2c, hi2c->timing.tSCL_low); // SCL低电平时间
    }
    I2C_SOFT_SDA_HIGH(hi2c); // 释放SDA
}

// 接收一个字节
static uint8_t I2C_Soft_ReadByte(I2C_Soft_HandleTypeDef *hi2c, uint8_t ack) {
    uint8_t data = 0;
    I2C_SOFT_SDA_HIGH(hi2c); // 释放SDA

    for (uint8_t i = 0; i < 8; i++) {
        data <<= 1;

        I2C_SOFT_SCL_HIGH(hi2c);
        I2C_SOFT_DELAY_US(hi2c, hi2c->timing.tSCL_high / 2); // 半个SCL高电平时间

        if (I2C_SOFT_SDA_READ(hi2c)) data |= 0x01;

        I2C_SOFT_DELAY_US(hi2c, hi2c->timing.tSCL_high / 2); // 剩余半个SCL高电平时间
        I2C_SOFT_SCL_LOW(hi2c);
        I2C_SOFT_DELAY_US(hi2c, hi2c->timing.tSCL_low); // SCL低电平时间
    }

    ack ? I2C_Soft_Ack(hi2c) : I2C_Soft_NAck(hi2c);
    return data;
}


