/*
 * actuators.h
 *
 *  Created on: Mar 27, 2025
 *      Author: Michał
 */

#ifndef INC_ACTUATORS_H_
#define INC_ACTUATORS_H_

#include "main.h"

void Actuators_Stop(void);
void Actuators_TempControl(int16_t temp);
void Actuators_BuzzerToggle(void);
void Actuators_BuzzerToggleFast(void);

#endif /* INC_ACTUATORS_H_ */
