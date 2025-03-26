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
volatile uint32_t timerTicks = 0;
volatile uint8_t timerFlag250 = 0;
volatile uint8_t timerFlag500 = 0;
volatile uint8_t timerFlag1000 = 0;
volatile uint8_t timerFlag2000 = 0;

volatile uint8_t adcFlag = 0;
uint16_t adcBuffer[2] = {0};

volatile uint8_t readTempFlag = 0;
//uint8_t romeCode1[] =  {41, 96, 121, 139, 13, 0, 0, 30};
//uint8_t romeCode2[] =  {41, 42, 71, 138, 178, 35, 6, 167};

volatile uint8_t buttonFlag = 0;
volatile uint8_t buttonChosenFlag = 0;

volatile uint8_t choosingEnabled = 0;
volatile uint32_t choosingTicks = 0;

volatile uint8_t joystickFlag = 0;

volatile uint32_t beerTimerTicks = 0;
volatile uint32_t enableTimer = 0;
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
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim1);
  ssd1306_Init();

  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
  UART_Print("Initialization done!");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint8_t romCode[DS18B20_ROM_CODE_SIZE] = {0};
  HAL_StatusTypeDef check = DS18B20_ReadAddress(romCode);
  if (check != HAL_OK){
	  UART_Print( "Error occurred!");
  }
  else{
	  uint8_t txNull[5] = {0};
	  HAL_UART_Transmit(&huart1, txNull, 5, 100);
	  HAL_UART_Transmit(&huart1, romCode, DS18B20_ROM_CODE_SIZE, 100);
	  HAL_UART_Transmit(&huart1, txNull, 5, 100);
  }
  ssd1306_Fill(Black);
  MENU_DisplayOptions();
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  if (timerFlag250){
		  timerFlag250 = 0;
		  if (!adcFlag){
			  adcFlag = 1;
			  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcBuffer, 2);
		  }
	  }
	  if (timerFlag500){
		  timerFlag500 = 0;
		  if (joystickFlag){
			  joystickFlag = 0;
			  if (adcBuffer[0] <= 500){
				  SSD1306_Print(0, 32, White, "Down ");
				  menuDirection = MENU_DOWN;
			  }
			  else if (adcBuffer[0] >= 3500){
				  SSD1306_Print(0, 32, White, "Up   ");
				  menuDirection = MENU_UP;
			  }
			  else if (adcBuffer[1] >= 3500){
				  SSD1306_Print(0, 32, White, "Left ");
				  menuDirection = MENU_LEFT;
			  }
			  else if (adcBuffer[1] <= 500){
				  SSD1306_Print(0, 32, White, "Right");
				  menuDirection = MENU_RIGHT;
			  }
		  }
		  if (choosingEnabled){
			  MENU_DisplayChosenOption();
			  if (timerTicks - choosingTicks >= 5000){
				  choosingEnabled = 0;
				  MENU_DisplayOptions();
			  }
		  }
	  }
	  if (timerFlag1000){
		  timerFlag1000 = 0;
		  if (menuConfig.window == MENU_MAIN){
			  HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);

			  if (enableTimer){
				  uint32_t baseSeconds = currentBeerRest->minutes * 60;
				  uint32_t totalSeconds =  (timerTicks - beerTimerTicks) / 1000;
				  if (baseSeconds <= totalSeconds){
					  uint8_t done = BEER_NextRest();
					  MENU_DisplayOptions();
					  if (done){
						  enableTimer = 0;
						  SSD1306_Print(98, 0, White, "00:00");
						  SSD1306_Print(0, 32, White, "All rests done!");
						  continue;
					  }
					  beerTimerTicks = timerTicks;
					  baseSeconds = currentBeerRest->minutes * 60;
					  totalSeconds =  (timerTicks - beerTimerTicks) / 1000;
				  }
				  totalSeconds = baseSeconds - totalSeconds;
				  uint32_t minutes = totalSeconds / 60;
				  uint32_t seconds = totalSeconds % 60;
				  SSD1306_Printf(98, 0, White, "%02ld:%02ld", minutes, seconds);
			  }

			  if (readTempFlag){
				  readTempFlag = 0;
				  int16_t temp = DS18B20_GetTemp_Int(NULL);
				  if (temp <= -200){
					  UART_Print("Error occurred during reading temperature! ");
				  }
				  UART_Printf("T: %d\n", temp);
				  SSD1306_Printf(0, 11, White, "T: %d\n", temp);
			  }
			  if (!readTempFlag){
				  check = DS18B20_StartMeasure(NULL);
				  if (check != HAL_OK){
					  UART_Print("Error occurred!");
				  }
				  else{
					  readTempFlag = 1;
				  }
			  }
		  }
	  }
	  if (timerFlag2000){
		  timerFlag2000 = 0;
	  }
	  if (buttonFlag){
		  buttonFlag = 0;
		  choosingEnabled = 1;
		  choosingTicks = timerTicks;
	  }
	  if (buttonChosenFlag){
		  buttonChosenFlag = 0;
		  choosingEnabled = 0;
		  UART_Print("buttonChosenFlag!");
		  if (menuConfig.option->value == 1 && menuConfig.window == MENU_MAIN){
			  enableTimer = !enableTimer;
			  beerTimerTicks = timerTicks - beerTimerTicks;
			  MENU_DisplayOptions();
			  continue;
		  }
		  else if (menuConfig.option->value == 3 && menuConfig.window == MENU_SETTINGS){
			  BEER_RestartRest();
			  enableTimer = 1;
			  beerTimerTicks = timerTicks;
		  }
		  MENU_SetConfigWindow();
		  ssd1306_Fill(Black);
		  MENU_DisplayOptions();
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
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1) {
    	timerTicks++;
		if (timerTicks % 250 == 0){
			timerFlag250 = 1;
		}
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
		UART_Printf("ADC buffer: [%d, %d]\n", adcBuffer[0], adcBuffer[1]);
		joystickFlag = 1;
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if (GPIO_Pin == BT1_Pin){
		if (!buttonFlag && !choosingEnabled){
			buttonFlag = 1;
		}
		if (!buttonChosenFlag && choosingEnabled){
			buttonChosenFlag = 1;
		}
		UART_Print("Button 1 pressed!");
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
	  UART_Print("Error handler callback!");
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
