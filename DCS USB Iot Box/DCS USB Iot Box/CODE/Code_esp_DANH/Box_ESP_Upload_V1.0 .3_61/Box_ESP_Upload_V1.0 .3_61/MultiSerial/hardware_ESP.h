/**
   @file 	user_frame.h
   @version     1.01
   @date        2021.11.27
   @author 	Nguyen Thanh Cong
   @contact     thanhcong402@gmail.com
   @contribution

   @description


   @license

*/


#ifndef _hardware_ESP_
#define _hardware_ESP_


//--------------khai báo serial-----------------------------------------------

/*dùng giao tiếp với hub printer; STM32F205*/
#define SerialToPrinter Serial2 
#define SerialToPrinter_tx 18
#define SerialToPrinter_rx 17

/*dùng giao tiếp với barcode/4g*/
#define SerialToBarcodeModem Serial1 
#define SerialToBarcodeModem_tx 5
#define SerialToBarcodeModem_rx 4


#define SerialDebug Serial

// #define use_debug_serial
#define SerialDEBUG Serial

/* debug*/

// define led bao
#define led_sign 47
#define LED led_sign

//--------------khai báo BOOT PIN CONTROL-----------------------------------------------

#define BOOT_PRINTER 16
#define RESET_PRINTER 15

#define BOOT_BARCODE 40
#define RESET_BARCODE 39

#define LED_DEBUG 47  
#define PWRKEY_MODEM 1
#define SIMSTATUS_MODEM 38

// #define sim_inserted digitalRead(SIMSTATUS_MODEM)

#define CS_SD 14
#define CS_EXTROM 9
#define CS_ETHERNET 10
#define RS_ETHERNET 7
#define CS_UART 21 //=1 : BARCODE, =0: 4G

//--------------khai báo chân input-----------------------------------------------
#define INPUT_ETHERNET 8
#define INPUT_BUTTON 0

enum uart_mode_select
{
	UART_4G = 0,
	UART_BARCODE
};

//--------------khai báo RTC-----------------------------------------------
// I2C


// Choose the analog pin to get semi-random data from for SSL
// Pick a pin that's not connected or attached to a randomish voltage source
// const int rand_pin = 33;
//đã chỉnh lại thư viện, dùng hàm random-->nên chân này đưa vào cho đủ chức năng, không có sử dụng
// C:\Users\pc\Documents\Arduino\libraries\SSLClient-master\src\SSLClient.cpp: line 343
const int rand_pin = 33;

void hardware_init()
{

   pinMode(led_sign,OUTPUT);
   pinMode(INPUT_BUTTON,INPUT);

   pinMode(BOOT_PRINTER,OUTPUT);
   digitalWrite( BOOT_PRINTER, 0);  
   pinMode(RESET_PRINTER,OUTPUT);
   digitalWrite( RESET_PRINTER, 1);  

   pinMode(BOOT_BARCODE,OUTPUT);
   digitalWrite( BOOT_BARCODE, 0);
   pinMode(RESET_BARCODE,OUTPUT);
   digitalWrite( RESET_BARCODE, 1);
   
   pinMode(RS_ETHERNET,OUTPUT);
   digitalWrite( RS_ETHERNET, 1);

   pinMode(CS_EXTROM,OUTPUT);
   digitalWrite( CS_EXTROM, 1);

   pinMode(CS_SD,OUTPUT);
   digitalWrite( CS_SD, 1);

   pinMode(CS_UART,OUTPUT);
   digitalWrite(CS_UART,1);

   // pinMode(LED_DEBUG,OUTPUT);
   // digitalWrite(LED_DEBUG,0);
   digitalWrite(CS_UART,0);

   pinMode(PWRKEY_MODEM,OUTPUT);
   digitalWrite(PWRKEY_MODEM,0);
   pinMode(SIMSTATUS_MODEM,INPUT_PULLUP);
   
   SerialToPrinter.begin(115200,SERIAL_8N1,SerialToPrinter_tx,SerialToPrinter_rx);
   SerialToBarcodeModem.begin(115200,SERIAL_8N1,SerialToBarcodeModem_tx,SerialToBarcodeModem_rx);
   
}

#endif
