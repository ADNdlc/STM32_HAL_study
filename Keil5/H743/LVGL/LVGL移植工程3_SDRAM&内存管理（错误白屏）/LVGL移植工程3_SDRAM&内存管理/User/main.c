/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-09-06
 * @brief       触摸屏 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 阿波罗 H743开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/MPU/mpu.h"

#include "./BSP/SDRAM/sdram.h"
#include "./MALLOC/malloc.h"

/* LVGL&FreeRTOS相关包含 */
#include "lvgl_demo.h"


int main(void)
{
    
    
    sys_cache_enable();                        /* 打开L1-Cache */
    HAL_Init();                                /* 初始化HAL库 */
    sys_stm32_clock_init(160, 5, 2, 4);        /* 设置时钟, 400Mhz */
    delay_init(400);                           /* 延时初始化 */
    usart_init(115200);                        /* 串口初始化 */
    mpu_memory_protection();                   /* 保护相关存储区域 */
    led_init();                                /* 初始化LED */
	key_init();                                /* 初始化KEY */
    sdram_init();                              /* 初始化SDRAM */
    
	//my_mem_init(SRAMIN);
	
	/* LVGL相关初始化 */
//	btim_timx_int_init(100-1, 2000-1);	//定时器(时基) 时钟200Mhz	psc=2000, arr=100
//	lv_init();
//	lv_port_disp_init();
//	lv_port_indev_init();
	
	/* LVGL相关组件demo */
//	lv_obj_t* switch_obj = lv_switch_create(lv_scr_act());
//	lv_obj_set_size(switch_obj, 120, 60);
//	lv_obj_align(switch_obj, LV_ALIGN_CENTER, 0, 0);
	
//	lv_demo_stress();	//压力测试
	
//	lv_demo_music();	//音乐播放器

	/* LVGL&FreeRTOS相关组件demo */
	lvgl_demo();
	
	while(1){
//		delay_ms(5);
//		lv_timer_handler();
		
		
	}
}


