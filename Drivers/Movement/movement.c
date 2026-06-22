/*
 * movement.c
 *
 *  Created on: Apr 29, 2026
 *      Author: ugklp
 */

#include "movement.h"
#include "motors.h"

void move_forward(uint8_t speed)
{
	SetMotorSpeed(MOTOR_A, speed, true);
	SetMotorSpeed(MOTOR_B, speed, true);
}

void move_backward(uint8_t speed)
{
	SetMotorSpeed(MOTOR_A, speed, false);
	SetMotorSpeed(MOTOR_B, speed, false);
}

void turn_right(uint8_t speed)
{
	SetMotorSpeed(MOTOR_A, speed, true);
	SetMotorSpeed(MOTOR_B, 0, true);
}

void turn_left(uint8_t speed)
{
	SetMotorSpeed(MOTOR_A, 0, true);
	SetMotorSpeed(MOTOR_B, speed, true);
}
