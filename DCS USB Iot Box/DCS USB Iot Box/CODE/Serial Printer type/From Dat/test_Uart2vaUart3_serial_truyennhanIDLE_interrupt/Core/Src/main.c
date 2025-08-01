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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
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
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define BUFFER_SIZE 4096
typedef struct{
	uint8_t ui8_rx2Data[BUFFER_SIZE];
	uint8_t ui8_rx3Data[BUFFER_SIZE];

	uint8_t ui8_tx2Data[BUFFER_SIZE];
	uint8_t ui8_tx3Data[BUFFER_SIZE];

	uint8_t ui8_indexUart2;
	uint8_t ui8_indexUart3;

	uint32_t ui32_timer;
	uint32_t ui32_startTick;
	uint32_t ui32_CountTX;

	int uart2_processingData;
	int uart3_processingData;
	int uart2_receivedData;
	int uart3_receivedData;

	uint16_t ui16_tx2Size;
	uint16_t ui16_tx3Size;

	uint16_t ui16_countIT2;
	uint16_t ui16_countIT3;

	uint16_t ui16_count2;
	uint16_t ui16_count3;

	uint16_t ui16_countTX2;
	uint16_t ui16_countTX3;

	uint16_t ui16_data2Size;
	uint16_t ui16_data3Size;

}xulyuart_typedef;

xulyuart_typedef xulyUart = {
		.ui8_indexUart2 = 0,
		.ui8_indexUart3 = 0,

		.uart2_processingData = 0,
		.uart2_receivedData = 0,
		.uart3_processingData = 0,
		.uart3_receivedData = 0,

		.ui32_CountTX = 0,

		.ui16_countIT2 = 0,
		.ui16_countIT3 = 0,

		.ui16_count2 = 0,
		.ui16_count3 = 0,

		.ui16_countTX2 = 0,
		.ui16_countTX3 = 0,

		.ui16_data2Size = 0,
		.ui16_data3Size = 0
};

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart->Instance == USART2)
	{
//		xulyUart.ui16_countIT2++;
		xulyUart.ui16_data2Size += Size;

		xulyUart.ui16_tx3Size = Size;

		if(xulyUart.ui16_tx3Size < 1024)
		{
			xulyUart.ui16_countIT2++;
		}
		memcpy(xulyUart.ui8_tx3Data, xulyUart.ui8_rx2Data, xulyUart.ui16_tx3Size);

		xulyUart.uart2_receivedData = 1;
//		xulyUart.ui8_indexUart2 = 1;

		HAL_UARTEx_ReceiveToIdle_IT(&huart2, xulyUart.ui8_rx2Data, BUFFER_SIZE);
//		PrintUart2Data(xulyUart.ui8_rx2Data, xulyUart.ui16_tx3Size);
	}
	if (huart->Instance == USART3)
	{
//		xulyUart.ui16_countIT3++;
		xulyUart.ui16_data3Size += Size;

		xulyUart.ui16_tx2Size = Size;

		if(xulyUart.ui16_tx2Size < 1024)
		{
			xulyUart.ui16_countIT3++;
		}

		memcpy(xulyUart.ui8_tx2Data, xulyUart.ui8_rx3Data, xulyUart.ui16_tx2Size);
		xulyUart.uart3_receivedData = 1;
//		xulyUart.ui8_indexUart3 = 1;
		HAL_UARTEx_ReceiveToIdle_IT(&huart3, xulyUart.ui8_rx3Data, BUFFER_SIZE);
//		PrintUart3Data(xulyUart.ui8_rx3Data, xulyUart.ui16_tx2Size);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART2)
	{
		xulyUart.ui16_countTX2++;
		xulyUart.uart2_processingData = 0;
	}
	if (huart->Instance == USART3)
	{
		xulyUart.ui16_countTX3++;
		xulyUart.uart3_processingData = 0;
	}
}
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
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(GPIOC, EN1_Pin, 1);//1
  HAL_GPIO_WritePin(GPIOC, EN2_Pin, 0);//0

  char str[]="\nSTM205VGT6 hello";
//  HAL_UART_Transmit(&huart1, (uint8_t *)str, sizeof(str), 100);
//  HAL_UART_Transmit(&huart2, (uint8_t *)str, sizeof(str), 100);
//  HAL_UART_Transmit(&huart3, (uint8_t *)str, sizeof(str), 100);

  HAL_UARTEx_ReceiveToIdle_IT(&huart2, xulyUart.ui8_rx2Data, BUFFER_SIZE);
  HAL_UARTEx_ReceiveToIdle_IT(&huart3, xulyUart.ui8_rx3Data, BUFFER_SIZE);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		if(xulyUart.ui32_timer<HAL_GetTick())
		{
			xulyUart.ui32_timer=HAL_GetTick()+1000;
			char str3[]="\nSTM3205VGT6 3333";
			char str2[]="\nSTM3205VGT6 2222";
//			HAL_UART_Transmit(&huart2, (uint8_t *) str2,sizeof(str2) , 10);
//			HAL_UART_Transmit(&huart3, (uint8_t *) str3,sizeof(str3) , 10);

			HAL_GPIO_TogglePin(GPIOD, LED_Pin);
		}

		  if(xulyUart.uart3_receivedData == 1 && xulyUart.uart2_processingData == 0)
		  {
			  HAL_UART_Transmit_IT(&huart2, xulyUart.ui8_tx2Data, xulyUart.ui16_tx2Size);
			  xulyUart.uart2_processingData = 1;
			  xulyUart.uart3_receivedData = 0;
		  }
		  //nhan data tu uart 2 gui cho uart3
		  if(xulyUart.uart2_receivedData == 1 && xulyUart.uart3_processingData == 0)
		  {
			  HAL_UART_Transmit_IT(&huart3, xulyUart.ui8_tx3Data, xulyUart.ui16_tx3Size);
			  xulyUart.uart3_processingData = 1;
			  xulyUart.uart2_receivedData = 0;
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 20;
  RCC_OscInitStruct.PLL.PLLN = 192;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 38400;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  /* DMA1_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, EN1_Pin|EN2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : EN1_Pin */
  GPIO_InitStruct.Pin = EN1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(EN1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : EN2_Pin */
  GPIO_InitStruct.Pin = EN2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(EN2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
