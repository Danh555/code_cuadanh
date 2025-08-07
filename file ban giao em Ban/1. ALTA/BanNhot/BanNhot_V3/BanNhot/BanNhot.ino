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

  // processSensor();

  UART_PC.receive(ui8_data_receive);
  processSerial();
  // if (ui8_mode == AUTO)
  // {
  //   setLEDPixelAuto();
  // }
  if (ui8_mode == CONTROLRGB)
  {
    setLEDPixelControlRGB();
  }
  // else if (ui8_mode == CONTROLCOLOR)
  // {
  //   setLEDPixelControlColor();
  // }
  /**********************Đèn báo hoạt động**************************/
  denbao(1000);
  tatcoibao();

  // if (ui2_reset == 1)
  // {
  //   ui2_reset = 0;
  //   RESET();
  // }
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
    // order_command[0] = ui8_data_receive[2];
    // order_command[1] = ui8_data_receive[3];

    if (order_command[0] == last_order_command[0] && order_command[1] == last_order_command[1])
    {
      ui8_respond_repeatedCommand = 1;
      return;
    }

    switch (ui8_data_receive[1])
    {
      /*****************NHẬN TRẠNG THÁI VÒI NHỚT******************/
    case 0x04:
      memcpy(last_order_command, order_command, 2);

      // ---- Trạng thái vòi hút nhớt
      if (ui8_data_receive[5] == 0x00 && ui8_data_receive[6] == 0x00)
        ThungNhotThai.trangthai_voi_nhantuPC = DANGBAN;
      else if (ui8_data_receive[5] == 0x00 && ui8_data_receive[6] == 0xff)
        ThungNhotThai.trangthai_voi_nhantuPC = NGUNGBAN;
      else if (ui8_data_receive[5] == 0x00 && ui8_data_receive[6] == 0x01)
        ThungNhotThai.trangthai_voi_nhantuPC = TRANGTHAICHO; // Trạng thái chờ
      else
      {
        ui2_respond_function_notExist = 1; // Nếu không nhận được lệnh hợp lệ
        break;
      }
      if (ui2_respond_function_notExist == 0) // Nếu nhận được lệnh hợp lệ
      {
        // ui8_mode = AUTO;
        ui2_respond_confirmReceived = 1;
      }
      break;

    /*****************NHẬN VỊ TRÍ VÒI BÁN NHỚT******************/
    case 0X07:
      memcpy(last_order_command, order_command, 2);

      // ---- Trạng thái vòi nhớt 1
      if (ui8_data_receive[5] == 0x01 && ui8_data_receive[6] == 0xff)
        ThungNhot1.trangthai_voi_nhantuPC = DANGBAN; // Bắt đầu bán
      else if (ui8_data_receive[5] == 0x01 && ui8_data_receive[6] == 0x00)
        ThungNhot1.trangthai_voi_nhantuPC = NGUNGBAN; // Ngừng bán
      else if (ui8_data_receive[5] == 0x01 && ui8_data_receive[6] == 0x01)
        ThungNhot1.trangthai_voi_nhantuPC = TRANGTHAICHO; // Trạng thái chờ

      // ----Trạng thái vòi nhớt 2
      else if (ui8_data_receive[5] == 0x02 && ui8_data_receive[6] == 0xff)
        ThungNhot2.trangthai_voi_nhantuPC = DANGBAN; // Bắt đầu bán
      else if (ui8_data_receive[5] == 0x02 && ui8_data_receive[6] == 0x00)
        ThungNhot2.trangthai_voi_nhantuPC = NGUNGBAN; // Ngừng bán
      else if (ui8_data_receive[5] == 0x02 && ui8_data_receive[6] == 0x01)
        ThungNhot2.trangthai_voi_nhantuPC = TRANGTHAICHO; // Trạng thái chờ
      else
      {
        ui2_respond_function_notExist = 1; // Nếu không nhận được lệnh hợp lệ
        break;
      }
      if (ui2_respond_function_notExist == 0) // Nếu nhận được lệnh hợp lệ
      {
        ui8_mode = AUTO;
        ui2_respond_confirmReceived = 1;
      }
      break;

    /*****************NHẬN TRẠNG THÁI THÙNG PHI******************/
    case 0X11:
      memcpy(last_order_command, order_command, 2);

      //----- Trạng thái thùng phi 1
      if (ui8_data_receive[5] == 0x01 && ui8_data_receive[6] == 0xff)
        ThungNhot1.trangthai_thung_nhantuPC = DAY; //  Đầy
      else if (ui8_data_receive[5] == 0x01 && ui8_data_receive[6] == 0x00)
        ThungNhot1.trangthai_thung_nhantuPC = CAN; //  Cạn

      //----- Trạng thái thùng phi 2
      else if (ui8_data_receive[5] == 0x02 && ui8_data_receive[6] == 0xff)
        ThungNhot2.trangthai_thung_nhantuPC = DAY; //  Đầy
      else if (ui8_data_receive[5] == 0x02 && ui8_data_receive[6] == 0x00)
        ThungNhot2.trangthai_thung_nhantuPC = CAN; //  Cạn

      //----- Trạng thái thùng phi thải
      else if (ui8_data_receive[5] == 0x03 && ui8_data_receive[6] == 0xff)
        ThungNhotThai.trangthai_thung_nhantuPC = DAY; //  Đầy
      else if (ui8_data_receive[5] == 0x03 && ui8_data_receive[6] == 0x00)
        ThungNhotThai.trangthai_thung_nhantuPC = CAN; //  Cạn
      else
      {
        ui2_respond_function_notExist = 1; // Nếu không nhận được lệnh hợp lệ
        break;
      }
      if (ui2_respond_function_notExist == 0) // Nếu nhận được lệnh hợp lệ
      {
        ui8_mode = AUTO;
        ui2_respond_confirmReceived = 1;
      }
      break;

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
        // ui8_mode = CONTROLRGB;
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
        ui8_mode = CONTROLRGB;
        ui2_respond_confirmReceived = 1;
      }
      break;

      /*****************Mode Cotrol Color**************************/
    case 0x0B:
      memcpy(last_order_command, order_command, 2);

      if (ui8_data_receive[0] != 0x07) // Nếu nhận size khác size mặc định
      {
        ui2_respond_function_notExist = 1; // Nếu không nhận được lệnh hợp lệ
        return;
      }

      //----- Điều khiển LED thùng phi thải
      if (ui8_data_receive[5] == 0x00)
      {
        processSerial_ModeControlColor(&ThungNhotThai);
      }
      //----- Điều khiển LED thùng phi 1
      else if (ui8_data_receive[5] == 0x01)
      {
        processSerial_ModeControlColor(&ThungNhot1);
      }
      //----- Điều khiển LED thùng phi 2
      else if (ui8_data_receive[5] == 0x02)
      {
        processSerial_ModeControlColor(&ThungNhot2);
      }

      else
      {
        ui2_respond_function_notExist = 1; // Nếu không nhận được lệnh hợp lệ
        break;
      }

      if (ui2_respond_function_notExist == 0) // Nếu nhận được lệnh hợp lệ
      {
        ui8_mode = CONTROLCOLOR;
        ui2_respond_confirmReceived = 1;
      }
      break;

      /*****************Default************************/
    default:
      ui2_respond_function_notExist = 1; // Nếu không nhận được lệnh hợp lệ
      break;
    }
    processState();
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

void processSerial_ModeControlColor(ThungNhot *thungnhot)
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
  (*thungnhot).colorIndex = ui8_data_receive[7];
  (*thungnhot).colorStaticReadySet = 1;
}

/*****************Xử lý nhận trạng thái******************/

void processState()
{
  processState(&ThungNhot1);
  processState(&ThungNhot2);
  processState(&ThungNhotThai);
}
void processState(ThungNhot *thungnhot)
{
  //----- Nếu trạng thái thùng nhận từ PC đầy hoặc cạn --> Trạng thái thùng là trạng thái nhận từ PC
  if ((*thungnhot).trangthai_thung_nhantuPC == DAY || (*thungnhot).trangthai_thung_nhantuPC == CAN)
  {
    (*thungnhot).trangthai_thung = (*thungnhot).trangthai_thung_nhantuPC;
    (*thungnhot).trangthai_thung_nhantuPC = NO_STATE;
  }
  //----- Nếu trạng thái vòi nhận từ PC là đang bán -->Trạng thái thùng là đang hoạt động
  if ((*thungnhot).trangthai_voi_nhantuPC == DANGBAN)
  {
    (*thungnhot).trangthai_thung = DANGHOATDONG;
    (*thungnhot).trangthai_voi_nhantuPC = NO_STATE;
  }
  //----- Nếu trạng thái vòi nhận từ PC trạng thái chờ
  if ((*thungnhot).trangthai_voi_nhantuPC == TRANGTHAICHO)
  {
    if ((*thungnhot).trangthai_thung == DANGHOATDONG) // Nếu trạng thái thùng trước đó là đang hoạt động thì chuyển thành đang chờ hoàn tất
    {
      (*thungnhot).trangthai_thung = DANGCHO;
      (*thungnhot).trangthai_voi_nhantuPC = NO_STATE;
    }
    else
      (*thungnhot).trangthai_voi_nhantuPC = NO_STATE;
  }
  //----- Nếu trạng thái vòi nhận từ PC là ngừng bán
  if ((*thungnhot).trangthai_voi_nhantuPC == NGUNGBAN)
  {
    if ((*thungnhot).trangthai_thung == DANGHOATDONG) // Nếu trạng thái thùng trước đó là đang hoạt động thì chuyển về bình thường
    {
      (*thungnhot).trangthai_thung = BINHTHUONG;
      (*thungnhot).trangthai_voi_nhantuPC = NO_STATE;
    }
    else if ((*thungnhot).trangthai_thung == DANGCHO) // Nếu trạng thái thùng trước đó là đang chờ hoàn tất thì chuyển về bình thường
    {
      (*thungnhot).trangthai_thung = BINHTHUONG;
      (*thungnhot).trangthai_voi_nhantuPC = NO_STATE;
    }
    else // Nếu trạng thái thùng trước đó là đầy hoặc cạn thì giữ nguyên
      (*thungnhot).trangthai_voi_nhantuPC = NO_STATE;
  }
}

/**********************Vòng for set màu cho cả Strip************************/
void setOneColorAllStrip(ThungNhot *thungnhot, uint8_t r, uint8_t g, uint8_t b)
{

  for (int i = 0; i < NUMPIXELS; i++)
  {
    (*thungnhot).LED_strip.setPixelColor(i, r, g, b);
  }
}

/**********************Set LED Pixel Auto************************/
void setLEDPixelAuto()
{
  setLEDPixelAuto(&ThungNhot1);
  setLEDPixelAuto(&ThungNhot2);
  setLEDPixelAuto(&ThungNhotThai);
}
void setLEDPixelAuto(ThungNhot *thungnhot)
{
  //----- Nếu trạng thái thay đổi mới set lại màu
  if ((*thungnhot).trangthai_LED != (*thungnhot).trangthai_thung)
  {
    (*thungnhot).trangthai_LED = (*thungnhot).trangthai_thung;

    if ((*thungnhot).trangthai_LED == BINHTHUONG)
      setOneColorAllStrip(thungnhot, 0, 0, 255); // Nếu vòi bình thường thì xanh dương
    else if ((*thungnhot).trangthai_LED == DANGHOATDONG)
      setOneColorAllStrip(thungnhot, 0, 255, 0); // Nếu vòi đang bơm thì xanh lá
    else if ((*thungnhot).trangthai_LED == DANGCHO)
      setOneColorAllStrip(thungnhot, 255, 200, 0); // Nếu vòi đang ở trạng thái chờ thì màu vàng
    else if ((*thungnhot).trangthai_LED == DAY)
    {
      if (thungnhot == &ThungNhotThai)
        setOneColorAllStrip(thungnhot, 255, 50, 0); // Nếu là thùng thải đầy thì báo cam
      else
        setOneColorAllStrip(thungnhot, 255, 255, 255); // Nếu là thùng nhớt bán thì báo trắng
    }
    else if ((*thungnhot).trangthai_LED == CAN)
    {
      if (thungnhot == &ThungNhotThai)
        setOneColorAllStrip(thungnhot, 255, 255, 255); // Nếu là thùng thải cạn thì báo trắng
      else
        setOneColorAllStrip(thungnhot, 255, 50, 0); // Nếu là thùng nhớt bán cạn thì báo tím
    }

    (*thungnhot).LED_strip.show(); // Show LED
  }

  //----- Hiệu ứng LED khi vòi đang bơm (nháy xanh lá)
  else if ((*thungnhot).trangthai_LED == DANGHOATDONG)
  {
    if (millis() > (*thungnhot).time_hieuung)
    {
      (*thungnhot).time_hieuung = millis() + timedelay_hieuung;
      (*thungnhot).state_hieuung = !(*thungnhot).state_hieuung;

      if ((*thungnhot).state_hieuung == 0)
        setOneColorAllStrip(thungnhot, 0, 255, 0);
      else if ((*thungnhot).state_hieuung == 1)
        setOneColorAllStrip(thungnhot, 0, 0, 0);
      (*thungnhot).LED_strip.show();
    }
  }
  //----- Hiệu ứng LED khi vòi đang ở trạng thái chờ (nháy vàng)
  else if ((*thungnhot).trangthai_LED == DANGCHO)
  {
    if (millis() > (*thungnhot).time_hieuung)
    {
      (*thungnhot).time_hieuung = millis() + timedelay_hieuung;
      (*thungnhot).state_hieuung = !(*thungnhot).state_hieuung;

      if ((*thungnhot).state_hieuung == 0)
        setOneColorAllStrip(thungnhot, 255, 200, 0);
      else if ((*thungnhot).state_hieuung == 1)
        setOneColorAllStrip(thungnhot, 0, 0, 0);
      (*thungnhot).LED_strip.show();
    }
  }
}
/**********************Set LED Pixel Control RGB**************************/
void setLEDPixelControlRGB()
{
  // for (ThungNhot &thungnhot : ThungNhotArray)
  // {
  //   setLEDPixel(&thungnhot);
  // }

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

/**********************Set LED Pixel Control Color************************/
void setLEDPixelControlColor()
{
  setLEDPixelControlColor(&ThungNhot1);
  setLEDPixelControlColor(&ThungNhot2);
  setLEDPixelControlColor(&ThungNhotThai);
}
void setLEDPixelControlColor(ThungNhot *thungnhot)
{
  if ((*thungnhot).colorMode == STATIC && (*thungnhot).colorStaticReadySet == 1)
  {
    (*thungnhot).colorStaticReadySet = 0;
    setColor(thungnhot);
    (*thungnhot).LED_strip.show();
  }
  else if ((*thungnhot).colorMode == BLINK)
  {
    if (millis() > (*thungnhot).time_hieuung)
    {
      (*thungnhot).time_hieuung = millis() + timedelay_hieuung;
      (*thungnhot).state_hieuung = !(*thungnhot).state_hieuung;

      if ((*thungnhot).state_hieuung == 0)
        setColor(thungnhot);
      else if ((*thungnhot).state_hieuung == 1)
        setOneColorAllStrip(thungnhot, 0, 0, 0);
      (*thungnhot).LED_strip.show();
    }
  }
}

void setColor(ThungNhot *thungnhot)
{
  if ((*thungnhot).colorIndex == 0x00)
    setOneColorAllStrip(thungnhot, 0x00, 0x00, 0x00); // Tắt đèn
  else if ((*thungnhot).colorIndex == 0x01)
    setOneColorAllStrip(thungnhot, 0xff, 0xff, 0xff); // Trắng
  else if ((*thungnhot).colorIndex == 0x02)
    setOneColorAllStrip(thungnhot, 0xff, 0x00, 0x00); // Đỏ
  else if ((*thungnhot).colorIndex == 0x03)
    setOneColorAllStrip(thungnhot, 0x00, 0xff, 0x00); // Xanh lá
  else if ((*thungnhot).colorIndex == 0x04)
    setOneColorAllStrip(thungnhot, 0x00, 0x00, 0xff); // Xanh dương
  else if ((*thungnhot).colorIndex == 0x05)
    setOneColorAllStrip(thungnhot, 0xff, 0xff, 0x00); // Vàng
  else if ((*thungnhot).colorIndex == 0x06)
    setOneColorAllStrip(thungnhot, 0xff, 0x00, 0xff); // Tím
  else if ((*thungnhot).colorIndex == 0x07)
    setOneColorAllStrip(thungnhot, 0xff, 0x32, 0x00); // Cam
}
/**********************RESET trạng thái thùng************************/
void RESET()
{

  ui8_mode = CONTROLRGB;
  for (ThungNhot *thungnhot : ThungNhotArray)
  {
    (*thungnhot).clearState();
  }

  // for (Sensor *sensor : SensorArray)
  // {
  //   (*sensor).clearState();
  // }

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
