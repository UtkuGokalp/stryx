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
#include "joystick.h"

void robot_main(void)
{
	radiopacket_t joystick_info = { 0 };
	InitMotors();
	InitBatteryDisplay();
	InitDistanceSensor();
	InitRadio();

	while (1)
	{
		//if (GetDistanceInCm() <= 20.0f) //Detected obstacle
		//{
		//	if (joystick_info.y_right > JOYSTICK_DEADZONE) //If the user is trying to go forward
		//	{
		//		//Go back a bit
		//		StopMotor(MOTOR_A | MOTOR_B);
		//		HAL_Delay(50); //Wait for the motors to be done with inertia
		//		move_backward(128);
		//		HAL_Delay(1000);
		//		StopMotor(MOTOR_A | MOTOR_B);
		//	}
		//}
		//else
		//{
			if (IsPacketReady())
			{
				ReadPacket(&joystick_info);
			}
			if (GetDistanceInCm() <= 20.0f)
			{
				//joystick_info.y_right = 0;
				//Go back a bit
				StopMotor(MOTOR_A | MOTOR_B);
				HAL_Delay(50); //Wait for the motors to be done with inertia
				move_backward(128);
				HAL_Delay(1000);
				StopMotor(MOTOR_A | MOTOR_B);
			}
			DriveFromJoystick(joystick_info.x_right, joystick_info.y_right);
		//}
	}
}
