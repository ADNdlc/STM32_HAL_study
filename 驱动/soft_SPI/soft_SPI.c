/*
 * soft_SPI.c
 *
 *  Created on: Jun 24, 2025
 *      Author: 12114
 */

#include "soft_SPI.h"


// 引脚操作宏
#define PIN_LOW(port, pin)   (port)->BSRR = ((uint32_t)(pin) << 16)
#define PIN_HIGH(port, pin)  (port)->BSRR = (pin)

#define PIN_WRITE(port, pin, value)  (port)->BSRR = (value ? (pin) : ((uint32_t)(pin) << 16))
#define READ_PIN(port, pin)  ((port)->IDR & (pin))

// 片选控制宏
#define SOFT_SPI_SELECT(hspi)    HAL_GPIO_WritePin((hspi)->config.nss_port, (hspi)->config.nss_pin, GPIO_PIN_RESET)
#define SOFT_SPI_DESELECT(hspi)  HAL_GPIO_WritePin((hspi)->config.nss_port, (hspi)->config.nss_pin, GPIO_PIN_SET)

// 延时宏
#define DELAY() do { \
    uint32_t cycles = hspi->config.delay_cycles; \
    while(cycles--) { __NOP(); } \
} while(0)

// 模式0传输函数 (CPOL=0, CPHA=0)
static void TransmitReceive_Mode0(SoftSPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size) {
    if(hspi->config.firstBit == SOFT_SPI_FIRST_BIT_MSB) {//高位先出，低位先入
        //传输Size个字节
        for(uint16_t i = 0; i < Size; i++) {
            uint8_t txByte = pTxData ? pTxData[i] : 0xFF;
            uint8_t rxByte = 0;

            //传输一个字节
            for(int8_t bit = 7; bit >= 0; bit--) {
                // 设置MOSI
                PIN_WRITE(hspi->config.mosi_port, hspi->config.mosi_pin, txByte & (1 << bit));
                DELAY();

                // 第一个边沿 (上升沿采样)
                PIN_HIGH(hspi->config.clk_port, hspi->config.clk_pin);
                DELAY();

                // 读取MISO
                if(READ_PIN(hspi->config.miso_port, hspi->config.miso_pin)) rxByte |= (1 << bit);

                // 第二个边沿 (下降沿准备)
                PIN_LOW(hspi->config.clk_port, hspi->config.clk_pin);
                DELAY();
            }

            if(pRxData) pRxData[i] = rxByte;
        }
    }
    else {  //低位先出，高位先入
        for(uint16_t i = 0; i < Size; i++) {
            uint8_t txByte = pTxData ? pTxData[i] : 0xFF;
            uint8_t rxByte = 0;

            for(int8_t bit = 0; bit < 8; bit++) {
                // 设置MOSI
                PIN_WRITE(hspi->config.mosi_port, hspi->config.mosi_pin, txByte & (1 << bit));
                DELAY();

                // 第一个边沿 (上升沿采样)
                PIN_HIGH(hspi->config.clk_port, hspi->config.clk_pin);
                DELAY();

                // 读取MISO
                if(READ_PIN(hspi->config.miso_port, hspi->config.miso_pin)) rxByte |= (1 << bit);

                // 第二个边沿 (下降沿准备)
                PIN_LOW(hspi->config.clk_port, hspi->config.clk_pin);
                DELAY();
            }

            if(pRxData) pRxData[i] = rxByte;
        }
    }
}

// 模式1传输函数 (CPOL=0, CPHA=1)
static void TransmitReceive_Mode1(SoftSPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size) {
    if(hspi->config.firstBit == SOFT_SPI_FIRST_BIT_MSB) {
        for(uint16_t i = 0; i < Size; i++) {
            uint8_t txByte = pTxData ? pTxData[i] : 0xFF;
            uint8_t rxByte = 0;

            for(int8_t bit = 7; bit >= 0; bit--) {
                // 第一个边沿 (上升沿准备)
                PIN_HIGH(hspi->config.clk_port, hspi->config.clk_pin);
                
                // 设置MOSI
                PIN_WRITE(hspi->config.mosi_port, hspi->config.mosi_pin, txByte & (1 << bit));
                DELAY();

                // 第二个边沿 (下降沿采样)
                PIN_LOW(hspi->config.clk_port, hspi->config.clk_pin);
                
                // 读取MISO
                if(READ_PIN(hspi->config.miso_port, hspi->config.miso_pin)) rxByte |= (1 << bit);
                DELAY();
            }

            if(pRxData) pRxData[i] = rxByte;
        }
    }
    else {
        for(uint16_t i = 0; i < Size; i++) {
            uint8_t txByte = pTxData ? pTxData[i] : 0xFF;
            uint8_t rxByte = 0;

            for(int8_t bit = 0; bit < 8; bit++) {
                // 第一个边沿 (上升沿准备)
                PIN_HIGH(hspi->config.clk_port, hspi->config.clk_pin);
                
                // 设置MOSI
                PIN_WRITE(hspi->config.mosi_port, hspi->config.mosi_pin, txByte & (1 << bit));
                DELAY();

                // 第二个边沿 (下降沿采样)
                PIN_LOW(hspi->config.clk_port, hspi->config.clk_pin);
                
                // 读取MISO
                if(READ_PIN(hspi->config.miso_port, hspi->config.miso_pin)) rxByte |= (1 << bit);
                DELAY();
            }

            if(pRxData) pRxData[i] = rxByte;
        }
    }
}

// 模式2传输函数 (CPOL=1, CPHA=0)
static void TransmitReceive_Mode2(SoftSPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size) {
    if(hspi->config.firstBit == SOFT_SPI_FIRST_BIT_MSB) {
        for(uint16_t i = 0; i < Size; i++) {
            uint8_t txByte = pTxData ? pTxData[i] : 0xFF;
            uint8_t rxByte = 0;

            for(int8_t bit = 7; bit >= 0; bit--) {
                // 设置MOSI
                PIN_WRITE(hspi->config.mosi_port, hspi->config.mosi_pin, txByte & (1 << bit));
                DELAY();

                // 第一个边沿 (下降沿采样)
                PIN_LOW(hspi->config.clk_port, hspi->config.clk_pin);
                DELAY();

                // 读取MISO
                if(READ_PIN(hspi->config.miso_port, hspi->config.miso_pin)) rxByte |= (1 << bit);

                // 第二个边沿 (上升沿准备)
                PIN_HIGH(hspi->config.clk_port, hspi->config.clk_pin);
                DELAY();
            }

            if(pRxData) pRxData[i] = rxByte;
        }
    }
    else {
        for(uint16_t i = 0; i < Size; i++) {
            uint8_t txByte = pTxData ? pTxData[i] : 0xFF;
            uint8_t rxByte = 0;

            for(int8_t bit = 0; bit < 8; bit++) {
                // 设置MOSI
                PIN_WRITE(hspi->config.mosi_port, hspi->config.mosi_pin, txByte & (1 << bit));
                DELAY();

                // 第一个边沿 (下降沿采样)
                PIN_LOW(hspi->config.clk_port, hspi->config.clk_pin);
                DELAY();

                // 读取MISO
                if(READ_PIN(hspi->config.miso_port, hspi->config.miso_pin)) rxByte |= (1 << bit);

                // 第二个边沿 (上升沿准备)
                PIN_HIGH(hspi->config.clk_port, hspi->config.clk_pin);
                DELAY();
            }

            if(pRxData) pRxData[i] = rxByte;
        }
    }
}

// 模式3传输函数 (CPOL=1, CPHA=1)
static void TransmitReceive_Mode3(SoftSPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size) {
    if(hspi->config.firstBit == SOFT_SPI_FIRST_BIT_MSB) {
        for(uint16_t i = 0; i < Size; i++) {
            uint8_t txByte = pTxData ? pTxData[i] : 0xFF;
            uint8_t rxByte = 0;

            for(int8_t bit = 7; bit >= 0; bit--) {
                // 第一个边沿 (下降沿准备)
                PIN_LOW(hspi->config.clk_port, hspi->config.clk_pin);
                
                // 设置MOSI
                PIN_WRITE(hspi->config.mosi_port, hspi->config.mosi_pin, txByte & (1 << bit));
                DELAY();

                // 第二个边沿 (上升沿采样)
                PIN_HIGH(hspi->config.clk_port, hspi->config.clk_pin);
                
                // 读取MISO
                if(READ_PIN(hspi->config.miso_port, hspi->config.miso_pin)) rxByte |= (1 << bit);
                DELAY();
            }

            if(pRxData) pRxData[i] = rxByte;
        }
    }
    else {
        for(uint16_t i = 0; i < Size; i++) {
            uint8_t txByte = pTxData ? pTxData[i] : 0xFF;
            uint8_t rxByte = 0;

            for(int8_t bit = 0; bit < 8; bit++) {
                // 第一个边沿 (下降沿准备)
                PIN_LOW(hspi->config.clk_port, hspi->config.clk_pin);
                
                // 设置MOSI
                PIN_WRITE(hspi->config.mosi_port, hspi->config.mosi_pin, txByte & (1 << bit));
                DELAY();

                // 第二个边沿 (上升沿采样)
                PIN_HIGH(hspi->config.clk_port, hspi->config.clk_pin);
                
                // 读取MISO
                if(READ_PIN(hspi->config.miso_port, hspi->config.miso_pin)) rxByte |= (1 << bit);
                DELAY();
            }

            if(pRxData) pRxData[i] = rxByte;
        }
    }
}

// 初始化函数
void SoftSPI_Init(SoftSPI_HandleTypeDef *hspi, SoftSPI_ConfigTypeDef *config) {
    // 复制配置
    hspi->config = *config;

    // 配置GPIO
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // CLK引脚
    GPIO_InitStruct.Pin = config->clk_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(config->clk_port, &GPIO_InitStruct);

    // MOSI引脚
    GPIO_InitStruct.Pin = config->mosi_pin;
    HAL_GPIO_Init(config->mosi_port, &GPIO_InitStruct);

    // MISO引脚
    GPIO_InitStruct.Pin = config->miso_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(config->miso_port, &GPIO_InitStruct);

    // NSS引脚（可选）
    if(config->nss_port && config->nss_mode != SOFT_SPI_NSS_DISABLE) {
        GPIO_InitStruct.Pin = config->nss_pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        HAL_GPIO_Init(config->nss_port, &GPIO_InitStruct);
        HAL_GPIO_WritePin(config->nss_port, config->nss_pin, GPIO_PIN_SET);  // 初始状态为高电平（未选中）
    }

    // 设置初始时钟状态
    if(config->mode == SOFT_SPI_MODE0 || config->mode == SOFT_SPI_MODE1) {
        PIN_LOW(config->clk_port, config->clk_pin);    // CPOL = 0
    } else {
        PIN_HIGH(config->clk_port, config->clk_pin);   // CPOL = 1
    }

    // 根据模式选择传输函数
    switch(config->mode) {
        case SOFT_SPI_MODE0:
            hspi->TransmitReceive = TransmitReceive_Mode0;
            break;
        case SOFT_SPI_MODE1:
            hspi->TransmitReceive = TransmitReceive_Mode1;
            break;
        case SOFT_SPI_MODE2:
            hspi->TransmitReceive = TransmitReceive_Mode2;
            break;
        case SOFT_SPI_MODE3:
            hspi->TransmitReceive = TransmitReceive_Mode3;
            break;
        default:
            hspi->TransmitReceive = TransmitReceive_Mode0;  // 默认使用模式0
            break;
    }
}


void HAL_SoftSPI_Transmit(SoftSPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout) {
    //自动片选(如果不为NULL)
    if(hspi->config.nss_mode == SOFT_SPI_NSS_AUTO) {
        HAL_GPIO_WritePin(hspi->config.nss_port, hspi->config.nss_pin, GPIO_PIN_RESET);  // 片选有效(低电平)
    }
    hspi->TransmitReceive(hspi, pData, NULL, Size);
    if(hspi->config.nss_mode == SOFT_SPI_NSS_AUTO) {
        HAL_GPIO_WritePin(hspi->config.nss_port, hspi->config.nss_pin, GPIO_PIN_SET);    // 片选无效(高电平)
    }
}

void HAL_SoftSPI_Receive(SoftSPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout) {
    if(hspi->config.nss_mode == SOFT_SPI_NSS_AUTO) {
        HAL_GPIO_WritePin(hspi->config.nss_port, hspi->config.nss_pin, GPIO_PIN_RESET);  // 片选有效(低电平)
    }
    hspi->TransmitReceive(hspi, NULL, pData, Size);
    if(hspi->config.nss_mode == SOFT_SPI_NSS_AUTO) {
        HAL_GPIO_WritePin(hspi->config.nss_port, hspi->config.nss_pin, GPIO_PIN_SET);    // 片选无效(高电平)
    }
}

void HAL_SoftSPI_TransmitReceive(SoftSPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout) {
    if(hspi->config.nss_mode == SOFT_SPI_NSS_AUTO) {
        HAL_GPIO_WritePin(hspi->config.nss_port, hspi->config.nss_pin, GPIO_PIN_RESET);  // 片选有效(低电平)
    }
    hspi->TransmitReceive(hspi, pTxData, pRxData, Size);
    if(hspi->config.nss_mode == SOFT_SPI_NSS_AUTO) {
        HAL_GPIO_WritePin(hspi->config.nss_port, hspi->config.nss_pin, GPIO_PIN_SET);    // 片选无效(高电平)
    }
}

