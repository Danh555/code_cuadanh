

#pragma once
#include "Arduino.h"
#include <stdio.h>
#include <stdint.h>

#define DEBUG
#ifdef DEBUG
	#define uart_debug Serial
	#define uart_PC Serial2
#else
	#define uart_debug Serial2
	#define uart_PC Serial
#endif

/*-------------DEFINE DRIVER DONG CO---------------------*/
#define M1_DIR 1
#define M1_PWM 2
#define M2_DIR 3
#define M2_PWM 4
#define K_DIR 33
#define K_PWM 34
#define E_CHA 18 // encoder cua thang 
#define E_CHB 21 // encoder cua thang 

/*-------------MOTOR NHA QUA-----------------------------*/

enum PortP_Pin{
	PIN_P0 = 0,
	PIN_P1,
    PIN_P2,
    PIN_P3,
    PIN_P4,
    PIN_P5,
    PIN_P6,
    PIN_P7,
    PIN_P8,
    PIN_P9,
    PIN_P10,
    PIN_P11,
    PIN_P12,
    PIN_P13,
    PIN_P14,
    PIN_P15,
    PIN_P16,
    PIN_P17
};

enum PortN_Pin{
	PIN_N0 = 0,
	PIN_N1,
    PIN_N2,
    PIN_N3,
    PIN_N4,
    PIN_N5,
    PIN_N6,
    PIN_N7,
    PIN_N8,
    PIN_N9,
    PIN_N10,
    PIN_N11,
    PIN_N12,
    PIN_N13,
    PIN_N14,
    PIN_N15,
    PIN_N16,
    PIN_N17
};

// #define COLLUM_1 PIN_P11
// #define COLLUM_2 PIN_P10
// #define COLLUM_3 PIN_P9
// #define COLLUM_4 PIN_P8
// #define COLLUM_5 PIN_P7
// #define COLLUM_6 PIN_P6
// #define COLLUM_7 PIN_P5
// #define COLLUM_8 PIN_P4

// #define ROW_1 PIN_N8
// #define ROW_2 PIN_N9
// #define ROW_3 PIN_N10
// #define ROW_4 PIN_N11
// #define ROW_5 PIN_N4
// #define ROW_6 PIN_N5

#define ROW_1 PIN_N11
#define ROW_2 PIN_N10
// #define ROW_3 PIN_N9
// #define ROW_4 PIN_N8
// #define ROW_5 PIN_N7
// #define ROW_6 PIN_N6
// #define ROW_7 PIN_N5
// #define ROW_8 PIN_N4

#define ROW_3 PIN_N4
#define ROW_4 PIN_N5
#define ROW_5 PIN_N6
#define ROW_6 PIN_N7
#define ROW_7 PIN_N8
#define ROW_8 PIN_N9

#define ROW_9 PIN_N3
#define ROW_10 PIN_N2
#define ROW_11 PIN_N1
#define ROW_12 PIN_N0

#define COLLUM_1 PIN_P8
#define COLLUM_2 PIN_P9
#define COLLUM_3 PIN_P10
#define COLLUM_4 PIN_P11
#define COLLUM_5 PIN_P4
#define COLLUM_6 PIN_P5
#define COLLUM_7 PIN_P6
#define COLLUM_8 PIN_P7
#define COLLUM_9 PIN_P0
#define COLLUM_10 PIN_P1
#define COLLUM_11 PIN_P2
#define COLLUM_12 PIN_P3

unsigned char COL[12]={COLLUM_1,COLLUM_2,COLLUM_3,COLLUM_4,COLLUM_5,COLLUM_6,COLLUM_7,COLLUM_8,COLLUM_9,COLLUM_10,COLLUM_11,COLLUM_12};
unsigned char ROW[12]={ROW_1,ROW_2,ROW_3,ROW_4,ROW_5,ROW_6,ROW_7,ROW_8,ROW_9,ROW_10,ROW_11,ROW_12};
unsigned char DIR_PIN[3]={M1_DIR,M2_DIR,K_DIR};
unsigned char PWM_PIN[3]={M1_PWM,M2_PWM,K_PWM};



#define addr_ExIO_Motor_COL 0x25 //(Motor from 1 to 4)
#define addr_ExIO_Motor_ROL 0x27 //(Relay and Motor 5)

// #define DEBUG

// #ifdef DEBUG
// 	#define uart_debug Serial
// 	#define uart_PC Serial2
// #else
// 	#define uart_debug Serial2
// 	#define uart_PC Serial
// #endif


//-----------Serial------------//
uint8_t ui8_Byteindex = 0;
uint8_t ui8_data_send[15];
uint8_t ui8_data_rec[100];
uint8_t ui8_status = 0x00;
uint8_t ui8_function = 0x00;
uint8_t ui8_slaveID = 0x03;
bool ui2_BTcomplete = false;
bool ui2_BTreceive = false;
bool ui2_start = false;
uint16_t ui16_order_command, ui16_last_order_command = 0xFFFF;
uint16_t ui16_self_order_command = 0x0001;
uint32_t ui32_timechar = 0;

//-----------Serial debug-------------------//
String inputString = "";            // a String to hold incoming data
bool stringComplete = false;        // whether the string


/*===============bien toan cuc===============================*/
uint8_t ui8_phanhoi_dungFunction = 0;
uint8_t ui8_phanhoi_saiCRC = 0;
uint8_t ui8_phanhoi_saiFunction = 0;
uint8_t ui8_phanhoi_trunglenh = 0;


/*===============define dk motor thang may, tam che, khoa tu===============================*/
#define MAX_SPEED 255 //t? 0-255
#define MIN_SPEED 0

#define df_run_up 1
#define df_run_down 2
#define df_run_stop 0


int running=df_run_stop;
int vitrithangmay=1;
//EE-SX672-WR
//Brown VCC (5-24VDC)
//Blue	GND	(0V)
//Black	Output (NPN Open collector output)

#define brake_enable HIGH
#define brake_disable LOW
unsigned long currentMillis11;
unsigned long currentMillis12;

unsigned long currentMillis13;
unsigned long currentMillis14;

//---------------define direction of motor------------------//

#define chieuquay_thangmay_len 1
#define chieuquay_thangmay_xuong 0

#define chieuquay_tamche_ra 1
#define chieuquay_tamche_vao 0

#define khoa_mo 1
#define khoa_dong 0


#define ledPin 46

#define ledon digitalWrite(ledPin,LOW);
#define ledoff digitalWrite(ledPin,HIGH);
#define ledpixel 38
 
int counter_test=0;
 
 //-----------------Define, variables----------------------- 
uint8_t row_=0;
uint8_t  column_=0;
boolean start_drop_sku=false;

//------------------End output define---------------------------
struct display
{
   int red;
   int green;
   int blue;
};
typedef struct display RGBdisplay;

RGBdisplay RGB_Player[8];
//----------------------------------------------------------
//---------------UARST------------------------------------------------
int inByte = 0;         // incoming serial byte
// boolean stringComplete = false;  // whether the string is complete
int serialInArray[30]; //= new int[3];    // Where we'll put what we receive
int dataProcess[30];
int serialCount = 0;                 // A count of how many bytes we receive
boolean firstContact = false;        // Whether we've heard from the microcontroller
//---------------END UARST------------------------------------------------
//----------------------------------------------------------

boolean ledState=false;
int resend_interval=0;
boolean  start_send=false;
boolean  u2_led_state=false;

int tamper_interval=0;
boolean update_tamper =0;
boolean check_motor =0;
boolean wait_off_led=0;
unsigned long time_off_led;
unsigned long u32_timeflash;
int erro_status=0;
int dem=0;
int chongnhieu=0;
//----------------------------------------------------------
//-----------------Khay chua san pham-----------------------
//khay 1 dang thap
//khay 2 ok
//khay 3 cao
//khay 4 cao
//khay 5 cao==4

// int Vitrilaysanpham[6]={530,450,370,290,190,100};//A B C D E F
//int Vitrilaysanpham[5]={430,280,37,281,37};//A B C D E
// int Vitrilaysanpham[3] = {190,100};
// int Vitrilaysanpham[3] = {30,170};
// int Vitrilaysanpham[2] = {170,50};
// int Vitrilaysanpham[6]={35,55,75,110,125,145};//A B C D E F
// uint16_t Vitrilaysanpham[6]={350,550,750,1100,1250,1450};//A B C D E F
// uint16_t Vitrilaysanpham[6]={1500,20,35,45,55,65};//A B C D E F
// uint16_t Vitrilaysanpham[6]={1100,2000,3000,3700,4400,5100};//A B C D E F may binh thuongh
// uint16_t Vitrilaysanpham[6]={4200,7300,10800,12800,15500,18000};//A B C D E F
// uint16_t Vitrilaysanpham[6]={7000,10800,15500,19800,25000,28000};//A B C D E F danh cho may so 2
// uint16_t Vitrilaysanpham[6]={1300,2300,3400,4400,5000,6000};//A B C D E F
//-----------------End Khay chua san pham-----------------------
//----------------------------------------------------------
// Variables will change:
int buttonState[8];             // the current reading from the input pin
int lastButtonState[8];   // the previous reading from the input pin
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime[8];  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

uint8_t ui8_respone2PC = 0;
uint32_t ui32_timeduocguilenh = 0;

uint32_t ui32_timeout_nhaqua=0;
uint32_t ui32_timeout_khoacua=0;
uint32_t ui32_timeout_rsqua=0;
uint8_t ui8_check_khoacua=0;
int ui8_vitridangchay=0;

uint8_t ui8_songuoi_quet=0;
uint8_t ui8_songuoi_tuphanmem=0;
uint32_t ui32_tiemout_rskhay=0;
uint8_t ui8_trangthai_cualayqua=0;
uint8_t ui8_trangthai_thungrotqua=0;
uint32_t ui32_timeout_checkcuaqua=0;
uint8_t ui8_trangthaikhoacua=0;
uint8_t ui8_every_ok=0;
uint8_t ui8_dusolon=0;
uint8_t ui8_goilenh_xuong=0;
int check_landau=0;
uint8_t ui8_dakhoa_cua=0;
uint8_t ui8_dabamnut=0;
uint8_t ui8_toivitri_antoan=0;
uint8_t ui8_solangoilenh=0;
uint32_t ui32_timeout_chaytamngan=0;
int chay_tamche=0;
uint8_t ui8_dungtamche=0;
uint8_t ui8_tamchechay_xong=0;

uint16_t ui16_lastspeed=0;
uint8_t ui8_check_data=0;

#define use_elavator_

enum ERRO_STATUS
 {
     sku_success = 0, //nhả sản phẩm thành công
     sku_empty, //hết sản phẩm ở vị trí này
     motor_erro, //lỗi động cơ
     sku_over, //không tồn tại sản phẩm ở vị trí này
     df_cmd_erro, //Lỗi cmd nhỏ hơn cmd hiện hành
     khoacua,
     mocua,
     dongcua,
     nutbam,
     thangmay
 };
 const String msg[] =
 {
     "Nha san pham thanh cong",
     "Het san pham",
     "motor_erro",
     "Khong co san pham nay",
     "Loi CMD code"
 };

uint32_t ui32_timecho_guilenh=0;
uint8_t ui8_phanhoi_trangthainhaqua=0;
uint8_t ui8_phanhoi_huthangmay=0;
uint8_t ui8_trangthai_thangmay=0;
uint8_t ui8_phanhoi_hudongconhaqua=0;
uint8_t ui8_trangthai_dongconhaqua=0;

int8_t state = 0;
unsigned long tempus;
unsigned long temp_main=0;
int temp_serial=0;
int wait_off=0;

int led_status=0;
uint32_t ui32_timeout_chopled=0;
uint32_t ui32_timeout_comeback=0;
uint8_t ui8_hoanthanh=0;
uint8_t ui8_check_doilenh=0;
uint32_t ui32_timeout_vehome=0;
uint8_t ui8_traloi_PC=0;
uint8_t ui8_trangthainhaqua_thanhcong=0;
uint8_t ui8_trangthainhaqua_thatbai=0;
uint8_t ui8_phanhoi_cua=0;
// uint8_t ui8_trangthai_cuamo=0;
// uint8_t ui8_trangthai_cuadong=0;
#define tg_xuong 1500

String str = "";
String data_lo = "";
String data_hi = "";
String firmware = "ROCK.001";
void setup_motor()
{
    for(int i=0; i<3; i++)
    {
        pinMode(DIR_PIN[i],OUTPUT);
        pinMode(PWM_PIN[i],OUTPUT);
        digitalWrite(DIR_PIN[i],0);
        digitalWrite(PWM_PIN[i],0);
    }

    pinMode(ledPin,OUTPUT);
    digitalWrite(ledPin,0);

    pinMode(E_CHA,INPUT_PULLUP);
}

