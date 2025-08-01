/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
/* định dạng phần cứng */
#define HWVS_TYPE0		0	/*	hardware đầu tiên: đang demo bên Sing */
#define HWVS_TYPE1		1	/*	phiên bản có cổng USB, serial...không có ROM ngoài, chip STM32F205VGT 1M */
#define HWVS_TYPE2		2	/*	phiên bản có cổng USB, serial...không có ROM ngoài, chip STM32F205VET 256k */
#define HWVS_TYPE3		3   /*	phiên bản có cổng USB, serial...có ROM ngoài, chip STM32F205VGT 1M */

/* định dạng máy in */
#define TM_T88IV		1U
#define TM_T82II		2U
//#define TM_T82X		3U
#define TM_T81			3U	/* phiên bản chạy demo bên Sing chạy được cho các dòng Epson khác */
#define TM_UNKNOW		4U
#define TM_T88III		5U	/* serial */


/* config phần cứng */
//VERSION_NAME = HWVERSION.PRINTER_MODEL.RELEASEVS

#define HWVERSION			HWVS_TYPE3
#define PRINTER_MODEL		TM_T88III
#define RELEASEVS			4


#define chip256k 1
#define chip1M	2

#if (HWVERSION == HWVS_TYPE1 || HWVERSION == HWVS_TYPE3)
	#define usingchip chip1M
#else
	#define usingchip chip256k
#endif

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define EN1_Pin GPIO_PIN_4
#define EN1_GPIO_Port GPIOC
#define EN2_Pin GPIO_PIN_5
#define EN2_GPIO_Port GPIOC
#define LED_Pin GPIO_PIN_14
#define LED_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */
#define BUFFER_TX_SERVER_SIZE 2048 /* mỗi lần truy�?n gói qua server*/
#define MAX_BUFFER_TX_SIZE 4096 /* chứa data truy�?n qua ESP */

#define MAX_USER_PRINTED_SUPPORT_AT_THE_SAME_TIME 2	/*  */

#define FLASH_USER_HEADDER_ADDR ((uint32_t)0x08020000)
#define FLASH_USER_START_ADDR ((uint32_t)0x08020028)

/* vùng 2 */
#define FLASH_USER_HEADDER_ADDR2 ((uint32_t)0x080A0000)
#define FLASH_USER_START_ADDR2 ((uint32_t)0x080A0028)

typedef enum
{
  USER_TYPE_CONTROL   = 0U,
  USER_TYPE_DATA,
  USER_TYPE_PRINTED,
} USER_RequestTypeDef;

typedef enum
{
	DEBUG_BUSY =0U,
	DEBUG_OK,
} USER_DebugTypeDef;

typedef enum {
  ESP_STATE_SETUP = 0,
  ESP_SEND,
  ESP_WAITSEND,
  ESP_INIT,
  ESP_WAIT_ESPIDLE,
  ESP_NEXT_PKG,
  ESP_RECEIVE,
}ESP_StateTypedef;


typedef struct
{
	  uint8_t			ui8_need_reinint;/* nếu có kết nối từ real device-->khởi tạo kết nối Virtual_Device */
	  uint8_t			ui8_user_forward_state;
	  uint8_t 			ui8_chophep_docdata_tu_realdevice;	/* class FF: lệnh đi�?u khiển clear feature*/
	  uint16_t 			ui16_interval;	/* dùng để kiểm tra interupt usb type */
//	  uint8_t 			ui8_thongbao_ketnoiapp;				/* thông báo có data nhận được từ Host PC tới Virtual_Device */
	  uint8_t 			ui8_need_reinint_Virtual_Device; /* nếu có kết nối từ real device-->khởi tạo kết nối Virtual_Device */
	  uint16_t 			ui16_counterbusy; /*  */
	  uint8_t 			datafromdevice[50]; /*  */
	  uint8_t 			ui8_status_esp;
	  uint8_t 			ui8_finished_printer;
	  uint32_t 			u32_addr_data_user;
	  uint32_t 			ui32_txprinterlength;
	  uint32_t 			ui32_timerxset;
	  uint32_t 			ui32_counter_send_status;
	  uint32_t 			lentosend;
	  uint8_t 			ui8_package_tx;
	  uint8_t 			ui8_printer_status;/* =1: đã kết nối máy in thành công, =0 erro*/
	  uint8_t 			ui8_counterflashled;/* */
	  uint8_t 			ui8_statusled;/* */
	  uint32_t 			ui32_timer_reset_device;/* th�?i gian cần reset lại thiết bị, nếu có máy in gắn vào, mà không thể khởi tạo kết nối với máy in được*/
	  uint8_t 			ui8_need_reset_device; /* =1: cần reset lại thiết bị, nếu có máy in gắn vào, mà không thể khởi tạo kết nối với máy in được*/

}USER_cacbien_chung_TypeDef;	/* my_var_user */

typedef struct
{
	  uint8_t 			txtorealdevice; 	/* =1  đã truy�?n và có phản hồi, =0 đang truy�?n , =2: có lỗi truy�?n */
	  uint16_t 			timeouttxtodevice;
	  uint32_t 			time_wait_next_print_pck; 	/* =1  đã truy�?n và có phản hồi, =0 đang truy�?n */
	  uint32_t 			timesend;
	  uint8_t			batdauin; /* =1: bắt đầu thực hiện chuyển gói in qua printer*/
	  uint8_t			printer_period; /* giai đoạn chuyển tiếp data printed qua máy in*/
	  uint8_t 			codataturealdevice; /* =1U: có data từ real device */
	  uint16_t 			chieudaidatarealdevice; /*  */
	  uint16_t 			counter_sent_data_to_pc; /*  */
	  uint8_t 			index_truyenlenpc; /*  */
	  uint32_t 			timer_syncdata; /* class FF: lệnh đi�?u khiển clear feature*/
	  uint8_t			check_data_index;

}USER_cacbien_chung2_TypeDef;

typedef struct
{
		uint16_t 		counter;
		uint32_t 		u32_timeout;
		uint32_t 		u32_meas;
		uint8_t 		bien1;

}USER_cacbien_dungdebug_code_TypeDef;

//USER_cacbien_dungdebug_code_TypeDef biendebug_code;

#define MAX_BUFFER_CMD_ESP 20
typedef struct
{
	/*nhận data*/
	uint8_t 			RX_buf[2];	/* buffer tạm */
	uint8_t 			str[MAX_BUFFER_CMD_ESP];	/* chứa chuỗi nhận được */
	uint8_t 			busy;	/*=1: đang xử lý gói data */
	uint8_t 			newinchar;	/*data mới nhất*/
	uint8_t 			RX;
	uint8_t 			index_uarrt;
	uint8_t 			newpagkage;
	uint8_t 			newcmd;	/*=1 nhận theo gói từ uart*/

	/*truy�?n data*/
	uint8_t 			indextx;
	uint8_t 			status_tx_datruyenxong;/* =1: đã truy�?n xong */
	uint8_t 			status_tx_dangtruyen;
	uint16_t 			ui16_vitridoctin;
	uint16_t 			ui16_vitriluutin;
	uint16_t 			cmd_tx; /* =1: có data để truy�?n */
	uint16_t 			lentx;
	uint16_t 			index_pkg;
	uint8_t 			datatosend[MAX_BUFFER_TX_SIZE];

}USER_cacbien_truyenquaESP_TypeDef;

/* -----------------biến uart ---------------------*/

#define MAX_USER_GET_USB_REQUEST 10	/* bộ đệm chứa request từ PC */
#define CHUNK_SIZE 64	// phải nh�? hơn chi�?u dài của noidung_data

typedef struct _USER_REQUESET_TypeDef
{
	uint8_t		noidung_loai_request;	/* CONTROL, DATA */
	uint8_t		noidung_chieudaicontrol;/* chứa chi�?u dài data control*/
	uint8_t		noidung_data[CHUNK_SIZE +1];	/* chứa data nhận được 65*/
	uint16_t 	noidung_chieudai;	/* chi�?u dài của data nhận được */
}
USER_REQUESET_TypeDef;

typedef struct
{
  uint16_t                 dataprint_tonglenh;/* số lượng gói data in trong buffer*/
  uint16_t                 dataprint_doing;/* đang in tới gói data này*/
  uint16_t                 print_length_tam;/* chi�?u dài gói data đang nhận vào*/
  uint16_t                 request_tong_nhan;
  uint16_t                 request_dang_thuc_thi;
  uint16_t                 request_dang_guitin;
  uint16_t                 request_tx_to_esp;	/* đang truy�?n qua esp */
  uint16_t                 need_getreadyrx;/* =1: cần g�?i hàm sẵn sàng nhận data*/
  uint32_t                 data_print_size;/* tổng kích thước file in*/
  uint32_t                 u32_addr_data_store;/* */
  uint32_t                 u32_addr_header_store;/* */
  uint8_t                  ui8_startprint;
  uint8_t                  ui8_printing;/* thông báo đang nhận data in*/
  uint8_t                  WAIT_SEND_TO_SERVER;/* =1U: ch�? truy�?n qua ESP xong mới kích hoạt lại nhận data */
  uint8_t                  chophepluu;
  uint8_t                  ui8_countzlp;
  uint8_t                  duocphepdebug;/* =1U: được phép debug serial, =0U: busy: đang truy�?n data qua server chip*/

  /* quản lý lưu data vào flash */
  uint8_t                  number_printed; /* số file trong hàng đợi */
  uint8_t                  number_printed_processing; /* file đang xử lý */
  uint32_t                 package_index; /* chứa từng đoạn gói dữ liệu in */
  uint32_t				   chieudaitungfile[10]; /* chi�?u dài tương ứng đoạn dữ liệu in */

  /* quản lý sniffer log*/
  uint8_t                  chophepsniff;
  uint8_t                  readytopausesniff;
  uint32_t					timer_pausesniff;


  USER_REQUESET_TypeDef	  request_noidung[MAX_USER_GET_USB_REQUEST];
}USER_TypeDef;	/*	user_manage_request	pcommand */

#define BUFFER_SIZE 1024

typedef struct{
	uint8_t ui8_RX_buf[2];
	uint8_t ui8_str[MAX_BUFFER_CMD_ESP];
	uint8_t ui8_newinchar;
	uint8_t ui8_RX;
	uint8_t ui8_indexUart;
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

#define DF_SERIAL_TO_SERVER     10U

#if (DF_SERIAL_TO_SERVER > 0)
#define SERIAL_TO_SERVER(...)    printf(__VA_ARGS__);\
								 printf("\n");
#else
#define SERIAL_TO_SERVER(...)
#endif

#define USBD_DEBUG_LEVEL     10U

#if (USBD_DEBUG_LEVEL > 0)
#define USBD_CongLog(...)    printf("#PRT: ") ;\
							 printf(__VA_ARGS__);

#else
#define USBD_CongLog(...)
#endif

#if (USBD_DEBUG_LEVEL > 0)
#define USBD_UsrLogCog(...)    printf(__VA_ARGS__);
#else
#define USBD_UsrLogCog(...)
#endif


#define USBH_DEBUG_LEVEL	10U

#if (USBH_DEBUG_LEVEL > 0U)
#define  USBH_UsrLog(...)   do { \
                            printf(__VA_ARGS__); \
                            printf("\n"); \
} while (0)
#else
#define USBH_UsrLog(...) do {} while (0)
#endif

#if (USBD_DEBUG_LEVEL > 0)
#define USBD_UsrLog(...)    printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_UsrLog(...)
#endif

void clear_flash();
void test_write_byte();
int check_status_esp();

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
