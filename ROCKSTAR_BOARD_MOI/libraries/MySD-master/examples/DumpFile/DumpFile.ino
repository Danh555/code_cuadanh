/*
  SD card file dump

  Modified to work with mySD and ESP32/ESP8266
  
  This example shows how to read a file from the SD card using the
  SD library and send it over the serial port.

  The circuit:
   SD card attached to SPI bus as follows:
  const int MY_CS = 13;
  const int MY_MISO = 2;
  const int MY_MOSI = 15;
  const int MY_CLOCK = 14;

  created  22 December 2010
  by Limor Fried
  modified 9 Apr 2012
  by Tom Igoe
  modified 25 May 2020
  by Stefan Fambach http://www.fambach.net

  This example code is in the public domain.

*/

#include <mySD.h>

// set up variables using the SD utility library functions:

const int MY_CS = 13;
const int MY_MISO = 2;
const int MY_MOSI = 15;
const int MY_CLOCK = 14;

const char * fileName = "datalog.txt";

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");
  if (!SD.begin(MY_CS, MY_MOSI,MY_MISO, MY_CLOCK)) { 
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(fileName);

  // if the file is available, read it and post the content to the serial port
  if (dataFile) {
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}

void loop() {
}
