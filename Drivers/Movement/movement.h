/*
 * movement.h
 *
 *  Created on: Apr 29, 2026
 *      Author: ugklp
 */

#ifndef MOVEMENT_MOVEMENT_H_
#define MOVEMENT_MOVEMENT_H_

#include <stdint.h>

void move_forward(uint8_t speed);
void move_backward(uint8_t speed);
void turn_right(uint8_t speed);
void turn_left(uint8_t speed);

#endif /* MOVEMENT_MOVEMENT_H_ */
