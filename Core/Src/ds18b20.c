/*
 * ds18b20.c
 *
 *  Created on: Jan 31, 2025
 *      Author: Michał
 */
#include <string.h>
#include <math.h>
#include "ds18b20.h"
#include "one_wire.h"

#define DS18B20_SCRATCHPAD_SIZE    9
#define DS18B20_READ_ROM           0x33
#define DS18B20_MATCH_ROM          0x55
#define DS18B20_SKIP_ROM           0xCC
#define DS18B20_CONVERT_T          0x44
#define DS18B20_READ_SCRATCHPAD    0xBE

static HAL_StatusTypeDef DS18B20_SendCmd(const uint8_t* romCode, uint8_t cmd)
{
	if (OneWire_Reset() != HAL_OK){
		return HAL_ERROR;
	}
	if (!romCode){
		OneWire_Write(DS18B20_SKIP_ROM);
	}
	else{
		OneWire_Write(DS18B20_MATCH_ROM);
		for (uint8_t i = 0; i < DS18B20_ROM_CODE_SIZE; i++){
			OneWire_Write(romCode[i]);
		}
	}
	OneWire_Write(cmd);
	return HAL_OK;
}

static HAL_StatusTypeDef DS18B20_ReadScratchpad(const uint8_t* romCode, uint8_t* scratchpad)
{
	uint8_t crc;
	if (DS18B20_SendCmd(romCode, DS18B20_READ_SCRATCHPAD) != HAL_OK){
		return HAL_ERROR;
	}
	for (uint8_t i = 0; i < DS18B20_SCRATCHPAD_SIZE; i++){
		scratchpad[i] = OneWire_Read();
	}
	crc = OneWire_CRC(scratchpad, DS18B20_SCRATCHPAD_SIZE - 1);
	if (scratchpad[DS18B20_SCRATCHPAD_SIZE - 1] == crc){
		return HAL_OK;
	}
	return HAL_ERROR;
}

HAL_StatusTypeDef DS18B20_ReadAddress(uint8_t* romCode)
{
	uint8_t crc;
	if (OneWire_Reset() != HAL_OK){
		return HAL_ERROR;
	}
	OneWire_Write(DS18B20_READ_ROM);
	for (uint8_t i = 0; i < DS18B20_ROM_CODE_SIZE; i++){
		romCode[i] = OneWire_Read();
	}
	return HAL_OK;
	crc = OneWire_CRC(romCode, DS18B20_ROM_CODE_SIZE - 1);
	if (romCode[DS18B20_ROM_CODE_SIZE - 1] == crc){
		return HAL_OK;
	}
	return HAL_ERROR;
}

HAL_StatusTypeDef DS18B20_StartMeasure(const uint8_t* romCode)
{
	return DS18B20_SendCmd(romCode, DS18B20_CONVERT_T);
}

float DS18B20_GetTemp_Float(const uint8_t* romCode)
{
	uint8_t scratchpad[DS18B20_SCRATCHPAD_SIZE];
	int16_t temp;
	if (DS18B20_ReadScratchpad(romCode, scratchpad) != HAL_OK){
		return -273.0f;
	}
	memcpy(&temp, &scratchpad[0], sizeof(temp));
	return temp / 16.0f;
}

int16_t DS18B20_GetTemp_Int(const uint8_t* romCode)
{
	return (int16_t) roundf(DS18B20_GetTemp_Float(romCode));
}
