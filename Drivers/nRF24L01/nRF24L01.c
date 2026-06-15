/*
 * nRF24L01.c
 *
 *  Created on: Jun 14, 2026
 *      Author: ugklp
 */

#include "nRF24L01.h"
#include <stdbool.h>
#include <string.h>
#include <utils.h>

#define MAX_DATA_BYTE_COUNT				32

#define READ_REGISTER_CMD(reg)			(reg)
#define WRITE_REGISTER_CMD(reg)			(0x20 | reg)
#define READ_RX_PAYLOAD_CMD				0x61
#define FLUSH_RX_CMD					0xE2
#define NOP_CMD							0xFF

#define REG_CONFIG 				 	 0x00
#define REG_EN_AA 					 0x01
#define REG_EN_RX_ADDR 				 0x02
#define REG_SETUP_AW 				 0x03
#define REG_SETUP_RETR 				 0x04
#define REG_RF_CH 				 	 0x05
#define REG_RF_SETUP 				 0x06
#define REG_STATUS 				 	 0x07
#define REG_OBSERVE_TX 				 0x08
#define REG_CD 				 		 0x09
#define REG_RX_ADDR_P0 				 0x0A
#define REG_RX_ADDR_P1 				 0x0B
#define REG_RX_ADDR_P2 				 0x0C
#define REG_RX_ADDR_P3 				 0x0D
#define REG_RX_ADDR_P4 				 0x0E
#define REG_RX_ADDR_P5 				 0x0F
#define REG_TX_ADDR 				 0x10
#define REG_RX_PW_P0 				 0x11
#define REG_RX_PW_P1 				 0x12
#define REG_RX_PW_P2 				 0x13
#define REG_RX_PW_P3 				 0x14
#define REG_RX_PW_P4 				 0x15
#define REG_RX_PW_P5 				 0x16
#define REG_FIFO_STATUS 			 0x17
#define REG_DYNPD		 			 0x1C
#define REG_FEATURE		 			 0x1D

extern SPI_HandleTypeDef hspi3;

HAL_StatusTypeDef SendCommand(uint8_t command, uint8_t* data_bytes, size_t data_byte_count, uint8_t* received_data)
{
	if (data_byte_count > MAX_DATA_BYTE_COUNT)
	{
		data_byte_count = MAX_DATA_BYTE_COUNT;
	}
	uint8_t txData[MAX_DATA_BYTE_COUNT + 1] = { 0 };
	txData[0] = command;
	if (data_bytes == NULL)
	{
		data_byte_count = 0; //In case the user passed something wrong
	}
	else
	{
		memcpy(txData + 1, data_bytes, data_byte_count);
	}

	HAL_GPIO_WritePin(nRF24L01_CSN_GPIO_Port, nRF24L01_CSN_Pin, GPIO_PIN_RESET); //Start talking to the chip

	uint8_t temp_rx_buffer[MAX_DATA_BYTE_COUNT + 1] = { 0 }; //Buffer to hold the received data so the first byte received can be discarded for making the API simpler
	HAL_StatusTypeDef result = HAL_SPI_TransmitReceive(&hspi3, txData, temp_rx_buffer, data_byte_count + 1, 1000);
	if (received_data != NULL)
	{
		//If the user actually cares about the data, give it to them (after discarding the very
		//first byte)
		memcpy(received_data, temp_rx_buffer + 1, data_byte_count);
	}

	HAL_GPIO_WritePin(nRF24L01_CSN_GPIO_Port, nRF24L01_CSN_Pin, GPIO_PIN_SET); //Release the chip
	return result;
}

//The caller is responsible for making sure result pointer points to a memory location that can
//actually hold the amount of bytes necessary.
void ReadRegister(uint8_t reg, uint8_t bytesToRead, uint8_t* result)
{
	#define MAX_REGISTER_WIDTH_IN_BYTES		5

	if (bytesToRead > MAX_REGISTER_WIDTH_IN_BYTES)
	{
		bytesToRead = MAX_REGISTER_WIDTH_IN_BYTES;
	}
	uint8_t dummy_bytes[MAX_REGISTER_WIDTH_IN_BYTES] = { 0x00 };
	SendCommand(READ_REGISTER_CMD(reg), dummy_bytes, bytesToRead, result);

	#undef MAX_REGISTER_WIDTH_IN_BYTES
}

void WriteRegister()
{
	//TODO: IMPLEMENT
}

void ReadRxPayload()
{
	//TODO: IMPLEMENT
}

void FlushRxFIFO()
{
	/*
	  Flush RX FIFO, used in RX mode
Should not be executed during transmission of
acknowledge, that is, acknowledge package will
not be completed.
	*/
	//TODO: IMPLEMENT
}

void NOP()
{
	SendCommand(NOP_CMD, NULL, 0, NULL);
}

void InitRadio(void)
{
	HAL_GPIO_WritePin(nRF24L01_CE_GPIO_Port, nRF24L01_CE_Pin, GPIO_PIN_SET); //Enable the chip
	HAL_GPIO_WritePin(nRF24L01_CSN_GPIO_Port, nRF24L01_CSN_Pin, GPIO_PIN_SET); //Start the chip in released mode
	//For some reason, the very first SPI transmission behaves weirdly for some reason
	//(bits shifted or zeroed out, seemingly randomly). Using this NOP here essentially
	//creates a dummy SPI transmission, which seems to fix every subsequent one.
	NOP();
}
