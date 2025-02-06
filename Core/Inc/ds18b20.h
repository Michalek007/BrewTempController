/*
 * ds18b20.h
 *
 *  Created on: Jan 31, 2025
 *      Author: Michał
 */

#ifndef INC_DS18B20_H_
#define INC_DS18B20_H_

#include "main.h"

#define DS18B20_ROM_CODE_SIZE	8
#define DS18B20_CHECK_CRC	0

HAL_StatusTypeDef DS18B20_ReadAddress(uint8_t* romCode);

HAL_StatusTypeDef DS18B20_StartMeasure(const uint8_t* romCode);

float DS18B20_GetTemp_Float(const uint8_t* romCode);

int16_t DS18B20_GetTemp_Int(const uint8_t* romCode);


#endif /* INC_DS18B20_H_ */
