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
#include "dac.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "LCD/lcd.h"
#include "DS3231/ds3231.h"
#include "GPS/GPS_RingBuff.h"
#include "DAC/DAC_LIB.h"
#include "PrayTimes/PrayTimes.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef enum{
	INIT_MENU,
	LOAD_MENU,
	HOME_MENU,
}menu_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

menu_t menu = INIT_MENU;
uint8_t i = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void InitFunc(void);
void LoadFunc(void);
void HomeFunc(void);

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
  MX_DAC_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		switch(menu)
		{
			case INIT_MENU:
			{	
				InitFunc();
			}break;
			case LOAD_MENU:
			{	
				LoadFunc();
			}break;
			case HOME_MENU:
			{	
				HomeFunc();
			}break;
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void InitFunc(void)
{
	lcd_init();
	lcd_clear();
	lcd_gotoxy(0, 0);
	lcd_puts("  Diba Digital  ");
	lcd_gotoxy(0, 1);
	lcd_puts(" Call to prayer ");
	HAL_Delay(2000);
	
	Index_Max_Prayer = (uint32_t)(prayer_35_LEN / 65535);
	
	HAL_UART_Receive_DMA(&huart1, rxBuf, rxBuf_Len);
	init_prayer_times();
	
	//delete
	DS3231_SetTime(50, 22, 5, 1, 15, 2, 24);
	
	menu = LOAD_MENU;
	lcd_clear();
}

void LoadFunc(void)
{
	uint32_t nowTime, Time;
	nowTime = HAL_GetTick();
	if(nowTime >= gps_lastTime)
		Time = nowTime - gps_lastTime;
	else
		Time = (UINT32_MAX - gps_lastTime) + nowTime; 

	if(Time >= 100) //100ms
	{
		gps_lastTime = nowTime;
		GPS_CheckBytes();
	}
	if(FLAG_LOAD_GPS == 1)
	{
		FLAG_LOAD_GPS = 0;
		menu = HOME_MENU;
		lcd_clear();
	}
}

int8_t FindIdxPrayTimes(int i)
{
	if(dsTime.Hour < praytimes[i].Hour)
	{
		return 1;
	}
	else if(dsTime.Hour == praytimes[i].Hour)
	{
		if(dsTime.Minute < praytimes[i].Minute)
		{
			return 1;
		}
		else if(dsTime.Minute == praytimes[i].Minute)
		{
			if(dsTime.Second <= praytimes[i].Second)
			{
				if(dsTime.Second == praytimes[i].Second)
					FLAG_PRAYER = 1;
				return 1;
			}
		}
	}
	return 0;
}

void HomeFunc(void)
{
	char str_time_ds[20], str_praytimes[20];
	
	DS3231_GetTime();
	lcd_gotoxy(0, 0);
	sprintf(str_time_ds, "%02d:%02d:%02d", dsTime.Hour, dsTime.Minute, dsTime.Second);
	lcd_puts(str_time_ds);

	if(FLAG_PRAYER == 0)
	{
		if(FindIdxPrayTimes(0))
		{
			i = 0;
		}
		else if(FindIdxPrayTimes(2))
		{
			i = 2;
		}
		else if(FindIdxPrayTimes(5))
		{
			i = 5;
		}
		else
		{
			i = 0;
		}
		
		lcd_gotoxy(0, 1);
		sprintf(str_praytimes, "%02d:%02d:%02d", praytimes[i].Hour, praytimes[i].Minute, praytimes[i].Second);
		lcd_puts(str_praytimes);
	}	
	else if(FLAG_PRAYER == 1)
	{
		FLAG_PRAYER = 2; //play
		HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t *)&prayer_35[idx_music*65536], 65535, DAC_ALIGN_12B_R);
		HAL_TIM_Base_Start(&htim2);
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
