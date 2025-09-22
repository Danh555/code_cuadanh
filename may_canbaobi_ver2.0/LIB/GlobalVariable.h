#include <HX711.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <SimpleKalmanFilter.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif


// define chân GPIO cho LoadCell
#define LOADCELL_DOUT_PIN   2
#define LOADCELL_SCK_PIN    A2
#define calibration_factor 420 //-742.72917232022  //-742.6165198237885

#define Serial_debug    Serial
#define id_i2c 0x27
HX711 scale;


#define LED_PIN    A1

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 6
// #define BUTTON_PIN A3

Adafruit_NeoPixel pixels1(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
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
float array_scale [5] = {};
float array_get [5] = {};

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
float weightsTemp = 0.00;
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
byte lando = 10; //So lan lay gia tri do.
unsigned int add_eeprom;
boolean isNum = false;

int test_can=0;
unsigned long timeEeprom = 0; 
float ui8_prewest = 0; // Biến lưu giá trị trước đó của nút nhấn

uint32_t ui32_timeoutkl=0; // Biến thời gian khóa không cho đo liên tục
uint8_t ui8_calib=0; // Biến trạng thái calib cân
uint8_t ui8_detect=0; // Biến trạng thái detect vật
float get_sample=0; // Biến lưu giá trị đọc từ cân
uint32_t time_chopchop=0;
int led_state=0;
uint8_t ui8_ledbaoloi=0;
uint8_t ui8_trangthai_overload=0;
uint8_t ui32_tg_baoled=0;


#define buttonPin A3     // chân nút bấm
int buttonState;             
int lastButtonState = HIGH;  // dùng pull-up nên trạng thái nghỉ là HIGH
int lastReading = HIGH;     // trạng thái nút bấm lần trước

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 1000;   // chống dội 50ms

unsigned long lastClickTime = 0;
unsigned long doubleClickDelay = 1000; // khoảng thời gian phân biệt single/double click

unsigned long lastClickTime_nut1 = 0;
unsigned long doubleClickDelay_nut1 = 1000;

int clickCount = 0;

uint8_t ui8_tienhanh_tare=0; // Biến trạng thái thực hiện tare

uint32_t ui32_tg_bamnut=0; // Biến thời gian bấm nút
uint8_t ui8_solan_bamnut=0; // Biến đếm số lần bấm nút
uint8_t ui8_bamnut=0; // Biến trạng thái bấm nút
uint8_t ui8_modesetting=0; // Biến trạng thái chế độ setting
uint8_t ui8_kiemtra_mode=0; // Biến kiểm tra chế độ setting

uint32_t pressStartTime = 0; // Thời gian bắt đầu nhấn nút
bool buttonPressed = 0;    // Trạng thái nút bấm

uint8_t ui8_check_thoat=0; // Biến kiểm tra thoát chế độ setting
uint8_t ui8_nut1_bam=0; // Biến trạng thái nút 1 đã bấm	
uint32_t ui32_lasttime_bamnut1=0; // Biến thời gian nút 1 đã bấm
uint8_t ui8_solan_bamnut_nut1=0; // Biến đếm số lần bấm nút 1
uint8_t ui8_landau=0; // Biến trạng thái lần đầu hiển thị
uint8_t ui8_bienluumau=0; // Biến lưu màu led
uint32_t ui8_doichopled=0;

uint32_t ui32_timeout_modesetting=0; // Biến thời gian thoát chế độ setting
uint8_t check_nutthoat=0;
uint8_t ui8_ktnutthoat=0;

String kitu_test="PUT YOUR ITEM";
String buffer;
int position=0;	

unsigned long previousMillis = 0;
const long scrollInterval = 500;   // tốc độ chạy
const long pauseTime = 5000;       // dừng giữa (ms)

bool isPaused = false;
unsigned long pauseStart = 0;
int trangthai_nutnhan=0;
int trangthai_nutnhan_1=0;