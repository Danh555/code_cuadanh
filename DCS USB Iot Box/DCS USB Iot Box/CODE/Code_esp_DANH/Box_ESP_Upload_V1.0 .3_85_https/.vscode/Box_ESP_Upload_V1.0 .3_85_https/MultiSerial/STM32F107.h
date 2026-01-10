#include "usb/usb_host.h"
#include "Hardware_STM32F107.h"

// void setup()
// {
//   harware_int();
//   SD_int();
//   //handleFlash();
//    listDir(SPIFFS, "/", 0);
//    Serial.println("Chọn Barcode:                   SEL_QR");
//    Serial.println("Chọn Modem 4G:                  SEL_4G");
//    Serial.println("Chọn chương trình:              FILE");
//    Serial.println("INIT STM32F1:                   INIT_CHIP");
//    Serial.println("Xóa bộ nhớ STM32F1:             STM32F1Erase");
//    Serial.println("Tải xuống trương trình STM32F1: STM32F1Flash");
// }
uint32_t timer = 0;
int stt = 0;
// int rdtmp_test;
bool NameFile=false;
uint8_t ui8_check_upload = 0;
uint8_t ui8_check_kiemtraupcode = 0;

void napcode()
{
  if (timer < millis())
  {
    if (stt)
    {
      stt = 0;
      timer = millis() + 1000;
    } else
    {
      stt = 1;
      timer = millis() + 50;
    }
    digitalWrite(LED_STT, stt);
  }
  if (SerialQR4G.available())
  {
    SerialDebug.write(SerialQR4G.read());
  }

  int i = 0;
  bool printer = false;
  String text = "";
  while (SerialDebug.available())
  {
    printer = true;
    //SerialQR4G.write(SerialDebug.read());
    char a = SerialDebug.read();
    text = text + a;
  }

  
  if (printer)
  {
    if(NameFile)
    {
       FileBin=text;
       Serial.println("file bin upload: "+FileBin);
       NameFile=false;
    }
    if (text == "SEL_QR")//--------------------------SEL_QR------------------
    {
      SerialQR4G.begin(115200, SERIAL_8N1, 5, 4);
      digitalWrite(SEL_QR_4G, HIGH);
      digitalWrite(RST_F107, LOW);
      delay(50);
      digitalWrite(Boot_F107, LOW);
      delay(50);
      digitalWrite(RST_F107, HIGH);
      Serial.println("OK SEL_QR_4G = HIGH");
    }
    else if(text == "HELP")
    {
       Serial.println("Chọn Barcode:                   SEL_QR");
       Serial.println("Chọn Modem 4G:                  SEL_4G");
       Serial.println("Chọn chương trình:              FILE");
       Serial.println("INIT STM32F1:                   INIT_CHIP");
       Serial.println("Xóa bộ nhớ STM32F1:             STM32F1Erase");
       Serial.println("Tải xuống trương trình STM32F1: STM32F1Flash");
    }
    else if (text == "SEL_4G")//--------------------------SEL_4G------------------
    {
      SerialQR4G.begin(115200, SERIAL_8N1, 5, 4);
      digitalWrite(SEL_QR_4G, LOW);
      Serial.println("OK SEL_QR_4G = LOW");
      SerialQR4G.println("AT+CPIN?");
    }
    else if (text == "INIT_CHIP")//--------------------------INIT_CHIP-------------
    {
      SerialQR4G.begin(115200, SERIAL_8E1, 5, 4);
      delay(100);
      digitalWrite(SEL_QR_4G, HIGH);
      FlashMode_test();
      delay(200);
      init_chip_test();
    }
    else if (text == "STM32F1Erase")//-----------------------STM32F1Erase------------phai init truoc
    {
      //      digitalWrite(SEL_QR_4G, HIGH);
      //      FlashMode();
      //      delay(200);
      check_stmErase_test();
    }
    else if (text == "STM32F1Flash")//-----------------------STM32F1Flash------------phai Erase truoc
    {
      SerialQR4G.begin(115200, SERIAL_8E1, 5, 4);
      delay(100);
      digitalWrite(SEL_QR_4G, HIGH);
      FlashMode_test();
      delay(200);
      init_chip_test();
      delay(500);
      check_stmErase_test();
      handleFlash_test();
    }else if (text == "FILE")
    {
      NameFile=true;
      // listDir(SPIFFS, "/", 0); 
      Serial.println("Nhập tên file");   
    }
    
    Serial.println("Echo: "+text);
  }
  
}

void upload_code_STM32barcode()
{
  SerialQR4G.begin(115200, SERIAL_8E1, 5, 4);
  delay(100);
  digitalWrite(SEL_QR_4G, HIGH);
  // FlashMode_test();
  // delay(200);
  // init_chip_test();
  // delay(500);
  check_stmErase_test();
  handleFlash_test();
  ui8_check_upload=1;
  
}

void init_chip_STM32()
{
  SerialQR4G.begin(115200, SERIAL_8E1, 5, 4);
  delay(100);
  digitalWrite(SEL_QR_4G, HIGH);
  FlashMode_test();
  delay(200);
  init_chip_test();

}

void init_chip_STM32_printer()
{
  SerialQR4G.begin(115200, SERIAL_8E1, 18, 17);
  delay(100);
  digitalWrite(SEL_QR_4G, HIGH);
  FlashMode_STM32_printer();
  delay(200);
  init_chip_test();
}

void reset_chip()
{
  SerialQR4G.begin(115200, SERIAL_8N1, 5, 4);
  digitalWrite(SEL_QR_4G, HIGH);
  digitalWrite(RST_F107, LOW);
  delay(50);
  digitalWrite(Boot_F107, LOW);
  delay(50);
  digitalWrite(RST_F107, HIGH);
  Serial.println("OK SEL_QR_4G = HIGH");
}

void upload_code_STM32_printer()
{
  SerialQR4G.begin(115200, SERIAL_8E1, 18, 17);
  delay(100);
  digitalWrite(SEL_QR_4G, HIGH);
  // RunMode_STM32_printer();
  // FlashMode_STM32_printer();
  // delay(200);
  // init_chip_test();
  // delay(500);
  check_stmErase_test();
  handleFlash_test();
  ui8_check_upload=1;
   
}

void resetchip_printer()
{
  SerialQR4G.begin(115200, SERIAL_8E1, 18, 17);
  digitalWrite(SEL_QR_4G, HIGH);
  digitalWrite(RST_F205, LOW);
  delay(50);
  digitalWrite(Boot_F205, LOW);
  delay(50);
  digitalWrite(RST_F205, HIGH);
  Serial.println("OK SEL_QR_4G = HIGH");
}