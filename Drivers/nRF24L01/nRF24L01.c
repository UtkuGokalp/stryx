/*
 * nRF24L01.c
 *
 *  Created on: Jun 14, 2026
 *      Author: ugklp
 */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "nRF24L01.h"
#include "utils.h"
#include "usbd_cdc_if.h"

#define MAX_DATA_BYTE_COUNT				32		//Maximum number of bytes allowed by the chip when sending a command (excluding the command byte)
#define RADIO_ADDR						"CNTRL"

#define READ_REGISTER_CMD(reg)			(reg)
#define WRITE_REGISTER_CMD(reg)			(0x20 | reg)
#define READ_RX_PAYLOAD_CMD				0x61
#define FLUSH_RX_CMD					0xE2
#define NOP_CMD							0xFF

#define REG_CONFIG 				 	 0x00 //1 byte register
#define REG_EN_AA 					 0x01 //1 byte register
#define REG_EN_RX_ADDR 				 0x02 //1 byte register
#define REG_SETUP_AW 				 0x03 //1 byte register
#define REG_SETUP_RETR 				 0x04 //1 byte register
#define REG_RF_CH 				 	 0x05 //1 byte register
#define REG_RF_SETUP 				 0x06 //1 byte register
#define REG_STATUS 				 	 0x07 //1 byte register
#define REG_OBSERVE_TX 				 0x08 //1 byte register
#define REG_CD 				 		 0x09 //1 byte register
#define REG_RX_ADDR_P0 				 0x0A //5 byte register
#define REG_RX_ADDR_P1 				 0x0B //5 byte register
#define REG_RX_ADDR_P2 				 0x0C //1 byte register
#define REG_RX_ADDR_P3 				 0x0D //1 byte register
#define REG_RX_ADDR_P4 				 0x0E //1 byte register
#define REG_RX_ADDR_P5 				 0x0F //1 byte register
#define REG_TX_ADDR 				 0x10 //5 byte register
#define REG_RX_PW_P0 				 0x11 //1 byte register
#define REG_RX_PW_P1 				 0x12 //1 byte register
#define REG_RX_PW_P2 				 0x13 //1 byte register
#define REG_RX_PW_P3 				 0x14 //1 byte register
#define REG_RX_PW_P4 				 0x15 //1 byte register
#define REG_RX_PW_P5 				 0x16 //1 byte register
#define REG_FIFO_STATUS 			 0x17 //1 byte register
#define REG_DYNPD		 			 0x1C //1 byte register
#define REG_FEATURE		 			 0x1D //1 byte register

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
//The result is in little endian.
void ReadRegister(uint8_t reg, uint8_t bytesCount, uint8_t* result)
{
	#define MAX_REGISTER_WIDTH_IN_BYTES		5

	if (bytesCount > MAX_REGISTER_WIDTH_IN_BYTES)
	{
		bytesCount = MAX_REGISTER_WIDTH_IN_BYTES;
	}
	uint8_t dummy_bytes[MAX_REGISTER_WIDTH_IN_BYTES] = { 0x00 };
	SendCommand(READ_REGISTER_CMD(reg), dummy_bytes, bytesCount, result);

	#undef MAX_REGISTER_WIDTH_IN_BYTES
}

void WriteRegister(uint8_t reg, uint8_t* data, size_t byteCount)
{
	SendCommand(WRITE_REGISTER_CMD(reg), data, byteCount, NULL);
}

//A buffer of sizeof(radiopacket_t) bytes is expected.
//TODO: Try to optimize this implementation
void ReadRxPayload(uint8_t* result_buffer)
{
	//Pause listening while reading RX FIFO
	HAL_GPIO_WritePin(nRF24L01_CE_GPIO_Port, nRF24L01_CE_Pin, GPIO_PIN_RESET);

	//TODO: Once the communication is working properly, check to see if you can
	//use the same array for both the dummy data and the received data
	//What bytes are sent to the chip (after the cmd byte) doesn't matter, but the
	//correct bytes being read does matter. So reducing these two into one (if possible)
	//can free a small amount of memory
	uint8_t dummy_data[sizeof(radiopacket_t)] = { 0 };
	uint8_t received_data[sizeof(radiopacket_t)] = { 0 };
	SendCommand(READ_RX_PAYLOAD_CMD, dummy_data, sizeof(radiopacket_t), received_data);

	//TODO: This might be implemented better maybe, without having to copy memory
	memcpy(result_buffer, received_data, sizeof(radiopacket_t));

	uint8_t value = 0x70; //Clear all interrupt flags
	WriteRegister(REG_STATUS, &value, 1);

	//Resume listening
	HAL_GPIO_WritePin(nRF24L01_CE_GPIO_Port, nRF24L01_CE_Pin, GPIO_PIN_SET);
}

//From the datasheet: Should not be executed during transmission
//of ACK, that is, ACK package will not be completed
void FlushRxFIFO(void)
{
	SendCommand(FLUSH_RX_CMD, NULL, 0, NULL);
}

void NOP(void)
{
	SendCommand(NOP_CMD, NULL, 0, NULL);
}

void InitRadio(void)
{
	//This function uses HAL_Delay() to ensure the chip powers up properly, without timing issues.
	//This is not an ideal design choice, but this function is meant to be called once at startup
	//and the highest delay is a little longer than 10ms. The alternative is using peripherals or
	//DWT. I don't think either one justifies the added complexity, given how infrequently
	//this function is meant to be called.

	HAL_Delay(12); //The power on time (after Vdd >= 1.9V) is 10.3ms, give the chip a tiny bit more time than that.

	//Start the chip in released mode to make sure the following SPI communications work properly.
	HAL_GPIO_WritePin(nRF24L01_CSN_GPIO_Port, nRF24L01_CSN_Pin, GPIO_PIN_SET);

	//Make sure the chip is disabled while setting things up.
	HAL_GPIO_WritePin(nRF24L01_CE_GPIO_Port, nRF24L01_CE_Pin, GPIO_PIN_RESET);

	//For some reason, the very first SPI transmission behaves weirdly for some reason
	//(bits shifted or zeroed out, seemingly randomly). Using this NOP here essentially
	//creates a dummy SPI transmission, which seems to fix every subsequent one.
	NOP();

	FlushRxFIFO(); //Ensure we are starting from a clean slate

	//Configure the chip to:
	//interrupt only on RX_DR,
	//enable CRC,
	//use 2 byte CRC
	//power up and
	//to be primarily receiver
	//Refer to datasheet's config register section for more details.
	uint8_t value = 0x3F;
	WriteRegister(REG_CONFIG, &value, 1);

	//The chip needs 1.5ms of settling time after
	//PWR_UP bit in config register is set to 1.
	HAL_Delay(2);

	//Although this should be the default state upon power up, make sure all pipes have
	//auto ack enabled (although we only need pipe 0)
	value = 0x00;//0x3F;
	WriteRegister(REG_EN_AA, &value, 1);

	//Enable all data pipes
	value = 0x3F;
	WriteRegister(REG_EN_RX_ADDR, &value, 1);

	//Although the default value, make sure the address width is set to 5 bytes
	value = 0x03;
	WriteRegister(REG_SETUP_AW, &value, 1);

	//Minimum delay between retries and retry transmission 3 times (this is the defaul
	value = 0x03;
	WriteRegister(REG_SETUP_RETR, &value, 1);

	//Although the default value, make sure the address width is set to 5 bytes
	value = 0x03;
	WriteRegister(REG_SETUP_AW, &value, 1);

	value = 76; //This is the default RF channel used by the Arduino library used for the TX side.
	WriteRegister(REG_RF_CH, &value, 1);

	//Air data rate = 1Mbps, power is 0 dBm and LNA gain is set to 0 (whatever that means in this context, I cannot find the explanation in the datasheet)
	value = 0x06;
	WriteRegister(REG_RF_SETUP, &value, 1);

	value = 0x70; //Clear all interrupt flags
	WriteRegister(REG_STATUS, &value, 1);

	//Tell the chip we are going to get sizeof(radiopacket_t) bytes on pipe 0
	value = sizeof(radiopacket_t);
	WriteRegister(REG_RX_PW_P0, &value, 1);

	//We are using 5 byte address
	uint8_t addr[5] = { 0 };
	for (size_t i = 0; i < 5; i++)
	{
		uint8_t send_reversed = 0;
		addr[i] = (uint8_t)RADIO_ADDR[send_reversed ? (4 - i) : i];
	}
	WriteRegister(REG_RX_ADDR_P0, addr, 5);
	WriteRegister(REG_TX_ADDR, addr, 5); //This will be used when transmitting auto ACK packets back to the main transmitter.

	//Enable the chip's RX mode
	HAL_GPIO_WritePin(nRF24L01_CE_GPIO_Port, nRF24L01_CE_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
}

uint8_t IsPacketReady()
{
	uint8_t result = 0;
	ReadRegister(REG_FIFO_STATUS, 1, &result);
	uint8_t rxFifoEmpty = result & 0x01;
	return !rxFifoEmpty;
}

void ReadPacket(radiopacket_t* packet)
{
	if (packet != NULL && IsPacketReady())
	{
		uint8_t buffer[sizeof(radiopacket_t)] = { 0 };
		ReadRxPayload(buffer);
		*packet = *(radiopacket_t*)buffer;
	}
}

/*
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == nRF24L01_IRQ_Pin)
	{
		//TODO: Check FIFO_STATUS (0x17) register to see if the RX FIFO is full. If it ever is,
		//it means we are unable to process the incoming data fast enough!
		//char msg[32] = { 0 };
		//sprintf(msg, "Data available on RX FIFO\n");
		//CDC_Transmit_FS((uint8_t*)msg, strlen(msg));
		//A new packet is received, process it
		//uint8_t result[12] = { 0 };
		//ReadRxPayload(result);
		//radiopacket_t packet = *(radiopacket_t*)result;
		//(void)packet;

		//TODO: Set internal flag as data ready

		//Write 1 to bit 6 to notify the chip that we handled the interrupt
		uint8_t value = 0;
		ReadRegister(REG_STATUS, 1, &value);
		value |= 0x40;
		WriteRegister(REG_STATUS, &value, 1);
	}
}
*/

void PrintRegisters(void)
{
	const char* reg_names[] =
	{
		"REG_CONFIG",
		"REG_EN_AA",
		"REG_EN_RX_ADDR",
		"REG_SETUP_AW",
		"REG_SETUP_RETR",
		"REG_RF_CH",
		"REG_RF_SETUP",
		"REG_STATUS",
		"REG_OBSERVE_TX",
		"REG_CD",
		"REG_RX_ADDR_P0",
		"REG_RX_ADDR_P1",
		"REG_RX_ADDR_P2",
		"REG_RX_ADDR_P3",
		"REG_RX_ADDR_P4",
		"REG_RX_ADDR_P5",
		"REG_TX_ADDR",
		"REG_RX_PW_P0",
		"REG_RX_PW_P1",
		"REG_RX_PW_P2",
		"REG_RX_PW_P3",
		"REG_RX_PW_P4",
		"REG_RX_PW_P5",
		"REG_FIFO_STATUS",
		"REG_DYNPD",
		"REG_FEATURE",
	};
	size_t name_index = 0;
	for (uint8_t reg = REG_CONFIG; reg <= REG_FEATURE; reg++)
	{
		char msg[128] = { 0 };
		if ((reg == REG_RX_ADDR_P0) || (reg == REG_RX_ADDR_P1) || (reg == REG_TX_ADDR))
		{
			//These are 5 byte registers
			uint8_t result[5] = { 0 };
			ReadRegister(reg, 5, result);
			sprintf(msg, "%s (0x%02X): 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n", reg_names[name_index], reg, result[0], result[1], result[2], result[3], result[4]);
			CDC_Transmit_FS((uint8_t*)msg, strlen(msg));
		}
		else
		{
			//The rest are 1 byte registers
			uint8_t result = 0;
			ReadRegister(reg, 1, &result);
			sprintf(msg, "%s (0x%02X): 0x%02X\n", reg_names[name_index], reg, result);
			CDC_Transmit_FS((uint8_t*)msg, strlen(msg));
		}

		if (reg == REG_FIFO_STATUS)
		{
			reg = REG_DYNPD - 1; //-1 because it will be incremented after this line
		}
		name_index++;
	}
}
