/*
 * LCD.c
 *
 *  Created on: Jun 16, 2025
 *      Author: 12114
 */


#include "LCD.h"

uint16_t LCD_Buffer0[800][480] __attribute__((section(".sdram_section"), aligned(16)));
uint16_t LCD_Buffer1[800][480] __attribute__((section(".sdram_section"), aligned(16)));


void LTDC_Draw_Point0(uint16_t x,uint16_t y,uint32_t color)
{
	*(uint16_t*)((uint32_t)LCD_Buffer0+2*(480*y+x))=color;
	// 上方代码中：2为每个像素颜色值的大小，意为2字节，如果颜色格式大于RGB565，则为4字节，480为本文中使用的屏幕宽度像素值，
}

void LTDC_Draw_Point1(uint16_t x,uint16_t y,uint32_t color)
{
	*(uint16_t*)((uint32_t)LCD_Buffer1+2*(800*y+x))=color;
	// 上方代码中：2为每个像素颜色值的大小，意为2字节，如果颜色格式大于RGB565，则为4字节，480为本文中使用的屏幕宽度像素值，
}
