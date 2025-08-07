#ifndef BANNHOT_H
#define BANNHOT_H

/*
File: BanNhot.h
Date: 24/06/19
Description: This file is the library header for BanNhot.ino, providing necessary declarations and functionalities used in the BanNhot project.
*/

/*
Cách khai báo biến và tên hàm:

Tên hàm:
  - eeBbCc          : Đối với hàm void
  - aa_bb_cc        : Đối với hàm int hoặc các hàm có trả về

Biến trong main:
  - ui(X)_aa_bb_cc  : Với X là kích thước biến (bit)

Struct hoặc Class:
  - AaBbCc          : Đối với tên Struct hoặc Class
  - AaBbCc          : Đối tượng Struct hoặc Class

Biến trong Struct hoặc Class:
  - eeBbCc          : Thường với tiếng Anh
  - aa_bb_cc        : Với tiếng Việt

Mảng:
  - AaBbCc          : Đối với mảng Struct hoặc Class
  - aa_bb_cc        : Đối với mảng biến (có ui(X) hoặc không)
*/

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "lib/CRC16.h"
#include "src/ReadSensorDebounce.h"
#include "src/SerialComunication.h"

#include "src/ControlDebugSerial.h"

//-----------Firmware number------------//
uint16_t firmware = 0x0001;
//-----------UART PC------------//
SerialComunication UART_PC(Serial, 0x04);
//-----------SENSOR------------//

#define S1 12 // Pin S1
#define S2 13 // Pin S2
#define S3 14 // Pin S3

Sensor CuaTrai(S1);
Sensor CuaPhai(S2);
Sensor CuaTren(S3);
Sensor *SensorArray[] = {&CuaTrai, &CuaPhai, &CuaTren};

//-----------LED------------//
#define numLED 3
#define LED1 11 // 41 // LED1 --> PIXEL1
#define LED2 10 // 40 // LED2 --> PIXEL2
#define LED0 9  // 42 // LED0 --> PIXEL3
int LED[numLED] = {LED0, LED1, LED2};

//-----------LED, BUZZER ON BOARD------------//
#define BUZZER 1
#define LED_BLINK 2

#define THOIGIANCHOGUILENH 100

bool led_status = 0;
bool buz_status = 0;

uint32_t ui32_time_blink = 0;

uint32_t ui32_timesend;

uint32_t ui32_currentTime = 0;
uint32_t ui32_timecho_guilenh;

uint32_t timeout_buz = 0;

uint8_t order_command[2] = {0x00, 0x00};
uint8_t self_order_command[2] = {0x00, 0x00};
uint8_t last_order_command[2] = {0x00, 0x00};

//-----------Serial------------//

uint8_t ui8_data_send[15];
uint8_t ui8_data_receive[20];

uint8_t ui8_status = 0x00;
uint8_t ui8_function = 0x00;
uint8_t ui8_slaveID = 0x04;

//-----------Respond------------//
bool ui2_respond_confirmReceived = false;
bool ui2_respond_incorrectCRC = false;
bool ui2_respond_function_notExist = false;
bool ui8_respond_repeatedCommand = false;
bool ui2_respond_firmware = false;
bool ui2_reset = false;

//-----------Trạng thái thùng nhớt------------//
#define NO_STATE 255 // Không có trạng thái
enum TrangThaiThungNhot
{
    CAN = 0,      // Trạng thái bình chứa nhớt cạn
    DAY,          // Trạng thái bình chứa nhớt đầy
    BINHTHUONG,   // Trạng thái bình thường (trong thùng có nhớt)
    DANGHOATDONG, // Trạng thái thùng đang hoạt động
    DANGCHO       // Trạng thái thùng đang chờ

};

enum TrangThaiVoiBanNhot
{
    NGUNGBAN = 0, // Trạng thái vòi ngừng bán
    DANGBAN,      // Trạng thái vòi đang bán
    TRANGTHAICHO, // Trạng thái vòi đang chờ
};

enum MODE
{
    AUTO = 0,    // Trạng thái điều khiển tự động
    CONTROLRGB,  // Trạng thái điều từng thông số RGC
    CONTROLCOLOR // Trạng thái điều khiển màu

};

uint8_t ui8_mode = CONTROLRGB;

uint8_t ui8_RED = 0;
uint8_t ui8_GREEN = 0;
uint8_t ui8_BLUE = 0;

enum COLORMODE
{
    STATIC = 0, // Trạng thái led tĩnh
    BLINK       // Trạng thái led nhấp nháy
};
uint8_t ui8_color_mode = STATIC;

//-----------Khai báo cho LED PIXEl------------//
#define NUMPIXELS 100 // Số bóng LED trên một Strip
#define BRIGHTNESS 50 // Độ sáng (từ 0 đến 255)

// Thời gian nháy hiệu ứng khi đang bơm
uint32_t ui32_time_hieuung[3] = {0, 0, 0};
const uint32_t timedelay_hieuung = 1000; // Nháy sau 1 s

bool ui2_state_hieuung[3] = {0, 0, 0}; // Mảng lưu trạng thái nháy

struct ThungNhot
{
    const uint8_t LEDPin;

    bool state_hieuung;
    uint8_t trangthai_LED;
    uint8_t trangthai_thung;
    uint8_t trangthai_voi;
    uint8_t trangthai_thung_nhantuPC;
    uint8_t trangthai_voi_nhantuPC;
    uint32_t time_hieuung;
    Adafruit_NeoPixel LED_strip;

    uint8_t RED;
    uint8_t GREEN;
    uint8_t BLUE;
    uint8_t colorIndex;
    uint8_t colorMode;
    uint8_t colorStaticReadySet;

    ThungNhot(uint8_t LEDPin) : LEDPin(LEDPin), trangthai_LED(NO_STATE), time_hieuung(0), state_hieuung(0),
                                trangthai_thung(BINHTHUONG), trangthai_voi(NGUNGBAN),
                                trangthai_thung_nhantuPC(NO_STATE), trangthai_voi_nhantuPC(NO_STATE),
                                RED(0), GREEN(0), BLUE(0), colorMode(STATIC), colorStaticReadySet(1),
                                colorIndex(0xff)
    {
        LED_strip = Adafruit_NeoPixel(NUMPIXELS, LEDPin, NEO_RBG + NEO_KHZ800);
    }

    void setupLED()
    {
        LED_strip.begin();
        LED_strip.setBrightness(BRIGHTNESS);
        LED_strip.show();
    }

    void clearState()
    {

        trangthai_thung = BINHTHUONG;
        trangthai_thung_nhantuPC = NO_STATE;
        trangthai_voi = NGUNGBAN;
        trangthai_voi_nhantuPC = NO_STATE;
        trangthai_LED = NO_STATE;
        time_hieuung = 0;
        state_hieuung = 0;

        colorMode = STATIC;
        colorStaticReadySet = 1;
        RED = 0;
        GREEN = 0;
        BLUE = 255;
    }
};

ThungNhot ThungNhot1(LED1);    // Nhớt 1
ThungNhot ThungNhot2(LED2);    // Nhớt 2
ThungNhot ThungNhotThai(LED0); // Nhớt thải

ThungNhot *ThungNhotArray[] = {&ThungNhot1, &ThungNhot2, &ThungNhotThai};

uint8_t mapping_sensor(uint8_t sensor)
{
    if (sensor == CuaTrai.Pin)
        return 0x01;
    else if (sensor == CuaPhai.Pin)
        return 0x02;
    else if (sensor == CuaTren.Pin)
        return 0x03;
    else
        return 0xff;
}

uint8_t mapping_state(uint8_t state)
{
    switch (state)
    {
    case 0:
        return 0x00;
    case 1:
        return 0xff;
    default:
        return 0xcc;
    }
}

void raiseOrderCommand(uint8_t *order)
{
    (*(order + 1))++;
    *(order + 0) = (*(order + 1) == 0x00) ? ((*(order + 0))++) : (*(order + 0));
}

#endif