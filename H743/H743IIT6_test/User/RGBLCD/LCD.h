/*
 * LCD.h
 *
 *  Created on: Jun 16, 2025
 *      Author: 12114
 */

#ifndef RGBLCD_LCD_H_
#define RGBLCD_LCD_H_

#include "stm32H7xx_hal.h"

extern uint16_t LCD_Buffer0[800][480];
extern uint16_t LCD_Buffer1[800][480];

void LTDC_Draw_Point0(uint16_t x,uint16_t y,uint32_t color);
void LTDC_Draw_Point1(uint16_t x,uint16_t y,uint32_t color);

#endif /* RGBLCD_LCD_H_ */
