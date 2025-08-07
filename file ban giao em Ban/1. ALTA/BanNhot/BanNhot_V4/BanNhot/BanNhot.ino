#include <Arduino.h>
#include "BanNhot.h"

void setup()
{
  UART_PC.begin(115200);

  ThungNhot1.setupLED();
  ThungNhot2.setupLED();
  ThungNhotThai.setupLED();

  // pinMode(S0, INPUT_PULLUP);
  pinMode(S1, INPUT_PULLUP);
  pinMode(S2, INPUT_PULLUP);
  pinMode(S3, INPUT_PULLUP);

  pinMode(BUZZER, OUTPUT);
  pinMode(LED_BLINK, OUTPUT);

  ui32_currentTime = millis();

  RESET();
}

/**********************Loop************************/
void loop()
{
  /**********************Main Funtion**************************/
  guilenhlenPC();
  readSensor();

  UART_PC.receive(ui8_data_receive);
  processSerial();

  setLEDPixelControlRGB();

  /**********************Đèn báo hoạt động**************************/
  denbao(1000);
  tatcoibao();
}

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
  if (UART_PC.timeSend + THOIGIANCHOGUILENH <= millis())
  {
    GUILENH(ui2_respond_confirmReceived, ui8_data_receive[1], 0x00, confirmReceived(ui8_function));
    GUILENH(ui2_respond_incorrectCRC, ui8_data_receive[1], 0x01, respondIncorrectCommand(ui8_function, ui8_status));
    GUILENH(ui2_respond_function_notExist, ui8_data_receive[1], 0x02, respondIncorrectCommand(ui8_function, ui8_status));
    GUILENH(CuaTrai.change, CuaTrai.Pin, CuaTrai.state, sendSensorState(ui8_function, ui8_status));
    GUILENH(CuaPhai.change, CuaPhai.Pin, CuaPhai.state, sendSensorState(ui8_function, ui8_status));
    GUILENH(CuaTren.change, CuaTren.Pin, CuaTren.state, sendSensorState(ui8_function, ui8_status));
    GUILENH(ui2_respond_firmware, 0xFB, 0x00, sendFirmware(ui8_function));
  }
}

/**********************Gửi phản hồi đã nhận được dữ liệu lên PC**************************/
void confirmReceived(uint8_t function_code)
{
  uint8_t bytecount = 1;
  uint8_t datasize = bytecount + 4;
  uint8_t data[datasize + 1] = {datasize, function_code, ui8_data_receive[2], ui8_data_receive[3], ((ui8_data_receive[0] - 4) > 0 ? (ui8_data_receive[0] - 4) : 0), 0x00};
  memcpy(ui8_data_send, data, datasize + 1);
  UART_PC.send(ui8_data_send);
}

/**********************Gửi phản hồi nhận được lệnh sai lên PC**************************/
void respondIncorrectCommand(uint8_t function_code, uint8_t status)
{
  uint8_t bytecount = 2;
  uint8_t datasize = bytecount + 4;
  uint8_t data[datasize + 1] = {datasize, 0xFE, ui8_data_receive[2], ui8_data_receive[3], ((ui8_data_receive[0] - 4) > 0 ? (ui8_data_receive[0] - 4) : 0), function_code, status};
  memcpy(ui8_data_send, data, datasize + 1);
  UART_PC.send(ui8_data_send);
}

/**********************Gửi trạng thái cảm biến************************/
void sendSensorState(uint8_t sensor, uint8_t state)
{
  uint8_t bytecount = 2;
  uint8_t datasize = bytecount + 4;
  raiseOrderCommand(self_order_command);
  uint8_t data[datasize + 1] = {datasize, 0X08, self_order_command[0], self_order_command[1], bytecount, mapping_sensor(sensor), mapping_state(state)};
  memcpy(ui8_data_send, data, datasize + 1);
  UART_PC.send(ui8_data_send);
}

/**********************Gửi firmware************************/
void sendFirmware(uint8_t function_code)
{
  uint8_t bytecount = 4;
  uint8_t datasize = bytecount + 4;
  uint8_t data[datasize + 1] = {datasize, function_code, ui8_data_receive[2], ui8_data_receive[3], bytecount, 0x00, 0x00, 0x00, 0x00};
  *(data + 5) = firmware >> 8;
  *(data + 6) = firmware & 0x00FF;
  *(data + 7) = 0x00;
  *(data + 8) = 0x01;
  memcpy(ui8_data_send, data, datasize + 1);
  UART_PC.send(ui8_data_send);
}

/**********************Đọc cảm biến************************/
void readSensor()
{
  CuaTrai.readSensorDebounce(10);
  CuaPhai.readSensorDebounce(10);
  CuaTren.readSensorDebounce(10);
}

/**********************Xử lý tín hiệu Serial************************/
void processSerial()
{
  if (UART_PC.incorrectCRC == true)
  {
    ui2_respond_incorrectCRC = true;
    UART_PC.incorrectCRC = false;
    return;
  }
  if (UART_PC.dataReceived == true)
  {
    UART_PC.dataReceived = false;
    memcpy(order_command, (ui8_data_receive + 2), 2);
    if (order_command[0] == last_order_command[0] && order_command[1] == last_order_command[1])
    {
      ui8_respond_repeatedCommand = 1;
      return;
    }

    switch (ui8_data_receive[1])
    {

    /*****************RESET******************/
    case 0x10:
      memcpy(last_order_command, order_command, 2);

      if (ui8_data_receive[5] == 0x00 && ui8_data_receive[6] == 0x00)
      {
        RESET();
      }
      else
      {
        ui2_respond_function_notExist = 1; // Nếu không nhận được lệnh hợp lệ
        break;
      }
      if (ui2_respond_function_notExist == 0) // Nếu nhận được lệnh hợp lệ
      {
        ui2_respond_confirmReceived = 1;
      }
      break;

    /*****************FIRMWARE************************/
    case 0xFB:
      memcpy(last_order_command, order_command, 2);

      if (ui8_data_receive[4] == 0x00)
        ui2_respond_firmware = true;
      else
      {
        ui2_respond_function_notExist = 1; // Nếu không nhận được lệnh hợp lệ
        break;
      }
      break;

    /*****************Mode Cotrol RGB************************/
    case 0x0A:
      memcpy(last_order_command, order_command, 2);

      if (ui8_data_receive[0] != 0x09) // Nếu nhận size khác size mặc định
      {
        ui2_respond_function_notExist = 1; // Nếu không nhận được lệnh hợp lệ
        return;
      }

      //----- Điều khiển LED thùng phi thải
      if (ui8_data_receive[5] == 0x00)
      {
        processSerial_ModeControlRGB(&ThungNhotThai);
      }
      //----- Điều khiển LED thùng phi 1
      else if (ui8_data_receive[5] == 0x01)
      {
        processSerial_ModeControlRGB(&ThungNhot1);
      }
      //----- Điều khiển LED thùng phi 2
      else if (ui8_data_receive[5] == 0x02)
      {
        processSerial_ModeControlRGB(&ThungNhot2);
      }
      else
      {
        ui2_respond_function_notExist = 1; // Nếu không nhận được lệnh hợp lệ
        break;
      }

      if (ui2_respond_function_notExist == 0) // Nếu nhận được lệnh hợp lệ
      {
        ui2_respond_confirmReceived = 1;
      }
      break;

      /*****************Default************************/
    default:
      ui2_respond_function_notExist = 1; // Nếu không nhận được lệnh hợp lệ
      break;
    }
  }
}
void processSerial_ModeControlRGB(ThungNhot *thungnhot)
{
  if (ui8_data_receive[6] == 0x00) // Điều khiển đèn tĩnh
    (*thungnhot).colorMode = STATIC;
  else if (ui8_data_receive[6] == 0x01) // Điều khiển đèn nhấp nháy
    (*thungnhot).colorMode = BLINK;
  else
  {
    ui2_respond_function_notExist = 1; // Nếu không nhận được lệnh hợp lệ
    return;
  }
  (*thungnhot).RED = ui8_data_receive[7];   // Nhận giá trị red 0x00 - 0xFF
  (*thungnhot).GREEN = ui8_data_receive[8]; // Nhận giá trị green 0x00 - 0xFF
  (*thungnhot).BLUE = ui8_data_receive[9];  // Nhận giá trị blue 0x00 - 0xFF
  (*thungnhot).colorStaticReadySet = 1;
}

/**********************Vòng for set màu cho cả Strip************************/
void setOneColorAllStrip(ThungNhot *thungnhot, uint8_t r, uint8_t g, uint8_t b)
{

  for (int i = 0; i < NUMPIXELS; i++)
  {
    (*thungnhot).LED_strip.setPixelColor(i, r, g, b);
  }
}

/**********************Set LED Pixel Control RGB**************************/
void setLEDPixelControlRGB()
{
  setLEDPixelControlRGB(&ThungNhot1);
  setLEDPixelControlRGB(&ThungNhot2);
  setLEDPixelControlRGB(&ThungNhotThai);
}
void setLEDPixelControlRGB(ThungNhot *thungnhot)
{
  if ((*thungnhot).colorMode == STATIC && (*thungnhot).colorStaticReadySet == 1)
  {
    (*thungnhot).colorStaticReadySet = 0;
    setOneColorAllStrip(thungnhot, (*thungnhot).RED, (*thungnhot).GREEN, (*thungnhot).BLUE);
    (*thungnhot).LED_strip.show();
  }
  else if ((*thungnhot).colorMode == BLINK)
  {
    if (millis() > (*thungnhot).time_hieuung)
    {
      (*thungnhot).time_hieuung = millis() + timedelay_hieuung;
      (*thungnhot).state_hieuung = !(*thungnhot).state_hieuung;

      if ((*thungnhot).state_hieuung == 0)
        setOneColorAllStrip(thungnhot, (*thungnhot).RED, (*thungnhot).GREEN, (*thungnhot).BLUE);
      else if ((*thungnhot).state_hieuung == 1)
        setOneColorAllStrip(thungnhot, 0, 0, 0);
      (*thungnhot).LED_strip.show();
    }
  }
}

/**********************RESET trạng thái thùng************************/
void RESET()
{
  for (ThungNhot *thungnhot : ThungNhotArray)
  {
    (*thungnhot).clearState();
  }

  uint8_t temp_arr[2] = {0x00, 0x00};
  memcpy(order_command, temp_arr, 2);
  memcpy(self_order_command, temp_arr, 2);
  memcpy(last_order_command, temp_arr, 2);

  setOneColorAllStrip(&ThungNhot1, 0, 0, 255);
  setOneColorAllStrip(&ThungNhot2, 0, 0, 255);
  setOneColorAllStrip(&ThungNhotThai, 0, 0, 255);
}

void testLED()
{
  for (int i = 0; i < NUMPIXELS; i++)
  {
    ThungNhot1.LED_strip.setPixelColor(i, 255, 255, 255, BRIGHTNESS);
    ThungNhot2.LED_strip.setPixelColor(i, 255, 255, 255, BRIGHTNESS);
    ThungNhotThai.LED_strip.setPixelColor(i, 255, 255, 255, BRIGHTNESS);
  }
  ThungNhot1.LED_strip.show();
  ThungNhot2.LED_strip.show();
  ThungNhotThai.LED_strip.show();
}

/**********************Đèn onboard báo hoạt động (blink ms)**************************/
void denbao(int time)
{
  if (millis() - ui32_time_blink > time)
  {
    ui32_time_blink = millis();
    led_status = !led_status;
    digitalWrite(LED_BLINK, led_status);
  }
}

/**********************Set thời gian còi onboard hoạt động (ms)**************************/
void batcoibao(int time)
{
  digitalWrite(BUZZER, HIGH);
  timeout_buz = millis() + time;
  buz_status = 1;
}

/**********************Tắt còi nếu thời gian vượt qua thời gian đã Set**************************/
void tatcoibao()
{
  if (buz_status == 1 && millis() >= timeout_buz)
  {
    digitalWrite(BUZZER, LOW);
    buz_status = 0;
  }
}
