/*
 * adc.c
 *
 *  Created on: Oct 21, 2021
 *      Author: Administrator
 */

#include "adc.h"

uint16_t ADC_IN_1(void) //ADC1采样
{
	HAL_ADC_Start(&hadc1);//开始转化
	HAL_ADC_PollForConversion(&hadc1,500);//等待结束
	if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))//判断标志位
	{
		return HAL_ADC_GetValue(&hadc1);//返回转换值ֵ
	}
	return 0;
}

uint16_t ADC_IN_2(void) //ADC2采样
{
	HAL_ADC_Start(&hadc2);//
	HAL_ADC_PollForConversion(&hadc2,500);//
	if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc2), HAL_ADC_STATE_REG_EOC))//
	{
		return HAL_ADC_GetValue(&hadc2);//
	}
	return 0;
}
