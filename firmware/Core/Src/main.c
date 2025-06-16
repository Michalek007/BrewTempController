/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ds18b20.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "menu.h"
#include "beer.h"
#include "timer.h"
#include "actuators.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint8_t readTempFlag = 0;
//volatile uint8_t criticalMinTempFlag = 0;
//volatile uint8_t criticalMaxTempFlag = 0;

volatile uint8_t choosingEnabled = 0;
volatile uint32_t choosingTimerTicks = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART1_UART_Init();
	MX_TIM1_Init();
	MX_I2C1_Init();
	MX_USART2_UART_Init();
	MX_ADC1_Init();
	MX_I2C2_Init();
	MX_TIM2_Init();
	/* USER CODE BEGIN 2 */
	Actuators_MotorStop();

	HAL_TIM_Base_Start_IT(&htim1);
	ssd1306_Init();

	HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
	UART_Print("Initialization done!");
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	ssd1306_Fill(Black);
	MENU_DisplayOptions();
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		if (timerFlag250) {
			timerFlag250 = 0;
			Actuators_BuzzerToggleFast();
		}
		if (timerFlag500) {
			timerFlag500 = 0;
			if (choosingEnabled) {
				if (TIM1_GetTicks() - choosingTimerTicks >= 10000) {
					choosingEnabled = 0;
					MENU_DisplayOptions();
				}
			}
			Actuators_BuzzerToggle();
		}
		if (timerFlag1000) {
			timerFlag1000 = 0;
			HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
			if (menuConfig.window == MENU_MAIN) {

				if (Timer_Enable()) {
					uint32_t baseSeconds = currentBeerRest->minutes * 60;
					uint32_t totalSeconds = Timer_GetTotalSeconds();
					if (baseSeconds <= totalSeconds) {
						uint8_t done = BEER_NextRest();
						MENU_DisplayOptions();
						if (done) {
							Timer_Stop();
							Actuators_Stop();
							doneFlag = 1;
							MENU_DisplayEndMessage();
							continue;
						}
						Timer_Start();
						baseSeconds = currentBeerRest->minutes * 60;
						totalSeconds = Timer_GetTotalSeconds();
					}
					totalSeconds = baseSeconds - totalSeconds;
					uint8_t minutes = totalSeconds / 60;
					uint8_t seconds = totalSeconds % 60;
					MENU_DisplayTimer(minutes, seconds);
				}

				if (readTempFlag) {
					readTempFlag = 0;
					int16_t temp = DS18B20_GetTemp_Int(NULL);
					if (temp <= -200) {
						UART_Print("Error occurred during reading temperature! ");
					}
					UART_Printf("T: %d\n", temp);
					MENU_DisplayTemperature(temp);

					if (!doneFlag) {
						Actuators_TempControl(temp);
					}
				}
				if (!readTempFlag) {
					HAL_StatusTypeDef check = DS18B20_StartMeasure(NULL);
					if (check != HAL_OK) {
						UART_Print("Error occurred during temperature measurement!");
					} else {
						readTempFlag = 1;
					}
				}
			}
		}
		if (timerFlag2000) {
			timerFlag2000 = 0;
		}
		if (bt1Flag) {
			bt1Flag = 0;
			if (choosingEnabled) {
				choosingEnabled = 0;
				MENU_SelectedOptionHandler();
			} else {
				choosingEnabled = 1;
				choosingTimerTicks = TIM1_GetTicks();
				MENU_HighlightSelectedOption();
			}
		}
		if (bt2Flag) {
			bt2Flag = 0;
			if (choosingEnabled) {
				MENU_SetNextOption();
			} else {
				choosingEnabled = 1;
			}
			choosingTimerTicks = TIM1_GetTicks();
			MENU_HighlightSelectedOption();
		}
		if (bt3Flag) {
			bt3Flag = 0;
			Actuators_MotorToggle();
		}
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
