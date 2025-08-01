/*
  SD card datalogger

  Modified to work with mySD and ESP32/ESP8266

  This example shows how to log data from three analog sensors
  to an SD card using the SD library.

  The circuit:
   analog sensors on analog ins 0, 1, and 2
   SD card attached to SPI bus as follows:
	const int MY_CS = 13;
	const int MY_MISO = 2;
	const int MY_MOSI = 15;
	const int MY_CLOCK = 14;

  created  24 Nov 2010
  modified 9 Apr 2012
  by Tom Igoe
  modified 25 May 2020
  by Stefan Fambach http://www.fambach.net

  This example code is in the public domain.

*/
#include <mySD.h>

// set up variables using the SD utility library functions:
File root;

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


  if (!SD.begin(MY_CS, MY_MOSI,MY_MISO, MY_CLOCK)) {  
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  /* open "test.txt" for writing */
  root = SD.open(fileName, FILE_WRITE);

  if (root) {
    Serial.println("Open Success");
    root.flush();
    root.close();
  } else {
    /* if the file open error, print an error */
    Serial.println("Open Failed");
  }
}


void loop() {
  // make a string for assembling the data to log:
  String dataString = "";

  // read three sensors and append to the string:
  for (int analogPin = 0; analogPin < 3; analogPin++) {
    int sensor = analogRead(analogPin);
    dataString += String(sensor);
    if (analogPin < 2) {
      dataString += ",";
    }
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(fileName, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.flush();
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}








