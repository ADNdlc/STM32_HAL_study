/*
 * soft_SPI.h
 *
 *  Created on: Jun 24, 2025
 *      Author: 12114
 */

#ifndef SOFT_SPI_H_
#define SOFT_SPI_H_

#include "stm32h7xx_hal.h"

// 添加结构体类型的前向声明
typedef struct SoftSPI_HandleTypeDef SoftSPI_HandleTypeDef;

/*	SPI工作模式配置
 *
 * 时钟极性(CPOL)定义了时钟空闲状态电平：
 * CPOL=0，表示当SCLK=0时处于空闲态，所以有效状态就是SCLK处于高电平时
 * CPOL=1，表示当SCLK=1时处于空闲态，所以有效状态就是SCLK处于低电平时
 * 时钟相位(CPHA)定义数据的采集时间。
 * CPHA=0，在时钟的第一个跳变沿（上升沿或下降沿）进行数据采样。，在第2个边沿发送数据
 * CPHA=1，在时钟的第二个跳变沿（上升沿或下降沿）进行数据采样。，在第1个边沿发送数据
 *
 */
typedef enum {
    SOFT_SPI_MODE0 = 0,  // CPOL=0, CPHA=0
    SOFT_SPI_MODE1,      // CPOL=0, CPHA=1
    SOFT_SPI_MODE2,      // CPOL=1, CPHA=0
    SOFT_SPI_MODE3       // CPOL=1, CPHA=1
} SoftSPI_ModeTypeDef;

typedef enum {
    SOFT_SPI_FIRST_BIT_MSB = 0,  // MSB优先
    SOFT_SPI_FIRST_BIT_LSB = 1   // LSB优先
} SoftSPI_FirstBitTypeDef;

typedef enum {
    SOFT_SPI_NSS_SOFT = 0,  // 用户手动控制
    SOFT_SPI_NSS_AUTO = 1,  // 自动控制
    SOFT_SPI_NSS_DISABLE = 2 // NSS禁用
} SoftSPI_NSSModeTypeDef;

// SPI配置结构体
typedef struct {
    //引脚配置
    GPIO_TypeDef* clk_port;
    uint16_t clk_pin;
    GPIO_TypeDef* mosi_port;
    uint16_t mosi_pin;
    GPIO_TypeDef* miso_port;
    uint16_t miso_pin;
    GPIO_TypeDef* nss_port;  // 可选片选引脚
    uint16_t nss_pin;
    
    uint8_t delay_cycles;   // 时钟延时周期（0-255）
    
    //传输模式
    SoftSPI_FirstBitTypeDef     firstBit;    // MSB/LSB优先
    SoftSPI_NSSModeTypeDef      nss_mode;    // NSS控制模式
    SoftSPI_ModeTypeDef         mode;        // SPI模式
} SoftSPI_ConfigTypeDef;

// SPI句柄
struct SoftSPI_HandleTypeDef {
    SoftSPI_ConfigTypeDef config;  // 确保此类型已定义
    void (*TransmitReceive)(SoftSPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);
};



// 初始化函数
void SoftSPI_Init(SoftSPI_HandleTypeDef *hspi, SoftSPI_ConfigTypeDef *config);

// 标准HAL风格API
void HAL_SoftSPI_Transmit(SoftSPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout);
void HAL_SoftSPI_Receive(SoftSPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout);
void HAL_SoftSPI_TransmitReceive(SoftSPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout);


#endif
