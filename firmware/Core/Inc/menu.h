/*
 * menu.h
 *
 *  Created on: Feb 10, 2025
 *      Author: Michał
 */

#ifndef INC_MENU_H_
#define INC_MENU_H_

#include "main.h"

typedef enum {
	MENU_MAIN, MENU_SETTINGS, MENU_DETAILS
} MENU_Window;

typedef struct {
	uint8_t x;
	uint8_t y;
	uint8_t value;
	char string[10];
} MENU_Option;

typedef struct {
	MENU_Option *lastOption;
	MENU_Option *option;
	MENU_Window window;
} MENU_Config;

extern MENU_Config menuConfig;
extern volatile uint8_t doneFlag;

void MENU_DisplayBeerRests(void);
void MENU_DisplayTimer(uint8_t minutes, uint8_t seconds);
void MENU_DisplayTemperature(int16_t temp);
void MENU_DisplayEndMessage(void);
void MENU_DisplayOptions(void);
void MENU_HighlightSelectedOption(void);
void MENU_SetNextOption(void);
void MENU_SetConfigWindow(void);
void MENU_SelectedOptionHandler(void);

#endif /* INC_MENU_H_ */
