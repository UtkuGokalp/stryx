/*
 * neopixel.c
 *
 *  Created on: Apr 19, 2026
 *      Author: ugklp
 */

#include "neopixel.h"
#include "main.h"

extern TIM_HandleTypeDef htim1;

//For whatever reason, if the array is 24 bits, the timer sends 28 bits but when
//the array (and the size passed to HAL_TIM_PWM_Start_DMA) is 28, it sends 24 bits
uint8_t packets[28] = { 0 };

void send_neopixel_signal(uint8_t r, uint8_t g, uint8_t b)
{
	uint32_t packet = (b << 16) | (r << 8) | g;

	for (uint32_t i = 0; i < 24; i++)
	{
		if ((packet >> i) & 1)
		{
			packets[i] = 26 - 1;
		}
		else
		{
			packets[i] = 13 - 1;
		}
	}

	HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t*)packets, 28);
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim1)
	{
		__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, 0);
	}
}
