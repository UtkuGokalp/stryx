/*
 * batt_mon.c
 *
 *  Created on: Apr 18, 2026
 *      Author: ugklp
 */

#include <stdint.h>
#include "batt_mon.h"
#include "main.h"
#include "utils.h"

#define BUFF_SIZE 8

extern ADC_HandleTypeDef hadc1;
static uint16_t buff[BUFF_SIZE] = { 0 };
uint8_t percentage = 0;

void InitBatteryMonitor(void)
{
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)buff, BUFF_SIZE);
}

uint8_t GetBatteryPercent(void)
{
	uint32_t sum = 0; //Sum can be larger than max 16 bit so use 32 bits
	for (int i = 0; i < BUFF_SIZE; i++)
	{
		sum += buff[i];
	}
	uint16_t avg = sum / BUFF_SIZE;
	percentage = map(avg, 2300, 3180, 0, 255);
	return percentage;
}
