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

#endif /* INC_BEER_H_ */
