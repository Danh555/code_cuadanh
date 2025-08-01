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
String stringtmp_test;
int rdtmp_test;
uint8_t check_upcode = 0;

String STM32_CHIPNAME_TEST[47] = {
  "Unknown Chip",
  "STM32F030x8/05x",
  "STM32F03xx4/6",
  "STM32F030xC",
  "STM32F04xxx/070x6",
  //"STM32F070x6",
  "STM32F070xB/071xx/072xx",
  //"STM32F071xx/072xx",
  "STM32F09xxx",
  "STM32F10xxx-LD",
  "STM32F10xxx-MD",
  "STM32F10xxx-HD",
  "STM32F10xxx-MD-VL",
  "STM32F10xxx-HD-VL",
  "STM32F105/107",
  "STM32F10xxx-XL-D",
  "STM32F20xxxx",
  "STM32F373xx/378xx",
  //"STM32F378xx",
  "STM32F302xB(C)/303xB(C)/358xx",
  //"STM32F358xx",
  "STM32F301xx/302x4(6/8)/318xx",
  //"STM32F318xx",
  "STM32F303x4(6/8)/334xx/328xx",
  "STM32F302xD(E)/303xD(E)/398xx",
  //"STM32F398xx",
  "STM32F40xxx/41xxx",
  "STM32F42xxx/43xxx",
  "STM32F401xB(C)",
  "STM32F401xD(E)",
  "STM32F410xx",
  "STM32F411xx",
  "STM32F412xx",
  "STM32F446xx",
  "STM32F469xx/479xx",
  "STM32F413xx/423xx",
  "STM32F72xxx/73xxx",
  "STM32F74xxx/75xxx",
  "STM32F76xxx/77xxx",
  "STM32H74xxx/75xxx",
  "STM32L01xxx/02xxx",
  "STM32L031xx/041xx",
  "STM32L05xxx/06xxx",
  "STM32L07xxx/08xxx",
  "STM32L1xxx6(8/B)",
  "STM32L1xxx6(8/B)A",
  "STM32L1xxxC",
  "STM32L1xxxD",
  "STM32L1xxxE",
  "STM32L43xxx/44xxx",
  "STM32L45xxx/46xxx",
  "STM32L47xxx/48xxx",
  "STM32L496xx/4A6xx"
};

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
  int res =0;
	
	for(int i =0;i<3;i++)
	{
		Serial1.write(STM32INIT);		
		uint32_t u32tam = millis() + 4000;
		while (!Serial1.available())
		{
			if(millis() >u32tam)
			{
				SerialDebug.println("chip no respone timeout");				
				break;
			}
		}

		if(millis() < u32tam)
		{
			SerialDebug.println("chip has respone");			
			i=10;
			break;
		}

	}
	/*init chip*/
	stringtmp_test = "ERROR";
		
	while (Serial1.available())
	{
		rdtmp_test = Serial1.read();

		if (rdtmp_test == STM32ACK)
		{
			stringtmp_test = STM32_CHIPNAME_TEST[stm32GetId()];
			res =1;
			SerialDebug.print("stringtmp = ");
      SerialDebug.println(stringtmp_test);
	  	SerialDebug.println("da nhan chip");
		}		
	}
}
void check_stmErase_test()
{
    if (stm32Erase() == STM32ACK)
    {
      Serial.println("Okay STM32 Erase");
    }   
    else if(stm32Erasen() == STM32ACK)
    {
      Serial.println("Okay STM32 Erase"); 
    }
    else
    {
      Serial.println("Erase Failed"); 
    }

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
    // reset_chip();
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

void FlashMode_STM32_printer()
{
  digitalWrite(Boot_F205, HIGH);
  delay(100);
  digitalWrite(RST_F205, LOW);
  // digitalWrite(LED, LOW);
  delay(50);
  digitalWrite(RST_F205, HIGH);
  delay(200);
}

void RunMode_STM32_printer()  
{ 
  digitalWrite(Boot_F205, LOW);
  delay(100);
  digitalWrite(RST_F205, LOW);
  delay(50);
  digitalWrite(RST_F205, HIGH);
  delay(1000);
}

#endif
