/*
 * robot.c
 *
 *  Created on: Apr 7, 2026
 *      Author: ugklp
 */

#include "robot.h"
#include "main.h"
#include "battery_display.h"
#include "HCSR04.h"
#include "movement.h"
#include "motors.h"

void robot_main(void)
{
	InitMotors();
	InitBatteryDisplay();
	InitDistanceSensor();

	while (1)
	{
		if (GetDistanceInCm() >= 20.0f)
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
