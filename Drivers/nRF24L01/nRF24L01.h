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

typedef struct __attribute__((packed)) radiopacket_t
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

//This function is meant to be called once at startup, therefore it uses HAL_Delay() internally
//to ensure correct operation without adding too much complexity or hardware usage. Be aware.
void InitRadio(void);
//Returns if a packet is ready to be read from the chip.
uint8_t IsPacketReady();
//Reads the packet if packet is ready, does nothing otherwise.
void ReadPacket(radiopacket_t* packet);
//Prints the register values in the chip for debugging (through USB).
void PrintRegisters(void);

#endif /* NRF24L01_NRF24L01_H_ */
