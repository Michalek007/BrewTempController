/*
 * beer.c
 *
 *  Created on: Feb 12, 2025
 *      Author: Michał
 */
#include <stdio.h>
#include "beer.h"

#define BEER_RESTS 3

BEER_Rest beerRest1 = {.name="Rest1", .tempMin=50, .tempMax=60, .minutes=1};
BEER_Rest beerRest2 = {.name="Rest2", .tempMin=68, .tempMax=72, .minutes=1};
BEER_Rest beerRest3 = {.name="Rest3", .tempMin=74, .tempMax=76, .minutes=1};


BEER_Rest* currentBeerRest = &beerRest1;
BEER_Rest* beerRests[] = {&beerRest1, &beerRest2, &beerRest3};
static uint8_t currentBeerRestIndex = 0;


uint8_t BEER_NextRest(void){
	if (currentBeerRestIndex >= BEER_RESTS-1){
		return 1;
	}
	currentBeerRestIndex++;
	currentBeerRest = beerRests[currentBeerRestIndex];
	return 0;
}

void BEER_RestartRest(void){
	currentBeerRest = &beerRest1;
	currentBeerRestIndex = 0;
}

void BEER_GetTempString(BEER_Rest* beerRest, char* string){
	sprintf(string, "%d-%d", beerRest->tempMin, beerRest->tempMax);
}

void BEER_GetTimerString(BEER_Rest* beerRest, char* string){
	sprintf(string, "%02d:00", beerRest->minutes);
}
