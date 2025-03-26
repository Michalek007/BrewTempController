/*
 * one_wire.c
 *
 *  Created on: Jan 31, 2025
 *      Author: Michał
 */
#include "one_wire.h"
#include "usart.h"

static void UART_SetBaudRate(uint32_t baudRate)
{
  huart2.Init.BaudRate = baudRate;
  if (HAL_HalfDuplex_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

static HAL_StatusTypeDef UART_TransmitByte(uint8_t* byte)
{
	return HAL_UART_Transmit(&huart2, byte, 1, 100);
}

static HAL_StatusTypeDef UART_ReceiveByte(uint8_t* byte)
{
	  return HAL_UART_Receive(&huart2, byte, 1, 100);
}

static void WriteBit(uint8_t value)
{
  uint8_t dataOut = value ? 0xFF: 0x00;
  UART_TransmitByte(&dataOut);
}

static uint8_t ReadBit(void)
{
  uint8_t dataOut = 0xFF;
  uint8_t dataIn = 0x00;
  UART_TransmitByte(&dataOut);
  UART_ReceiveByte(&dataIn);
  return dataIn & 0x01;
}

static uint8_t ByteCRC(uint8_t crc, uint8_t byte)
{
	for (uint8_t i=0; i<8; i++){
		uint8_t b = crc ^ byte;
		crc >>= 1;
		if (b & 0x01){
			crc ^= 0x8c;
		}
		byte >>= 1;
	}
	return crc;
}

HAL_StatusTypeDef OneWire_Reset(void)
{
  uint8_t dataOut = 0xF0;
  uint8_t dataIn = 0x00;
  UART_SetBaudRate(9600);
  UART_TransmitByte(&dataOut);
  UART_ReceiveByte(&dataIn);
  UART_SetBaudRate(115200);
  if (dataIn != 0xF0){
	  return HAL_OK;
  }
  return HAL_ERROR;
}

uint8_t OneWire_Read(void)
{
	uint8_t value = 0;
	for (uint8_t i=0; i<8; i++){
		value |= (ReadBit() << i);
	}
	return value;
}

void OneWire_Write(uint8_t byte)
{
	for (uint8_t i=0; i<8; i++){
		WriteBit(byte & 0x01);
		byte >>= 1;
	}
}

uint8_t OneWire_CRC(const uint8_t* data, uint8_t len)
{
	uint8_t crc = 0;
	for (uint8_t i=0; i<len; i++){
		crc = ByteCRC(crc, data[i]);
	}
	return crc;
}

