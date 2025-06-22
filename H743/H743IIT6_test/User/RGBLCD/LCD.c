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


void LTDC_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint32_t color,uint32_t buffer)
{
	uint32_t psx,psy,pex,pey; //锟???? LCD 闈㈡澘涓哄熀鍑嗙殑鍧愭爣锟????,涓嶉殢妯珫灞忓彉鍖栵拷?锟藉彉锟????
	uint32_t timeout=0;
	uint16_t offline;
	uint32_t addr;
	// 按屏幕使用方式对坐标进行变换
	if(1)// 横屏
	{
		psx=sx;psy=sy;
		pex=ex;pey=ey;
	}else // 竖屏
	{
		psx=sy;psy=272-ex-1; // 272为屏幕高度像素
		pex=ey;pey=272-sx-1;
	}
	offline=480-(pex-psx+1); // 480为屏幕宽度像素
	addr=((uint32_t)buffer+2*(480*psy+psx)); // 2为像素字节大小，480为屏幕宽度像素
	DMA2D->CR&=~(DMA2D_CR_START); //停止DMA2D
	DMA2D->CR=DMA2D_R2M; //DMA2D配置为寄存器到内存
	DMA2D->OOR=offline; //行偏移
	DMA2D->OMAR=addr; //颜色输出地址
	DMA2D->NLR=(pey-psy+1)|((pex-psx+1)<<16); //设定行数寄存器
	DMA2D->OCOLR=color; //颜色输入地址
	DMA2D->CR|=DMA2D_CR_START; //启动 DMA2D
	while((DMA2D->ISR&(DMA2D_FLAG_TC))==0) //等待传输完成
	{
		timeout++;
		if(timeout>0X1FFFFF)break;//超时退出
	}
	DMA2D->IFCR|=DMA2D_FLAG_TC;//清除传输完成标志
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
		psx=sy;psy=272-ex-1; // 272为屏幕高度像素
		pex=ey;pey=272-sx-1;
	}
	offline=480-(pex-psx+1);// 480为屏幕宽度像素
	addr=((uint32_t)buffer+2*(480*psy+psx)); // 2为像素字节大小，480为屏幕宽度像素
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
