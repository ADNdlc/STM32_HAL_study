/*
 * LCD.c
 *
 *  Created on: Jun 16, 2025
 *      Author: 12114
 */


#include "LCD.h"

uint16_t LCD_Buffer0[800][480] __attribute__((section(".sdram_section"), aligned(16)));
uint16_t LCD_Buffer1[800][480] __attribute__((section(".sdram_section"), aligned(16)));


void LTDC_Draw_Point(uint16_t x,uint16_t y,uint32_t color,uint32_t addr)
{
	*(uint16_t*)((uint32_t)addr+2*(800*y+x))=color;
	// 上方代码中：2为每个像素颜色值的大小，意为2字节，如果颜色格式大于RGB565，则为4字节，480为本文中使用的屏幕宽度像素值，
}


static inline void DMA2D_Fill( void * pDst, uint32_t width, uint32_t height, uint32_t lineOff, uint32_t pixelFormat,  uint32_t color)
{
    /* DMA2D配置 */
    DMA2D->CR      = 0x00030000UL;                                  // 配置为寄存器到储存器模式
    DMA2D->OCOLR   = color;                                         // 设置填充使用的颜色，格式应该与设置的颜色格式相同
    DMA2D->OMAR    = (uint32_t)pDst;                                // 填充区域的起始内存地址
    DMA2D->OOR     = lineOff;                                       // 行偏移，即跳过的像素，注意是以像素为单位
    DMA2D->OPFCCR  = pixelFormat;                                   // 设置颜色格式
    DMA2D->NLR     = (uint32_t)(width << 16) | (uint16_t)height;    // 设置填充区域的宽和高，单位是像素

    /* 启动传输 */
    DMA2D->CR   |= DMA2D_CR_START;

    /* 等待DMA2D传输完成 */
    while (DMA2D->CR & DMA2D_CR_START) {}
}

void FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color,uint32_t Buffer){
    void* pDist = &(((uint16_t*)Buffer)[y*800 + x]);
    DMA2D_Fill(pDist, w, h, 800 - w, LTDC_PIXEL_FORMAT_RGB565, color);
}


void LTDC_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t* color,uint32_t buffer)
{
	uint32_t psx,psy,pex,pey;
	uint32_t timeout=0;
	uint16_t offline;
	uint32_t addr;

	// 按屏幕使用方式对坐标进行变换
	if(1) // 横屏
	{
		psx=sx;psy=sy;
		pex=ex;pey=ey;
	}else // 竖屏
	{
		psx=sy;psy=480-ex-1; // 272为屏幕高度像素
		pex=ey;pey=480-sx-1;
	}
	offline=800-(pex-psx+1);// 480为屏幕宽度像素
	addr=((uint32_t)buffer+2*(800*psy+psx)); // 2为像素字节大小，480为屏幕宽度像素
	DMA2D->CR&=~(DMA2D_CR_START); //停止DMA2D
	DMA2D->CR=DMA2D_M2M; //DMA2D配置为内存到内存
	DMA2D->FGMAR=(uint32_t)color; // 需要传输的颜色数据地址
	DMA2D->OMAR=addr; //输出存储器地址
	DMA2D->FGOR=0; // 前景层偏移
	DMA2D->OOR=offline; //行偏移
	DMA2D->NLR=(pey-psy+1)|((pex-psx+1)<<16); //设定行数寄存器
	DMA2D->CR|=DMA2D_CR_START; //启动 DMA2D
	while((DMA2D->ISR&(DMA2D_FLAG_TC))==0) //等待传输完成
	{
		timeout++;
		if(timeout>0X1FFFFF)break;//超时退出
	}
	DMA2D->IFCR|=DMA2D_FLAG_TC;//清除传输完成标志
}
