/*
  SD card basic file example

  Modified to work with mySD and ESP32/ESP8266

  This example shows how to create and destroy an SD card file
  The circuit:
   SD card attached to SPI bus as follows:
const int MY_CS = 13;
const int MY_MISO = 2;
const int MY_MOSI = 15;
const int MY_CLOCK = 14;

  created   Nov 2010
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe
  modified 25 May 2020
  by Stefan Fambach http://www.fambach.net

  This example code is in the public domain.

*/
#include <mySD.h>

// set up variables using the SD utility library functions:
File myFile;

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

  if (SD.exists("example.txt")) {
    Serial.println("example.txt exists.");
  } else {
    Serial.println("example.txt doesn't exist.");
  }

  // open a new file and immediately close it:
  Serial.println("Creating example.txt...");
  myFile = SD.open("example.txt", FILE_WRITE);
  myFile.close();

  // Check to see if the file exists:
  if (SD.exists("example.txt")) {
    Serial.println("example.txt exists.");
  } else {
    Serial.println("example.txt doesn't exist.");
  }

  // delete the file:
  Serial.println("Removing example.txt...");
  SD.remove("example.txt");

  if (SD.exists("example.txt")) {
    Serial.println("example.txt exists.");
  } else {
    Serial.println("example.txt doesn't exist.");
  }
}

void loop() {
  // nothing happens after setup finishes.
}


