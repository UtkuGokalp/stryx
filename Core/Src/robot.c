/*
 * robot.c
 *
 *  Created on: Apr 7, 2026
 *      Author: ugklp
 */

#include <string.h>
#include "robot.h"
#include "main.h"
#include "batt_mon.h"
#include "neopixel.h"
#include "motors.h"
#include "movement.h"
#include "HCSR04.h"

typedef struct
{
	uint8_t battLevel, r, g, b;
} BatteryLevelIndicator_t;

void UpdateBatteryLevelDisplay(void)
{
	BatteryLevelIndicator_t levels[] =
	{
		(BatteryLevelIndicator_t) { .battLevel = 255, .r =   0, .g = 255, .b = 0 }, //100%
		(BatteryLevelIndicator_t) { .battLevel = 230, .r =  25, .g = 255, .b = 0 }, // 90%
		(BatteryLevelIndicator_t) { .battLevel = 180, .r =  51, .g = 255, .b = 0 }, // 80%
		(BatteryLevelIndicator_t) { .battLevel = 158, .r = 102, .g = 255, .b = 0 }, // 70%
		(BatteryLevelIndicator_t) { .battLevel = 153, .r = 153, .g = 255, .b = 0 }, // 60%
		(BatteryLevelIndicator_t) { .battLevel = 128, .r = 255, .g = 255, .b = 0 }, // 50%
		(BatteryLevelIndicator_t) { .battLevel = 102, .r = 255, .g = 170, .b = 0 }, // 40%
		(BatteryLevelIndicator_t) { .battLevel =  77, .r = 255, .g =  85, .b = 0 }, // 30%
		(BatteryLevelIndicator_t) { .battLevel =  51, .r = 255, .g =  35, .b = 0 }, // 20%
		(BatteryLevelIndicator_t) { .battLevel =  25, .r = 255, .g =   0, .b = 0 }, // 10%
		(BatteryLevelIndicator_t) { .battLevel =   0, .r =   0, .g =   0, .b = 0 }, //  0%
	};

	uint8_t percent = GetBatteryPercent();
	for (size_t i = 0; i < sizeof(levels) / sizeof(levels[0]); i++)
	{
		if (percent <= levels[i].battLevel && percent >= levels[i + 1].battLevel)
		{
			send_neopixel_signal(levels[i].r, levels[i].g, levels[i].b);
		}
	}
}

float distance_cm = 21.0f;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	if (htim->Instance == TIM7) //If battery level indication needs updating
	{
		UpdateBatteryLevelDisplay();
	}
	else if (htim->Instance == TIM6) //If distance needs to be measured again
	{
		StartDistanceMeasurement();
		distance_cm = get_distance_cm(); //Calculate distance
	}
}

void robot_main(void)
{
	InitMotors();
	InitBatteryMonitor();
	InitDistanceSensor();

	while (1)
	{
		if (distance_cm >= 20.0f)
		{
			move_forward(128);
		}
		else
		{
			StopMotor(MOTOR_A | MOTOR_B);
			//Wall detected, turn around
			//move_backward(128);
			//HAL_Delay(1000);
			//turn_left(128);
			//HAL_Delay(2000);
		}
	}
}
