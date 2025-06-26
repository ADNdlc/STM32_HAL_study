#include "GT9147.h"

GT9147_HandleTypeDef	GT9147 = {
	.INT_Pin = GPIO_PIN_7,
	.INT_Port = GPIOH,
	.RST_Pin = GPIO_PIN_8,
	.RST_Port = GPIOI,
	.H_V = GT9147_VERTICAL,
	.Point = {{0}}

};
