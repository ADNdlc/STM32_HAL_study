/*
 * LCD.h
 *
 *  Created on: Jun 16, 2025
 *      Author: 12114
 */

#ifndef RGBLCD_LCD_H_
#define RGBLCD_LCD_H_

#include "stm32H7xx_hal.h"

typedef struct {

#define Vertical 480
#define Horizontal 800

    union addr_Buffer0{
        uint16_t(* RGB565)[Vertical];
        uint32_t(* RGB888)[Vertical];
    } ADDR_Buffer0;

    union addr_Buffer1{
        uint16_t(* RGB565)[Vertical];
        uint32_t(* RGB888)[Vertical];
    } ADDR_Buffer1;

    //void(*LCD_intit)(void);
} LCDInterface;

LCDInterface *getLCDInterface(void);

#endif /* RGBLCD_LCD_H_ */
