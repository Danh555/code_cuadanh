/*
  Listfiles

  This example shows how print out the files in a
  directory on a SD card

  Modified to work with mySD and ESP32/ESP8266

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
  modified 2 Feb 2014
  by Scott Fitzgerald

  This example code is in the public domain.

*/
#include <mySD.h>

const int MY_CS = 13;
const int MY_MISO = 2;
const int MY_MOSI = 15;
const int MY_CLOCK = 14;

File root;

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

  root = SD.open("/");

  printDirectory(root, 0);

  Serial.println("done!");
}

void loop() {
  // nothing happens after setup finishes.
}

void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}


