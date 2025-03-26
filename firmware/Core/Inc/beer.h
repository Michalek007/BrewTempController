/*
 * beer.h
 *
 *  Created on: Feb 12, 2025
 *      Author: Michał
 */

#ifndef INC_BEER_H_
#define INC_BEER_H_

#include "main.h"

typedef struct {
	char* name;
	uint8_t tempMax;
	uint8_t tempMin;
	uint8_t minutes;
} BEER_Rest;

extern BEER_Rest beerRest1;
extern BEER_Rest beerRest2;
extern BEER_Rest beerRest3;

extern BEER_Rest* beerRests[];
extern BEER_Rest* currentBeerRest;

uint8_t BEER_NextRest(void);
void BEER_RestartRest(void);
void BEER_GetTempString(BEER_Rest* beerRest, char* string);
void BEER_GetTimerString(BEER_Rest* beerRest, char* string);

#endif /* INC_BEER_H_ */
