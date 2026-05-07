/*
 * HCSR04.c
 *
 *  Created on: May 2, 2026
 *      Author: ugklp
 */

#include "main.h"
#include "HCSR04.h"
#include "stdbool.h"

#define BUFFER_SIZE 3

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim15;

float distances[BUFFER_SIZE] = { 0 };

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
			uint32_t pulseLength = pulseEnd - pulseStart;
			float distance_um = (pulseLength * 340.0f) / 2.0f; //Calculation from the datasheet based on the speed of sound
			float distance_cm = distance_um / 1e4; //Convert to cm
			if (distance_cm >= 2.0f && distance_cm <= 400.0f)
			{
				//The range of the sensor is between 2 and 400 cm. Reject anything else.
				distances[bufferIndex] = distance_cm;
				bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;
			}
		}
		isRisingEdge = !isRisingEdge;
	}
}

int median3(int a, int b, int c) {
    if (a > b) { int t = a; a = b; b = t; }
    if (b > c) { int t = b; b = c; c = t; }
    if (a > b) { int t = a; a = b; b = t; }
    return b;
}

float get_distance_cm(void)
{
	return median3(distances[0], distances[1], distances[2]);
}
