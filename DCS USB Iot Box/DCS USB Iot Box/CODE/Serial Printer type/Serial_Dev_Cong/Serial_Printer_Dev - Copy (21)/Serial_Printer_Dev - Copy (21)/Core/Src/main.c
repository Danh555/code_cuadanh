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

/**
  ******************************************************************************
  * Ghi chú:
  * máy in Xprinter: khi in trực tiếp (PC-->USB2Serial-->Printer): in vẫn bị khựng
  * baud 38400: khựng nhi�?u, 115200: khựng ít
  *******************************************************************************
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
#include "comparePrintercode.h"
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
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
uint32_t ui32_package_index_total =0U;
uint16_t lentosend = 0U;
uint32_t u32_addr_data_user =0U;
uint32_t ui32_total_data_print= 0U;
uint32_t ui32_package_index_sending= 0U;
uint32_t ui32_data_offset= 0U;
uint8_t ui8_finished_printer = 0U;
uint32_t ui32_txprinterlength =0U;
uint8_t PRINTER_TX_Buffer[2100];
USER_cacbien_truyenquaESP_TypeDef quanlyESP;
uint8_t ui8_printer_status =0;//default not ready
uint8_t  status_led =0U;
uint32_t ui32_counter_led=0;
uint32_t timer_process_bill=0;

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
uint32_t get_addr_header();

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

//#define MAX_BUFFER_CMD_ESP 10000
#define WAIT_TIME_MS 20
#define RESET_TIME_MS 30000  // 30s
#define RX5_BUFF_SIZE   20

uint8_t ui8_status_esp =0U;
uint32_t ui32_timerxset =0;



USER_TypeDef user_manage_request;
USER_TypeDef *pcommand =&user_manage_request;

USER_TypeDef user_manage_request_printer;/* quản lý dữ liệu Uart từ printer gửi */
USER_TypeDef *pcommandprinter =&user_manage_request_printer;


USER_REQUESET_TypeDef *pmydataglobal;
/* -------------- end biến uart ---------------------*/

// Thêm biến toàn cục để lưu tổng số gói và tổng số byte đã nhận
uint32_t total_packets = 0;
uint32_t total_bytes = 0;

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

uint32_t ui32_counter_send_status =0 ;

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
				ui8_printer_status =0;
			}
			else if (data[j+1] == 0x00 && data[j+2] == 0x03 && data[j+3] == 0x0f)
			{
				printf("(Read Data): Turn off machine (PAPER OUT)\n");
			}
			break;
		case 0x5f:/* pc send: 1D 49 43, printer send model */
            if (data[j+1] == 0x54 && data[j+2] == 0x4d && data[j+3] == 0x2d && data[j+4] == 0x54 &&
                data[j+5] == 0x38 && data[j+6] == 0x38 && data[j+7] == 0x49 && data[j+8] == 0x49 &&
                data[j+9] == 0x49 && data[j+10] == 0x00)
            {
            	printf("(Read Data): _TM-T88III.\n");

            	ui8_printer_status =1;
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

void xuly_package_ESP()
{
//	USBH_UsrLog("rx rec = %s _END", quanlyESP.str);
/*
 #G n$
 	 G: get infor
 	 n=1: get Manufactor
 	 n=2: get model

 #R n$
 	 R: phản hồi
 	 n = 0: ACK
 	 n = 1: IDLe
 	 n = 2: Erro

 #F n$
 	 F: xóa flash
 	 n = 1 : xóa
 */
	if (quanlyESP.str[1] == 'G')
	{
		if (quanlyESP.str[3] == '1')
		{
			USBH_UsrLog("#CMD G1 Manufacture");
		}
		else if (quanlyESP.str[3] == '2')
		{
			USBH_UsrLog("#CMD G2 Product");
		}
		else if (quanlyESP.str[3] == '3')
		{
			USBH_UsrLog("#CMD G3 Serinumber");
		}
	}
	else if (quanlyESP.str[1] == 'R')
	{
		if (quanlyESP.str[3] == '0')
		{
			ui8_status_esp =0U;/*ready*/
		}
		else if (quanlyESP.str[3] == '1')
		{
			ui32_timerxset =0;// reset timeout
		}
		else if (quanlyESP.str[3] == '2')
		{
			ui32_timerxset =0;// reset timeout

			USBH_UsrLog(" resend package ");
			if (quanlyESP.index_pkg)
			{
				quanlyESP.index_pkg--;
			}
			if (u32_addr_data_user >= lentosend)
			{
				u32_addr_data_user -=lentosend;
			}
		}
	}
	else if (quanlyESP.str[1] == 'T')
	{
		if (quanlyESP.str[3] == '1')
		{
			user_manage_request.number_printed++;
			USBH_UsrLog(" test finish printed %d", user_manage_request.number_printed);
			u32_addr_data_user =0U;
		}
		else if (quanlyESP.str[3] == '2')
		{
			USBH_UsrLog(" test format flash ");
			USBH_UsrLog("Format disk");
				/*format disk*/
			Flash_Erase_sector(ADDR_FLASH_SECTOR_5);
			Flash_Erase_sector(ADDR_FLASH_SECTOR_6);
			Flash_Erase_sector(ADDR_FLASH_SECTOR_7);
			Flash_Erase_sector(ADDR_FLASH_SECTOR_8);
			Flash_Erase_sector(ADDR_FLASH_SECTOR_9);
			Flash_Erase_sector(ADDR_FLASH_SECTOR_10);
			Flash_Erase_sector(ADDR_FLASH_SECTOR_11);
			SERIAL_TO_SERVER("finished");
		}
		else if (quanlyESP.str[3] == '3')
		{
			USBH_UsrLog("test format");
			clear_flash();
		}
	}
	else if (quanlyESP.str[1] == 'S')
	{
		if (quanlyESP.str[3] == '1')
		{
			USBH_UsrLog("start sniff");

			/* thiết lập nhận data pringting */

			 int vitridoctin = pcommand->number_printed % MAX_USER_PRINTED_SUPPORT_AT_THE_SAME_TIME;

			 if(vitridoctin ==1)
			 {
				 pcommand->u32_addr_data_store = FLASH_USER_START_ADDR2;
				 pcommand->u32_addr_header_store =FLASH_USER_HEADDER_ADDR2;
			 }
			 else
			 {
				 pcommand->u32_addr_data_store = FLASH_USER_START_ADDR;
				 pcommand->u32_addr_header_store =FLASH_USER_HEADDER_ADDR;
			 }

			 pcommand->readytopausesniff =0U;
			 pcommand->data_print_size =0; //reset size
			 pcommand->chophepsniff =1U;
			 pcommand->timer_pausesniff = HAL_GetTick();

		}
		else if (quanlyESP.str[3] == '2')
		{
			USBH_UsrLog("end sniff");
			pcommand->readytopausesniff =0U;
			pcommand->chophepsniff =0U;
			pcommand->ui8_startprint =0U;
			pcommand->ui8_printing =0U;
			pcommand->WAIT_SEND_TO_SERVER =1U;
			pcommand->number_printed++;

			uint32_t diachitam = pcommand->u32_addr_header_store;
			//chỉ có 1 gói duy nhất
			pcommand->package_index =1;

			/* save total package */
			 Flash_Write_Data(diachitam, &pcommand->package_index, 1U);

			 USBD_UsrLogCog(" total package = %ld \n", pcommand->package_index);

			 diachitam+=4;
			 /* save len package */
			Flash_Write_Data(diachitam, &pcommand->data_print_size, 1U);

		}
	}

	quanlyESP.busy =0;
}

uint8_t sosanhlenhfct(USER_REQUESET_TypeDef *pmydata, int len, uint16_t *myRxDataLength)
{
    uint8_t sosanhlenh = 0;

    // Kiểm tra điều kiện bắt đầu hoặc kết thúc dựa trên trạng thái in
    if (pcommand->ui8_startprint == 0U)
    {
        // Kiểm tra điều kiện bắt đầu in
        if (check_start_condition(pmydata->noidung_data, len))
        {
            sosanhlenh = 1;
            USBD_UsrLogCog("\n start print 2\n");
        }
    }
    else
    {
        // Kiểm tra điều kiện kết thúc in
        if (check_end_condition(pmydata->noidung_data, len))
        {
            /*  Kết thúc in */
            sosanhlenh = 2;

            *myRxDataLength = len;
            pcommand->data_print_size += *myRxDataLength;
            int numofwords = (*myRxDataLength / 4) + ((*myRxDataLength % 4) != 0);
            Flash_Write_Data(pcommand->u32_addr_data_store, (uint32_t *)pmydata->noidung_data, numofwords);
            pcommand->u32_addr_data_store += *myRxDataLength;

            uint8_t lastindex = pcommand->package_index;
            pcommand->chieudaitungfile[lastindex] = pcommand->data_print_size;
            USBD_UsrLogCog(" save last = %d, len = %ld \n", lastindex, pcommand->chieudaitungfile[lastindex]);

            USBD_UsrLogCog("\n end print 6\n");
        }
    }

    return sosanhlenh;
}


#if (0)
uint8_t sosanhlenhfct(USER_REQUESET_TypeDef *pmydata, int len, uint16_t myRxDataLength)
{
	uint8_t sosanhlenh =0;
	if(pcommand->ui8_startprint ==0U)
	  {
		  /* Xprinter :
		  thông báo in bill:
		  1B 40 1D 55 42 02 18 1B 4A C0 1D 76 30 00 48 00 18 00		có cắt giấy
		  1B 40 1B 4A 30       1B 4A C0 1D 76 30 00 48 00 18 00		không cắt giấy
		  */
		if( pmydata->noidung_data[0] == 0x1b && pmydata->noidung_data[1] == 0x40)
		  {
			  sosanhlenh =1;
			  USBD_UsrLogCog("\n start print 2\n");
		  }


	  }
	  else
	  {
		  //tìm điểm kết thúc in
//		  ....1D 53 1B 72 1D 54
		  if (pmydata->noidung_data[len-4] == 0x1bU && pmydata->noidung_data[len -3] == 0x72U)
		  {

			/*  Kết thúc in */
			sosanhlenh =2;

			myRxDataLength = len;
			pcommand->data_print_size +=myRxDataLength;
			int numofwords = (myRxDataLength/4)+((myRxDataLength%4)!=0);
			Flash_Write_Data(pcommand->u32_addr_data_store, (uint32_t *)pmydata->noidung_data, numofwords);
			pcommand->u32_addr_data_store +=myRxDataLength;


			uint8_t lastindex = pcommand->package_index;
			pcommand->chieudaitungfile[lastindex] = pcommand->data_print_size;
			USBD_UsrLogCog(" save last = %d, len = %ld \n", lastindex, pcommand->chieudaitungfile[lastindex]);

			USBD_UsrLogCog("\n end print 6\n");
		  }
	  }
	return sosanhlenh;
}
#endif

void kiemtra_request_tu_PC()
{
      if(user_manage_request.request_dang_thuc_thi < user_manage_request.request_tong_nhan)
      {
    	  USER_REQUESET_TypeDef *pmydata;
    	  int vitridoctin = user_manage_request.request_dang_thuc_thi % MAX_USER_GET_USB_REQUEST;
    	  pmydata =&user_manage_request.request_noidung[vitridoctin];
    	  int len = pmydata->noidung_chieudai;
    	  uint16_t myRxDataLength;
    	  user_manage_request.request_dang_thuc_thi++;

    	  //lưu vào vùng flash
    	  if( pcommand->chophepsniff ==1U && len > 0U)
		  {
    		  int numofwords = (len/4)+((len%4)!=0);
    		  myRxDataLength = numofwords*4;

    		  // lưu chi�?u dài
    		  uint32_t j = (uint32_t)len;
    		  Flash_Write_Data(pcommand->u32_addr_data_store, &j, 1U);

    		  pcommand->u32_addr_data_store += 4;
    		  pcommand->data_print_size += 4;

    		//lưu data
			Flash_Write_Data(pcommand->u32_addr_data_store, (uint32_t *)pmydata->noidung_data, numofwords);

			//cập nhật chi�?u dài tổng
			pcommand->data_print_size +=myRxDataLength;
			pcommand->u32_addr_data_store +=myRxDataLength;
		  }


    	  /* --------------- start so sanh de xu ly -------------------------------------*/
    	  uint8_t sosanhlenh =sosanhlenhfct(pmydata,len,&myRxDataLength);

    	  if(sosanhlenh ==1)
    	  		{
    	  			 //so sánh bộ nhớ trống
    	  			 uint8_t tamnho = pcommand->number_printed - pcommand->number_printed_processing;
    	  			 if (tamnho >= MAX_USER_PRINTED_SUPPORT_AT_THE_SAME_TIME)
    	  			 {
    	  				 sosanhlenh =0;
    	  				 USBD_UsrLogCog("\n reach max printed support \n");
    	  			 }
    	  			 else
    	  			 {
    	  				//bắt đầu in
    	  				 if (pcommand->duocphepdebug ==DEBUG_OK)
    	  				 {
    	  					 USBD_UsrLog("#CMD S1");
    	  				 }

    	  				 /* reset các biến */
    	  				 pcommand->data_print_size = 0U;
    	  				 pcommand->ui8_startprint =1U;
    	  				 pcommand->dataprint_doing =0U;
    	  				 pcommand->dataprint_tonglenh =0U;
    	  				 pcommand->print_length_tam =0U;
    	  				 pcommand->package_index =0U;


    	  				 /* thiết lập nhận data pringting */

    	  				 int vitridoctin = pcommand->number_printed % MAX_USER_PRINTED_SUPPORT_AT_THE_SAME_TIME;

    	  				 if(vitridoctin ==1)
    	  				 {
    	  					 pcommand->u32_addr_data_store = FLASH_USER_START_ADDR2;
    	  					 pcommand->u32_addr_header_store =FLASH_USER_HEADDER_ADDR2;
    	  				 }
    	  				 else
    	  				 {
    	  					 pcommand->u32_addr_data_store = FLASH_USER_START_ADDR;
    	  					 pcommand->u32_addr_header_store =FLASH_USER_HEADDER_ADDR;
    	  				 }
    	  			 }
    	  		}
    	  		else if(sosanhlenh ==2)
    	  		{
    	  			USBD_UsrLogCog(" have end, stt = %x \n", pcommand->ui8_startprint);
    	  			//kết thúc in
    	  			if(pcommand->ui8_startprint == 1U)
    	  			{
    	  				pcommand->ui8_startprint =0U;
    	  				pcommand->ui8_printing =0U;
    	  				 pcommand->WAIT_SEND_TO_SERVER =1U;
    	  				 pcommand->number_printed++;


    	  				 pcommand->package_index++;

    	  				 /* tính tổng gói từ các gói thành phần */
    	  				 uint32_t pp = pcommand->package_index;
    	  				 uint32_t tamlen =0U;
    	  				 uint32_t diachitam = pcommand->u32_addr_header_store;

    	  				 /* save total package */
    	  				 Flash_Write_Data(diachitam, &pcommand->package_index, 1U);
    	  				 USBD_UsrLogCog(" total package = %ld \n", pcommand->package_index);

    	  				 diachitam+=4;
    	  				 /* save len package */
    	  				 if (pp ==1U)
    	  				 {
    	  					 USBD_UsrLogCog(" 1 package = %ld \n", pcommand->data_print_size);
    	  					 Flash_Write_Data(diachitam, &pcommand->data_print_size, 1U);
    	  				 }
    	  				 else
    	  				 {
    	  					 for (int k =0; k< pp;k++)
    	  					 {
    	  						 tamlen += pcommand->chieudaitungfile[k];/* get save file*/
    	  						 Flash_Write_Data(diachitam, &pcommand->chieudaitungfile[k], 1U);
    	  						 diachitam += 4;

    	  						 USBD_UsrLogCog(" index = %d, total = %ld \n", k, tamlen);
    	  					 }
    	  				 }


    	 				/* reset timer*/
    	 				/* sau 1 giây mới bắt đầu xử lý bill mới */
    	 				timer_process_bill = HAL_GetTick() + 1000;
    	  			}

    	  		}
    	  		/* --------------- end so sanh de xu ly -------------------------------------*/


    	  		/* --------------- start copy data vào vùng nhớ -------------------------------------*/

    	  		/* xác định data bắt đầu in */
    	  		if((len == 0x40) && (pcommand->ui8_startprint ==1U))
    	  		{
    	  			pcommand->ui8_printing =1U;
    	  		}

    	  		if (pcommand->ui8_printing ==1U)
    	  		{
    	  			if(len > 0U)
    	  			{
    	  				myRxDataLength =len;
    	  				pcommand->data_print_size +=myRxDataLength;

    	  				/* cập nhật chi�?u dài file */
    	  				uint8_t indenew = pcommand->package_index;
    	  				pcommand->chieudaitungfile[indenew] = pcommand->data_print_size;

    	  				/*store data to flash */
    	  				int u = len %4;
    	  				if (u)
    	  				{
    	  					u = 4 -u;
    	  					myRxDataLength += u;

    	  					/* lưu chi�?u dài file 1*/
    	  					uint8_t l = pcommand->package_index;
    	  					pcommand->chieudaitungfile[l] = pcommand->data_print_size;

    	  					USBD_UsrLogCog(" save index = %d, len = %ld \n", l, pcommand->chieudaitungfile[l]);

    	  					pcommand->data_print_size =0U;
    	  					pcommand->package_index++;
    	  				}

    	  				int numofwords = (myRxDataLength/4)+((myRxDataLength%4)!=0);
    	  				Flash_Write_Data(pcommand->u32_addr_data_store, (uint32_t *)pmydata->noidung_data, numofwords);

    	  //				Flash_Write_byte(pcommand->u32_addr_data_store, (uint32_t *)pmydata->noidung_data, myRxDataLength);

    	  				pcommand->u32_addr_data_store +=myRxDataLength;
    	  //				pcommand->data_print_size +=myRxDataLength;
    	  			}
    	  		}
    	  		/* --------------- end copy data vào vùng nhớ -------------------------------------*/



      }
}
/* transmit_to_esp_process
 * dùng ngắt tx truy�?n qua ESP
 *
 *
 */
void transmit_to_esp_process()
{
	switch (quanlyESP.indextx)
	{
	  case ESP_INIT:
	  {
		  ui8_status_esp =1U;
		  quanlyESP.indextx =ESP_WAIT_ESPIDLE;
		  break;
	  }
	  case ESP_WAIT_ESPIDLE:
	  {
		  /* ch�? esp rảnh */
		  check_status_esp();
		  if (ui8_status_esp ==0U)
		  {
			  quanlyESP.indextx =ESP_STATE_SETUP;
		  }
		  break;
	  }
	  case ESP_STATE_SETUP:/* idle */
	  {
		  HAL_StatusTypeDef tt = HAL_UART_Transmit_IT(&huart1,PRINTER_TX_Buffer,quanlyESP.lentx);
		  if (tt == HAL_OK)
		  {
			  quanlyESP.indextx =ESP_WAITSEND;
		  }
		  break;
	  }
	  case ESP_WAITSEND:
	  {
			if (quanlyESP.status_tx_datruyenxong == 1)
			  {
#if (0)
				uint32_t tam1 = HAL_GetTick() - biendebug_code.u32_meas;
				USBH_UsrLog("#tx finish = %d %d time = %ld ms",user_manage_request.request_tx_to_esp,user_manage_request.request_tong_nhan,tam1);
#endif
				quanlyESP.cmd_tx =0U;
				quanlyESP.status_tx_datruyenxong =0;
				quanlyESP.indextx =ESP_INIT;
			  }
		  break;
	  }

	  default:
		  quanlyESP.indextx =ESP_INIT;
		  break;
	}
}
int get_header()
{
	int kq =0;
	USBD_CongLog("--------------get_header------------------------------\n");

	 uint32_t totaltam =0;

	u32_addr_data_user =get_addr_header();

	/*đ�?c tổng số package */

	uint32_t pp =0;
	ui32_total_data_print =0;

	Flash_Read_Data(u32_addr_data_user, &pp, 1U);
	ui32_package_index_total = pp;

	u32_addr_data_user += 4; //địa chỉ gói đầu tiên
	USBD_CongLog("total package =%ld\n",pp);

	if(pp==1)
	{
		Flash_Read_Data(u32_addr_data_user, &totaltam, 1U);
		ui32_total_data_print = totaltam;
		USBD_UsrLogCog(" read 1 package = %ld \n", ui32_total_data_print);
	}
	else if (pp > 0 && pp < 10)
	{
		ui32_total_data_print =0U;
		uint32_t quai[6];
		Flash_Read_Data(u32_addr_data_user, quai, 5U);

		int uu = (int)pp;
		for (int kk = 0; kk < uu; kk++)
		 {
//			 Flash_Read_Data(u32_addr_data_user, &totaltam, 1U);

			totaltam=quai[kk];
			ui32_total_data_print += totaltam;

			 u32_addr_data_user += 4;

			 USBD_UsrLogCog(" index = %d, addr = %lx, len1 = %ld, total = %ld \n", kk, u32_addr_data_user, totaltam, ui32_total_data_print);
		 }
	}
	else
	{
		kq =1;
	}

	if (ui32_total_data_print > 3*128*1024)/* 3 sector *128k*/
	{
		ui32_total_data_print =0;
		USBD_UsrLogCog(" set totallen = 0 \n");
		kq=1;
	}

	USBD_CongLog("Read =%ld\n",ui32_total_data_print);

	USBD_CongLog("--------------End get_header------------------------------\n");
	return kq;
}
int get_nextdata()
{
	int kq =0;
	USBD_CongLog("--------------get_nextdata------------------------------\n");
	/* offset cho data tiếp theo */
	u32_addr_data_user += ui32_data_offset;
	USBD_UsrLogCog(" u32_addr_data_user = %lx \n", u32_addr_data_user);

	 uint32_t totaltam =0;
	 uint32_t diachitam =0;
	 uint32_t tam =0;
	/*lấy địa chỉ tạm*/

	 diachitam = ui32_package_index_sending*4;
	 tam = get_addr_header();
	 diachitam += tam;//offset
	 diachitam += 4;//b�? qua total package

	 Flash_Read_Data(diachitam, &totaltam, 1U);

	 USBD_UsrLogCog(" new package %ld, len1 = %ld \n", ui32_package_index_sending, totaltam);

	 /* d�?i offset data */
	 int u = totaltam %4;
	if (u)
	{
		ui32_data_offset = 4 -u;
		USBD_UsrLogCog(" next offset data = %ld \n", ui32_data_offset);
	}


	 ui32_txprinterlength = totaltam;

	if (ui32_txprinterlength > 3*128*1024)/* 3 sector *128k*/
	{
		ui32_txprinterlength =0;
		USBD_UsrLogCog(" set len = 0 \n");
		kq =1;
	}

	 ui32_package_index_sending++; //chỉ tới gói tiếp theo
	USBD_CongLog("--------------End get_nextdata------------------------------\n");
	return kq;
}
int check_status_esp()
{
	int kq =1;
	if (ui8_status_esp ==0U) return 0; /*idle*/

	uint32_t tam = HAL_GetTick();

	if(tam > ui32_timerxset)
	{
		/*
		 nếu timeout thì truy�?n lại gói check
		 đến khi nào ready mới xử lý truy�?n
		*/
		SERIAL_TO_SERVER("#CMD RD");
		ui32_timerxset = HAL_GetTick() + 1000;
		ui32_counter_send_status=HAL_GetTick() + 25000;

		//kq =0;
		//ui8_status_esp =0U;

		/*nếu timeout thì truy�?n lại gói khởi động
		 ch�? ESP sẵn sàng nhận dữ liệu
		 * */

		//if(u32_addr_data_user == FLASH_USER_START_ADDR)
		//{
			//u32_addr_data_user =0;
		//}
	}

	return kq;
}
uint32_t get_addr_header()
{
	uint32_t kq=0;
	int vitridoctin = user_manage_request.number_printed_processing % MAX_USER_PRINTED_SUPPORT_AT_THE_SAME_TIME;

	 if(vitridoctin ==1)
	 {
		 kq =FLASH_USER_HEADDER_ADDR2;
	 }
	 else
	 {
		 kq =FLASH_USER_HEADDER_ADDR;
	 }
	 return kq;
}
uint32_t get_addr_data()
{
	uint32_t kq=0;
	int vitridoctin = user_manage_request.number_printed_processing % MAX_USER_PRINTED_SUPPORT_AT_THE_SAME_TIME;

	 if(vitridoctin ==1)
	 {
		 kq =FLASH_USER_START_ADDR2;
	 }
	 else
	 {
		 kq =FLASH_USER_START_ADDR;
	 }
	 return kq;
}
void clear_flash()
{
#if (DEBUG_NO_ERASER_FLASH ==10U)
	USBD_UsrLog(" clear_flash: DEBUG_NO_ERASER_FLASH");

#else
	uint32_t diachicanxoa = get_addr_header();
	uint16_t numberofwords = (ui32_total_data_print/4)+((ui32_total_data_print%4)!=0);
	Flash_Erase_sector_leng(diachicanxoa, numberofwords);
	SERIAL_TO_SERVER("clear @%lx, len = %ld", diachicanxoa, ui32_total_data_print);
#endif
}
void set_erro()
{
	SERIAL_TO_SERVER(" set_erro ");
	/*xóa flash sau khi truy�?n qua esp*/
	clear_flash();
	ui8_finished_printer =0U;
	u32_addr_data_user = 0U;
	user_manage_request.number_printed_processing++;
}

void check_data_print()
{
	if (ui8_finished_printer)
	{
		//kiểm tra status ESP
		uint32_t Rx_Dataread[1024];
		 int numofwords;
		 uint8_t u[5];

		 //load giá trị ban đầu
		if (u32_addr_data_user == 0U)
		{
			int eroo = get_header();
			if(eroo)
			{
				set_erro();
				return;
			}
			else
			{
				ui32_package_index_sending =0U;
				ui32_data_offset =0;
				eroo = get_nextdata();

				if (eroo)
				{
					set_erro();
					return;
				}
				else
				{
					u32_addr_data_user = get_addr_data();
					quanlyESP.index_pkg =0U;
					//bắt đầu in
		//			SERIAL_TO_SERVER("#CMD 11%d",ui32_total_data_print);
					quanlyESP.lentx = sprintf ((char *)PRINTER_TX_Buffer, "#CMD 11%ld\n", ui32_total_data_print);
					quanlyESP.cmd_tx =1U;
					return;
				}
			}
		}

		if (ui32_txprinterlength > 0U)
		{
				/* truy�?n data*/
				if (ui32_txprinterlength > BUFFER_TX_SERVER_SIZE)
				{
					ui32_txprinterlength-=BUFFER_TX_SERVER_SIZE;
					lentosend = BUFFER_TX_SERVER_SIZE;
				}
				else
				{
					lentosend = ui32_txprinterlength;
					ui32_txprinterlength =0U;
				}

				numofwords = (lentosend/4)+((lentosend%4)!=0);
				Flash_Read_Data(u32_addr_data_user, Rx_Dataread, numofwords);

				int so_byte_da_truyen = 4*(numofwords -1);
				int so_byte_conphaitruyen = lentosend - so_byte_da_truyen;

				int tamtru = sprintf ((char *)PRINTER_TX_Buffer, "#CMD P%dL%04d", quanlyESP.index_pkg, (uint16_t)lentosend);

				PRINTER_TX_Buffer[tamtru]='\n';
				tamtru++;

				/*truy�?n khúc đầu*/
				for(int i =0; i< numofwords-1 ; i++)
				{
					convertdata(Rx_Dataread[i] , u);
//					HAL_UART_Transmit(&huart1,u,4,0xFFFF);
					PRINTER_TX_Buffer[tamtru]=u[0];
					PRINTER_TX_Buffer[tamtru+1]=u[1];
					PRINTER_TX_Buffer[tamtru+2]=u[2];
					PRINTER_TX_Buffer[tamtru+3]=u[3];
					tamtru += 4;
				}

				/*truy�?n khúc cuối*/

				convertdata(Rx_Dataread[numofwords-1] , u);
//				HAL_UART_Transmit(&huart1,u,so_byte_conphaitruyen,0xFFFF);
				for(int m =0;m<so_byte_conphaitruyen;m++)
				{
					PRINTER_TX_Buffer[tamtru]=u[m];
					tamtru++;
				}
//				SERIAL_TO_SERVER("EOP");
//				SERIAL_TO_SERVER("endpackage");

				PRINTER_TX_Buffer[tamtru]='E';
				tamtru++;
				PRINTER_TX_Buffer[tamtru]='O';
				tamtru++;
				PRINTER_TX_Buffer[tamtru]='P';
				tamtru++;
				PRINTER_TX_Buffer[tamtru]='\n';
				tamtru++;
				PRINTER_TX_Buffer[tamtru]='e';
				tamtru++;
				PRINTER_TX_Buffer[tamtru]='o';
				tamtru++;
				PRINTER_TX_Buffer[tamtru]='p';
				tamtru++;
				PRINTER_TX_Buffer[tamtru]='\n';
				tamtru++;

				quanlyESP.index_pkg ++;
				u32_addr_data_user +=lentosend;
				quanlyESP.lentx =tamtru;
				quanlyESP.cmd_tx =1U;
		}
		else
		{
			/*kiểm tra xem còn gói package nào không*/
			if (ui32_package_index_sending < ui32_package_index_total)
			{
				/*get next package*/
				get_nextdata();
			}
			else
			{
				/* kết thúc truy�?n*/
//				SERIAL_TO_SERVER("#CMD 10");

				quanlyESP.lentx = sprintf ((char *)PRINTER_TX_Buffer, "#CMD 10\n");
				quanlyESP.cmd_tx =1U;

				/*xóa flash sau khi truy�?n qua esp*/
//				USBD_UsrLog(" !!!!!!!!!!! ------------debug 1957: uncomment this line");

				clear_flash();

				wait_res_init(1000);

				ui8_finished_printer =0U;
				u32_addr_data_user = 0U;
				user_manage_request.number_printed_processing++;
				user_manage_request.duocphepdebug =DEBUG_OK;
			}
		}
	}
	else
	{
		ui8_status_esp =1U;
		quanlyESP.cmd_tx =0U;
		//kiểm tra xem có data in hay không
		if (user_manage_request.number_printed_processing < user_manage_request.number_printed)
		{
			if(HAL_GetTick() > timer_process_bill )
			{
				ui8_finished_printer =1;
				user_manage_request.duocphepdebug =DEBUG_BUSY;
			}
		}
	}
}
/*
 * xuly_data_print_itx()
 *
 *
 */
void xuly_data_print_itx()
{
	if (quanlyESP.cmd_tx == 1U)
	{
		transmit_to_esp_process();
	}
	else
	{
		check_data_print();
	}
}

/**
  * @brief  chuyentiepPCtoPrinter
  *         Gửi dữ liệu nhận được từ Printer, chuyển cho PC
  * @param
  * @Note:
  */

void chuyentiepPrintertoPC()
{
	//nhan data tu uart 3 gui cho uart2
	if(xulyUart.uart3_receivedData == 1 && xulyUart.uart2_processingData == 0)
	  {
			HAL_UART_Transmit_IT(&huart2, xulyUart.ui8_tx2Data, xulyUart.ui16_tx2Size);
			xulyUart.uart2_processingData = 1;
			xulyUart.uart3_receivedData = 0;

			printf("to PC Size = %d : ", xulyUart.ui16_tx2Size);
			for (int i = 0; i < xulyUart.ui16_tx2Size; i++) {
					printf("%02x ", xulyUart.ui8_tx2Data[i]);

					if (i>0 && i % 20 == 0) {
						printf("\n");
					}
			  }
			printf("\n");

			PrintUart3Data(xulyUart.ui8_rx3Data, xulyUart.ui16_tx2Size);
	  }
}

/**
  * @brief  chuyentiepPCtoPrinter
  *         Gửi dữ liệu nhận được từ PC, chuyển cho Printer
  * @param
  * @retval none
  */
void chuyentiepPCtoPrinter()
{
	if (xulyUart.uart3_processingData == 1)return;	/* đang gửi tin */

		if(user_manage_request.request_dang_guitin < user_manage_request.request_tong_nhan)
		{
			USER_REQUESET_TypeDef *pmydata;
			  int vitridoctin = user_manage_request.request_dang_guitin % MAX_USER_GET_USB_REQUEST;
			  pmydata =&user_manage_request.request_noidung[vitridoctin];
			  int len = pmydata->noidung_chieudai;
			  user_manage_request.request_dang_guitin++;

			  /* chuyển tiếp tin */
//			  printf("to prt : %d \n", len);
			  HAL_UART_Transmit_IT(&huart3, pmydata->noidung_data, len);
			  xulyUart.uart3_processingData = 1;
		}

	//nhan data tu uart 2 gui cho uart3
#if (xuly1buffer)
	  if(xulyUart.uart2_receivedData == 1 && xulyUart.uart3_processingData == 0)
	  {
		  HAL_UART_Transmit_IT(&huart3, xulyUart.ui8_tx3Data, xulyUart.ui16_tx3Size);
		  xulyUart.uart3_processingData = 1;
		  xulyUart.uart2_receivedData = 0;
	  }
#endif
}

void inmainstatus()
{
	if ( HAL_GetTick() > ui32_counter_led )
	{

		if (status_led == 1U) {
			status_led =0;
			HAL_GPIO_WritePin(GPIOD,LED_Pin,1);
			if(ui8_finished_printer == 1U)
			{
				ui32_counter_led=HAL_GetTick() + 100;
			}
			else
			{
				if(ui8_printer_status ==1U)
				{
					ui32_counter_led=HAL_GetTick() + 50;
				}
				else
				{
					ui32_counter_led=HAL_GetTick() + 500;
				}
			}
		}
		else
		{
			status_led =1;
			HAL_GPIO_WritePin(GPIOD,LED_Pin,0);
			if(ui8_finished_printer ==1U )
			{
				ui32_counter_led=HAL_GetTick() + 100;
			}
			else
			{
				if(ui8_printer_status ==1U)
				{
					ui32_counter_led=HAL_GetTick() + 950;
				}
				else
				{
					ui32_counter_led=HAL_GetTick() + 500;
				}
			}
		}
	}
}

void send_satatus()
{
	if(ui8_finished_printer ==1U )
	{
		ui32_counter_send_status=HAL_GetTick() + 25000;
		return;
	}

	if ( HAL_GetTick() > ui32_counter_send_status )
	{
		ui32_counter_send_status=HAL_GetTick() + 25000;
		SERIAL_TO_SERVER("CMD I%d %d.%d.%d", ui8_printer_status,HWVERSION,PRINTER_MODEL,RELEASEVS);
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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(GPIOC, EN1_Pin, 1);
  HAL_GPIO_WritePin(GPIOC, EN2_Pin, 0);

  xulyUart.CountTX = 0;
  xulyUart.ui8_indexUart = 0;

  HAL_UART_Receive_IT(&huart1, xulyUart.ui8_RX_buf, 1);

  /* thiết lập buffer nhận data từ PC */
  int i = pcommand->request_tong_nhan % MAX_USER_GET_USB_REQUEST;
  pmydataglobal =&pcommand->request_noidung[i];
  HAL_UARTEx_ReceiveToIdle_IT(&huart2, pmydataglobal->noidung_data, CHUNK_SIZE);

  HAL_UARTEx_ReceiveToIdle_IT(&huart3, xulyUart.ui8_rx3Data, CHUNK_SIZE);

//  uint32_t flash_write_start_time = 0;

  initialize_command_pair(0);
  USBD_UsrLogCog("\n init values: \n");
  USBD_UsrLogCog(" selected_start_cmd: %d \n",0);

  printf("selected_start_cmd = %d : ", selected_start_len);
  			for (int i = 0; i < selected_start_len; i++) {
  					printf("%02x ", selected_start_cmd[i]);

  					if (i>0 && i % 20 == 0) {
  						printf("\n");
  					}
  			  }
  			printf("\n");

  		  printf("selected_end_sig = %d : ", selected_end_len);
  		  			for (int i = 0; i < selected_end_len; i++) {
  		  					printf("%02x ", selected_end_sig[i]);

  		  					if (i>0 && i % 20 == 0) {
  		  						printf("\n");
  		  					}
  		  			  }
  		  			printf("\n");



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  inmainstatus();
	  send_satatus();
      kiemtra_request_tu_PC();
      xuly_data_print_itx();

      chuyentiepPCtoPrinter();
      chuyentiepPrintertoPC();

	  if(quanlyESP.newcmd == 1)
	  {
		  quanlyESP.newcmd = 0;
		  xuly_package_ESP();
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
  huart2.Init.BaudRate = 115200;
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
  huart3.Init.BaudRate = 115200;
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

	//	#data$
#if (1)
	 if(huart->Instance == USART1)
	 {

	/*xử lý nhận uart theo gói*/
		 	 uint8_t inchar = quanlyESP.RX_buf[0];

		 	if (quanlyESP.busy == 0)
		 	{
				 if(inchar =='#')
				{
					quanlyESP.RX=1;
					quanlyESP.index_uarrt =0;
				}

				if(quanlyESP.RX ==1)
				{
						quanlyESP.str[quanlyESP.index_uarrt]=inchar;
						quanlyESP.index_uarrt++;

						if (quanlyESP.index_uarrt > MAX_BUFFER_CMD_ESP -1 )
						{
							/*tràn buffer*/
							quanlyESP.RX=0;
							quanlyESP.index_uarrt =0;
						}
						if (inchar =='$')
						{
							quanlyESP.newcmd =1;
							quanlyESP.RX=0;
							quanlyESP.busy =1;
						}
				}
				else
				{
					quanlyESP.newpagkage =1;
					quanlyESP.newinchar=inchar;
				}
		 	}

			HAL_UART_Receive_IT(&huart1, quanlyESP.RX_buf,1);
	 }
#endif

}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	static int vitriluutin;

	if (huart->Instance == USART2)
	{

		/* in debug giá trị nhận từ PC*/
#if (0)
		if(Size <64)
		{
			printf("From PC Size = %d : ", Size);
			for (int i = 0; i < Size; i++) {
			        printf("%02x ", pmydataglobal->noidung_data[i]);

			        if (i>0 && i % 20 == 0) {
			            printf("\n");
			        }
			  }
			printf("\n");
		}
#endif

		/* thông báo có data mới */
		pmydataglobal->noidung_chieudai = Size;
		pcommand->request_tong_nhan++;

		/* chuẩn bị buffer để lưu data mới */
		vitriluutin = pcommand->request_tong_nhan % MAX_USER_GET_USB_REQUEST;
		pmydataglobal =&pcommand->request_noidung[vitriluutin];
		HAL_UARTEx_ReceiveToIdle_IT(&huart2, pmydataglobal->noidung_data, CHUNK_SIZE);

	}
	if (huart->Instance == USART3)
	{
		/* thông báo có data mới */
		xulyUart.ui16_tx2Size = Size;
		memcpy(xulyUart.ui8_tx2Data, xulyUart.ui8_rx3Data, xulyUart.ui16_tx2Size);
		xulyUart.uart3_receivedData = 1;

		/* chuẩn bị buffer để lưu data mới */
		HAL_UARTEx_ReceiveToIdle_IT(&huart3, xulyUart.ui8_rx3Data, CHUNK_SIZE);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		quanlyESP.status_tx_datruyenxong = 1;
	}
	if (huart->Instance == USART2)
	{
		xulyUart.uart2_processingData = 0;
	}
	if (huart->Instance == USART3)
	{
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
