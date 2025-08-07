#include <HX711.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// define chân GPIO cho LoadCell
#define LOADCELL_DOUT_PIN   4
#define LOADCELL_SCK_PIN    3
#define calibration_factor -742.6165198237885 //-742.72917232022  //-742.6165198237885

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

String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

uint32_t ui32_timeout_hienthi = 0; // Thời gian hiển thị
uint8_t ui8_batdauhienthi=0;
uint8_t ui8_moichao=0;
float ui8_khoiluong=0;