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
	SetMotorDirection(MOTOR_A | MOTOR_B, true);
	SetMotorSpeed(MOTOR_A, speed);
	SetMotorSpeed(MOTOR_B, speed);
}

void move_backward(uint8_t speed)
{
	SetMotorDirection(MOTOR_A | MOTOR_B, false);
	SetMotorSpeed(MOTOR_A, speed);
	SetMotorSpeed(MOTOR_B, speed);
}

void turn_right(uint8_t speed)
{
	SetMotorDirection(MOTOR_A | MOTOR_B, true);
	SetMotorSpeed(MOTOR_A, speed);
	SetMotorSpeed(MOTOR_B, 0);
}

void turn_left(uint8_t speed)
{
	SetMotorDirection(MOTOR_A | MOTOR_B, true);
	SetMotorSpeed(MOTOR_A, 0);
	SetMotorSpeed(MOTOR_B, speed);
}
