
/*
 * File: Game_phat_nhac.ino
 * Author: HOANG QUOC BINH
 * Date: 15/04/2023
 * Description: This is file for Chanel Sound using ESP32
 */

#include <CRC16.h>

#define CB1 14 // Label SEN2
#define CB2 27 // Label SEN3
#define CB3 35 // Label SEN4
#define CB4 34 // Label SEN5
#define CB5 36 // Label SEN7

#ifndef LED_BUILTIN
#define LED_BUILTIN 2 // Thay đổi 13 bằng số chân của đèn LED trên bo mạch của bạn nếu cần
#endif

#define thoigianled_binhthuong 1000

//---------------Serial----------------//
uint16_t ui16_Byteindex = 0;
uint8_t ui8_data_send[15];
uint8_t ui8_data_rec[15];
uint16_t ui16_status = 0x00;
uint16_t ui16_function = 0x00;
uint16_t ui16_slaveID = 0x03;
bool ui2_BTcomplete = false;
bool ui2_BTreceive = false;
bool ui2_start = false;
uint16_t ui16_order_command, ui16_last_order_command = 0xFFFF;
uint8_t ui8_byteWeights[7];
uint32_t ui32_timechar;

//-------------Sensor-----------------//
#define socambien 5
int lastCBState[socambien];
unsigned long lastCBTime[socambien] = {0, 0, 0, 0, 0};

//-------------Global Var------------//
uint8_t ui8_trangthaiCB1, ui8_trangthaiCB2, ui8_trangthaiCB3, ui8_trangthaiCB4, ui8_trangthaiCB5;
uint8_t ui8_dathaydoitrangthai;
uint32_t ui32_timecho;
uint32_t thoigianled;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(CB1, INPUT_PULLUP);
  pinMode(CB2, INPUT_PULLUP);
  pinMode(CB3, INPUT_PULLUP);
  pinMode(CB4, INPUT_PULLUP);
  pinMode(CB5, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  // put your main code here, to run repeatedly:
  readSerial();
  docvaguitrangthai();
  batled_binhthuong();
}

void docvaguitrangthai()
{
  ui8_trangthaiCB1 = doccambien(CB1, 0);
  ui8_trangthaiCB2 = doccambien(CB2, 1);
  ui8_trangthaiCB3 = doccambien(CB3, 2);
  ui8_trangthaiCB4 = doccambien(CB4, 3);
  ui8_trangthaiCB5 = doccambien(CB5, 4);

  // int tong = tam1 + tam2 + tam3 + tam4 + tam5;
  if (ui8_dathaydoitrangthai == 1 && millis() - ui32_timecho > 50)
  {
    ui8_dathaydoitrangthai = 0;
    ui16_function = 0x04;
    send_trangthai();
  }
}

int doccambien(int CBpin, int vitri)
{
  // read the state of the switch into a local variable:
  int reading = digitalRead(CBpin);
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastCBState[vitri])
  {
    lastCBTime[vitri] = millis();
    ui8_dathaydoitrangthai = 1;
    ui32_timecho = millis();
  }

  if ((millis() - lastCBTime[vitri]) > 10)
  {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    if (reading == LOW)
    {
      return 1;
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastCBState[vitri] = reading;
  return 0;
}

void readSerial()
{
  if (Serial.available())
  {
    ui32_timechar = millis();
    while (Serial.available())
    {
      unsigned char inChar = (unsigned char)Serial.read();
      // Serial.print(inChar, HEX);
      if (!ui2_start)
      {
        ui2_BTreceive = true;
        ui2_start = true;
        ui16_Byteindex = 0;
      }
      if (ui2_start && (millis() - ui32_timechar) < 10)
      {
        ui8_data_rec[ui16_Byteindex] = inChar;
        ui16_Byteindex++;
        ui32_timechar = millis();
      }
    }
  }

  if (ui2_BTreceive && ((millis() - ui32_timechar) > 50))
  {
    ui2_BTreceive = false;
    ui2_BTcomplete = true;
  }
  if (ui2_BTcomplete == true)
  {
    if (ui16_Byteindex != 0 && ui8_data_rec[0] != ui16_slaveID)
    {
      ui16_status = 0x02;
      ui8_data_rec[0] = 0x02;
      send_data();
      ui16_Byteindex = 0;
      ui2_BTcomplete = false;
      ui2_start = false;
    }
    else if (ui16_Byteindex != 0 && ui8_data_rec[0] == ui16_slaveID)
    {
      process();
      ui16_Byteindex = 0;
      ui2_BTcomplete = false;
      ui2_start = false;
    }
  }
}

void process()
{
  unsigned char uc8_data[ui16_Byteindex - 2], uc8_checkcrc[2];
  unsigned char uc8_crchigh, uc8_crclow;
  unsigned int ui16_crc = 0;
  unsigned int ui16_du_lieu_data_nhan;
  for (int i = 0; i < ui16_Byteindex - 2; i++)
  {
    uc8_data[i] = ui8_data_rec[i];
  }
  uc8_checkcrc[0] = ui8_data_rec[ui16_Byteindex - 2];
  uc8_checkcrc[1] = ui8_data_rec[ui16_Byteindex - 1];
  ui16_crc = calcCRC16(ui16_Byteindex - 2, uc8_data);
  uc8_crclow = ui16_crc >> 8;
  uc8_crchigh = ui16_crc & 0x00FF;
  if (uc8_crchigh == uc8_checkcrc[1] && uc8_crclow == uc8_checkcrc[0])
  {
    ui16_crc = 0;
    ui16_order_command = uc8_data[2];
    ui16_order_command = (ui16_order_command << 8) | uc8_data[3];
    // ui16_du_lieu_data_nhan = uc8_data[5];
    // ui16_du_lieu_data_nhan = (ui16_du_lieu_data_nhan << 8);
    // ui16_du_lieu_data_nhan = ui16_du_lieu_data_nhan | uc8_data[6];
    if (ui16_order_command == ui16_last_order_command)
      return;
    switch (uc8_data[1])
    {
      // case 0x04:
      // {
      // ui16_last_order_command = ui16_order_command;

      // ui16_status = 0x00;
      // send_data();
      // break;
      // }

    case 0x10:
    {
      ui16_last_order_command = 0x0000;
      ui16_status = 0x00;
      send_data();
      break;
    }
    default:
    {
      Serial.write(0x01);
      Serial.write(0x02);
      Serial.write(0x03);
      Serial.write(0x04);
    }
    }
  }
  else
  {
    ui16_status = 0x01;
    send_data();
  }
}

void send_trangthai()
{
  ui8_data_send[0] = ui16_slaveID;
  ui8_data_send[1] = ui16_function;
  ui8_data_send[2] = ui16_last_order_command >> 8;
  ui8_data_send[3] = ui16_last_order_command & 0x00FF;
  ui8_data_send[4] = 0x05;
  ui8_data_send[5] = ui8_trangthaiCB1 == 1 ? 0xFF : 0x00;
  ui8_data_send[6] = ui8_trangthaiCB2 == 1 ? 0xFF : 0x00;
  ui8_data_send[7] = ui8_trangthaiCB3 == 1 ? 0xFF : 0x00;
  ui8_data_send[8] = ui8_trangthaiCB4 == 1 ? 0xFF : 0x00;
  ui8_data_send[9] = ui8_trangthaiCB5 == 1 ? 0xFF : 0x00;

  int ui16_crc_pro = calcCRC16(10, ui8_data_send);
  ui8_data_send[10] = ui16_crc_pro >> 8;
  ui8_data_send[11] = ui16_crc_pro & 0x00FF;
  for (int k = 0; k < 12; k++)
  {
    Serial.write(ui8_data_send[k]);
  }
}

void send_data()
{
  // ui8_data_send[0] = 0x03;
  // ui8_data_send[1] = ui16_function;
  // ui8_data_send[2] = 0x00;
  // ui8_data_send[3] = 0x01;
  // ui8_data_send[4] = 0x01;
  for (int i = 0; i < 5; i++)
  {
    ui8_data_send[i] = ui8_data_rec[i];
  }
  ui8_data_send[5] = ui16_status;
  int ui16_crc_pro = calcCRC16(6, ui8_data_send);
  ui8_data_send[6] = ui16_crc_pro >> 8;
  ui8_data_send[7] = ui16_crc_pro & 0x00FF;
  for (int k = 0; k < 8; k++)
  {
    Serial.write(ui8_data_send[k]);
  }
}
void batled_binhthuong()
{
  int tam = digitalRead(LED_BUILTIN);
  if (millis() - thoigianled >= thoigianled_binhthuong)
  {
    digitalWrite(LED_BUILTIN, !tam);
    thoigianled = millis();
  }
}
