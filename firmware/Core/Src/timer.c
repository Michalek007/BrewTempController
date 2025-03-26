/*
 * timer.c
 *
 *  Created on: Feb 18, 2025
 *      Author: Michał
 */
#include "timer.h"

static volatile uint32_t beerTimerTicks = 0;
static volatile uint32_t enableTimer = 0;

void Timer_Toggle(uint32_t timerTicks){
	enableTimer = !enableTimer;
	beerTimerTicks = timerTicks - beerTimerTicks;
}
