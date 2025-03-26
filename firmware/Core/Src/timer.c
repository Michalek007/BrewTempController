/*
 * timer.c
 *
 *  Created on: Feb 18, 2025
 *      Author: Michał
 */
#include "timer.h"
#include "tim.h"

static volatile uint32_t beerTimerTicks = 0;
static volatile uint32_t enableTimer = 0;

void Timer_Toggle(void){
	enableTimer = !enableTimer;
	beerTimerTicks = TIM1_GetTicks() - beerTimerTicks;
}

void Timer_Start(void){
	enableTimer = 1;
	beerTimerTicks = TIM1_GetTicks();
}

void Timer_Stop(void){
	enableTimer = 0;
	beerTimerTicks = 0;
}

uint8_t Timer_Enable(void){
	return enableTimer;
}

uint32_t Timer_GetTotalSeconds(void){
	return (TIM1_GetTicks() - beerTimerTicks) / 1000;
}
