#include <Arduino.h>
#include "RVM_V3.h"

static TimerHandle_t auto_reload_timer = NULL;
void setup()
{
  SerialDebug.begin(115200);
  SerialPC.begin(115200);
  RS485.begin(115200);

  if (!EEPROM.begin(1024))
  {
    SerialDebug.println("Failed to initialise EEPROM");
    return;
  }

  uint16_t value = EEPROM.read(EEPROM_ADD::ADD_COUNT_RESET0) | (EEPROM.read(EEPROM_ADD::ADD_COUNT_RESET1) << 8);
  SerialDebug.print("\nSố lần reset: ");
  SerialDebug.println(++value);
  EEPROM.write(EEPROM_ADD::ADD_COUNT_RESET0, value);
  EEPROM.write(EEPROM_ADD::ADD_COUNT_RESET1, (value >> 8) & 0xFF);
  EEPROM.commit();

  if (!pcf_PortA.begin())
  {
    SerialDebug.println("KHONG BAT DAU VOI MODULE IO PORT A");
  }
  delay(50);

  if (!pcf_PortB.begin())
  {
    SerialDebug.println("KHONG BAT DAU VOI MODULE IO PORT B");
  }
  delay(50);

  if (!pcf_PortRelay.begin())
  {
    SerialDebug.println("KHONG BAT DAU VOI MODULE IO PORT RELAY");
  }
  delay(50);

  if (!pcf_Motor.begin())
  {

    SerialDebug.println("KHONG BAT DAU VOI MODULE MOTOR");
  }
  delay(50);

  for (Input_PCF8574 *Cambien : CambienArray)
  {
    (*Cambien).setup();
  }

  Motor_M1.setup();
  Motor_M2.setup();
  Motor_M3.setup();
  Motor_M4.setup();
  Motor_M5.setup();

  Relay1.setup();
  Relay2.setup();
  Relay3.setup();
  Relay4.setup();
  Relay5.setup();

  SerialDebug.print("\nRVM [");
  for (uint8_t i = 0; i < 3; i++)
  {
    SerialDebug.print(firmware[i], HEX);
    if (i == 2)
      SerialDebug.println("]");
    else
      SerialDebug.print(".");
  }

  vTaskDelay(2000 / portTICK_PERIOD_MS);

  // Start serial read task
  xTaskCreatePinnedToCore( // Use xTaskCreate() in vanilla FreeRTOS
      taskSerial,          // Function to be called
      "Read Serial",       // Name of task
      10240,               // Stack size (bytes in ESP32, words in FreeRTOS)
      NULL,                // Parameter to pass
      2,                   // Task priority (must be same to prevent lockup)
      NULL,                // Task handle
      1);                  // Run on one core for demo purposes (ESP32 only)

  xTaskCreatePinnedToCore(taskReadSensor, "taskReadSensor", 10240, NULL, 3, NULL, 1);
  xTaskCreatePinnedToCore(taskSendto_R485Slave, "taskSendto_R485Slave", 10240, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskDebug, "taskDebug", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskControlMotorDC, "taskControlMotorDC", 10240, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskTest, "taskTest", 10240, NULL, 1, NULL, 1);

  // Create an auto-reload timer
  auto_reload_timer = xTimerCreate(
      "Auto-reload timer",    // Name of timer
      1 / portTICK_PERIOD_MS, // Period of timer (in ticks)
      pdTRUE,                 // Auto-reload
      (void *)1,              // Timer ID
      timerReadSensor);       // Callback function

  xTimerStart(auto_reload_timer, portMAX_DELAY);

  // Delete "setup and loop" task
  vTaskDelete(NULL);
}

//-----------------------------------------------------------------------------------------------------------------------
void timerReadSensor(TimerHandle_t xTimer)
{
  for (Input_PCF8574 *Cambien : CambienArray)
  {
    (*Cambien).readDebounce(5);
  }
}

void taskReadSensor(void *parameter)
{
  while (1)
  {
#if 0
    for (Input_PCF8574 *Cambien : CambienArray)
    {
      (*Cambien).readDebounce(10);
    }
#endif
    // SerialDebug.print("Stack of taskReadSensor: ");
    // SerialDebug.println(uxTaskGetStackHighWaterMark(NULL));
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void taskSerial(void *parameters)
{
  while (1)
  {
#if 1
    SerialPC.receive(ui8_buffer_receive_PC);

    if (SerialPC.isReceived)
    {
      for (int i = 0; i < ui8_buffer_receive_PC[0] + 1; i++)
      {
        SerialDebug.write(ui8_buffer_receive_PC[i]);
      }
    }
    processSerial();

    vTaskDelay(10 / portTICK_PERIOD_MS);
#endif
  }
}

void taskSendto_R485Slave(void *parameter)
{
  while (1)
  {
    RS485.receive(ui8_buffer_receive_slave, slave1);
    if (RS485.isReceived)
    {
      SerialDebug.print("\nReceive: ");
      for (uint8_t i = 0; i <= ui8_buffer_receive_slave[0]; i++)
      {
        SerialDebug.print(ui8_buffer_receive_slave[i], HEX);
        SerialDebug.print(" ");
      }
      RS485.isReceived = false;
    }

#if 0
    // SerialDebug.print("Stack of taskSendto_R485Slave: ");
    // SerialDebug.println(uxTaskGetStackHighWaterMark(NULL));
    MotorNghienChai.YeuCau = TrangThaiMotorNghien::FORWARD;

    MotorCua.YeuCau = TrangThaiMotorCua::CUA_NANGLEN;
    vTaskDelay(100 / portTICK_PERIOD_MS);
    MotorPhanLoai1.YeuCau = TrangThaiMotorPhanLoai1::PHANLOAI1_LEFT;
    SerialDebug.println("FORWARD");
    vTaskDelay(10000 / portTICK_PERIOD_MS);

    MotorNghienChai.YeuCau = TrangThaiMotorNghien::REVERSE;

    MotorCua.YeuCau = TrangThaiMotorCua::CUA_HAXUONG;
    vTaskDelay(100 / portTICK_PERIOD_MS);
    MotorPhanLoai1.YeuCau = TrangThaiMotorPhanLoai1::PHANLOAI1_RIGHT;
    SerialDebug.println("REVERSE");
    vTaskDelay(10000 / portTICK_PERIOD_MS);

    MotorNghienChai.YeuCau = TrangThaiMotorNghien::STOP;

    MotorCua.YeuCau = TrangThaiMotorCua::CUA_DUNGLAI;
    vTaskDelay(100 / portTICK_PERIOD_MS);
    MotorPhanLoai1.YeuCau = TrangThaiMotorPhanLoai1::PHANLOAI1_STOP;
    SerialDebug.println("Dung lai");
    vTaskDelay(5000 / portTICK_PERIOD_MS);

#endif
    // test_send_slave_RS485();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void taskControlMotorDC(void *parameter)
{
  while (1)
  {

#if 1
    for (CoCauMotorDC *Motor : MotorDCArray)
    {
      if ((*Motor).TrangThaiLoi == 1 && (*Motor).TrangThai == STOP && (*Motor).YeuCau == STOP)
        continue;
      if ((*Motor).TrangThaiLoi == 1 && ((*Motor).TrangThai != STOP || (*Motor).YeuCau != STOP))
      {
        (*Motor).TrangThai = STOP;
        (*Motor).YeuCau = STOP;
        (*Motor).Stop();
        continue;
      }
      if ((*Motor).TrangThai == STOP && (*Motor).YeuCau == STOP)
      {
      }
      else if ((*Motor).TrangThai == STOP && (*Motor).YeuCau == FORWARD)
      {
        (*Motor).TrangThai = FORWARD;
        (*Motor).timeBegin = millis();
        (*Motor).timeGiaToc = millis();
        (*Motor).TrangThaiTocDo = TrangThaiTocDoMotor::GIATOC;
        (*Motor).Tocdo = TOCDOBANDAU;
      }
      else if ((*Motor).TrangThai == STOP && (*Motor).YeuCau == REVERSE)
      {
        (*Motor).TrangThai = REVERSE;
        (*Motor).timeBegin = millis();
        (*Motor).timeGiaToc = millis();
        (*Motor).TrangThaiTocDo = TrangThaiTocDoMotor::GIATOC;
        (*Motor).Tocdo = TOCDOBANDAU;
      }
      else if ((*Motor).TrangThai == FORWARD && (*Motor).YeuCau == STOP)
      {
        (*Motor).Stop();
        (*Motor).TrangThai = STOP;
      }
      else if ((*Motor).TrangThai == FORWARD && (*Motor).YeuCau == FORWARD)
      {
        if ((*Motor).TrangThaiTocDo == TrangThaiTocDoMotor::ONDINH)
          continue;
        else
          (*Motor).TangTocForward();
      }
      else if ((*Motor).TrangThai == FORWARD && (*Motor).YeuCau == REVERSE)
      {
        (*Motor).Stop();
        (*Motor).TrangThai = STOP;
      }
      else if ((*Motor).TrangThai == REVERSE && (*Motor).YeuCau == STOP)
      {
        (*Motor).Stop();
        (*Motor).TrangThai = STOP;
      }
      else if ((*Motor).TrangThai == REVERSE && (*Motor).YeuCau == FORWARD)
      {
        (*Motor).Stop();
        (*Motor).TrangThai = STOP;
      }
      else if ((*Motor).TrangThai == REVERSE && (*Motor).YeuCau == REVERSE)
      {
        if ((*Motor).TrangThaiTocDo == TrangThaiTocDoMotor::ONDINH)
          continue;
        else
          (*Motor).TangTocReverse();
      }
    }
#endif

#if 1
    for (CoCauMotorServo *Motor : MotorServoArray)
    {
      if ((*Motor).TrangThaiLoi == 1 && (*Motor).TrangThai == STOP && (*Motor).YeuCau == STOP)
        continue;
      if ((*Motor).TrangThaiLoi == 1 && ((*Motor).TrangThai != STOP || (*Motor).YeuCau != STOP))
      {
        (*Motor).TrangThai = STOP;
        (*Motor).YeuCau = STOP;
        (*Motor).Stop();
        continue;
      }
      if ((*Motor).TrangThai == (*Motor).YeuCau)
      {
        continue;
      }
      else if ((*Motor).YeuCau == FORWARD)
      {
        (*Motor).runForward();
        (*Motor).TrangThai = FORWARD;
      }
      else if ((*Motor).YeuCau == REVERSE)
      {
        (*Motor).runReverse();
        (*Motor).TrangThai = REVERSE;
      }
      else if ((*Motor).YeuCau == STOP)
      {
        (*Motor).Stop();
        (*Motor).TrangThai = STOP;
      }
    }
#endif
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void taskDebug(void *parameter)
{
  while (1)
  {
    // for (Input_PCF8574 *Cambien : CambienArray)
    // {
    //   SerialDebug.print((*Cambien).state);
    //   SerialDebug.print(" ");
    // }
    // SerialDebug.println();

    check_error_slave_RS485();
    // SerialDebug.print("Stack of taskSerial: ");
    // SerialDebug.println(uxTaskGetStackHighWaterMark(NULL));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void taskTest(void *parameter)
{
  while (1)
  {
    if (CamBien1.state == 1 && MotorCua.YeuCau != TrangThaiMotorCua::CUA_DUNGLAI)
    {
      MotorCua.YeuCau = TrangThaiMotorCua::CUA_DUNGLAI;
      MotorNghienChai.TrangThaiLoi = TrangThaiLoi::COLOI;
      SerialDebug.println("Dung lai theo Cambien");
      SerialDebug.print("Trang Thai Loi Nghien Chai: ");
      SerialDebug.println(MotorNghienChai.TrangThaiLoi);
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

#if 0
// String inputString = "";
// bool stringComplete = false;
// void docserial()
// {
//   while (SerialDebug.available())
//   {
//     // get the new byte:
//     char inChar = (char)SerialDebug.read();
//     if (inChar == '\n')
//     {
//       stringComplete = true;
//     }
//     else
//     {
//       inputString += inChar;
//     }
//   }
// }

// void serialProcess()
// {
//   SerialDebug.println("TEST BANG DEBUG");

//   SerialDebug.println(inputString);

//   if (inputString == "FW")
//   {
//     Serial.println("CHAY TOI");
//     Motor_M3.TurnRight(255);
//   }

//   if (inputString == "RV")
//   {
//     Serial.println("CHAY LUI");
//     Motor_M3.TurnLeft(255);
//   }

//   if (inputString == "STOP")
//   {
//     Serial.println("DUNG");
//     Motor_M3.Stop();
//   }
// }

// void readSlave()
// {

//   memset(ui8_buffer_send_slave, 0xFF, 5);
//   ui8_buffer_send_slave[0] = 0x04;
//   RS485.send(ui8_buffer_send_slave, slave1);

//   vTaskDelay(100 / portTICK_PERIOD_MS);

//   // Nhận dữ liệu phản hồi từ Slave
//   uint8_t isReceived[100];
//   RS485.receive(ui8_buffer_receive_slave, slave1);

//   if (RS485.isReceived)
//   {
//     SerialDebug.print("Received data from Slave: ");
//     for (int i = 1; i <= ui8_buffer_receive_slave[0]; i++)
//     {
//       SerialDebug.print(ui8_buffer_receive_slave[i], HEX);
//       SerialDebug.print(" ");
//     }
//     SerialDebug.println();

//     RS485.isReceived = false;
//   }
//   else if (RS485.incorrectCRC)
//   {
//     SerialDebug.println("Incorrect CRC received.");
//     RS485.incorrectCRC = false;
//   }
//   else
//   {
//     SerialDebug.println("No response from Slave.");
//   }
// }
#endif

#define GUILENH(biendieukien, key, value, sendFunction) \
  if (biendieukien == 1)                                \
  {                                                     \
    ui8_function = key;                                 \
    ui8_status = value;                                 \
    sendFunction;                                       \
    biendieukien = 0;                                   \
    return;                                             \
  }

void guilenhlenPC()
{
  if (SerialPC.timeSend + THOIGIANCHOGUILENH <= millis())
  {
    GUILENH(ui2_respond_confirmReceived, ui8_buffer_receive_PC[1], 0x00, confirmReceived(ui8_function));
    GUILENH(ui2_respond_incorrectCRC, ui8_buffer_receive_PC[1], 0x01, respondIncorrectCommand(ui8_function, ui8_status));
    GUILENH(ui2_respond_function_notExist, ui8_buffer_receive_PC[1], 0x02, respondIncorrectCommand(ui8_function, ui8_status));
    // GUILENH(CuaTrai.change, CuaTrai.Pin, CuaTrai.state, sendSensorState(ui8_function, ui8_status));
    // GUILENH(CuaPhai.change, CuaPhai.Pin, CuaPhai.state, sendSensorState(ui8_function, ui8_status));
    // GUILENH(CuaTren.change, CuaTren.Pin, CuaTren.state, sendSensorState(ui8_function, ui8_status));
    GUILENH(ui2_respond_firmware, 0xFB, 0x00, sendFirmware(ui8_function));
  }
}

void processSerial()
{
  if (SerialPC.incorrectCRC == true)
  {
    ui2_respond_incorrectCRC = true;
    SerialPC.incorrectCRC = false;
    return;
  }
  if (SerialPC.isReceived == true)
  {
    SerialPC.isReceived = false;
    memcpy(order_command, (ui8_buffer_receive_PC + 2), 2);
    if (order_command[0] == last_order_command[0] && order_command[1] == last_order_command[1])
    {
      // ui2_respond_repeatedCommand = 1;
      // return;
    }

    switch (ui8_buffer_receive_PC[1])
    {
    case code::begin:
    {
      memcpy(last_order_command, order_command, 2);
      break;
    }

    case code::check_firmWare:
    {
      memcpy(last_order_command, order_command, 2);

      if (ui8_buffer_receive_PC[4] == 0x00)
        ui2_respond_firmware = true;
      else
      {
        ui2_respond_function_notExist = 1; // Nếu không nhận được lệnh hợp lệ
        break;
      }
      break;
    }

    case code::reset:
    {
      memcpy(last_order_command, order_command, 2);

      if (ui8_buffer_receive_PC[0] - 4 == 2 && // Check size
          ui8_buffer_receive_PC[5] == 0x00 &&
          ui8_buffer_receive_PC[6] == 0x00)
        ;
      else
      {
        ui2_respond_function_notExist = 1; // Nếu không nhận được lệnh hợp lệ
        break;
      }
      break;
    }

    case code::erase_error:
    {
      memcpy(last_order_command, order_command, 2);

      if (ui8_buffer_receive_PC[0] - 4 == 2 && // Check size
          ui8_buffer_receive_PC[5] == 0x00 &&
          ui8_buffer_receive_PC[6] == 0x01)
        ;
      else
      {
        ui2_respond_function_notExist = 1; // Nếu không nhận được lệnh hợp lệ
        break;
      }
      break;
    }

    case code::admin_control_motor:
    {
      memcpy(last_order_command, order_command, 2);
      if (ui8_buffer_receive_PC[0] - 4 != 2 || // Check size
          (ui8_buffer_receive_PC[6] != TrangThaiMotor::STOP &&
           ui8_buffer_receive_PC[6] != TrangThaiMotor::FORWARD &&
           ui8_buffer_receive_PC[6] != TrangThaiMotor::REVERSE))
      {
        ui2_respond_function_notExist = 1; // Nếu không nhận được lệnh hợp lệ
        break;
      }
      else
        switch (ui8_buffer_receive_PC[5])
        {
        case admin_control::CUA:
        {
          MotorCua.YeuCau = ui8_buffer_receive_PC[6];
          SerialDebug.print("Cua: ");
          SerialDebug.println(MotorCua.YeuCau);

          break;
        }
        case admin_control::BANGTAI:
        {
          MotorBangTai.YeuCau = ui8_buffer_receive_PC[6];
          SerialDebug.print("Bang tai: ");
          SerialDebug.println(MotorBangTai.YeuCau);
          break;
        }
        case admin_control::NANGBANGTAI:
        {
          MotorNangBangTai.YeuCau = ui8_buffer_receive_PC[6];
          break;
        }
        case admin_control::XOAYCHAI:
        {
          MotorXoayChai.YeuCau = ui8_buffer_receive_PC[6];
          break;
        }
        case admin_control::PHANLOAI1:
        {
          admin_control_RS485(ui8_buffer_receive_PC[5], ui8_buffer_receive_PC[6]);
          break;
        }
        case admin_control::PHANLOAI2:
        {
          admin_control_RS485(ui8_buffer_receive_PC[5], ui8_buffer_receive_PC[6]);
          break;
        }
        case admin_control::EPLON:
        {
          admin_control_RS485(ui8_buffer_receive_PC[5], ui8_buffer_receive_PC[6]);
          break;
        }
        case admin_control::NGHIENCHAI:
        {
          admin_control_RS485(ui8_buffer_receive_PC[5], ui8_buffer_receive_PC[6]);
          break;
        }
        default:
          ui2_respond_function_notExist = 1;
          break;
        }
    }

    case code::detect_result:
    {
      memcpy(last_order_command, order_command, 2);
      if (ui8_buffer_receive_PC[0] - 4 != 1 || // Check size
          (ui8_buffer_receive_PC[5] != detect_result::NOTHING &&
           ui8_buffer_receive_PC[5] != detect_result::BOTTLE &&
           ui8_buffer_receive_PC[5] != detect_result::CAN &&
           ui8_buffer_receive_PC[5] != detect_result::OTHER))
      {
        ui2_respond_function_notExist = 1; // Nếu không nhận được lệnh hợp lệ
        break;
      }
      else
        ui8_ketquanhandien = ui8_buffer_receive_PC[5];
    }
      /*****************Default************************/

    default:
      ui2_respond_function_notExist = 1; // Nếu không nhận được lệnh hợp lệ
      break;
    }

    if (ui2_respond_function_notExist == 0 && ui2_funtion_self_respond == 0) // Nếu nhận được lệnh hợp lệ
    {
      ui2_respond_confirmReceived = 1;
    }
  }
}

/**********************Gửi phản hồi đã nhận được dữ liệu lên PC**************************/
void confirmReceived(uint8_t function_code)
{
  uint8_t bytecount = 1;
  uint8_t datasize = bytecount + 4;
  uint8_t data[datasize + 1] = {datasize,
                                function_code,
                                ui8_buffer_receive_PC[2],
                                ui8_buffer_receive_PC[3],
                                (ui8_buffer_receive_PC[0] - 4) > 0 ? (ui8_buffer_receive_PC[0] - 4) : 0,
                                0x00};
  memcpy(ui8_buffer_send_PC, data, datasize + 1);
  SerialPC.send(ui8_buffer_send_PC);
}

/**********************Gửi phản hồi nhận được lệnh sai lên PC**************************/
void respondIncorrectCommand(uint8_t function_code, uint8_t status)
{
  uint8_t bytecount = 2;
  uint8_t datasize = bytecount + 4;
  uint8_t data[datasize + 1] = {datasize,
                                code::respond_incorrect_command,
                                ui8_buffer_receive_PC[2],
                                ui8_buffer_receive_PC[3],
                                ((ui8_buffer_receive_PC[0] - 4) > 0 ? (ui8_buffer_receive_PC[0] - 4) : 0),
                                function_code,
                                status};
  memcpy(ui8_buffer_send_PC, data, datasize + 1);
  SerialPC.send(ui8_buffer_send_PC);
}

/**********************Gửi trạng thái cảm biến************************/
void sendSensorState(uint8_t sensor, uint8_t state)
{
  uint8_t bytecount = 2;
  uint8_t datasize = bytecount + 4;
  raiseOrderCommand(self_order_command);
  uint8_t data[datasize + 1] = {datasize,
                                code::send_sensor_state,
                                self_order_command[0],
                                self_order_command[1],
                                bytecount,
                                mapping_sensor(sensor),
                                mapping_state(state)};
  memcpy(ui8_buffer_send_PC, data, datasize + 1);
  SerialPC.send(ui8_buffer_send_PC);
}

/**********************Gửi firmware************************/
void sendFirmware(uint8_t function_code)
{
  uint8_t bytecount = 3;
  uint8_t datasize = bytecount + 4;
  uint8_t data[datasize + 1] = {datasize,
                                code::check_firmWare,
                                ui8_buffer_receive_PC[2],
                                ui8_buffer_receive_PC[3],
                                bytecount,
                                firmware[0],
                                firmware[1],
                                firmware[2]};
  memcpy(ui8_buffer_send_PC, data, datasize + 1);
  SerialPC.send(ui8_buffer_send_PC);
}

void raiseOrderCommand(uint8_t *order)
{
  (*(order + 1))++;
  *(order + 0) = (*(order + 1) == 0x00) ? ((*(order + 0)) + 1) : (*(order + 0));
}

#define TATMOTOR(bienyeucauOFF, bientrangthaiON, bienTimeON, timeout, turnOffFunction) \
  if (bientrangthaiON == 1 && millis() - bienTimeON > timeout)                         \
  {                                                                                    \
    turnOffFunction;                                                                   \
    bientrangthaiON = 0;                                                               \
    bienyeucauOFF = 0;                                                                 \
    return;                                                                            \
  }                                                                                    \
  else if (bienyeucauOFF == 1)                                                         \
  {                                                                                    \
    turnOffFunction;                                                                   \
    bienyeucauOFF = 0;                                                                 \
    return;                                                                            \
  }

void setLEDRGB(uint16_t R, uint16_t G, uint16_t B)
{
  pwm_PortLED.setPWM(RE, 0, R);
  pwm_PortLED.setPWM(GR, 0, G);
  pwm_PortLED.setPWM(BL, 0, B);
}
void ERASE_ERROR_EEPROM()
{
  EEPROM.write(EEPROM_ADD::ADD_ERROR_EPLON, 0);
  EEPROM.write(EEPROM_ADD::ADD_ERROR_NGHIENCHAI, 0);
  EEPROM.write(EEPROM_ADD::ADD_ERROR_KETCUA, 0);
  EEPROM.write(EEPROM_ADD::ADD_ERROR_KETPHANLOAI1, 0);
  EEPROM.write(EEPROM_ADD::ADD_ERROR_KETPHANLOAI2, 0);
  EEPROM.commit();
}

void GET_EEPROM()
{
  ui8_error_EpLon = EEPROM.read(EEPROM_ADD::ADD_ERROR_EPLON);
  ui8_error_NghienChai = EEPROM.read(EEPROM_ADD::ADD_ERROR_NGHIENCHAI);
  ui8_error_KetCua = EEPROM.read(EEPROM_ADD::ADD_ERROR_KETCUA);
  ui8_error_KetPhanLoai1 = EEPROM.read(EEPROM_ADD::ADD_ERROR_KETPHANLOAI1);
  ui8_error_KetPhanLoai2 = EEPROM.read(EEPROM_ADD::ADD_ERROR_KETPHANLOAI2);
}

void debugEEPROM()
{
  SerialDebug.print("LOI EPLON: ");
  SerialDebug.println(EEPROM.read(EEPROM_ADD::ADD_ERROR_EPLON));
  SerialDebug.print("LOI NGHIEN CHAI: ");
  SerialDebug.println(EEPROM.read(EEPROM_ADD::ADD_ERROR_NGHIENCHAI));
  SerialDebug.print("LOI KET CUA: ");
  SerialDebug.println(EEPROM.read(EEPROM_ADD::ADD_ERROR_KETCUA));
  SerialDebug.print("LOI KET PHAN LOAI 1: ");
  SerialDebug.println(EEPROM.read(EEPROM_ADD::ADD_ERROR_KETPHANLOAI1));
  SerialDebug.print("LOI KET PHAN LOAI 2: ");
  SerialDebug.println(EEPROM.read(EEPROM_ADD::ADD_ERROR_KETPHANLOAI2));
}
void LEDRED()
{
  setLEDRGB(4095, 4096, 4096);
}
void LEDGREEN()
{

  setLEDRGB(4096, 4095, 4096);
}
void LEDBLUE()
{
  setLEDRGB(4096, 4096, 4095);
}
void LEDYELLOW()
{
  setLEDRGB(4095, 3211, 4096);
}
void LEDPURPLE()
{
  setLEDRGB(4095, 4096, 4095);
}
void LEDORANGE()
{
  setLEDRGB(4095, 100, 4096);
}
void LEDWHITE()
{
  setLEDRGB(4095, 4095, 4095);
}
void LEDLOIBANGTAI()
{
  setLEDRGB(4095, 4095, 4095);
}
void TATLED()
{
  setLEDRGB(4096, 4096, 4096);
}

void blink(int time)
{
  if (millis() - ui32_time_blink > time)
  {
    ui32_time_blink = millis();
    led_status = !led_status;
    digitalWrite(LED_ONBOARD, led_status);
  }
}

void checkEncoder()
{
  if (MotorNghienChai.TrangThai == TrangThaiMotorNghien::STOP)
  {
    return;
  }
  if (CamBien2.state != Encoder.state)
  {
    Encoder.state = CamBien2.state;
    Encoder.lastTime = millis();
  }

  if (millis() - Encoder.lastTime > 2000)
  {
    Encoder.trangThaiLoi = TrangThaiLoi::COLOI;
  }
}

void admin_control_RS485(uint8_t Motor, uint8_t YeuCau)
{
  uint8_t bytecount = 2;
  uint8_t datasize = bytecount + 4;
  raiseOrderCommand(self_order_command);
  uint8_t data[datasize + 1] = {datasize,
                                codeSlaveRS485::admin_control_motor,
                                self_order_command[0],
                                self_order_command[1],
                                bytecount,
                                Motor,
                                YeuCau};
  memcpy(ui8_buffer_send_slave, data, datasize + 1);
  RS485.send(ui8_buffer_send_slave, slave1);
}

void check_error_slave_RS485()
{
  uint8_t bytecount = 1;
  uint8_t datasize = bytecount + 4;
  raiseOrderCommand(self_order_command);
  uint8_t data[datasize + 1] = {datasize,
                                codeSlaveRS485::check_error,
                                self_order_command[0],
                                self_order_command[1],
                                bytecount,
                                0x00};
  memcpy(ui8_buffer_send_slave, data, datasize + 1);
  RS485.send(ui8_buffer_send_slave, slave1);
}

void erase_error_slave_RS485()
{
  uint8_t bytecount = 1;
  uint8_t datasize = bytecount + 4;
  raiseOrderCommand(self_order_command);
  uint8_t data[datasize + 1] = {datasize,
                                codeSlaveRS485::erase_error,
                                self_order_command[0],
                                self_order_command[1],
                                bytecount,
                                0x00};
  memcpy(ui8_buffer_send_slave, data, datasize + 1);
  RS485.send(ui8_buffer_send_slave, slave1);
}

void check_firmware_slave_RS485()
{
  uint8_t bytecount = 1;
  uint8_t datasize = bytecount + 4;
  raiseOrderCommand(self_order_command);
  uint8_t data[datasize + 1] = {datasize,
                                codeSlaveRS485::check_firmware,
                                self_order_command[0],
                                self_order_command[1],
                                bytecount,
                                0x00};
  memcpy(ui8_buffer_send_slave, data, datasize + 1);
  RS485.send(ui8_buffer_send_slave, slave1);
}

void control_BoPhanLoai(uint8_t Vitri)
{
  uint8_t bytecount = 1;
  uint8_t datasize = bytecount + 4;
  raiseOrderCommand(self_order_command);
  uint8_t data[datasize + 1] = {datasize,
                                codeSlaveRS485::PHANLOAI,
                                self_order_command[0],
                                self_order_command[1],
                                bytecount,
                                Vitri};
  memcpy(ui8_buffer_send_slave, data, datasize + 1);
  RS485.send(ui8_buffer_send_slave, slave1);
}

void control_BoEp(uint8_t Chieuchay)
{
  uint8_t bytecount = 1;
  uint8_t datasize = bytecount + 4;
  raiseOrderCommand(self_order_command);
  uint8_t data[datasize + 1] = {datasize,
                                codeSlaveRS485::EPLON,
                                self_order_command[0],
                                self_order_command[1],
                                bytecount,
                                Chieuchay};
  memcpy(ui8_buffer_send_slave, data, datasize + 1);
  RS485.send(ui8_buffer_send_slave, slave1);
}

void control_BoNghien(uint8_t Chieuchay)
{
  uint8_t bytecount = 1;
  uint8_t datasize = bytecount + 4;
  raiseOrderCommand(self_order_command);
  uint8_t data[datasize + 1] = {datasize,
                                codeSlaveRS485::NGHIENCHAI,
                                self_order_command[0],
                                self_order_command[1],
                                bytecount,
                                Chieuchay};
  memcpy(ui8_buffer_send_slave, data, datasize + 1);
  RS485.send(ui8_buffer_send_slave, slave1);
}

void test_send_slave_RS485()
{

  admin_control_RS485(4, 1);
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  check_error_slave_RS485();
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  erase_error_slave_RS485();
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  check_firmware_slave_RS485();
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  control_BoPhanLoai(2);
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  control_BoEp(0);
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  control_BoNghien(0);
  vTaskDelay(2000 / portTICK_PERIOD_MS);
}
void loop()
{
}