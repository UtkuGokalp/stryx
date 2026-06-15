/*
 * nRF24L01.h
 *
 *  Created on: Jun 14, 2026
 *      Author: ugklp
 */

#ifndef NRF24L01_NRF24L01_H_
#define NRF24L01_NRF24L01_H_

#include "main.h"
#include <stdint.h>
#include <stddef.h>

typedef struct radiopacket_t
{
	uint8_t sender_id;
	uint8_t packet_type;
	uint8_t btn_right;
	uint8_t btn_left;
	uint16_t x_right;
	uint16_t x_left;
	uint16_t y_right;
	uint16_t y_left;
} radiopacket_t;

void InitRadio(void);
uint8_t IsPacketReady();
radiopacket_t ReadPacket(void);

#endif /* NRF24L01_NRF24L01_H_ */
