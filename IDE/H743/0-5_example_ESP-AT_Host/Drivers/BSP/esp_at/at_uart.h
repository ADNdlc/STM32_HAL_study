/*
 * sep_uart.h
 *
 *  Created on: Aug 9, 2025
 *      Author: 12114
 */

#ifndef BSP_ESP_AT_AT_UART_H_
#define BSP_ESP_AT_AT_UART_H_

#include "usart.h"

#define	loopbuffer_Size	1024	//模块回复消息的缓冲区大小

typedef struct{
	UART_HandleTypeDef* uart_port;	//使用的串口

	size_t		readIndex;					//读索引
	uint8_t 	loopbuff[loopbuffer_Size];	//数据缓冲区
} AT_UART_HandleTypeDef;




#endif
