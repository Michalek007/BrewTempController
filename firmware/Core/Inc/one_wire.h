/*
 * one_wire.h
 *
 *  Created on: Jan 31, 2025
 *      Author: Michał
 */

#ifndef INC_ONE_WIRE_H_
#define INC_ONE_WIRE_H_

#include "main.h"

HAL_StatusTypeDef OneWire_Reset(void);

uint8_t OneWire_Read(void);

void OneWire_Write(uint8_t byte);

uint8_t OneWire_CRC(const uint8_t *data, uint8_t len);

#endif /* INC_ONE_WIRE_H_ */
