#ifndef __Hardware_32W_4G_ETH__
#define __Hardware_32W_4G_ETH__
// ----------------------------include lib SD CARD-------------
#include <FS.h>
#include "SD.h"
#include "SPI.h"
#include <SPIFFS.h>

// ----------------------------define Serial-------------------
#define SerialQR4G Serial1
#define SerialDebug Serial
#define SEL_QR_4G 21
// ----------------------------define chip select--------------
#define CS_SD 14
#define CS_FL 9
#define CS_ETH 10
// ----------------------------define reset chip---------------
#define RST_ETH 7
#define RST_F205 15
#define RST_F107 39
#define RST_4G 47
#define PWR_4G 1
//-----------------------------define boot chip----------------
#define Boot_F107 40
#define Boot_F205 16
//
#define LED_STT 2

#include "stm32ota.h"
uint8_t binread_test[256];
int bini_test = 0;
uint32_t ui32_timeout_resetchip = 0;
String FileBin="file_update_wifi_stm32.bin";

void harware_int_test()
{
  SerialDebug.begin(115200);
  SerialQR4G.begin(115200, SERIAL_8N1, 5, 4);
  pinMode(LED_STT, OUTPUT);
  digitalWrite(LED_STT, HIGH);
  pinMode(RST_ETH, OUTPUT);
  digitalWrite(RST_ETH, HIGH);

  pinMode(RST_F205, OUTPUT);
  digitalWrite(RST_F205, HIGH);

  pinMode(RST_F107, OUTPUT);
  digitalWrite(RST_F107, HIGH);

  pinMode(RST_4G, OUTPUT);
  digitalWrite(RST_4G, HIGH);

  pinMode(PWR_4G, OUTPUT);
  digitalWrite(PWR_4G, HIGH);

  pinMode(Boot_F107, OUTPUT);
  digitalWrite(Boot_F107, LOW);
  //
  pinMode(Boot_F205, OUTPUT);
  digitalWrite(Boot_F205, LOW);

  pinMode(SEL_QR_4G, OUTPUT);
  digitalWrite(SEL_QR_4G, HIGH);
  SerialDebug.println("Hardware setup done");
}

void SD_int()
{
  if (!SPIFFS.begin(1)) {
    Serial.println("SPIFS Failed");
    return;
  }
//  uint8_t cardType = SD.cardType();

//  if (cardType == CARD_NONE) {
//    Serial.println("No SD card attached");
//    return;
//  }
//
//  Serial.print("SD Card Type: ");
//  if (cardType == CARD_MMC) {
//    Serial.println("MMC");
//  } else if (cardType == CARD_SD) {
//    Serial.println("SDSC");
//  } else if (cardType == CARD_SDHC) {
//    Serial.println("SDHC");
//  } else {
//    Serial.println("UNKNOWN");
//  }
//
//  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
//  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

void init_chip_test()
{
  Serial1.write(STM32INIT);
  while (!Serial1.available());
  Serial.println("\n[70] Check ACK");
  if (Serial1.read() == STM32ACK)
  {
    // Serial.println(STM32_CHIPNAME[stm32GetId()]);
  } else  {
    Serial.println( "ERROR");
  }
}
void check_stmErase_test()
{
  if (stm32Erase() == STM32ACK)
    Serial.println("Okay STM32 Erase");
  else if (stm32Erasen() == STM32ACK)
    Serial.println("Okay STM32 Erasen");
  else
    Serial.println("FALSE");
}
void handleFlash_test()
{
  String FileName, flashwr;
  int lastbuf = 0;
  uint8_t cflag, fnum = 256;
  //  File dir = SD.open("/");
  //  while (dir.openNextFile())
  //  {
  //    FileName = dir.name();
  //  }
  File fsUploadFile = SPIFFS.open("/"+FileBin, "r");
  
  if (fsUploadFile) {
    bini_test = fsUploadFile.size() / 256;
    lastbuf = fsUploadFile.size() % 256;
    flashwr = String(bini_test) + "-" + String(lastbuf) + "<br>";
    for (int i = 0; i < bini_test; i++) {
      fsUploadFile.read(binread_test, 256);
      Serial.print("FILE ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(binread_test[i], HEX);
      stm32SendCommand(STM32WR);
      while (!Serial1.available()) ;
      cflag = Serial1.read();
      if (cflag == STM32ACK)
        if (stm32Address(STM32STADDR + (256 * i)) == STM32ACK) {
          if (stm32SendData(binread_test, 255) == STM32ACK)
            flashwr += ".";
          else flashwr = "Error";
        }
    }
    fsUploadFile.read(binread_test, lastbuf);
    stm32SendCommand(STM32WR);
    while (!Serial1.available());
    cflag = Serial1.read();
    if (cflag == STM32ACK)
      if (stm32Address(STM32STADDR + (256 * bini_test)) == STM32ACK) {
        if (stm32SendData(binread_test, lastbuf) == STM32ACK)
          flashwr += "<br>Finished<br>";
        else flashwr = "Error";
      }
    //flashwr += String(binread_test[0]) + "," + String(binread_test[lastbuf - 1]) + "<br>";
    fsUploadFile.close();
    Serial.println(flashwr);
    ui32_timeout_resetchip = millis();
  }
}
void FlashMode_test()  {
  digitalWrite(Boot_F107, HIGH);
  delay(100);
  digitalWrite(RST_F107, LOW);
  delay(50);
  digitalWrite(RST_F107, HIGH);
  delay(1000);
}

void RunMode_test()  {
  digitalWrite(Boot_F107, LOW);
  delay(100);
  digitalWrite(RST_F107, LOW);
  delay(50);
  digitalWrite(RST_F107, HIGH);
  delay(1000);
}

#endif
