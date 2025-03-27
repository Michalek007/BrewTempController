/*
 * actuators.c
 *
 *  Created on: Mar 27, 2025
 *      Author: Michał
 */

#include "actuators.h"
#include "beer.h"

static volatile uint8_t criticalMinTempFlag = 0;
static volatile uint8_t criticalMaxTempFlag = 0;

void Actuators_Stop(void){
	  HAL_GPIO_WritePin(HEATER_GPIO_Port, HEATER_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
	  criticalMaxTempFlag = 0;
	  criticalMinTempFlag = 0;
}

void Actuators_TempControl(int16_t temp){
	  int16_t tempAverage = (currentBeerRest->tempMin + currentBeerRest->tempMax) / 2;
	  if (temp <= tempAverage){
		  if (temp < currentBeerRest->tempMin){
			  criticalMinTempFlag = 1;
		  }
		  else if (criticalMinTempFlag){
			  criticalMinTempFlag = 0;
			  HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
		  }
		  HAL_GPIO_WritePin(HEATER_GPIO_Port, HEATER_Pin, GPIO_PIN_SET);
	  }
	  else{
		  if (temp > currentBeerRest->tempMax){
			  criticalMaxTempFlag = 1;
		  }
		  else if (criticalMaxTempFlag){
			  criticalMaxTempFlag = 0;
			  HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
		  }
		  HAL_GPIO_WritePin(HEATER_GPIO_Port, HEATER_Pin, GPIO_PIN_RESET);
	  }
}

void Actuators_BuzzerToggle(void){
	if (criticalMinTempFlag){
	  HAL_GPIO_TogglePin(BUZZER_GPIO_Port, BUZZER_Pin);
	}
}

void Actuators_BuzzerToggleFast(void){
	if (criticalMaxTempFlag){
	  HAL_GPIO_TogglePin(BUZZER_GPIO_Port, BUZZER_Pin);
	}
}
