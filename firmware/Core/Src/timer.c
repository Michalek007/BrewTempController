/*
 * timer.c
 *
 *  Created on: Feb 18, 2025
 *      Author: Michał
 */
#include "timer.h"
#include "beer.h"
#include "menu.h"
#include "ssd1306.h"

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

void Timer_Display(void){
	if (!enableTimer){
		return;
	}
	uint32_t baseSeconds = currentBeerRest->minutes * 60;
	uint32_t totalSeconds =  (TIM1_GetTicks() - beerTimerTicks) / 1000;
	if (baseSeconds <= totalSeconds){
	  uint8_t done = BEER_NextRest();
	  MENU_DisplayOptions();
	  if (done){
		  enableTimer = 0;
		  SSD1306_Print(98, 0, White, "00:00");
		  SSD1306_Print(0, 32, White, "All rests done!");
//		  continue;
		  return;
	  }
	  beerTimerTicks = TIM1_GetTicks();
	  baseSeconds = currentBeerRest->minutes * 60;
	  totalSeconds =  (TIM1_GetTicks() - beerTimerTicks) / 1000;
	}
	totalSeconds = baseSeconds - totalSeconds;
	uint32_t minutes = totalSeconds / 60;
	uint32_t seconds = totalSeconds % 60;
	SSD1306_Printf(98, 0, White, "%02ld:%02ld", minutes, seconds);
}
