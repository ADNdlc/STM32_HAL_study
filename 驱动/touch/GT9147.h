/*
 * GT9147.h
 *
 *  Created on: Jun 26, 2025
 *      Author: 12114
 */

#ifndef TOUCH_GT9147_H_
#define TOUCH_GT9147_H_

#include "../soft_I2C/soft_I2C.h"

/* 错误码定义 */
#define GT9147_OK           0    // 操作成功
#define GT9147_ERROR       -1    // 一般错误
#define GT9147_COMM_ERROR  -2    // 通信错误

/*IIC读写命令*/
#define GT9X_CMD_WR        0X28    //写命令
#define GT9X_CMD_RD        0X29    //读命令

/*GT9XXX部分寄存器定义*/
#define GT9X_CTRL_REG      0X8040    //GT9XXX控制寄存器
#define GT9X_CFGS_REG      0X8047    //GT9XXX配置起始地址寄存器
#define GT9X_CHECK_REG     0X80FF    //GT9XXX校验和寄存器
#define GT9X_PID_REG       0X8140    //GT9XXX产品ID寄存器

#define GT9X_GSTID_REG     0X814E    //触摸状态寄存器，bit7表示有无触摸，bit0~3为触摸点个数

/*  坐标数据寄存器
    共5组每组6个寄存器，都是只读
    下面是第一组，每组结构相同：
    0x8150       触摸点1 X坐标,低8位
    0x8151       触摸点1 X坐标,高8位
    0x8152       触摸点1 Y坐标,低8位
    0x8153       触摸点1 Y坐标,高8位
    0x8154       触摸点1 宽度
    0x8155       触摸点1 高度
 */
#define GT9X_TP1_REG       0X8150    //第一个触摸点数据地址
#define GT9X_TP2_REG       0X8158    //第二个触摸点数据地址
#define GT9X_TP3_REG       0X8160    //第三个触摸点数据地址
#define GT9X_TP4_REG       0X8168    //第四个触摸点数据地址
#define GT9X_TP5_REG       0X8170    //第五个触摸点数据地址

/* Debug开关 */
#define GT9147_DEBUG       1         // 1:开启调试打印, 0:关闭调试打印

typedef struct {
    uint16_t X;          //X坐标
    uint16_t Y;          //Y坐标
#if GT9147_DEBUG
    uint16_t Width;      //宽度(仅调试时使用)
    uint16_t Height;     //高度(仅调试时使用)
#endif
} GT9147_Point_TypeDef;

typedef enum {
    GT9147_VERTICAL   = 0,  //竖屏
    GT9147_HORIZONTAL = 1   //横屏
} Orientation;

typedef struct {
    //INT和REST引脚
    GPIO_TypeDef* INT_Port;
    uint16_t INT_Pin;
    GPIO_TypeDef* RST_Port;
    uint16_t RST_Pin;

    I2C_Soft_HandleTypeDef *hi2c;

    GT9147_Point_TypeDef Point[5];//最多5个触摸点
    /* GT9147默认竖屏原点在左上角，函数中横屏转换的原点在右上角，坐标向左向下递增 */
    Orientation H_V;                  //横屏或竖屏标志
} GT9147_HandleTypeDef;

int8_t GT9147_Init(GT9147_HandleTypeDef *htouch);
int8_t  GT9147_Scan(GT9147_HandleTypeDef *htouch);

#endif /* TOUCH_GT9147_H_ */
