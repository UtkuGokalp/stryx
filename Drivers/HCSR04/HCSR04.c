/*
 * HCSR04.c
 *
 *  Created on: May 2, 2026
 *      Author: ugklp
 */

#include "main.h"
#include "HCSR04.h"
#include "stdbool.h"

#define BUFFER_SIZE 16

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim15;
static bool calculatingDistance = false;
uint32_t pulseLengths[BUFFER_SIZE] = { 0 };

void InitDistanceSensor(void)
{
	HAL_TIM_Base_Start_IT(&htim6);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
}

void StartDistanceMeasurement(void) //Called every ~60 ms
{
	//Send a 10 us pulse
	HAL_TIM_PWM_Stop(&htim15, TIM_CHANNEL_1);
	__HAL_TIM_SET_COUNTER(&htim15, 0);
	HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_1);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim)
{
	if (htim == &htim2)
	{
		if (calculatingDistance)
		{
			//Don't interfere if there is a currently ongoing calculation
			return;
		}
		static uint32_t pulseStart = 0, pulseEnd = 0;
		static int bufferIndex = 0;
		static bool isRisingEdge = true; //First edge of HCSR04 is a rising edge
		if (isRisingEdge)
		{
			pulseStart = htim->Instance->CNT;
		}
		else
		{
			pulseEnd = htim->Instance->CNT;

			//We captured how long the pulse is. Now process it
			if (pulseStart >= pulseEnd)
			{
				//Edge case but I've seen it happen. If it happens, simply ignore it
				return;
			}
			pulseLengths[bufferIndex] = pulseEnd - pulseStart;
			bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;

		}
		isRisingEdge = !isRisingEdge;
	}
}

float get_distance_cm(void)
{
	//As long as buffer size is small enough, the CPU can loop through it to calculate the sum.
	//TODO: Maybe make this more efficient by storing a sum or something.
	calculatingDistance = true;
	uint32_t sum = 0;
	for (int i = 0; i < BUFFER_SIZE; i++)
	{
		sum += pulseLengths[i];
	}
	float avg = (float)sum / BUFFER_SIZE;
	float distance_um = (avg * 340.0f) / 2.0f; //Calculation from the datasheet (sound of speed [m/s] in air divided by 2)
	calculatingDistance = false;
	return distance_um / 1e4; //Convert to cm

}
