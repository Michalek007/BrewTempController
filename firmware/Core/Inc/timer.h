/*
 * timer.h
 *
 *  Created on: Feb 18, 2025
 *      Author: Michał
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include "main.h"

void Timer_Toggle(void);
void Timer_Start(void);
void Timer_Stop(void);
uint8_t Timer_Enable(void);
uint32_t Timer_GetTotalSeconds(void);

#endif /* INC_TIMER_H_ */
