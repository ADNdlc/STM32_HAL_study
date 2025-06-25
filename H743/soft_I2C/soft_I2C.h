/*
 * soft_I2C.h
 *
 *  Created on: Jun 25, 2025
 *      Author: 12114
 */

#ifndef SOFT_I2C_SOFT_I2C_H_
#define SOFT_I2C_SOFT_I2C_H_

#include "stm32h7xx_hal.h"

// I2C工作模式枚举
typedef enum {
    I2C_SOFT_MODE_MASTER_TX,
    I2C_SOFT_MODE_MASTER_RX,
    I2C_SOFT_MODE_SLAVE_TX,
    I2C_SOFT_MODE_SLAVE_RX
} I2C_Soft_Mode;

// 延时配置结构体
typedef struct {
    float tSCL_low;     // SCL低电平时间(us)
    float tSCL_high;    // SCL高电平时间(us)
    float tSU_STA;      // 起始条件建立时间(us)
    float tHD_STA;      // 起始条件保持时间(us)
    float tSU_STO;      // 停止条件建立时间(us)
    float tSU_DAT;      // 数据建立时间(us)
    float tHD_DAT;      // 数据保持时间(us)
    float tBUF;         // 总线空闲时间(us)
} I2C_Soft_TimingConfig;

// I2C句柄结构体
typedef struct {
    // 用户配置参数
    GPIO_TypeDef* SCL_Port;
    uint16_t SCL_Pin;
    GPIO_TypeDef* SDA_Port;
    uint16_t SDA_Pin;
    I2C_Soft_Mode mode;
    uint8_t ownAddress;  // 从机模式下有效
    I2C_Soft_TimingConfig timing; // 时序配置

    // 内部状态变量
    uint8_t isBusy;
    uint32_t timeout;
    uint32_t delay_multiplier; // 延时乘数(自动计算)
} I2C_Soft_HandleTypeDef;

// 初始化函数
void I2C_Soft_Init(I2C_Soft_HandleTypeDef *hi2c, uint32_t sys_clk_freq);

// 设置时序配置
void I2C_Soft_SetTiming(I2C_Soft_HandleTypeDef *hi2c, const I2C_Soft_TimingConfig *timing);

// 主模式传输函数
HAL_StatusTypeDef I2C_Soft_Master_Transmit(I2C_Soft_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef I2C_Soft_Master_Receive(I2C_Soft_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);

// 状态检查函数
uint8_t I2C_Soft_IsDeviceReady(I2C_Soft_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Timeout);


#endif /* SOFT_I2C_SOFT_I2C_H_ */
