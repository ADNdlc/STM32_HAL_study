/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2023-04-01
 * @brief       LVGL V8����ϵͳ��ֲ ʵ��
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
#include "./BSP/SDRAM/sdram.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/MPU/mpu.h"
#include "./BSP/TOUCH/touch.h"
#include "./BSP/LCD/lcd.h"
#include "lvgl_demo.h"


int main(void)
{
    sys_cache_enable();                                         /* ��L1-Cache */
    HAL_Init();                                                 /* ��ʼ��HAL�� */
    sys_stm32_clock_init(192, 5, 2, 4);                         /* ����ʱ��, 480Mhz */
    delay_init(480);                                            /* ��ʱ��ʼ�� */
    usart_init(115200);                                         /* ���ڳ�ʼ�� */
    mpu_memory_protection();                                    /* ������ش洢���� */
    led_init();                                                 /* ��ʼ��LED */
    key_init();                                                 /* ��ʼ��KEY */
    sdram_init();                                               /* ��ʼ��SDRAM */

    /* ������������� */
    if (key_scan(0) == KEY0_PRES)                               /* KEY0����,��ִ��У׼���� */
    {
        lcd_clear(WHITE);                                       /* ���� */
        tp_adjust();                                            /* ��ĻУ׼ */
        tp_save_adjust_data();
    }
    
    lvgl_demo();                                                /* ����FreeRTOS���� */
}
