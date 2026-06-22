/*
 * motors.c
 *
 *  Created on: Apr 27, 2026
 *      Author: ugklp
 */

#include "motors.h"
#include "main.h"
#include "utils.h"

extern TIM_HandleTypeDef htim1;

void InitMotors(void)
{
	StopMotor(MOTOR_A | MOTOR_B); //Ensure the GPIO is set to that the motors will start stopped.
	SetMotorSpeed(MOTOR_A | MOTOR_B, 0, true); //Ensure the PWM duty cycles are set to 0 before starting the timers.
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
}

void StopMotor(MOTOR motor)
{
	/*
	  To perform stop operation, the PWM signal needs to be left high (100% duty cycle).
	*/
	if (motor == MOTOR_A)
	{
		HAL_GPIO_WritePin(MotorA_In1_GPIO_Port, MotorA_In1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MotorA_In2_GPIO_Port, MotorA_In2_Pin, GPIO_PIN_RESET);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, htim1.Instance->ARR + 1); //Leave PWM at 100% duty cycle (PWM Mode 1)
	}
	else if (motor == MOTOR_B)
	{
		HAL_GPIO_WritePin(MotorB_In1_GPIO_Port, MotorB_In1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MotorB_In2_GPIO_Port, MotorB_In2_Pin, GPIO_PIN_RESET);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, htim1.Instance->ARR + 1);//Leave PWM at 100% duty cycle (PWM Mode 1)
	}
	else if (motor == (MOTOR_A | MOTOR_B))
	{
		StopMotor(MOTOR_A);
		StopMotor(MOTOR_B);
	}
}

void BrakeMotor(MOTOR motor)
{
	/*
	 Both inputs being high is the only option that performs a short brake regardless of the PWM signal.
	 Implementing this as the brake function also allows us to leave PWM signal alone in this function.
	*/
	if (motor == MOTOR_A)
	{
		HAL_GPIO_WritePin(MotorA_In1_GPIO_Port, MotorA_In1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MotorA_In2_GPIO_Port, MotorA_In2_Pin, GPIO_PIN_SET);
	}
	else if (motor == MOTOR_B)
	{
		HAL_GPIO_WritePin(MotorB_In1_GPIO_Port, MotorB_In1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MotorB_In2_GPIO_Port, MotorB_In2_Pin, GPIO_PIN_SET);
	}
	else if (motor == (MOTOR_A | MOTOR_B))
	{
		BrakeMotor(MOTOR_A);
		BrakeMotor(MOTOR_B);
	}
}

void SetMotorDirection(MOTOR motor, bool forward)
{
	if (forward)
	{
		SetMotorDirection_CW(motor);
	}
	else
	{
		SetMotorDirection_CCW(motor);
	}
}

void SetMotorDirection_CCW(MOTOR motor)
{
	if (motor == MOTOR_A)
	{
		HAL_GPIO_WritePin(MotorA_In1_GPIO_Port, MotorA_In1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MotorA_In2_GPIO_Port, MotorA_In2_Pin, GPIO_PIN_SET);
	}
	else if (motor == MOTOR_B)
	{
		HAL_GPIO_WritePin(MotorB_In1_GPIO_Port, MotorB_In1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MotorB_In2_GPIO_Port, MotorB_In2_Pin, GPIO_PIN_SET);
	}
	else if (motor == (MOTOR_A | MOTOR_B))
	{
		SetMotorDirection_CCW(MOTOR_A);
		SetMotorDirection_CCW(MOTOR_B);
	}
}

void SetMotorDirection_CW(MOTOR motor)
{
	if (motor == MOTOR_A)
	{
		HAL_GPIO_WritePin(MotorA_In1_GPIO_Port, MotorA_In1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MotorA_In2_GPIO_Port, MotorA_In2_Pin, GPIO_PIN_RESET);
	}
	else if (motor == MOTOR_B)
	{
		HAL_GPIO_WritePin(MotorB_In1_GPIO_Port, MotorB_In1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MotorB_In2_GPIO_Port, MotorB_In2_Pin, GPIO_PIN_RESET);
	}
	else if (motor == (MOTOR_A | MOTOR_B))
	{
		SetMotorDirection_CW(MOTOR_A);
		SetMotorDirection_CW(MOTOR_B);
	}
}

void SetMotorSpeed(MOTOR motor, uint8_t speed, bool forward)
{
	//The direction needs to be set first in order to make
	//sure the motors are set to moving and not stopping
	SetMotorDirection(motor, forward);

	uint32_t ccr = map(speed, 0, 255, 0, htim1.Instance->ARR + 1);
	if (motor == MOTOR_A)
	{
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, ccr);
	}
	else if (motor == MOTOR_B)
	{
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, ccr);
	}
	else if (motor == (MOTOR_A | MOTOR_B))
	{
		SetMotorSpeed(MOTOR_A, speed, forward);
		SetMotorSpeed(MOTOR_B, speed, forward);
	}
}
