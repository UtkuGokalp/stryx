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
#include "nRF24L01.h"
#include "utils.h"

void robot_main(void)
{
	radiopacket_t joystick_info = { 0 };
	InitMotors();
	InitBatteryDisplay();
	InitDistanceSensor();
	InitRadio();

	while (1)
	{
		if (IsPacketReady())
		{
			ReadPacket(&joystick_info);
		}

		if (GetDistanceInCm() <= 20.0f)
		{
			//Detected obstacle, go back a bit
			move_backward(128);
			HAL_Delay(1000);
			StopMotor(MOTOR_A | MOTOR_B);
		}
		else
		{
			uint8_t speed_a = 0, speed_b = 0;
			int16_t x_speed = (int16_t)joystick_info.x_right - 512;
			int16_t y_speed = (int16_t)joystick_info.y_right - 512;
			if (x_speed > 0) //Want to move toward right
			{
				speed_a = speed_b + map(x_speed, 0, 512, 0, 128);
			}
			else if (x_speed < 0) //Want to move toward left
			{
				speed_b = speed_a + map(-x_speed, 0, 512, 0, 128);
			}
			else //x_speed = 0
			{

			}

			SetMotorSpeed(MOTOR_A, speed_a);
			SetMotorSpeed(MOTOR_B, speed_b);
			continue;

			if (y_speed > 0) //Want to move forward
			{
				move_forward(map(y_speed, 0, 512, 0, 255));
			}
			else if (y_speed < 0) //Want to move backward
			{
				move_backward(map(-y_speed, 0, 512, 0, 255));
			}
			else //y_speed = 0
			{

			}
		}

	}
}
