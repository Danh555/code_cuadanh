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
#include "FLASH_SECTOR_F2.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FLASH_SECTOR_ADDRESS1 ADDR_FLASH_SECTOR_1	/* Base @ of Sector 0, 16 Kbytes */
#define FLASH_SECTOR_ADDRESS2 ADDR_FLASH_SECTOR_2	/* Base @ of Sector 1, 16 Kbytes */
#define FLASH_SECTOR_ADDRESS3 ADDR_FLASH_SECTOR_3	/* Base @ of Sector 2, 16 Kbytes */
#define FLASH_SECTOR_ADDRESS4 ADDR_FLASH_SECTOR_4	/* Base @ of Sector 3, 16 Kbytes */
#define FLASH_SECTOR_ADDRESS5 ADDR_FLASH_SECTOR_5	/* Base @ of Sector 4, 64 Kbytes */
#define FLASH_SECTOR_ADDRESS6 ADDR_FLASH_SECTOR_6	/* Base @ of Sector 5, 128 Kbytes */
#define FLASH_SECTOR_ADDRESS7 ADDR_FLASH_SECTOR_7	/* Base @ of Sector 6, 128 Kbytes */
#define FLASH_SECTOR_ADDRESS8 ADDR_FLASH_SECTOR_8	/* Base @ of Sector 7, 128 Kbytes */
#define FLASH_SECTOR_ADDRESS9 ADDR_FLASH_SECTOR_9	/* Base @ of Sector 8, 128 Kbytes */
#define FLASH_SECTOR_ADDRESS10 ADDR_FLASH_SECTOR_10	/* Base @ of Sector 9, 128 Kbytes */
#define FLASH_SECTOR_ADDRESS11 ADDR_FLASH_SECTOR_11	/* Base @ of Sector 10, 128 Kbytes */
#define FLASH_SECTOR_ADDRESS12 ADDR_FLASH_SECTOR_12	/* Base @ of Sector 11, 128 Kbytes */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
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
static void MX_USART1_UART_Init(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
/* USER CODE BEGIN PFP */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar (int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc (int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,0xFFFF);

return ch;
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define BUFFER_SIZE 4096
#define MAX_BUFFER_CMD_ESP 20
#define RX5_BUFF_SIZE   20

#define MAX_USER_GET_UART_REQUEST 1000

bool save_to_flash_flag = false;
uint8_t flash_data_buffer[BUFFER_SIZE];
uint16_t flash_data_size = 0;

uint8_t USER_RX_Buffer[RX5_BUFF_SIZE];

uint32_t Rx_Data[BUFFER_SIZE];
uint8_t u[5];
uint8_t temp[5];

uint8_t ui8_status_esp =0U;
uint32_t ui32_timerxset =0;

uint8_t uart2_temp_buffer[BUFFER_SIZE];
uint16_t uart2_temp_size = 0;
uint32_t last_save_tick = 0;

typedef struct{
	uint8_t ui8_RX_buf[2];
	uint8_t ui8_str[MAX_BUFFER_CMD_ESP];
	uint8_t ui8_busy;
	uint8_t ui8_newinchar;
	uint8_t ui8_RX;
	uint8_t ui8_indexUart;
	uint8_t ui8_newpagkage;
	uint8_t ui8_newcmd;
	uint32_t CountTX;

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
		.CountTX = 0,
		.ui8_indexUart = 0,

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

typedef enum {
    UART_STATE_IDLE,
    UART_STATE_RECEIVED,
    UART_STATE_PROCESSING,
    UART_STATE_TRANSMITTING,
    UART_STATE_COMPLETED
} UART_State;

UART_State uart2_state = UART_STATE_IDLE;
UART_State uart3_state = UART_STATE_IDLE;

#if 0
// xu ly data nhan duoc tu may in (printer)
void PrintUart3Data(uint8_t* data, uint16_t size) {
//	printf("(Read Data): \n");
    for (int j = 0; j < size; j++) {
//        printf("%d = %02x ", j, data[j]);
//    	printf("%02x ", data[j]);

#if 1
    	//truong hop nap may in dang mo
    	if (data[j] == 0x3c && data[j+1] == 0x00 && data[j+2] == 0x00 && data[j+3] == 0x0f)
    	{
//    		if(xulyUart.ui8_indexUart3 == 1)
//    		{
    			printf("(Read Data): \n");
    			printf("Mo nap may \n");
//    			printf("%02x %02x %02x %02x \n", data[j], data[j+1], data[j+2], data[j+3]);
//    		}
    	} else if (data[j] == 0x3c && data[j+1] == 0x00 && data[j+2] == 0x03 && data[j+3] == 0x0f) //truong hop nap may in dang mo (nhưng đang hết giấy)
    	{
//    		if(xulyUart.ui8_indexUart3 == 1)
//    		{
        		printf("(Read Data): \n");
        		printf("Mo nap may (PAPER OUT) \n");
//        		printf("%02x %02x %02x %02x \n", data[j], data[j+1], data[j+2], data[j+3]);
//    		}
    	}

    	//truong hop nap may in dang dong nap
    	if (data[j] == 0x14 && data[j+1] == 0x00 && data[j+2] == 0x00 && data[j+3] == 0x0f)
    	{
//    		if(xulyUart.ui8_indexUart3 == 1)
//    		{
    			printf("(Read Data): \n");
    			printf("Dong nap may \n");
//    			printf("%02x %02x %02x %02x \n", data[j], data[j+1], data[j+2], data[j+3]);
//    		}
    	} else if (data[j] == 0x14 && data[j+1] == 0x00 && data[j+2] == 0x03 && data[j+3] == 0x0f) //truong hop nap may in dang mo (nhưng đang hết giấy)
    	{
//    		if(xulyUart.ui8_indexUart3 == 1)
//    		{
        		printf("(Read Data): \n");
        		printf("Dong nap may (PAPER OUT) \n");
//        		printf("%02x %02x %02x %02x \n", data[j], data[j+1], data[j+2], data[j+3]);
//    		}
    	}

//    	//truong hop may in dang feed giay
    	if (data[j] == 0x5c && data[j+1] == 0x00 && data[j+2] == 0x00 && data[j+3] == 0x0f)
    	{
    		printf("(Read Data): \n");
    		printf("FEED \n");
//    		printf("%02x %02x %02x %02x \n", data[j], data[j+1], data[j+2], data[j+3]);
    	} else if (data[j] == 0x5c && data[j+1] == 0x00 && data[j+2] == 0x03 && data[j+3] == 0x0f)
    	{
    		printf("(Read Data): \n");
    		printf("FEED (PAPER OUT) \n");
//    		printf("%02x %02x %02x %02x \n", data[j], data[j+1], data[j+2], data[j+3]);
    	}

    	//tat may in
    	if(data[j] == 0x1c && data[j+1] == 0x00 && data[j+2] == 0x00 && data[j+3] == 0x0f)
    	{
    		printf("(Read Data): \n");
    		printf("Turn off machine \n");
//    		printf("%02x %02x %02x %02x \n", data[j], data[j+1], data[j+2],data[j+3]);
    	} else if (data[j] == 0x1c && data[j+1] == 0x00 && data[j+2] == 0x03 && data[j+3] == 0x0f)
    	{
    		printf("(Read Data): \n");
    		printf("Turn off machine (PAPER OUT) \n");
//    		printf("%02x %02x %02x %02x \n", data[j], data[j+1], data[j+2], data[j+3]);
    	}

    	//mo may in


    	//thong tin may in
    	if(data[j] == 0x5f && data[j+1] == 0x54 && data[j+2] == 0x4d && data[j+3] == 0x2d && data[j+4] == 0x54 && data[j+5] == 0x38 && data[j+6] == 0x38 && data[j+7] == 0x49 && data[j+8] == 0x49 && data[j+9] == 0x49 && data[j+10] == 0x00)
    	{
//    		if(xulyUart.ui8_indexUart3 == 1)
//    		{
    			printf("(Read Data): \n");
    			printf("_TM-T88III. \n");
//    			printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n", data[j], data[j+1], data[j+2],
//    									data[j+3], data[j+4], data[j+5], data[j+6], data[j+7],
//										data[j+8], data[j+9], data[j+10]);
//    		}
    	}

    	//


#endif
    }
    printf("\n");
}
#endif

#if 1
void PrintUart3Data(uint8_t* data, uint16_t size)
{
	for (int j = 0; j < size; j++)
	{
		switch (data[j])
		{
		case 0x3c:
			if (data[j+1] == 0x00 && data[j+2] == 0x00 && data[j+3] == 0x0f)
			{
				printf("(Read Data): Mo nap may\n");
			}
			else if(data[j+1] == 0x00 && data[j+2] == 0x03 && data[j+3] == 0x0f)
			{
				printf("(Read Data): Mo nap may (PAPER OUT)\n");
			}
			break;
		case 0x14:
			if (data[j+1] == 0x00 && data[j+2] == 0x00 && data[j+3] == 0x0f)
			{
				printf("(Read Data): Dong nap may\n");
			}
			else if (data[j+1] == 0x00 && data[j+2] == 0x03 && data[j+3] == 0x0f)
			{
				printf("(Read Data): Dong nap may (PAPER OUT)\n");
			}
			break;
		case 0x5c:
			if (data[j+1] == 0x00 && data[j+2] == 0x00 && data[j+3] == 0x0f)
			{
				printf("(Read Data): FEED\n");
			}
			else if (data[j+1] == 0x00 && data[j+2] == 0x03 && data[j+3] == 0x0f)
			{
				printf("(Read Data): FEED (PAPER OUT)\n");
			}
			break;
		case 0x1c:
			if (data[j+1] == 0x00 && data[j+2] == 0x00 && data[j+3] == 0x0f)
			{
				printf("(Read Data): Turn off machine\n");
			}
			else if (data[j+1] == 0x00 && data[j+2] == 0x03 && data[j+3] == 0x0f)
			{
				printf("(Read Data): Turn off machine (PAPER OUT)\n");
			}
			break;
		case 0x5f:
            if (data[j+1] == 0x54 && data[j+2] == 0x4d && data[j+3] == 0x2d && data[j+4] == 0x54 &&
                data[j+5] == 0x38 && data[j+6] == 0x38 && data[j+7] == 0x49 && data[j+8] == 0x49 &&
                data[j+9] == 0x49 && data[j+10] == 0x00)
            {
            	printf("(Read Data): _TM-T88III.\n");
            }
            break;
		default:
			break;
		}
	}
	printf("\n");
}
#endif

#if (ChuaVietXongHamNay)
void ProcessUartData(uint8_t* data, uint16_t size) {
    for (uint16_t i = 0; i < size; i++) {
        // Check for start of print job
        if (!printing && data[i] == 0x1b && i + 5 < size &&
            data[i + 1] == 0x3d && data[i + 2] == 0x01 &&
            data[i + 3] == 0x1d && data[i + 4] == 0x28) {
            printing = true;
            print_buffer_index = 0;
            current_flash_sector = FLASH_SECTOR_ADDRESS5; // Start from the first sector
            printf("Start of print job\n");
        }

        // Check for end of print job
        if (printing && data[i] == 0x1b && i + 5 < size &&
            data[i + 1] == 0x3d && data[i + 2] == 0x01 &&
            data[i + 3] == 0x1d && data[i + 4] == 0x29) {
            printing = false;
            if (print_buffer_index > 0) {
                uint32_t num_of_words = (print_buffer_index / 4) + ((print_buffer_index % 4) != 0);
                Flash_Write_Data(current_flash_sector, (uint32_t *)print_buffer, num_of_words);
            }
            printf("End of print job\n");
        }

        if (printing) {
            print_buffer[print_buffer_index++] = data[i];

            // If buffer is full, write to flash
            if (print_buffer_index >= FLASH_SECTOR_SIZE) {
                Flash_Erase_sector(current_flash_sector);
                uint32_t num_of_words = FLASH_SECTOR_SIZE / 4;
                Flash_Write_Data(current_flash_sector, (uint32_t *)print_buffer, num_of_words);
                current_flash_sector += FLASH_SECTOR_SIZE;
                print_buffer_index = 0;
            }
        }
    }
}
#endif

#if 0
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
//		memcpy(ui8_tempBuf, xulyUart.ui8_rx2Data, xulyUart.ui16_tx3Size);

		xulyUart.uart2_receivedData = 1;
//		xulyUart.ui8_indexUart2 = 1;

		HAL_UARTEx_ReceiveToIdle_IT(&huart2, xulyUart.ui8_rx2Data, BUFFER_SIZE);
//		PrintUart2Data(xulyUart.ui8_rx2Data, xulyUart.ui16_tx3Size);
		uart2_state = UART_STATE_RECEIVED;
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
		PrintUart3Data(xulyUart.ui8_rx3Data, xulyUart.ui16_tx2Size);
		uart3_state = UART_STATE_RECEIVED;
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
#endif

void Handle_Uart()
{
	//test
	if(USER_RX_Buffer[0] == 't')
	{
		USER_RX_Buffer[0]=0;
		printf("Xin Chao \n");
		fflush(stdout); // Đảm bảo đầu ra ngay lập tức
	}
    else if (USER_RX_Buffer[0] == 'x')
    {
        USER_RX_Buffer[0] = 0;
        printf("Bat dau xoa flash\n");
        fflush(stdout); // Đảm bảo đầu ra ngay lập tức

        uint32_t start_tick = HAL_GetTick();

        // Xóa các sector của flash
        printf("Erasing sector 5\n");
        Flash_Erase_sector(FLASH_SECTOR_ADDRESS5);

        printf("Erasing sector 6\n");
        Flash_Erase_sector(FLASH_SECTOR_ADDRESS6);

        printf("Erasing sector 7\n");
        Flash_Erase_sector(FLASH_SECTOR_ADDRESS7);

        //neu xoa 3 vung nay se bi dung chuong trình
#if 0
        printf("Erasing sector 8\n");
        Flash_Erase_sector(FLASH_SECTOR_ADDRESS8);

        printf("Erasing sector 9\n");
        Flash_Erase_sector(FLASH_SECTOR_ADDRESS9);

        printf("Erasing sector 10\n");
        Flash_Erase_sector(FLASH_SECTOR_ADDRESS10);

        printf("Erasing sector 11\n");
        Flash_Erase_sector(FLASH_SECTOR_ADDRESS11);
#endif

        // Thử xóa các sector từ 9 đến 11 và kiểm tra lỗi
//        for (int i = 9; i <= 11; i++)
//        {
//            uint32_t sector_address = FLASH_SECTOR_ADDRESS9 + (i - 9) * (FLASH_SECTOR_ADDRESS10 - FLASH_SECTOR_ADDRESS9);
//            printf("Erasing sector %d\n", i);
//            fflush(stdout);
//
//            Flash_Erase_sector(sector_address);
//            HAL_Delay(100); // Thêm thời gian nghỉ ngơi
//        }

//        printf("Da xoa flash");

        uint32_t end_tick = HAL_GetTick();
        printf("Da xoa flash trong %lu ms\r\n", end_tick - start_tick);
        fflush(stdout); // Đảm bảo đầu ra ngay lập tức
    }
}

void xuly_command_ESP()
{
	USER_RX_Buffer[0] = xulyUart.ui8_newinchar;
	Handle_Uart();
}

void save_data_to_flash(uint8_t* data, uint32_t length) {
	int byte_count = 0;
    printf("Bat dau luu data vao flash\r\n");

    // Erase flash sector
    printf("Dang xoa sector flash\r\n");
    Flash_Erase_sector(FLASH_SECTOR_ADDRESS4);

    uint32_t num_of_words = (length / 4) + ((length % 4) != 0);

    printf("Dang ghi data vao flash\r\n");
    uint32_t start_tick = HAL_GetTick();
    Flash_Write_Data(FLASH_SECTOR_ADDRESS4, (uint32_t *)data, num_of_words);
    uint32_t end_tick = HAL_GetTick();
    printf("Ghi hoan tat trong %lu ms\r\n", end_tick - start_tick);

    Flash_Read_Data(FLASH_SECTOR_ADDRESS4, Rx_Data, num_of_words);

    printf("Data doc tu flash:\r\n");
    for (int i = 0; i < num_of_words; i++) {

    	byte_count += 4; // Mỗi lần in ra 4 byte

        convertdata(Rx_Data[i], u);
        printf("%02x %02x %02x %02x ", u[0], u[1], u[2], u[3]);

        if (byte_count > 20) {
            printf("\n"); // Xuống hàng
            byte_count = 0; // Đặt lại biến đếm
        }
    }
    printf("\n");
}

void wait_res_init(uint32_t timeout_wait_)
{
	ui32_timerxset = HAL_GetTick() + timeout_wait_;
	ui8_status_esp =1U;

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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(GPIOC, EN1_Pin, 1);
  HAL_GPIO_WritePin(GPIOC, EN2_Pin, 0);

//  char str[]="\nSTM205VGT6 hello";
//  HAL_UART_Transmit(&huart1, (uint8_t *)str, sizeof(str), 100);
//  HAL_UART_Transmit(&huart2, (uint8_t *)str, sizeof(str), 100);
//  HAL_UART_Transmit(&huart3, (uint8_t *)str, sizeof(str), 100);

  HAL_UART_Receive_IT(&huart1, xulyUart.ui8_RX_buf, 1);
  HAL_UARTEx_ReceiveToIdle_IT(&huart2, xulyUart.ui8_rx2Data, BUFFER_SIZE);
  HAL_UARTEx_ReceiveToIdle_IT(&huart3, xulyUart.ui8_rx3Data, BUFFER_SIZE);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//		if(xulyUart.ui32_timer<HAL_GetTick())
//		{
//			xulyUart.ui32_timer=HAL_GetTick()+1000;
////			char str3[]="\nSTM3205VGT6 3333";
////			char str2[]="\nSTM3205VGT6 2222";
////			HAL_UART_Transmit(&huart2, (uint8_t *) str2,sizeof(str2) , 10);
////			HAL_UART_Transmit(&huart3, (uint8_t *) str3,sizeof(str3) , 10);
//
//			HAL_GPIO_TogglePin(GPIOD, LED_Pin);
//		}

      if (xulyUart.ui32_timer < HAL_GetTick()) {
          xulyUart.ui32_timer = HAL_GetTick() + 5000;
          HAL_GPIO_TogglePin(GPIOD, LED_Pin);
      }

#if 0
      if(xulyUart.CountTX < xulyUart.ui8_indexUart)
      {
		  int vitridoctin = xulyUart.CountTX % MAX_USER_GET_UART_REQUEST;
		  uint8_t inchar = xulyUart.ui8_str[vitridoctin];
		  xulyUart.CountTX++;
//		  HAL_UART_Transmit_IT(&huart2, &inchar, 1);
//		  HAL_UART_Transmit(&huart1, &inchar, sizeof(inchar), 1000);
      }bdeeee
#endif

//	  if (xulyUart.ui8_newcmd == 1){
//		  HAL_UART_Transmit(&huart2, (uint8_t *) xulyUart.ui8_str,xulyUart.ui8_indexUart, 10);
////		  HAL_UART_Transmit(&huart3, (uint8_t *) xulyUart.ui8_str,xulyUart.ui8_indexUart, 10);
//		  xulyUart.ui8_newcmd =0;
//	  }
	  	  //nhan data tu uart 3 gui cho uart2
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
//		  HAL_UART_Transmit(&huart2, xulyUart.ui8_tx3Data,xulyUart.ui16_tx3Size, 100);
//		  save_data_to_flash(xulyUart.ui8_tx3Data, xulyUart.ui16_tx3Size);
		  xulyUart.uart3_processingData = 1;
		  xulyUart.uart2_receivedData = 0;
	  }

	  if(xulyUart.ui8_newpagkage == 1)
	  {
//		  HAL_UART_Transmit(&huart2, (uint8_t *) xulyUart.ui8_str,xulyUart.ui8_indexUart, 100);
		  xulyUart.ui8_newpagkage = 0;
		  xuly_command_ESP();
	  }

	  // Xử lý lưu dữ liệu vào flash
//	  if (save_to_flash_flag)
//	  {
//		  save_to_flash_flag = false;
//		  printf("flash_data_size: %d \n",flash_data_size);
//		  save_data_to_flash(flash_data_buffer, flash_data_size);
//	  }


#if 0
      switch (uart3_state) {
          case UART_STATE_RECEIVED:
              if (xulyUart.uart2_processingData == 0) {
                  HAL_UART_Transmit_IT(&huart2, xulyUart.ui8_tx2Data, xulyUart.ui16_tx2Size);
                  xulyUart.uart2_processingData = 1;
                  xulyUart.uart3_receivedData = 0;
                  uart3_state = UART_STATE_PROCESSING;
              }
              break;
          case UART_STATE_PROCESSING:
              break;
          case UART_STATE_COMPLETED:
              uart3_state = UART_STATE_IDLE;
              break;
          default:
              break;
      }

      switch (uart2_state)
      {
      case UART_STATE_RECEIVED:
    	  if (xulyUart.uart3_processingData == 0)
    	  {
    		  HAL_UART_Transmit_IT(&huart3, xulyUart.ui8_tx3Data, xulyUart.ui16_tx3Size);
    		  xulyUart.uart3_processingData = 1;
    		  xulyUart.uart2_receivedData = 0;
    		  uart2_state = UART_STATE_PROCESSING;
    	  }
    	  break;
      case UART_STATE_PROCESSING:
    	  break;
      case UART_STATE_COMPLETED:
    	  uart2_state = UART_STATE_IDLE;
    	  break;
      default:
    	  break;
      }
#endif

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
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		/*xử lý nhận uart theo gói*/
		uint8_t inchar = xulyUart.ui8_RX_buf[0];
		if (xulyUart.ui8_busy == 0)
		{
			if(inchar =='#')
			{
				xulyUart.ui8_RX = 1;
				xulyUart.ui8_indexUart = 0;
			}

			if(xulyUart.ui8_RX == 1)
			{
				xulyUart.ui8_str[xulyUart.ui8_indexUart] = inchar;
				xulyUart.ui8_indexUart++;

				if(xulyUart.ui8_indexUart > MAX_BUFFER_CMD_ESP -1)
				{
					/*tràn buffer*/
					xulyUart.ui8_RX = 0;
					xulyUart.ui8_indexUart = 0;
				}
				if(inchar == '$')
				{
					xulyUart.ui8_newcmd = 1;
					xulyUart.ui8_RX = 0;
					xulyUart.ui8_busy = 1;
				}
			}
			else
			{
				xulyUart.ui8_newpagkage = 1;
				xulyUart.ui8_newinchar = inchar;
			}
		}
		HAL_UART_Receive_IT(&huart1, xulyUart.ui8_RX_buf, 1);
	}
}

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
//		memcpy(ui8_tempBuf, xulyUart.ui8_rx2Data, xulyUart.ui16_tx3Size);

		xulyUart.uart2_receivedData = 1;
//		xulyUart.ui8_indexUart2 = 1;

		xulyUart.ui32_timer = HAL_GetTick();
		HAL_UARTEx_ReceiveToIdle_IT(&huart2, xulyUart.ui8_rx2Data, BUFFER_SIZE);
//		save_data_to_flash(xulyUart.ui8_rx2Data, xulyUart.ui16_tx3Size);
//		PrintUart2Data(xulyUart.ui8_rx2Data, xulyUart.ui16_tx3Size);

		// Đặt cờ và sao chép dữ liệu vào buffer flash
//		save_to_flash_flag = true;
//
//		memcpy(flash_data_buffer, xulyUart.ui8_rx2Data, Size);
//		flash_data_size = Size;

//		uart2_state = UART_STATE_RECEIVED;
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
		PrintUart3Data(xulyUart.ui8_rx3Data, xulyUart.ui16_tx2Size);
//		uart3_state = UART_STATE_RECEIVED;
	}
}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//	if (huart->Instance == USART1)
//	{
//		uint8_t inchar = xulyUart.ui8_RX_buf[0];
//		int vitridoctin = xulyUart.ui8_indexUart % 1000;
//		xulyUart.ui8_str[vitridoctin] = inchar;
//		xulyUart.ui8_indexUart++;
//		HAL_UART_Receive_IT(&huart1, xulyUart.ui8_RX_buf, 1);
//	}
//}

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
