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

#include <stdio.h>

#include "LCD/lcd.h"
#include "DS3231/ds3231.h"
#include "Audio/alarm.h"
#include "DAC/DAC_LIB.h"
#include "RingBuffer/RingBuffer.h"
#include "CheckSum/CheckSum.h"
#include "GSM/gsm.h"
#include "EEPROM/eeprom.h"
#include "MyHalFlash/MyHalFlash.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef enum
{
	INIT_MENU,
	HOME_MENU
}menu_t;
	
menu_t menu = INIT_MENU;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint32_t lastTime;
uint16_t cnt_led = 0;

uint8_t FLAG_BLINK_ALARM = 0;
uint8_t cnt_blink_alarm = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void Init_Menu_Func(void);
void Home_Menu_Func(void);

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
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

//	HAL_SuspendTick();
//	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
//	HAL_ResumeTick();

//	HAL_SuspendTick();
//	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
//	SystemClock_Config();
//	HAL_ResumeTick();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		switch(menu)
		{
			case INIT_MENU:
			{
				Init_Menu_Func();
			}break;
			case HOME_MENU:
			{
				Home_Menu_Func();
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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim1) // 1 ms
	{
		if(gsmState == GSM_State_10)
		{
			cnt_send_sms++;
			if(cnt_send_sms >= 4000)  // 4000 * 1ms = 4000 ms = 4s
			{
				cnt_send_sms = 0;
				FLAG_SEND_SMS = 1;
			}
		}
		
		if(gsmState == GSM_State_11)
		{
			cnt_call_alarm++;
			if(cnt_call_alarm >= 10000)  // 4000 * 1ms = 4000 ms = 4s
			{
				cnt_call_alarm = 0;
				FLAG_CALL_ALARM = 1;
			}
		}
		
		if(FLAG_ALARM_MENU == 1 || FLAG_ALARM_MENU == 2)
		{
			cnt_blink_alarm++;
			if(cnt_blink_alarm >= 200)  // 200 * 1ms = 200 ms
			{
				cnt_blink_alarm = 0;
				if(FLAG_BLINK_ALARM == 0)
					FLAG_BLINK_ALARM = 1;
				else 
					FLAG_BLINK_ALARM = 0;
			}
		}
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(gsmState == GSM_State_8 && FLAG_STATUS_ALARM == 1)
	{
		if(GPIO_Pin == PIR_IRQ_SIGNAL_Pin)
		{
				FLAG_PIR_SENSOR = 1;
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
		}
	}
}

void Init_Menu_Func(void)
{
	uint32_t count = 0;
	
	lcd_init();
	lcd_clear();
	lcd_gotoxy(0, 0);
	lcd_puts("  Diba Digital  ");
	lcd_gotoxy(0, 1);
	lcd_puts(" Security Alarm ");
	HAL_Delay(2000);
	
	HAL_TIM_Base_Start_IT(&htim1); //timer of system
	
	RingBuff_Init(&rxBuf);
	HAL_UART_Receive_DMA(&huart1, rxBuf.Buffer, rxBuf.Size);
	
	Index_Max_Alarm = (uint32_t)(alarm_LEN / 65535);
	
	MyHalFlash_Init();
	
	// unlock FLASH for Write
	do{
		if(EE_Init() == HAL_OK)
			break;
		count++;
	}while(count < 5);
		
	EE_ReadVariable(0, (uint16_t*) &FLAG_STATUS_ALARM);
	EE_ReadVariable(1, (uint16_t*) &FLAG_ALARM_MENU);
	EE_ReadVariable(2, (uint16_t*) &HourAlarm);
	EE_ReadVariable(3, (uint16_t*) &MinAlarm);
	EE_ReadVariable(4, (uint16_t*) &SecAlarm);
	
//	DS3231_SetTime(50, 04, 15, 1, 7, 4, 24);
	lcd_clear();
	menu = HOME_MENU;
	lcd_gotoxy(0, 0);
	lcd_puts("Please clse door");
}

void Home_Menu_Func(void)
{
	uint32_t nowTime, Time;
	char str[20];
		
	nowTime = HAL_GetTick();
	if(nowTime >= lastTime)
		Time = nowTime - lastTime;
	else
		Time = (UINT32_MAX - lastTime) + nowTime; 

	if(Time >= 100) //100ms
	{
		lastTime = nowTime;
		GSM_CheckBytes();
	}
	
	GSM_CheckState();
	
	if(FLAG_ALARM_MENU == 0)
	{
		if(FLAG_STATUS_ALARM == 0)
		{
			lcd_gotoxy(0, 0);
			lcd_puts("Please clse door");
		}
		else
		{
			lcd_gotoxy(0, 0);
			lcd_puts("Please open door");
		}
		DS3231_GetTime();
		lcd_gotoxy(0, 1);
		sprintf(str, "Clock: %02d:%02d:%02d", dsTime.Hour, dsTime.Minute, dsTime.Second);
		lcd_puts(str);
	}
	else if(FLAG_ALARM_MENU == 1)
	{
		lcd_clear();
		lcd_gotoxy(0, 0);
		lcd_puts("      Alarm     ");
		lcd_gotoxy(0, 1);
		sprintf(str, ">>>>%02d:%02d:%02d<<<<", HourAlarm, MinAlarm, SecAlarm);
		lcd_puts(str);
	
		// Alarm DAC
		HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t *)&alarm[idx_music*65536], 65535, DAC_ALIGN_12B_R);
		HAL_TIM_Base_Start(&htim2);
		//	HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
		
		FLAG_ALARM_MENU = 2;
	}
	else
	{
		if(FLAG_BLINK_ALARM == 1)
		{
			lcd_gotoxy(0, 0);
			lcd_puts("      Alarm     ");
		}
		else
		{
			lcd_gotoxy(0, 0);
			lcd_puts("                ");
		}
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
