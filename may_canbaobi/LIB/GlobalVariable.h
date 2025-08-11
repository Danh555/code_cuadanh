#include <HX711.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <SimpleKalmanFilter.h>

// define chân GPIO cho LoadCell
#define LOADCELL_DOUT_PIN   4
#define LOADCELL_SCK_PIN    3
#define calibration_factor 420 //-742.72917232022  //-742.6165198237885

#define Serial_debug    Serial
#define id_i2c 0x27
HX711 scale;


// set the LCD number of columns and rows
int lcdColumns = 20;
int lcdRows = 4;

LiquidCrystal_I2C lcd(id_i2c, lcdColumns, lcdRows);

long offset = 0;
uint8_t isDetected = 0;
uint16_t ScaleLimited = 10; 
bool CheckStt;
bool ledstt;
int calib=0;
bool ui2_detect = false;
bool ui2_ena_scale = false;

// Khai bao bien xu ly data Serial.
String inputString = "" , inputNum, inputKg, inputMet, inputTime;
boolean stringComplete = false; 
// boolean isNum = false;
boolean isKg = false;
boolean first_char = true;
// boolean input_time = false;

uint32_t ui32_timeout_hienthi = 0; // Thời gian hiển thị
uint8_t ui8_batdauhienthi=0;
uint8_t ui8_moichao=0;
float ui8_khoiluong=0;

/* --------------------- Mang tam luu & doc eeprom ---------------------------*/
float array_scale [3] = {};
float array_get [3] = {};

struct myObject {
	float field1;
	float field2;
};
unsigned int address;
boolean request_h = false;		//Biến nhận lệnh xử lý chiều cao
boolean request_w = false;		//Biến nhận lệnh xử lý cân nặng	
boolean request_l = false;		//Biến nhận lệnh xử lý LED
boolean h_measure_ser = false;   //Biến nhận lệnh đo từ serial, bằng 1 khi nhận lệnh từ Serial.
boolean input_time = false;


/* ------------------- Khai bao bien do can nang.------------------------ */
float weights = 0.00; 
float weightsTemp;
float test;
char byteWeights[4];
boolean calibLoadcell = false;
boolean calibHight = false;
long doc_analog; 	//thông số đọc được trước trừ bì.
float get_value_sub; 	//thông số đọc được sau trừ bì.
float khoiluong;	//giá trị qui đổi ra khối lượng (kg).
float scale_value;	//Hệ số để qui đổi ra khối lượng.
float scale_value_calib;
long offset_scale;
const float tolerances = 0.35;
byte lando = 45; //So lan lay gia tri do.
unsigned int add_eeprom;
boolean isNum = false;

int test_can=0;
unsigned long timeEeprom = 0; 
float ui8_prewest = 0; // Biến lưu giá trị trước đó của nút nhấn