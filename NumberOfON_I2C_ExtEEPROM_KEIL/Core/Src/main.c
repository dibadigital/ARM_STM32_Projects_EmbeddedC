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

#define DeviceAddress 0x50

#define VAL_FLAG_INIT_EE 0xAA

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

uint16_t Addr_FLAG_INIT_EEPROM = 0;
uint8_t FLAG_INIT_EEPROM = 0;

uint16_t Addr_Num_Of_On_Device = 0;
uint32_t Num_Of_ON_Device = 0;

uint8_t FLAG_ERROR = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

HAL_StatusTypeDef I2C_Mem_Write_Bytes(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
                                    uint16_t MemAddSize, uint32_t *pData, uint16_t Size, uint32_t Timeout);
	
HAL_StatusTypeDef I2C_Mem_Read_Bytes(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
                                   uint16_t MemAddSize, uint32_t *pData, uint16_t Size, uint32_t Timeout);
																	 
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
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */

	Addr_FLAG_INIT_EEPROM = 0;
	Addr_Num_Of_On_Device = 1;
	
	if(HAL_I2C_IsDeviceReady(&hi2c2, (DeviceAddress << 1), 3, 100) == HAL_OK)
	{
		//--- reset FLAG_INIT_EEPROM ---
//		HAL_I2C_Mem_Write(&hi2c2, (DeviceAddress << 1), 0, I2C_MEMADD_SIZE_16BIT, (uint8_t*) 0xFF, 1, 100);
//		HAL_Delay(5);
		
		if(HAL_I2C_Mem_Read(&hi2c2, (DeviceAddress << 1), Addr_FLAG_INIT_EEPROM, I2C_MEMADD_SIZE_16BIT, &FLAG_INIT_EEPROM, 1, 100) == HAL_OK)
		{
			if(FLAG_INIT_EEPROM != VAL_FLAG_INIT_EE)
			{
				FLAG_INIT_EEPROM = VAL_FLAG_INIT_EE;
				if(HAL_I2C_Mem_Write(&hi2c2, (DeviceAddress << 1), Addr_FLAG_INIT_EEPROM, I2C_MEMADD_SIZE_16BIT, &FLAG_INIT_EEPROM, 1, 100) == HAL_OK)
				{
					HAL_Delay(5);
				}
				else
				{
					FLAG_ERROR = 1;
				}
				
				Num_Of_ON_Device = 1;
				if(I2C_Mem_Write_Bytes(&hi2c2, (DeviceAddress << 1), Addr_Num_Of_On_Device, I2C_MEMADD_SIZE_16BIT, &Num_Of_ON_Device, 4, 100) == HAL_OK)
				{
					HAL_Delay(5);
				}
				else
				{
					FLAG_ERROR = 1;
				}
			}
			else
			{
				if(I2C_Mem_Read_Bytes(&hi2c2, (DeviceAddress << 1), Addr_Num_Of_On_Device, I2C_MEMADD_SIZE_16BIT, &Num_Of_ON_Device, 4, 100) == HAL_OK)
				{
					Num_Of_ON_Device++;
					if(I2C_Mem_Write_Bytes(&hi2c2, (DeviceAddress << 1), Addr_Num_Of_On_Device, I2C_MEMADD_SIZE_16BIT, &Num_Of_ON_Device, 4, 100) == HAL_OK)
					{
						HAL_Delay(5);
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
		}
		else
		{
			FLAG_ERROR = 1;
		}
		
		
		//Its for test only...
		if(I2C_Mem_Read_Bytes(&hi2c2, (DeviceAddress << 1), Addr_Num_Of_On_Device, I2C_MEMADD_SIZE_16BIT, &Num_Of_ON_Device, 4, 100) == HAL_OK)
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

HAL_StatusTypeDef I2C_Mem_Write_Bytes(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
                                    uint16_t MemAddSize, uint32_t *pData, uint16_t Size, uint32_t Timeout)
{
	HAL_StatusTypeDef status;
	uint8_t data[Size];
	
	for(uint16_t i = 0; i < Size; i++)
		data[i] = (*pData >> (8*(Size-1)-8*i));
	
//	data[0] = (*pData >> 24);
//	data[1] = (*pData >> 16);
//	data[2] = (*pData >> 8);
//	data[3] = *pData;
	
	status = HAL_I2C_Mem_Write(hi2c, DevAddress, MemAddress, MemAddSize, data, Size, Timeout);
	HAL_Delay(5);

	return status;
}

HAL_StatusTypeDef I2C_Mem_Read_Bytes(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
                                   uint16_t MemAddSize, uint32_t *pData, uint16_t Size, uint32_t Timeout)
{
	HAL_StatusTypeDef status;
	uint8_t data[Size];
	
	status = HAL_I2C_Mem_Read(hi2c, DevAddress, MemAddress, MemAddSize, data, Size, Timeout);
	
	*pData = 0;
	for(uint16_t i = 0; i < Size; i++)
		*pData |= (data[i] << (8*(Size-1)-8*i));
	
//	*pData = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | (data[3]);
	
	return status;
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
