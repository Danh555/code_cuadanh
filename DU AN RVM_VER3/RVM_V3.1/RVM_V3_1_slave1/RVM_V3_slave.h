#ifndef __RVMV3_H
#define __RVMV3_H

/*
File: RVM_V3.h
Date: 24/10/02
Description: This file is the library header for RVM_V3.ino, providing necessary declarations and functionalities used in the BanNhot project.
*/

/*
Cách khai báo biến và tên hàm:

Tên hàm:
  - eeBbCc          : Đối với hàm void
  - aa_bb_cc        : Đối với hàm int hoặc các hàm có trả về

Biến trong main:
  - ui(X)_aa_bb_cc  : Đối với biến toàn cục, với X là kích thước biến (bit)
  - Đối vói biến cục bộ trong funtion thì giống như biến trong Struct hoặc Class

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

#include "config.h"
#include "hardware_config.h"

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <EEPROM.h>

#include "src/SerialComunication.h"
#include "src/ReadSensorDebounce.h"

#define ShowSerial
#define THOIGIANCHOGUILENH 100

//-----------Firmware number------------//
uint8_t firmware[3] = {3, 1, 1};

//-----------UART DEBUG ------------//
// #define SerialDebug Serial
#define SerialDebug USBSerial

//-----------RS485 Communication------------//
#define slave1 0x09

//-----------Buffer------------//
#if 1
// uint8_t ui8_buffer_send_PC[15];
// uint8_t ui8_buffer_receive_PC[20];

uint8_t buffer_send[15];
uint8_t buffer_receive[20];

uint8_t ui8_status = 0x00;
uint8_t ui8_function = 0x00;

uint8_t order_command[2] = {0x00, 0x00};
uint8_t self_order_command[2] = {0x00, 0x00};
uint8_t last_order_command[2] = {0x01, 0x01};
#endif

//-----------Respond Flag------------//
#if 1
bool ui2_respond_confirmReceived = false;
bool ui2_funtion_self_respond = false;
bool ui2_respond_incorrectCRC = false;
bool ui2_respond_function_notExist = false;
bool ui2_respond_repeatedCommand = false;
// bool ui2_phanhoi_firmware = false;
bool ui2_reset = false;
#endif

//----------- EEPROM Addr & Error variable ------------//
#if 1
enum EEPROM_ADD
{
  ADD_COUNT_RESET0 = 0,
  ADD_COUNT_RESET1 = 1,
  ADD_ERROR_EPLON,
  ADD_ERROR_NGHIENCHAI,
  ADD_ERROR_KETPHANLOAI,

  ADD_STACKSIZE_TASK_SENSOR,
  ADD_STACKSIZE_TASK_SERIAL,
  ADD_STACKSIZE_TASK_R485,
  ADD_STACKSIZE_TASK_MOTOR,
  ADD_END_EEPROM
};
// uint8_t ui8_error_EpLon = 0;
// uint8_t ui8_error_NghienChai = 0;
// uint8_t ui8_error_KetPhanLoai1 = 0;
// uint8_t ui8_error_KetPhanLoai2 = 0;
#endif

//----------Code Funtion------------//
#if 1
namespace codeMaster
{

  const uint8_t PHANLOAI = 0x01;
  const uint8_t NGHIENCHAI = 0x02;
  const uint8_t EPLON = 0x03;
  const uint8_t check_error = 0x04;
  const uint8_t erase_error = 0x05;
  const uint8_t admin_control_motor = 0x06;
  const uint8_t check_firmware = 0x07;
};

namespace vitrithungracphanloai
{
  const uint8_t NOWHERE = 0X00;
  const uint8_t BOTTLE = 0x01;
  const uint8_t CAN = 0x02;
  const uint8_t OTHER = 0x03;
}

namespace vitricocauphanloai
{
  const uint8_t NOWHERE = 0x00;
  const uint8_t LEFT = 0x01;
  const uint8_t RIGHT = 0x02;
}

namespace admin_control_motor
{
  const uint8_t PHANLOAI1 = 0x04;
  const uint8_t PHANLOAI2 = 0x05;
  const uint8_t NGHIENCHAI = 0x06;
  const uint8_t EPLON = 0x07;
}

namespace trangthaiRS485
{
  const uint8_t DANGGUI = 0x01;
  const uint8_t DANGNHAN = 0x02;
}

#endif

//----------Global Variable------------//
#if 1

uint8_t ui8_trangthaiRS485 = trangthaiRS485::DANGNHAN;
uint8_t ui8_phanhoi_TrangthaiLoi = 0;
uint8_t ui8_phanhoi_firmware = 0;
uint8_t ui8_admin_control_motor = 0;

bool ui2_tatDongCo_doLoi_flag = 0;

uint32_t ui32_time_blink;
bool led_status = 0;

#endif

struct
{
  uint8_t vitriHientai = vitrithungracphanloai::NOWHERE;
  uint8_t vitriYeucau = vitrithungracphanloai::NOWHERE;
  uint8_t vitriPhanloai1 = vitricocauphanloai::NOWHERE;
  uint8_t vitriPhanloai2 = vitricocauphanloai::NOWHERE;
  uint8_t trangthaiLoi = TrangThaiLoi::KHONGLOI;

  uint32_t beginTime = 0;

  bool dangxulyFlag = 0;       // Bắt đầu thì trạng thái sẽ là không xử lý
  bool phanloai1_doneFlag = 1; // Bắt đầu thì trạng thái sẽ là hoàn thành chạy
  bool phanloai2_doneFlag = 1; // Bắt đầu thì trạng thái sẽ là hoàn thành

  const uint16_t timeError_phanloai1 = 10000;
  const uint16_t timeStop_phanloai2 = 2000;
} PhanLoai;

struct
{
  uint8_t trangthaiYeuCau = TrangThaiMotor::STOP;
  uint8_t trangthaiLoi = TrangThaiLoi::KHONGLOI;

  uint32_t beginTime = 0;
  uint32_t beginEpTime = 0;
  uint32_t lastChangeTime = 0;

  bool lastCBstate = 0;
  bool dangxulyFlag = 0;
  bool chovatFlag = 0;
  uint8_t count = 0;

  const uint16_t timeErrorGoAway = 4000;
  const uint16_t timeErrorGoBack = 10000;
  const uint16_t timeWait = 10000;

} EpLon;

struct
{

  uint8_t trangthaiYeuCau = TrangThaiMotor::STOP;
  uint8_t trangthaiLoi = TrangThaiLoi::KHONGLOI;
  uint32_t beginTime = 0;
  uint32_t beginStopTime = 0;
  uint32_t lastChangeTime = 0;

  bool yeucauStopFlag = 0;
  bool dangRunFlag = 0;
  bool lastCBstate = 0;

  const uint16_t timeError = 2000;
  const uint16_t timeWaitStop = 60000;
  const uint16_t timeAutoStop = 600000; // 10 phút

} NghienChai;

#endif //__RVMV3_H
