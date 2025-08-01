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
#include "src/Motor_DriverDIY.h"

#define THOIGIANCHOGUILENH 100

//-----------Firmware number------------//
uint8_t firmware[3] = {3, 1, 1};
#define v3_1

// #define debug

//-----------UART PC------------//
#define selfSlaveID 0x05
SerialUSB SerialPC(Serial, selfSlaveID);

//-----------UART DEBUG ------------//

#ifdef v3_0
#define SerialDebug Serial2
#elif defined(v3_1)
#define SerialDebug USBSerial
#endif

//-----------RS485 Communication------------//
RS485_Master RS485(Serial1, RS485_TX_PIN, RS485_RX_PIN);
#define slave1 0x01
#define slave2 0x02
#define slave3 0x03

//----------- EEPROM Addr & Error variable ------------//
#if 1
enum EEPROM_ADD
{
  ADD_COUNT_RESET0 = 0,
  ADD_COUNT_RESET1 = 1,
  ADD_ERROR_EPLON,
  ADD_ERROR_NGHIENCHAI,
  ADD_ERROR_KETCUA,
  ADD_ERROR_BANGTAI,
  ADD_ERROR_KETPHANLOAI1,
  ADD_ERROR_KETPHANLOAI2,

  ADD_STACKSIZE_TASK_SENSOR,
  ADD_STACKSIZE_TASK_SERIAL,
  ADD_STACKSIZE_TASK_R485,
  ADD_STACKSIZE_TASK_MOTOR,
  ADD_END_EEPROM
};
uint8_t ui8_error_EpLon = 0;
uint8_t ui8_error_NghienChai = 0;
uint8_t ui8_error_KetCua = 0;
uint8_t ui8_error_KetPhanLoai1 = 0;
uint8_t ui8_error_KetPhanLoai2 = 0;
#endif

//----------Code Funtion------------//
#if 1
namespace code
{
  // Đọc tín hiệu điều khiển của PC
  const uint8_t begin = 0x04;
  const uint8_t detect_result = 0x07;
  const uint8_t reset = 0x10;
  const uint8_t trash_rate = 0x20;
  const uint8_t reset_trashBin = 0x21;
  const uint8_t erase_error = 0x09;
  const uint8_t control_motor = 0x12;
  const uint8_t check_firmWare = 0xFB;
  const uint8_t admin_control_motor = 0x0F;

  // Gửi trạng thái tự động
  const uint8_t send_sensor_state = 0x08;
  const uint8_t send_detectstate_inZone = 0x11;

  const uint8_t send_state_motorEplon = 0x14;
  const uint8_t send_state_motorNhavat = 0x15;
  const uint8_t send_state_motorNghienchai = 0x16;
  const uint8_t send_state_ketbangtai = 0x17;
  const uint8_t send_state_cua = 0xFA;

  // Phản hồi lỗi truyền dữ liệu
  const uint8_t respond_incorrect_command = 0xFE;
};

namespace admin_control
{
  const uint8_t CUA = 0x00;
  const uint8_t BANGTAI = 0x01;
  const uint8_t NANGBANGTAI = 0x02;
  const uint8_t XOAYCHAI = 0x03;
  const uint8_t PHANLOAI1 = 0x04;
  const uint8_t PHANLOAI2 = 0x05;
  const uint8_t NGHIENCHAI = 0x06;
  const uint8_t EPLON = 0x07;
}

namespace detect_result
{
  const uint8_t NOTHING = 0x00;
  const uint8_t BOTTLE = 0x01;
  const uint8_t CAN = 0x02;
  const uint8_t OTHER = 0x03;
}

namespace codeSlaveRS485
{

  const uint8_t PHANLOAI = 0x01;
  const uint8_t NGHIENCHAI = 0x02;
  const uint8_t EPLON = 0x03;
  const uint8_t check_error = 0x04;
  const uint8_t erase_error = 0x05;
  const uint8_t admin_control_motor = 0x06;
  const uint8_t check_firmware = 0x07;
};
#endif

//-----------Buffer------------//
#if 1
uint8_t ui8_buffer_send_PC[15];
uint8_t ui8_buffer_receive_PC[20];

uint8_t buffer_send_slave[15];
uint8_t buffer_receive_slave[20];

uint8_t ui8_status = 0x00;
uint8_t ui8_function = 0x00;
uint8_t ui8_slaveID = 0x04;

uint8_t order_command[2] = {0x00, 0x00};
uint8_t self_order_command[2] = {0x00, 0x00};
uint8_t last_order_command[2] = {0x00, 0x00};
#endif

//----------Global Variable------------//
#if 1

uint8_t ui8_ketquanhandien = detect_result::NOTHING;
uint32_t ui32_time_blink;
uint32_t ui32_time_checkslave = 0;
uint32_t ui32_timebegin_nhanphanhoi = 0;
uint8_t ui8_check_slave = 0;
uint8_t ui8_receive_respond_from_slave = 0;
bool led_status = 0;

uint8_t ui8_control_BoPhanLoai = 0;
uint8_t ui8_control_BoEp = 0;
uint8_t ui8_control_BoNghien = 0;

uint8_t ui8_erase_error_slave = 0; // là số lượng slave cần xóa lỗi

#endif
//-----------Respond Flag------------//
#if 1
bool ui2_respond_confirmReceived = false;
bool ui2_funtion_self_respond = false;
bool ui2_respond_incorrectCRC = false;
bool ui2_respond_function_notExist = false;
bool ui2_respond_repeatedCommand = false;
bool ui2_respond_firmware = false;
bool ui2_reset = false;
#endif

uint8_t mapping_sensor(uint8_t sensor)
{
  return 1;
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

struct
{
  bool lastState = 0;
  bool state = 0;
  uint32_t lastTime = 0;
  uint8_t trangThaiLoi = TrangThaiLoi::KHONGLOI;
} Encoder;

#endif //__RVMV3_H
