/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-09-06
 * @brief       ������ ʵ��
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ������ H743������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
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

/* LVGL&FreeRTOS��ذ��� */
#include "lvgl_demo.h"


int main(void)
{
    
    
    sys_cache_enable();                        /* ��L1-Cache */
    HAL_Init();                                /* ��ʼ��HAL�� */
    sys_stm32_clock_init(160, 5, 2, 4);        /* ����ʱ��, 400Mhz */
    delay_init(400);                           /* ��ʱ��ʼ�� */
    usart_init(115200);                        /* ���ڳ�ʼ�� */
    mpu_memory_protection();                   /* ������ش洢���� */
    led_init();                                /* ��ʼ��LED */
	key_init();                                /* ��ʼ��KEY */
    sdram_init();                              /* ��ʼ��SDRAM */
    
	//my_mem_init(SRAMIN);
	
	/* LVGL��س�ʼ�� */
//	btim_timx_int_init(100-1, 2000-1);	//��ʱ��(ʱ��) ʱ��200Mhz	psc=2000, arr=100
//	lv_init();
//	lv_port_disp_init();
//	lv_port_indev_init();
	
	/* LVGL������demo */
//	lv_obj_t* switch_obj = lv_switch_create(lv_scr_act());
//	lv_obj_set_size(switch_obj, 120, 60);
//	lv_obj_align(switch_obj, LV_ALIGN_CENTER, 0, 0);
	
//	lv_demo_stress();	//ѹ������
	
//	lv_demo_music();	//���ֲ�����

	/* LVGL&FreeRTOS������demo */
	lvgl_demo();
	
	while(1){
//		delay_ms(5);
//		lv_timer_handler();
		
		
	}
}


