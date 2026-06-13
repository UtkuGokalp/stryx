/*
 * neopixel.h
 *
 *  Created on: Apr 19, 2026
 *      Author: ugklp
 */

#ifndef NEOPIXEL_NEOPIXEL_H_
#define NEOPIXEL_NEOPIXEL_H_

#include <stdint.h>

void init_neopixel_driver(void);
void send_neopixel_signal(uint8_t r, uint8_t g, uint8_t b);

#endif /* NEOPIXEL_NEOPIXEL_H_ */
