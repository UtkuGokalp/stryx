/*
 * motors.h
 *
 *  Created on: Apr 27, 2026
 *      Author: ugklp
 */

#ifndef MOTORS_INCLUDE_MOTORS_H_
#define MOTORS_INCLUDE_MOTORS_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
	MOTOR_NONE = 0,
	MOTOR_A,
	MOTOR_B,
} MOTOR;

void InitMotors(void);
void StopMotor(MOTOR motor);
void BrakeMotor(MOTOR motor);
void SetMotorDirection(MOTOR motor, bool forward);
void SetMotorDirection_CCW(MOTOR motor);
void SetMotorDirection_CW(MOTOR motor);
void SetMotorSpeed(MOTOR motor, uint8_t speed, bool forward);

#endif /* MOTORS_INCLUDE_MOTORS_H_ */
