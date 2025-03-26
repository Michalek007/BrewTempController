/*
 * menu.c
 *
 *  Created on: Feb 10, 2025
 *      Author: Michał
 */
#include <stdio.h>
#include <string.h>
#include "menu.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "beer.h"
#include "timer.h"

#define DISPLAY_WIDTH	128
#define DISPLAY_HEIGHT	64
#define FONT_WIDTH		6
#define FONT_HEIGHT		8

#define BOTTOM_LEFT_CORNER_X	0
#define BOTTOM_RIGHT_CORNER_X	DISPLAY_WIDTH-FONT_WIDTH*5
#define BOTTOM_Y				DISPLAY_HEIGHT-FONT_HEIGHT

#define TOP_LEFT_X	0
#define TOP_MID_X	42
#define TOP_RIGHT_X 84
#define TOP_Y		0

#define MAIN_OPTIONS_COUNT 2
#define SETTINGS_OPTIONS_COUNT 1

static MENU_Option optionMain0 = {.x=BOTTOM_LEFT_CORNER_X, .y=BOTTOM_Y, .value=0, .string="Settings"};

static uint8_t resume = 0;
static MENU_Option optionMain1 = {.x=BOTTOM_RIGHT_CORNER_X, .y=BOTTOM_Y, .value=1, .string=" Stop"};


static MENU_Option optionSettings0 = {.x=BOTTOM_RIGHT_CORNER_X, .y=BOTTOM_Y, .value=0, .string="Start"};

static MENU_Option* mainOptions[MAIN_OPTIONS_COUNT] = {&optionMain0, &optionMain1};
static MENU_Option* settingsOptions[SETTINGS_OPTIONS_COUNT] = {&optionSettings0};


MENU_Config menuConfig = {.lastOption=NULL, .option=&optionSettings0, .window=MENU_SETTINGS};


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

void MENU_DisplayTimer(uint8_t minutes, uint8_t seconds){
	SSD1306_Printf(98, 0, White, "%02ld:%02ld", minutes, seconds);
}

void MENU_DisplayTemperature(int16_t temp){
	  SSD1306_Printf(0, 11, White, "T: %d\n", temp);
}

void MENU_DisplayEndMessage(void){
	  SSD1306_Print(98, 0, White, "00:00");
	  SSD1306_Print(0, 32, White, "All rests done!");
}

void MENU_DisplayOptions(void){
	char temp[10];
	switch (menuConfig.window){
	case MENU_MAIN:
		SSD1306_PrintBuffer(TOP_LEFT_X, TOP_Y, White, currentBeerRest->name);
		BEER_GetTempString(currentBeerRest, temp);
		SSD1306_PrintBuffer(TOP_LEFT_X+42, TOP_Y, White,  temp);

		for (uint8_t i=0;i<MAIN_OPTIONS_COUNT;++i){
			SSD1306_PrintBuffer(mainOptions[i]->x, mainOptions[i]->y, White, mainOptions[i]->string);
		}
		ssd1306_UpdateScreen();
		break;
	case MENU_SETTINGS:
		MENU_DisplayBeerRests();
		for (uint8_t i=0;i<SETTINGS_OPTIONS_COUNT;++i){
			SSD1306_PrintBuffer(settingsOptions[i]->x, settingsOptions[i]->y, White, settingsOptions[i]->string);
		}
		ssd1306_UpdateScreen();
		break;
	case MENU_DETAILS:
		break;
	}
}

void MENU_HighlightSelectedOption(void){
	SSD1306_PrintBuffer(menuConfig.option->x, menuConfig.option->y, Black, menuConfig.option->string);
	if (menuConfig.lastOption != NULL){
		SSD1306_PrintBuffer(menuConfig.lastOption->x,menuConfig.lastOption->y, White, menuConfig.lastOption->string);
	}
    ssd1306_UpdateScreen();
}

void MENU_SetNextOption(void){
	uint8_t currentOptionValue = menuConfig.option->value;
	uint8_t nextOptionValue = currentOptionValue + 1 < MAIN_OPTIONS_COUNT ? currentOptionValue + 1 : 0;
	switch (menuConfig.window){
	case MENU_MAIN:
		menuConfig.option = mainOptions[nextOptionValue];
		menuConfig.lastOption = mainOptions[currentOptionValue];
		break;
	case MENU_SETTINGS:
		menuConfig.option = &optionSettings0;
		menuConfig.lastOption = NULL;
		break;
	case MENU_DETAILS:
		break;
	}
}

void MENU_SetConfigWindow(void){
	switch (menuConfig.window){
	case MENU_MAIN:
		if (menuConfig.option->value == 0){
			menuConfig.window = MENU_SETTINGS;
		}
		break;
	case MENU_SETTINGS:
		if (menuConfig.option->value == 0){
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

void MENU_SelectedOptionHandler(void){
	  if (menuConfig.option->value == 1 && menuConfig.window == MENU_MAIN){
		  Timer_Toggle();
		  if (menuConfig.option->value == 1){
			  if (resume){
				  strcpy(menuConfig.option->string, " Stop");
			  }
			  else{
				  strcpy(menuConfig.option->string, "Start");
			  }
			  resume = !resume;
		  }
		MENU_DisplayOptions();
		return;
	  }
	  else if (menuConfig.option->value == 0 && menuConfig.window == MENU_SETTINGS){
		  BEER_RestartRest();
		  Timer_Start();
	  }
	  MENU_SetConfigWindow();
	  ssd1306_Fill(Black);
	  MENU_SetNextOption();
	  MENU_DisplayOptions();
}
