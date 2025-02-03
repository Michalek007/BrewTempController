/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "ds18b20.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define MAX_PRINTF_LEN 100
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint32_t timerTicks = 0;
volatile uint8_t timerFlag500 = 0;
volatile uint8_t timerFlag1000 = 0;
volatile uint8_t timerFlag2000 = 0;

volatile uint8_t adcFlag = 0;
uint16_t adcBuffer[2] = {0};

uint8_t readTempFlag = 0;
//uint8_t romeCode1[] =  {41, 96, 121, 139, 13, 0, 0, 30};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void UART_Print(UART_HandleTypeDef *huart, const char* string);
void UART_Printf(UART_HandleTypeDef *huart, const char* string, ...);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
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
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim1);

  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
  UART_Print(&huart1,  "Initialization done!");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint8_t romCode[DS18B20_ROM_CODE_SIZE] = {0};
  HAL_StatusTypeDef check = DS18B20_ReadAddress(romCode);
  if (check != HAL_OK){
	  UART_Print(&huart1,  "Error occurred!");
  }
  else{
	  uint8_t txNull[5] = {0};
	  HAL_UART_Transmit(&huart1, txNull, 5, 100);
	  HAL_UART_Transmit(&huart1, romCode, DS18B20_ROM_CODE_SIZE, 100);
	  HAL_UART_Transmit(&huart1, txNull, 5, 100);
  }
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  if (timerFlag500){
		  timerFlag500 = 0;
		  if (!adc){
			  adcFlag = 1;
			  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcBuffer, 2);
		  }
	  }
	  if (timerFlag1000){
		  timerFlag1000 = 0;
//		  UART_Printf(&huart1, "Timer ticks: %d\n", timerTicks);
  		  HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
		  uint32_t totalSeconds = timerTicks / 1000;
		  uint32_t minutes = totalSeconds / 60;
		  uint32_t seconds = totalSeconds % 60;
		  UART_Printf(&huart1, "%02d:%02d\n", minutes, seconds);
	  }
	  if (timerFlag2000){
		  timerFlag2000 = 0;
		  if (readTempFlag){
			  readTempFlag = 0;
			  int16_t temp = DS18B20_GetTemp_Int(NULL);
			  if (temp <= -200){
				  UART_Print(&huart1, "Error occurred during reading temperature! ");
			  }
			  UART_Printf(&huart1, "T: %d\n", temp);
		  }
		  else{
			  check = DS18B20_StartMeasure(NULL);
			  if (check != HAL_OK){
				  UART_Print(&huart1,  "Error occurred!");
			  }
			  else{
				  readTempFlag = 1;
			  }
		  }
	  }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 256;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void UART_Print(UART_HandleTypeDef *huart, const char* string){
	HAL_UART_Transmit(huart, (uint8_t*)string, strlen(string), HAL_MAX_DELAY);
}

void UART_Printf(UART_HandleTypeDef *huart, const char* string, ...){
	va_list argp;
	va_start(argp, string);
	char stringf[MAX_PRINTF_LEN];
	if (vsprintf(stringf, string, argp) > 0){
		UART_Print(huart, stringf);
	}
	va_end(argp);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1) {
    	timerTicks++;
		if (timerTicks % 500 == 0){
			timerFlag500 = 1;
		}
    	if (timerTicks % 1000 == 0){
    		timerFlag1000 = 1;
		}
    	if (timerTicks % 2000 == 0){
    		timerFlag2000 = 1;
		}
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    if (hadc->Instance == ADC1) {
    	adcFlag = 0;
    }
}

void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef *hadc) {
    if (hadc->Instance == ADC1) {
		UART_Printf(&huart1, "ADC buffer: [%d, %d]\n", adcBuffer[0], adcBuffer[1]);
    }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
	  UART_Print(&huart1,  "Error handler callback!");
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
