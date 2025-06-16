/*
 * LCD.c
 *
 *  Created on: Jun 16, 2025
 *      Author: 12114
 */


#include "LCD.h"

uint16_t LCD_Buffer0[800][480] __attribute__((section(".sdram_section"), aligned(16)));
uint16_t LCD_Buffer1[800][480] __attribute__((section(".sdram_section"), aligned(16)));

