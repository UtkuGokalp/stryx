/*
 * joystick.c
 *
 *  Created on: Jun 17, 2026
 *      Author: ugklp
 */

#include "joystick.h"
#include "motors.h"
#include "utils.h"

#define JOY_CENTER 512
#define DEADZONE   30

static int16_t applyDeadzone(int16_t v)
{
    if (v > -DEADZONE && v < DEADZONE)
        return 0;
    return v;
}

static int16_t normalizeAxis(uint16_t raw)
{
    // Convert 0–1023 → -512 to +511
    return (int16_t)raw - JOY_CENTER;
}

static void setMotorSigned(int16_t value, MOTOR motor)
{
	bool forward = true;
    if (value < 0)
    {
    	forward = false;
    	value = -value;
    }
    SetMotorSpeed(motor, (uint8_t)value, forward);
}

void DriveFromJoystick(uint16_t joyX, uint16_t joyY)
{
	//y axis directly controls the base speed of each motor
	//x axis is the turn modifier -> x to the left means more speed to the right motor and vice versa

    int16_t x = normalizeAxis(joyX);
    int16_t y = normalizeAxis(joyY);

    x = applyDeadzone(x);
    y = applyDeadzone(y);

    // Optional scaling to avoid diagonal overdrive
    int32_t left  = clamp(y + x, -512, 512); //y + x
    int32_t right = clamp(y - x, -512, 512); //y - x

    // Scale -512..512 → 0..255
    left  = (left  * 255) / 512;
    right = (right * 255) / 512;

    setMotorSigned(left,  MOTOR_A);
    setMotorSigned(right, MOTOR_B);
}
