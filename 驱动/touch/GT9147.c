/*
 * GT9147.c
 *
 *  Created on: Jun 26, 2025
 *      Author: 12114
 */

#include "GT9147.h"
#include "stdio.h"
#include "string.h"

/* 引脚操作宏
 * 它依赖于当前作用域中有一个"htouch"变量。如果在没有"htouch"的地方使用，会导致编译错误
 */
#define Touch_INT_WRITE(value)   (value?((htouch)->INT_Port->BSRR = (htouch)->INT_Pin):\
								((htouch)->INT_Port->BSRR = (htouch)->INT_Pin << 16))
#define Touch_INT_READ()   (((htouch)->INT_Port->IDR & (htouch)->INT_Pin) ? 1 : 0)

#define Touch_RST_WRITE(value)   (value?((htouch)->RST_Port->BSRR = (htouch)->RST_Pin):\
								 ((htouch)->RST_Port->BSRR = (htouch)->RST_Pin << 16))
#define Touch_RST_READ()   (((htouch)->RST_Port->IDR & (htouch)->RST_Pin) ? 1 : 0)


//GT9147配置参数表
//第一个字节为版本号(0X60),必须保证新的版本号大于等于GT9147内部
//flash原有版本号,才会更新配置.
const uint8_t GT9147_CFG_TBL[]={
	0X60,0XE0,0X01,0X20,0X03,0X05,0X35,0X00,0X02,0X08,
	0X1E,0X08,0X50,0X3C,0X0F,0X05,0X00,0X00,0XFF,0X67,
	0X50,0X00,0X00,0X18,0X1A,0X1E,0X14,0X89,0X28,0X0A,
	0X30,0X2E,0XBB,0X0A,0X03,0X00,0X00,0X02,0X33,0X1D,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X32,0X00,0X00,
	0X2A,0X1C,0X5A,0X94,0XC5,0X02,0X07,0X00,0X00,0X00,
	0XB5,0X1F,0X00,0X90,0X28,0X00,0X77,0X32,0X00,0X62,
	0X3F,0X00,0X52,0X50,0X00,0X52,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0X0F,0X03,0X06,0X10,0X42,0XF8,0X0F,0X14,0X00,0X00,
	0X00,0X00,0X1A,0X18,0X16,0X14,0X12,0X10,0X0E,0X0C,
	0X0A,0X08,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X29,0X28,0X24,0X22,0X20,0X1F,0X1E,0X1D,
	0X0E,0X0C,0X0A,0X08,0X06,0X05,0X04,0X02,0X00,0XFF,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,
};

/**
 * @brief  发送GT9147配置参数
 * @param  mode: 0,参数不保存到flash
 *               1,参数保存到flash
 * @param  hi2c: 屏幕连接的I2C的句柄
 * @return int8_t: GT9147_OK:成功, GT9147_COMM_ERROR:通信错误
 */
static int8_t GT9147_Send_Cfg(uint8_t mode, I2C_Soft_HandleTypeDef *hi2c)
{
	uint8_t buf[2];
	uint8_t i=0;
	int8_t ret;

	buf[0]=0;
	buf[1]=mode;    //是否写入到GT9147 FLASH?  即是否掉电保存
	for(i=0;i<sizeof(GT9147_CFG_TBL);i++)
		buf[0]+=GT9147_CFG_TBL[i];//计算校验和
	buf[0]=(~buf[0])+1;

	ret = I2C_Soft_Master_Transmit(hi2c, GT9X_CFGS_REG, (uint8_t*)GT9147_CFG_TBL, sizeof(GT9147_CFG_TBL), 500);//发送寄存器配置
	if(ret != HAL_OK) return GT9147_COMM_ERROR;

	ret = I2C_Soft_Master_Transmit(hi2c, GT9X_CHECK_REG, buf, 2, 500);    //写入校验和,和配置更新标记
	if(ret != HAL_OK) return GT9147_COMM_ERROR;

	return GT9147_OK;
}

/**
 * @brief  初始化GT9147触摸屏
 * @note   配置RST输出低电平,INT输出低（或者高）电平后,位置100us以上,
 *         此阶段配置IIC地址为 0xBA/0xBB(或者0x28/0x29)
 *         接着配置RST输出高电平并维持5ms以上后,可以配置INT引脚为悬浮输入模式.
 *         等待50ms以上后,可以发送配置信息.
 * @param  htouch: 触摸屏句柄
 * @param  hi2c:   屏幕连接的I2C的句柄
 * @return int8_t: GT9147_OK:成功, GT9147_ERROR:错误, GT9147_COMM_ERROR:通信错误
 */
int8_t GT9147_Init(GT9147_HandleTypeDef *htouch){
	uint8_t temp[5];
	int8_t ret;
	GPIO_InitTypeDef GPIO_Initure;

	if(htouch == NULL) return GT9147_ERROR;

	GPIO_Initure.Pin = htouch->INT_Pin;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  //输出
	GPIO_Initure.Pull = GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;     //高速
	HAL_GPIO_Init(htouch->INT_Port, &GPIO_Initure);

	GPIO_Initure.Pin = htouch->RST_Pin;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; //推挽输出,上拉,高速
	HAL_GPIO_Init(htouch->RST_Port, &GPIO_Initure);

	//初始化电容屏的I2C总线,(必须在发送之前)
	ret = I2C_Soft_Init(htouch->hi2c);
	if(ret == HAL_OK){
#if GT9147_DEBUG
		printf("触摸屏I2C OK");        //打印ID
#endif
	}
	else{
#if GT9147_DEBUG
		printf("触摸屏I2C ERR:%d",ret);        //打印ID
#endif
		return GT9147_COMM_ERROR;
	}

	Touch_RST_WRITE(0);     //复位
	HAL_Delay(1);           //INT引脚电平维持100us以上
	Touch_RST_WRITE(1);     //释放复位
	HAL_Delay(7);           //释放复位后维持5ms以上，然后设置INT为浮空输入

	GPIO_Initure.Pin = htouch->INT_Pin;
	GPIO_Initure.Mode = GPIO_MODE_IT_RISING;
	GPIO_Initure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(htouch->INT_Port, &GPIO_Initure);    //初始化
	HAL_Delay(50);

	ret = I2C_Soft_Master_Receive(htouch->hi2c, GT9X_PID_REG, temp, 4, 500); //读取产品ID寄存器
	if(ret != HAL_OK) return GT9147_COMM_ERROR;
	if(strcmp((char*)temp,"9147")==0){

#if GT9147_DEBUG
		printf("ID OK:0x%s\r\n", temp);        //打印ID
#endif
	}else{

#if GT9147_DEBUG
		printf("ID NO:0x%s\r\n", temp);        //打印ID
#endif
	}

	temp[4] = 0;

	temp[0]=0X02;
	ret = I2C_Soft_Master_Transmit(htouch->hi2c, GT9X_CTRL_REG, temp, 1, 500);//软复位GT9147
	if(ret != HAL_OK) return GT9147_COMM_ERROR;
	HAL_Delay(10);

	ret = I2C_Soft_Master_Receive(htouch->hi2c, GT9X_CFGS_REG, temp, 1, 500);//读取GT_CFGS_REG寄存器
	if(ret != HAL_OK) return GT9147_COMM_ERROR;

	if(temp[0]<0X60)//默认版本比较低,需要更新flash配置
	{

#if GT9147_DEBUG
		printf("Default Ver:%d\r\n",temp[0]);
#endif

		ret = GT9147_Send_Cfg(0, htouch->hi2c);//更新配置，不保存到flash
		if(ret != GT9147_OK) return ret;
	}
	HAL_Delay(10);
	
	temp[0]=0X00;
	ret = I2C_Soft_Master_Transmit(htouch->hi2c, GT9X_CTRL_REG, temp, 1,500);//结束复位
	if(ret != HAL_OK) return GT9147_COMM_ERROR;

	return GT9147_OK;
}

/**
 * @brief  轮询扫描GT9147触摸屏
 * @param  htouch: 触摸屏句柄
 * @param  hi2c:   屏幕连接的I2C的句柄
 * @return 执行情况
 */
int8_t GT9147_Scan(GT9147_HandleTypeDef *htouch){
	uint8_t Status,num;
	uint8_t i;
	uint8_t buf[4];//存储一个坐标数
	int8_t ret;
	const uint16_t TP_REG_Array[] = {
		GT9X_TP1_REG, GT9X_TP2_REG, GT9X_TP3_REG, GT9X_TP4_REG, GT9X_TP5_REG
	};

	if(htouch == NULL) return GT9147_ERROR;

	ret = I2C_Soft_Master_Receive(htouch->hi2c, GT9X_GSTID_REG, &Status, 1 ,500);//读取触摸状态寄存器
	if(ret != HAL_OK) return GT9147_COMM_ERROR;

	if((Status & 0x80) && ((Status & 0x0F) <= 5)){//有触摸且触摸点个数小于等于5
		/*清除状态寄存器标志*/
		i = 0;
		I2C_Soft_Master_Transmit(htouch->hi2c, GT9X_GSTID_REG, &i, 1,500);//清标志
		num = Status & 0x0F;//获取触摸点个数

		for(i=0;i<num;i++){
			/* 读取触摸点坐标 */
			I2C_Soft_Master_Receive(htouch->hi2c, TP_REG_Array[i], buf, 4, 500);

			if(htouch->H_V==GT9147_VERTICAL){    //竖屏,GT9147默认输出的坐标是竖屏的
				htouch->Point[i].X = ((uint16_t)(buf[1]<<8) + buf[0]);
				htouch->Point[i].Y = ((uint16_t)(buf[3]<<8) + buf[2]);
			}
			else{                                //横屏(480x800屏幕)，转换为横屏坐标
				htouch->Point[i].Y = ((uint16_t)(buf[1]<<8) + buf[0]);
				htouch->Point[i].X = 800 - ((uint16_t)(buf[3]<<8) + buf[2]);
			}
#if GT9147_DEBUG
			printf("X:%d,Y:%d\r\n",htouch->Point[i].X,htouch->Point[i].Y);
#endif
		}

	}

	return GT9147_OK;
}
