/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-09-06
 * @brief       内存管理 实验
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
#include "./BSP/SDRAM/sdram.h"
#include "./USMART/usmart.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/MPU/mpu.h"
#include "./BSP/LCD/ltdc.h"
#include "./MALLOC/malloc.h"


const char *SRAM_NAME_BUF[SRAMBANK] = {" SRAMIN ", " SRAMEX ", " SRAM12 ", " SRAM4  ", "SRAMDTCM", "SRAMITCM"};

int main(void)
{
    uint8_t paddr[20];                       /* 存放P Addr:+p地址的ASCII值 */
    uint16_t memused = 0;
    uint8_t key;
    uint8_t i = 0;
    uint8_t *p = 0;
    uint8_t *tp = 0;
    uint8_t sramx = 0;                       /* 默认为内部sram */

    sys_cache_enable();                      /* 打开L1-Cache */
    HAL_Init();                              /* 初始化HAL库 */
    sys_stm32_clock_init(160, 5, 2, 4);      /* 设置时钟, 400Mhz */
    delay_init(400);                         /* 延时初始化 */
    usart_init(115200);                      /* 串口初始化 */
    usmart_init(200);                        /* 初始化USMART */
    mpu_memory_protection();                 /* 保护相关存储区域 */
    led_init();                              /* 初始化LED */
    sdram_init();                            /* 初始化SDRAM */
    lcd_init();                              /* 初始化LCD */
    key_init();                              /* 初始化KEY */

    my_mem_init(SRAMIN);                     /* 初始化内部内存池(AXI) */
    my_mem_init(SRAMEX);                     /* 初始化外部内存池(SDRAM) */
    my_mem_init(SRAM12);                     /* 初始化SRAM12内存池(SRAM1+SRAM2) */
    my_mem_init(SRAM4);                      /* 初始化SRAM4内存池(SRAM4) */
    my_mem_init(SRAMDTCM);                   /* 初始化DTCM内存池(DTCM) */
    my_mem_init(SRAMITCM);                   /* 初始化ITCM内存池(ITCM) */
    
    lcd_show_string(30,  50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30,  70, 200, 16, 16, "MALLOC TEST", RED);
    lcd_show_string(30,  90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "KEY0:Malloc  KEY2:Free", RED);
    lcd_show_string(30, 130, 200, 16, 16, "KEY_UP:SRAMx KEY1:Read", RED);

    lcd_show_string(60, 160, 200, 16, 16, "SRAMIN ", BLUE);
    lcd_show_string(30, 180, 200, 16, 16, "SRAMIN   USED:", BLUE);
    lcd_show_string(30, 200, 200, 16, 16, "SRAMEX   USED:", BLUE);
    lcd_show_string(30, 220, 200, 16, 16, "SRAM12   USED:", BLUE);
    lcd_show_string(30, 240, 200, 16, 16, "SRAM4    USED:", BLUE);
    lcd_show_string(30, 260, 200, 16, 16, "SRAMDTCM USED:", BLUE);
    lcd_show_string(30, 280, 200, 16, 16, "SRAMITCM USED:", BLUE);

    while (1)
    {
        key = key_scan(0);                            /* 不支持连按 */

        switch (key)
        {
            case 0:                                   /* 没有按键按下 */
                break;
            
            case KEY0_PRES:                           /* KEY0按下 */
                p = mymalloc(sramx, 2048);            /* 申请2K字节,并写入内容,显示在lcd屏幕上面 */

                if (p != NULL)
                {
                    sprintf((char *)p, "Memory Malloc Test%03d", i);        /* 向p写入一些内容 */
                }
                break;

            case KEY1_PRES:                           /* KEY1按下 */
                if (p != NULL)
                {
                    sprintf((char *)p, "Memory Malloc Test%03d", i);        /* 更新显示内容 */
                    lcd_show_string(30, 310, 209, 16, 16, (char *)p, BLUE); /* 显示P的内容 */
                }
                break;

            case KEY2_PRES:                           /* KEY2按下 */
                myfree(sramx, p);                     /* 释放内存 */
                p = 0;                                /* 指向空地址 */
                break;

            case WKUP_PRES:                           /* KEY UP按下 */
                sramx++;

                if (sramx >= SRAMBANK) 
                {
                    sramx = 0;
                }

                lcd_show_string(60, 160, 200, 16, 16, (char *)SRAM_NAME_BUF[sramx], BLUE);
                break;
        }

        if (tp != p && p != NULL)
        {
            tp = p;
            sprintf((char *)paddr, "P Addr:0X%08X", (uint32_t)tp);
            lcd_show_string(30, 310, 209, 16, 16, (char *)paddr, BLUE);        /* 显示p的地址 */

            if (p)
            {
                lcd_show_string(30, 330, 330, 16, 16, (char *)p, BLUE);        /* 显示P的内容 */
            }
            else 
            {
                lcd_fill(30, 310, 300, 209, WHITE);                            /* p=0,清除显示 */
            }
        }

        delay_ms(10);
        i++;

        if ((i % 20) == 0)                                                     /* DS0闪烁. */
        {
            memused = my_mem_perused(SRAMIN);
            sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
            lcd_show_string(30 + 112, 180, 200, 16, 16, (char *)paddr, BLUE);  /* 显示内部内存使用率 */
            
            memused = my_mem_perused(SRAMEX);
            sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
            lcd_show_string(30 + 112, 200, 200, 16, 16, (char *)paddr, BLUE);  /* 显示外部内存使用率 */
            
            memused = my_mem_perused(SRAM12);
            sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
            lcd_show_string(30 + 112, 220, 200, 16, 16, (char *)paddr, BLUE);  /* 显示SRAM12内存使用率 */
            
            memused = my_mem_perused(SRAM4);
            sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
            lcd_show_string(30 + 112, 240, 200, 16, 16, (char *)paddr, BLUE);  /* 显示SRAM4内存使用率 */
            
            memused = my_mem_perused(SRAMDTCM);
            sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
            lcd_show_string(30 + 112, 260, 200, 16, 16, (char *)paddr, BLUE);  /* 显示DCM内存使用率 */
            
            memused = my_mem_perused(SRAMITCM);
            sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
            lcd_show_string(30 + 112, 280, 200, 16, 16, (char *)paddr, BLUE);  /* 显示TCM内存使用率 */
            
            LED0_TOGGLE();                                                     /* LED0闪烁 */
        }
    }
}


