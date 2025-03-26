/*
 * menu.c
 *
 *  Created on: Feb 10, 2025
 *      Author: Michał
 */
#include <stdio.h>
#include "menu.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "beer.h"

#define DISPLAY_WIDTH	128
#define DISPLAY_HEIGHT	64
#define FONT_WIDTH		6
#define FONT_HEIGHT		8

#define BOTTOM_LEFT_CORNER_X	0
#define BOTTOM_RIGHT_CORNER_X	DISPLAY_WIDTH-FONT_WIDTH*4
#define BOTTOM_Y				DISPLAY_HEIGHT-FONT_HEIGHT

#define TOP_LEFT_X	0
#define TOP_MID_X	42
#define TOP_RIGHT_X 84
#define TOP_Y		0

void SSD1306_PrintBuffer(uint8_t cursorX, uint8_t cursorY, SSD1306_COLOR color, char* string);

static MENU_Option optionMain0 = {.x=BOTTOM_LEFT_CORNER_X, .y=BOTTOM_Y, .value=0, .string="Settings"};
static MENU_Option optionMain1 = {.x=BOTTOM_RIGHT_CORNER_X, .y=BOTTOM_Y, .value=1, .string="Stop"};


static MENU_Option optionSettings3 = {.x=BOTTOM_RIGHT_CORNER_X-6, .y=BOTTOM_Y, .value=3, .string="Start"};


MENU_Config menuConfig = {.lastOption=&optionSettings3, .option=&optionSettings3, .window=MENU_SETTINGS};
MENU_Direction menuDirection = MENU_LEFT;


void MENU_DisplayBeerRests(void){
	char temp[10];
	for (uint8_t i=0;i<3;++i){
		SSD1306_PrintBuffer(TOP_LEFT_X+i*42, TOP_Y, White, beerRests[i]->name);
		BEER_GetTempString(beerRests[i], temp);
		SSD1306_PrintBuffer(TOP_LEFT_X+i*42, TOP_Y+10, White,  temp);
		BEER_GetTimerString(beerRests[i], temp);
		SSD1306_PrintBuffer(TOP_LEFT_X+i*42, TOP_Y+20, White,  temp);
	}
}

void MENU_DisplayOptions(void){
	char temp[10];
	switch (menuConfig.window){
	case MENU_MAIN:
		SSD1306_PrintBuffer(TOP_LEFT_X, TOP_Y, White, currentBeerRest->name);
		BEER_GetTempString(currentBeerRest, temp);
		SSD1306_PrintBuffer(TOP_LEFT_X+42, TOP_Y, White,  temp);

		SSD1306_PrintBuffer(optionMain0.x, optionMain0.y, White, optionMain0.string);
		SSD1306_PrintBuffer(optionMain1.x, optionMain1.y, White, optionMain1.string);
		ssd1306_UpdateScreen();
		break;
	case MENU_SETTINGS:
		MENU_DisplayBeerRests();
		SSD1306_PrintBuffer(optionSettings3.x, optionSettings3.y, White, optionSettings3.string);
		ssd1306_UpdateScreen();
		break;
	case MENU_DETAILS:
		break;
	}
}

void MENU_DisplayChosenOption(void){
	switch (menuConfig.window){
	case MENU_MAIN:
		if (menuDirection == MENU_LEFT){
			menuConfig.option = &optionMain0;
			menuConfig.lastOption = &optionMain1;
		}
		else if (menuDirection == MENU_RIGHT){
			menuConfig.option = &optionMain1;
			menuConfig.lastOption = &optionMain0;
		}
		break;
	case MENU_SETTINGS:
		menuConfig.option = &optionSettings3;
		menuConfig.lastOption = &optionSettings3;
	    ssd1306_UpdateScreen();
		break;
	case MENU_DETAILS:
		break;
	}
	SSD1306_PrintBuffer(menuConfig.option->x, menuConfig.option->y, Black, menuConfig.option->string);
	SSD1306_PrintBuffer(menuConfig.lastOption->x,menuConfig.lastOption->y, White, menuConfig.lastOption->string);
    ssd1306_UpdateScreen();
}

void MENU_SetConfigWindow(void){
	switch (menuConfig.window){
	case MENU_MAIN:
		if (menuConfig.option->value == 0){
			menuConfig.window = MENU_SETTINGS;
		}
		break;
	case MENU_SETTINGS:
		if (menuConfig.option->value == 3){
			menuConfig.window = MENU_MAIN;
		}
		break;
	case MENU_DETAILS:
		if (menuConfig.option->value == 1){
			menuConfig.window = MENU_SETTINGS;
		}
		break;
	}
}

void SSD1306_PrintBuffer(uint8_t cursorX, uint8_t cursorY, SSD1306_COLOR color, char* string){
	  ssd1306_SetCursor(cursorX, cursorY);
	  ssd1306_WriteString(string, Font_6x8, color);
}
