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
#include "flash.h"
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void wait_res_init(uint32_t timeout_wait_);
void WriteBufferToFlash(uint8_t* buffer, uint32_t length);
void ReadAndPrintFlashData(uint32_t flash_address, uint32_t length);
void ReadAndPrintFirst4096Bytes(uint32_t flash_address);
void save_data_to_flash(uint8_t* data, uint32_t length);
void Write_Byte(uint8_t* buffer, uint32_t length);
void PrintTempBufferData();

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
#define MAX_BUFFER_CMD_ESP 10000
#define WAIT_TIME_MS 20
#define RESET_TIME_MS 30000  // 30s
#define RX5_BUFF_SIZE   20

#define CHUNK_SIZE 4096
#define READ_CHUNK_SIZE 4096  // Kích thước khối đọc mỗi lần, 4096 byte

#define MAX_TEMP_BUFFER_SIZE 1024 * 50  // Kích thước buffer tạm thời lớn nhất, ví dụ cho 50 gói 4096 byte

#define BLOCK_SIZE 1024

uint8_t temp_buffer[MAX_TEMP_BUFFER_SIZE];
uint32_t temp_buffer_index = 0;
uint32_t last_flash_write_time = 0;
uint32_t flash_sector_address = FLASH_SECTOR_ADDRESS5;  // Địa chỉ sector flash để ghi

uint8_t USER_RX_Buffer[RX5_BUFF_SIZE];

uint8_t ui8_status_esp =0U;
uint32_t ui32_timerxset =0;

uint8_t uart2_data_ready = 0;
uint32_t uart2_start_time = 0;
uint8_t uart2_data[BUFFER_SIZE];
uint32_t last_packet_time = 0;
int packetCount = 0;  // Biến đếm gói toàn cục

uint8_t resetFlag = 0;  // Biến co để ghi nhớ việc reset

// Thêm biến toàn cục để lưu tổng số gói và tổng số byte đã nhận
uint32_t total_packets = 0;
uint32_t total_bytes = 0;

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
	uint8_t count;

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
		.ui32_timer = 0,

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

#if 1
//void ProcessUart2Data(uint8_t* data, uint16_t size) {
//    // Kiểm tra nếu kích thước là chính xác 4096 byte
//    if (size <= CHUNK_SIZE) {
//
//        // Nếu kích thước là 6 byte, reset packetCount và đặt c�? reset
//        if (size == 6) {
//            packetCount = -1;  // �?ặt packetCount là -1 để lần g�?i tiếp theo sẽ tăng lên 0
//            resetFlag = 1;
//            printf("Reset packet count due to size 6.\n");
//        } else {
//            // Nếu c�? reset được đặt, đặt packetCount là 0 và tắt c�? reset
//            if (resetFlag) {
//                packetCount = 0;
//                resetFlag = 0;
//            } else {
//                packetCount++;
//            }
//            printf("Packet %d = %d \n", packetCount, size);
//        }
//
//        last_packet_time = HAL_GetTick();  // Cập nhật th�?i gian nhận gói cuối cùng
//    }
//}

#if 0
void ProcessUart2Data(uint8_t* data, uint16_t size) {
    // Kiểm tra nếu kích thước là chính xác 4096 byte
    if (size <= CHUNK_SIZE) {
        // Nếu kích thước là 6 byte, reset packetCount và đặt cờ reset
        if (size == 6) {
            packetCount = 0;  // Đặt packetCount là 0 để bắt đầu lại từ gói 0
            resetFlag = 1;    // Đặt cờ để cho phép in các gói tiếp theo
        } else if (resetFlag) {
            // Nếu cờ reset được đặt, in ra gói và tăng packetCount
            printf("Packet %d = %d \n", packetCount, size);

            uint32_t time1;
            uint32_t time2;

            if (temp_buffer_index + size <= MAX_TEMP_BUFFER_SIZE) {
                memcpy(&temp_buffer[temp_buffer_index], data, size);
                int num_of_words = (size / 4) + ((size % 4) != 0);

                time1 = HAL_GetTick();
                Flash_Write_Data(flash_sector_address, (uint32_t *)temp_buffer, num_of_words);
                time2 = HAL_GetTick();

                time2 -= time1;
                printf("\n Write addr = %lu, size = %d, time = %lu\n", flash_sector_address , num_of_words, time2);

                temp_buffer_index += size;
                flash_sector_address += size;  // Cập nhật địa chỉ sector flash
            }

            packetCount++;
            total_packets++;
            total_bytes += size;

            // Reset chỉ số buffer tạm sau khi ghi
            if (packetCount % 1 == 0) {
                temp_buffer_index = 0;  // Reset chỉ số buffer tạm
            }
        }

        last_packet_time = HAL_GetTick();  // Cập nhật thời gian nhận gói cuối cùng
    }
}
#endif

void ProcessUart2Data(uint8_t* data, uint16_t size) {
	uint32_t time1;
	uint32_t time2;

	uint32_t timer1;
	uint32_t timer2;
	uint16_t lentosend;
	uint32_t totaltam =0;
	uint16_t len_tam;
	int num_of_words;
	int numofwords;


    // Kiểm tra nếu kích thước là chính xác 4096 byte hoặc nhỏ hơn
    if (size <= CHUNK_SIZE) {
        // Nếu kích thước là 6 byte, reset packetCount và đặt cờ reset
        if (size == 6) {
            packetCount = 0;  // Đặt packetCount là 0 để bắt đầu lại từ gói 0
            resetFlag = 1;    // Đặt cờ để cho phép nhận các gói tiếp theo
//            printf("Reset packet count due to size 6.\n");
        } else if (resetFlag) {
            // Nếu cờ reset được đặt, in ra gói và tăng packetCount
        	len_tam = size;
            printf("Packet %d = %d\n", packetCount, size);
//            if (temp_buffer_index + size <= MAX_TEMP_BUFFER_SIZE) {
//                memcpy(&temp_buffer[temp_buffer_index], data, size);
//                temp_buffer_index += size;
//            }


            memcpy(&temp_buffer[temp_buffer_index], data, size);
            temp_buffer_index = size;

            Handle_Uart();
//            Write_Byte(temp_buffer, temp_buffer_index);

#if 0
            uint16_t remaining = size;
            uint8_t* current_data_ptr = temp_buffer;

            while (remaining > 0)
            {
            	lentosend = (remaining >= BLOCK_SIZE) ? BLOCK_SIZE : remaining;
//            	totaltam += lentosend;
            	num_of_words = lentosend;

            	time1 = HAL_GetTick();
            	Flash_Write_byte(flash_sector_address, current_data_ptr, num_of_words);
            	time2 = HAL_GetTick();
            	time2 -= time1;
            	printf("\n Write addr = %lu, size = %d, time = %ld\n", flash_sector_address, num_of_words, time2);
            	flash_sector_address += lentosend;

            	remaining -= lentosend;
            	current_data_ptr += lentosend;
            }
#endif

//Flash_Write_byte
#if 0

//        	flash_sector_address = ADDR_FLASH_SECTOR_5;
//            int num_of_words = (size / 4) + ((size % 4) != 0);

            lentosend = size;
            totaltam += lentosend;
            num_of_words = lentosend;

            time1 = HAL_GetTick();
            Flash_Write_byte(flash_sector_address, (uint8_t *)temp_buffer, num_of_words);
            time2 = HAL_GetTick();
            time2 -= time1;
            printf("\n Write addr = %lu, size = %d, time = %ld\n", flash_sector_address , num_of_words, time2);
            flash_sector_address += lendtosend;

#endif

//Flash_Write_data
#if 0
        	lentosend =size;
        	totaltam+=lentosend;

        	numofwords = (lentosend/4)+((lentosend%4)!=0);
        	timer1 = HAL_GetTick();
        	Flash_Write_Data(flash_sector_address, (uint32_t *)temp_buffer, numofwords);
        	timer2 = HAL_GetTick();
        	timer2 -= timer1;
        	printf("\n Write addr = %lu, size = %d, time = %ld\n", flash_sector_address , numofwords, timer2);
        	flash_sector_address += size;
#endif

            packetCount++;

//            printf("\nDebug flash addr = %lu, size = %d \n", flash_sector_address, size);
            total_packets++;
            total_bytes += size;

//            if(packetCount % 1 == 0)
//            {
//            	temp_buffer_index = 0;  // Reset chỉ số buffer tạm
//            }

            // Reset chỉ số buffer tạm
            temp_buffer_index = 0;

            // Đánh dấu thời gian nhận gói cuối cùng
            last_packet_time = HAL_GetTick();
        }
    }
}



//test luu
//void ProcessUart2Data(uint8_t* data, uint16_t size) {
//    // Kiểm tra nếu kích thước là chính xác 4096 byte
//    if (size <= CHUNK_SIZE) {
//        // Nếu kích thước là 6 byte, reset packetCount và đặt c�? reset
//        if (size == 6) {
//            packetCount = 0;  // �?ặt packetCount là 0 để bắt đầu lại từ gói 0
//            resetFlag = 1;    // �?ặt c�? để cho phép in các gói tiếp theo
////            printf("Reset packet count due to size 6.\n");
//        } else if (resetFlag) {
//            // Nếu c�? reset được đặt, in ra gói và tăng packetCount
//            printf("Packet %d = %d \n", packetCount, size);
//            packetCount++;
//
//            total_packets++;
//            total_bytes += size;
//
//            // Kiểm tra nếu đã nhận đủ 10 gói, ghi buffer vào flash và reset buffer
//            if (packetCount == 1) {
//            	save_data_to_flash(data, size);
//            }
//
//        }
//
//        last_packet_time = HAL_GetTick();  // Cập nhật th�?i gian nhận gói cuối cùng
//    }
//}

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

void wait_res_init(uint32_t timeout_wait_)
{
	ui32_timerxset = HAL_GetTick() + timeout_wait_;
	ui8_status_esp =1U;

}

void Handle_Uart()
{
	//test
	if(USER_RX_Buffer[0] == 't')
	{
		USER_RX_Buffer[0]=0;
		printf("Test \n");
		fflush(stdout); //  ?ảm bảo đầu ra ngay lập tức
	}

    else if (USER_RX_Buffer[0] == 'x')
    {
        USER_RX_Buffer[0] = 0;
        printf("Bat dau xoa flash\n");
        fflush(stdout); //  ?ảm bảo đầu ra ngay lập tức

        uint32_t start_tick = HAL_GetTick();

        // Xóa các sector của flash
        printf("Erasing sector 5\n");
        Flash_Erase_sector(FLASH_SECTOR_ADDRESS5);

        printf("Erasing sector 6\n");
        Flash_Erase_sector(FLASH_SECTOR_ADDRESS6);

        printf("Erasing sector 7\n");
        Flash_Erase_sector(FLASH_SECTOR_ADDRESS7);

        //neu xoa 3 vung nay se bi dung chuong trình
#if 1
        printf("Erasing sector 8\n");
        Flash_Erase_sector(FLASH_SECTOR_ADDRESS8);

        printf("Erasing sector 9\n");
        Flash_Erase_sector(FLASH_SECTOR_ADDRESS9);

        printf("Erasing sector 10\n");
        Flash_Erase_sector(FLASH_SECTOR_ADDRESS10);

        printf("Erasing sector 11\n");
        Flash_Erase_sector(FLASH_SECTOR_ADDRESS11);
#endif
        uint32_t end_tick = HAL_GetTick();
        printf("Da xoa flash trong %lu ms\r\n", end_tick - start_tick);
        fflush(stdout); //  ?ảm bảo đầu ra ngay lập tức
    }

    else if (USER_RX_Buffer[0] == 'r') // Thêm điều kiện để đọc dữ liệu từ flash
    {
        USER_RX_Buffer[0] = 0;
        printf("Bat dau doc flash\n");
        fflush(stdout); // Đảm bảo đầu ra ngay lập tức

        // Đọc và in ra 4096 byte đầu tiên từ flash
        ReadAndPrintFirst4096Bytes(FLASH_SECTOR_ADDRESS5);
        printf("Da doc flash xong. \n");
    }

    // Kiểm tra nếu ký tự nhận được là 'c'
    else if (USER_RX_Buffer[0] == 'c')
    {
        USER_RX_Buffer[0] = 0;
        printf("Bat dau doc temp_buffer\n");
        fflush(stdout); // Đảm bảo đầu ra ngay lập tức

        // In ra dữ liệu từ temp_buffer
//        ReadAndPrintFirst4096Bytes(FLASH_SECTOR_ADDRESS5);
//        PrintTempBufferData();
    }

    else if (USER_RX_Buffer[0] == 'l')
    {
        USER_RX_Buffer[0] = 0;
        printf("Bat dau doc temp_buffer\n");
        fflush(stdout); // Đảm bảo đầu ra ngay lập tức

        // In ra dữ liệu từ temp_buffer
        test_write_byte();
    }
}

void xuly_command_ESP()
{
	USER_RX_Buffer[0] = xulyUart.ui8_newinchar;
	Handle_Uart();
}

void PrintTempBufferData()
{
    printf("Data in temp_buffer:\n");
    for (uint32_t i = 0; i < 3871; i++) {
        printf("%02x ", temp_buffer[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

void WriteBufferToFlash(uint8_t* buffer, uint32_t length)
{
	uint32_t time1;
	uint32_t time2;
    int num_of_words = (length / 4) + ((length % 4) != 0);
    time1 = HAL_GetTick();
    Flash_Write_Data(flash_sector_address, (uint32_t *)buffer, num_of_words);
    time2 = HAL_GetTick();
    time2 -= time1;
    printf("\n Write addr = %lu, size = %d, time = %lu\n", flash_sector_address , num_of_words, time2);
    flash_sector_address += length;  // Cập nhật địa chỉ sector flash
}

void Write_Byte(uint8_t* buffer, uint32_t length)
{
	uint32_t time1;
	uint32_t time2;

	uint16_t lentosend;
	int num_of_words;

    uint16_t remaining = length;
    uint8_t* current_data_ptr = buffer;

    while (remaining > 0)
    {
    	lentosend = (remaining >= BLOCK_SIZE) ? BLOCK_SIZE : remaining;
//            	totaltam += lentosend;
    	num_of_words = lentosend;

    	time1 = HAL_GetTick();
    	Flash_Write_byte(flash_sector_address, current_data_ptr, num_of_words);
    	time2 = HAL_GetTick();
    	time2 -= time1;
    	printf("\n Write addr = %lu, size = %d, time = %ld\n", flash_sector_address, num_of_words, time2);
    	flash_sector_address += lentosend;

    	remaining -= lentosend;
    	current_data_ptr += lentosend;
    }
}

//void ReadAndPrintFirst4096Bytes(uint32_t flash_address) {
//    uint32_t num_of_words = (4096 / 4) + ((4096 % 4) != 0);
//    uint32_t Rx_Data[num_of_words];
//    uint8_t u[4];
//    int byte_count = 0;
//
//    // Đọc 4096 byte đầu tiên từ flash
//    Flash_Read_Data(flash_address, Rx_Data, num_of_words);
//
//    // In ra dữ liệu đã đọc
//    printf("First 4096 bytes from flash:\n");
//    for (int i = 0; i < num_of_words; i++) {
//        byte_count += 4; // Mỗi lần in ra 4 byte
//
//        convertdata(Rx_Data[i], u);
//        printf("%02x %02x %02x %02x ", u[0], u[1], u[2], u[3]);
//
//        if (byte_count >= 20) {
//            printf("\n"); // Xuống hàng sau mỗi 20 byte
//            byte_count = 0; // Đặt lại biến đếm
//        }
//    }
//    printf("\n"); // Xuống hàng sau khi in hết dữ liệu
//}

void ReadAndPrintFirst4096Bytes(uint32_t flash_address) {
    uint32_t size = 512; // Kích thước dữ liệu cần đọc
    uint32_t Rx_Data[size];
    uint8_t u[4];
    int byte_count = 0;

    // Đọc 4096 byte đầu tiên từ flash
    Flash_Read_Data(flash_address, Rx_Data, size);

    // In ra dữ liệu đã đọc
    printf("First 512 bytes from flash:\n");
    for (int i = 0; i < 512; i++) {
        byte_count += 4; // Mỗi lần in ra 4 byte

        convertdata(Rx_Data[i], u);
        printf("%02x %02x %02x %02x ", u[0], u[1], u[2], u[3]);

        if (byte_count >= 20) {
            printf("\n"); // Xuống hàng sau mỗi 20 byte
            byte_count = 0; // Đặt lại biến đếm
        }
    }
    printf("\n"); // Xuống hàng sau khi in hết dữ liệu
}

void save_data_to_flash(uint8_t* data, uint32_t length)
{
	int byte_count = 0;
    uint32_t Rx_Data[BUFFER_SIZE];
    uint8_t u[4];
    printf("Bat dau luu data vao flash\r\n");

    // Erase flash sector
    printf("Dang xoa sector flash\r\n");
    Flash_Erase_sector(FLASH_SECTOR_ADDRESS5);

    uint32_t num_of_words = (length / 4) + ((length % 4) != 0);

    printf("Dang ghi data vao flash\r\n");
    uint32_t start_tick = HAL_GetTick();
    Flash_Write_Data(FLASH_SECTOR_ADDRESS5, (uint32_t *)data, num_of_words);
    uint32_t end_tick = HAL_GetTick();
    printf("Ghi hoan tat trong %lu ms\r\n", end_tick - start_tick);

    Flash_Read_Data(FLASH_SECTOR_ADDRESS5, Rx_Data, num_of_words);

    printf("Data doc tu flash:\r\n");
    for (int i = 0; i < num_of_words; i++) {

    	byte_count += 4; // Mỗi lần in ra 4 byte

        convertdata(Rx_Data[i], u);
        printf("%02x %02x %02x %02x ", u[0], u[1], u[2], u[3]);

        if (byte_count > 20) {
            printf("\n"); // Xuống hàng
            byte_count = 0; // �?ặt lại biến đếm
        }
    }
    printf("\n");
}

void test_write_byte()
{
	printf("Test read/write flash");
	uint16_t lentosend;
	uint8_t datatest[4096];
	uint8_t tam =0;
	 uint32_t tt;
	 uint32_t tt2;

	 uint32_t totaltam =0;
	int numofwords;
	uint32_t u32_addr_data_user =0U;

#if (0)

	  Flash_Write_Data(0x08020000 , (uint32_t *)data2, 9);


	  Flash_Read_Data(0x08020000 , Rx_Data, 10);

	  USBD_UsrLog("Rx_Data 1 = ");
		for(int i =0; i< 9; i++)
		{

			USBD_CongLog("%02x ",Rx_Data[i]);

		}
		USBD_CongLog("\n");
#endif



#if (0)
	  numofwords = (strlen(data)/4)+((strlen(data)%4)!=0);

	  tt = HAL_GetTick();

	  Flash_Write_Data(0x08021000 , (uint32_t *)data, numofwords);

		tt2 = HAL_GetTick();
		tt2 -= tt;
		USBD_CongLog(" Time = %d \n",tt2);


	  Flash_Read_Data(0x08021000 , Rx_Data, numofwords);
//	  Convert_To_Str(Rx_Data, string);
	  USBD_UsrLog("Rx_Data 2 = ");
		for(int i =0; i< numofwords; i++)
		{

			USBD_CongLog("%02x ",Rx_Data[i]);

		}
		USBD_CongLog("\n");
#endif

		/*Khởi tạo giá trị test*/
		tam = 0;
		for(int j =0; j < 4096; j++)
		{
			datatest[j] = tam;

				tam++;
				if (tam > 0xff) tam = 0;
		}

		/*xóa vùng nhớ*/
		Flash_Erase_sector(FLASH_SECTOR_ADDRESS5);

		u32_addr_data_user =FLASH_SECTOR_ADDRESS5;

#if (1)
		lentosend =0x40U;
		totaltam+=lentosend;

		numofwords = lentosend;
		tt = HAL_GetTick();
		Flash_Write_byte(u32_addr_data_user, (uint8_t *)datatest, numofwords);
		tt2 = HAL_GetTick();
		tt2 -= tt;
		printf("\n Write index =%d, addr = %x, size = %d, time = %d\n",0U, u32_addr_data_user , numofwords, tt2);
		u32_addr_data_user +=lentosend;

		//chiều dài là số lẻ 4 thì bị sai?????????????????
		lentosend =0x15U;
//		lentosend =0x18U;

		totaltam+=lentosend;

		numofwords = lentosend;
		tt = HAL_GetTick();
		Flash_Write_byte(u32_addr_data_user, (uint8_t *)datatest, numofwords);
		tt2 = HAL_GetTick();
		tt2 -= tt;
		printf("\n Write index =%d, addr = %x, size = %d, time = %d\n",1U, u32_addr_data_user , numofwords, tt2);
		u32_addr_data_user +=lentosend;
#endif

		printf("--------------End Test write file------------------------------\n");

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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(GPIOC, EN1_Pin, 1);
  HAL_GPIO_WritePin(GPIOC, EN2_Pin, 0);

  xulyUart.CountTX = 0;
  xulyUart.ui8_indexUart = 0;

  HAL_UART_Receive_IT(&huart1, xulyUart.ui8_RX_buf, 1);
  HAL_UARTEx_ReceiveToIdle_IT(&huart2, xulyUart.ui8_rx2Data, BUFFER_SIZE);
  HAL_UARTEx_ReceiveToIdle_IT(&huart3, xulyUart.ui8_rx3Data, BUFFER_SIZE);

//  uint32_t flash_write_start_time = 0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      if (xulyUart.ui32_timer < HAL_GetTick()) {
          xulyUart.ui32_timer = HAL_GetTick() + 1000;
          HAL_GPIO_TogglePin(GPIOD, LED_Pin);
      }

      if (uart2_data_ready && (HAL_GetTick() - uart2_start_time >= WAIT_TIME_MS)) {
          ProcessUart2Data(xulyUart.ui8_rx2Data, xulyUart.ui16_tx3Size);
          uart2_data_ready = 0;
          // Đặt thời gian bắt đầu ghi flash
//          flash_write_start_time = HAL_GetTick();
      }


//      // Thực hiện ghi flash nếu có dữ liệu trong bộ đệm tạm và đã qua khoảng 70ms nhưng không quá 30ms cho việc ghi
//      if (flash_write_start_time > 0 && HAL_GetTick() - flash_write_start_time >= (WAIT_TIME_MS - 50)) {  // 70ms - 50ms = 20ms
//          if (temp_buffer_index > 0) {
//              int num_of_words = (temp_buffer_index / 4) + ((temp_buffer_index % 4) != 0);
//
//              uint32_t time1 = HAL_GetTick();
//              Flash_Write_Data(flash_sector_address, (uint32_t *)temp_buffer, num_of_words);
//              uint32_t time2 = HAL_GetTick();
//
//              printf("\nWrite addr = %lu, size = %d, time = %lu\n", flash_sector_address - temp_buffer_index, num_of_words, time2 - time1);
//
//              flash_sector_address += temp_buffer_index;  // Cập nhật địa chỉ sector flash
//              temp_buffer_index = 0;  // Reset chỉ số buffer tạm
//              flash_write_start_time = 0;  // Reset thời gian bắt đầu ghi flash
//          }
//      }

      // Kiểm tra nếu đã quá th�?i gian RESET_TIME_MS mà không nhận được gói dữ liệu nào
//      if (HAL_GetTick() - last_packet_time >= RESET_TIME_MS) {
//    	  packetCount = 0;  // Reset bộ đếm gói
//          printf("Packet count reset due to inactivity.\n");
//          last_packet_time = HAL_GetTick();  // Cập nhật lại th�?i gian nhận gói cuối cùng
//      }

      // Kiểm tra nếu đã quá thời gian RESET_TIME_MS mà không nhận được gói dữ liệu nào
      if (HAL_GetTick() - last_packet_time >= RESET_TIME_MS) {
//          printf("Packet count reset due to inactivity.\n");

          packetCount = 0;  // Reset bộ đếm gói
          resetFlag = 1; // Đặt lại cờ để nhận gói mới
          last_packet_time = HAL_GetTick();  // Cập nhật lại thời gian nhận gói cuối cùng
      }

	  if(xulyUart.uart3_receivedData == 1 && xulyUart.uart2_processingData == 0)
	  {
		  HAL_UART_Transmit_IT(&huart2, xulyUart.ui8_tx2Data, xulyUart.ui16_tx2Size);
//		  ProcessUart2Data(xulyUart.ui8_tx2Data, xulyUart.ui16_tx2Size);
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

#if 1
	  if(xulyUart.CountTX < xulyUart.ui8_indexUart)
	  {
		  if(xulyUart.ui8_busy == 0)
		  {
			  int vitridoctin = xulyUart.CountTX % 1000;
			  uint8_t inchar = xulyUart.ui8_str[vitridoctin];
			  xulyUart.CountTX++;
			  xulyUart.count++;
			  xulyUart.ui8_busy = 1;
//			  HAL_UART_Transmit_IT(&huart2, &inchar, 1);
		  }
	  }

	  if(xulyUart.ui8_newpagkage == 1)
	  {
//		  HAL_UART_Transmit(&huart2, (uint8_t *) xulyUart.ui8_str,xulyUart.ui8_indexUart, 100);
		  xulyUart.ui8_newpagkage = 0;
		  xuly_command_ESP();
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
		// Xu ly uart theo packet
		uint8_t inchar = xulyUart.ui8_RX_buf[0];
		int vitridoctin = xulyUart.ui8_indexUart % 1000;
		xulyUart.ui8_str[vitridoctin] = inchar;
		xulyUart.ui8_newinchar = inchar;
		xulyUart.ui8_indexUart++;
		xulyUart.ui8_newpagkage = 1;
		HAL_UART_Receive_IT(&huart1, xulyUart.ui8_RX_buf, 1);
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart->Instance == USART2)
	{
		xulyUart.ui16_countIT2++;
		xulyUart.ui16_data2Size += Size;

		xulyUart.ui16_tx3Size = Size;

		if(xulyUart.ui16_tx3Size < 1024)
		{
			xulyUart.ui16_countIT2++;
		}

		memcpy(xulyUart.ui8_tx3Data, xulyUart.ui8_rx2Data, xulyUart.ui16_tx3Size);

		uart2_data_ready = 1;
		xulyUart.uart2_receivedData = 1;

		xulyUart.ui32_timer = HAL_GetTick();
		HAL_UARTEx_ReceiveToIdle_IT(&huart2, xulyUart.ui8_rx2Data, BUFFER_SIZE);
//		ProcessUart2Data(xulyUart.ui8_tx3Data, xulyUart.ui16_tx3Size);
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
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		xulyUart.ui8_busy = 0;
	}
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
