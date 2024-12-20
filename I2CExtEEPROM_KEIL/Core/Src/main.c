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
#include "i2c.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define DeviceAddress 0x50

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint16_t myAddress = 1;
uint8_t myData_tx = 120;
uint8_t myData_rx = 0;

uint8_t FLAG_ERROR = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

HAL_StatusTypeDef byte_write(uint8_t dev_add, uint16_t add, uint8_t data)
{
	HAL_StatusTypeDef status;
	uint8_t ee[3];
	
	dev_add <<= 1;
	ee[0] = (add >> 8); //H
	ee[1] = add;				//L
	ee[2] = data;
	status = HAL_I2C_Master_Transmit(&hi2c1, dev_add, ee, 3, 100);
	HAL_Delay(5);
	return status;
}

HAL_StatusTypeDef random_read(uint8_t dev_add, uint16_t add, uint8_t *data)
{
	HAL_StatusTypeDef status, status1, status2;
	uint8_t ee[2];
	
	dev_add <<= 1;
	ee[0] = (add >> 8); //H
	ee[1] = add;				//L
	status1 = HAL_I2C_Master_Transmit(&hi2c1, dev_add, ee, 2, 100);
	HAL_Delay(5);
	status2 = HAL_I2C_Master_Receive(&hi2c1, dev_add, data, 1, 100); 
	HAL_Delay(5);
	
	if(status1 == HAL_OK && status2 == HAL_OK)
		status = HAL_OK;
	else
		status = HAL_ERROR;
	return status;
}
	
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
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

//	if(HAL_I2C_IsDeviceReady(&hi2c1, (DeviceAddress << 1), 3, 100) == HAL_OK)
//	{
//		if(byte_write(DeviceAddress, myAddress, myData_tx) == HAL_OK)
//		{
//			if(random_read(DeviceAddress, myAddress, &myData_rx) == HAL_OK)
//			{
//				//OK
//				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
//			}	
//			else
//			{
//				FLAG_ERROR = 1;
//			}
//		}
//		else
//		{
//			FLAG_ERROR = 1;
//		}
//	}
//	else
//	{
//		FLAG_ERROR = 1;
//	}

	if(HAL_I2C_IsDeviceReady(&hi2c1, (DeviceAddress << 1), 3, 100) == HAL_OK)
	{
		if(HAL_I2C_Mem_Write(&hi2c1, (DeviceAddress << 1), myAddress, I2C_MEMADD_SIZE_16BIT, &myData_tx, 1, 100) == HAL_OK)
		{
			HAL_Delay(5);
			if(HAL_I2C_Mem_Read(&hi2c1, (DeviceAddress << 1), myAddress, I2C_MEMADD_SIZE_16BIT, &myData_rx, 1, 100) == HAL_OK)
			{
				//OK
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
			}
			else
			{
				FLAG_ERROR = 1;
			}
		}
		else
		{
			FLAG_ERROR = 1;
		}
	}
	else
	{
		FLAG_ERROR = 1;
	}
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if(FLAG_ERROR == 1)
			HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		
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
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
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
