/*
 * W9825G6KH.h
 *
 *  Created on: Jun 14, 2025
 *      Author: 12114
 */

#ifndef W9825G6KH_W9825G6KH_H_
#define W9825G6KH_W9825G6KH_H_

#include "fmc.h"


/**
  * @brief  FMC SDRAM 数据基地址(SDRAM Bank1)
  */
#define SDRAM_BANK_ADDR     ((uint32_t)0xC0000000)


void SDRAM_InitSequence(void);
void FMC_SDRAM_WriteBuffer(uint8_t *pBuffer,uint32_t WriteAddr,uint32_t n);
void FMC_SDRAM_ReadBuffer(uint8_t *pBuffer,uint32_t ReadAddr,uint32_t n);

#endif /* W9825G6KH_W9825G6KH_H_ */
