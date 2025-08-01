#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include "SPIFFS.h"
#include <EthernetLarge.h>
#include <SSLClient.h>
#include <SPI.h>
// #include "trust_anchors_github.h"
// #include "hardware_ESP.h"
// #define bin "DatMinhHo/OTA/main/Blink_Led.ino.esp32.bin"
// String host = "raw.githubusercontent.com"; // Host => bucket-name.s3.region.amazonaws.com
// int port = 443;

// const int rand_pin = A5;
// EthernetClient myclient;
// SSLClient client(myclient, TAs, (size_t)TAs_NUM_github, rand_pin);

const char* file_update_eth_esp32 = "/file_update_eth_esp32.bin";
const char* file_update_eth_stm32 = "/file_update_eth_stm32.bin";
const char* file_update_eth_STM32_BARCODE = "/file_update_eth_STM32_BARCODE.bin";

int printWebData = 1;
unsigned long byteCount = 0;
// unsigned long buffer[1024];
long contentLength = 0;
bool isValidContentType = false;
uint8_t ui8_update_fw_eth = 0;
uint8_t ui8_update_eth_esp32 = 0;
uint8_t ui8_update_eth_STM32 = 0;

String getHeaderValue(String header, String headerName) 
{
  return header.substring(strlen(headerName.c_str()));
}

void firmwareUpdate_Ethernet(SSLClient &client, String host, int port, String bin) 
{
  Serial.println("Connecting to: " + String(host));
  // Connect to S3
  if (client.connect(host.c_str(), port)) 
  {
    // Connection Succeed.
    // Fecthing the bin
    Serial.println("CONNECT THANH CONG");
    Serial.println("Fetching Bin: " + String(bin));

    // Get the contents of the bin file
    client.print(String("GET ") + bin + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Cache-Control: no-cache\r\n" +
                 "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) 
    {
      if (millis() - timeout > 5000) 
      {
        Serial.println("Client Timeout !");
        client.stop();
        return;
      }
    }
    int contentLength=-1;
    String payload="";
    while (client.connected()) 
    {
      String line = client.readStringUntil('\n');
      if (line.startsWith(F("Content-Length: ")))
      {
        contentLength = line.substring(15).toInt();
        Serial.print("contentLength: ");
        Serial.println(contentLength);
      }
      if (line == "\r") 
      {
        Serial.println("headers received");
        break;
      } // than tru 
    }
  }
}

void save_fileOTA_ETHERNET(SSLClient &client,  const char* CONFIG_FILE)
{
  // Serial.println("bat dau ghi file");
  File file = SPIFFS.open(CONFIG_FILE, "w");
  while(client.connected())
  {
    int len = client.available();
    if (len > 0) 
    {
      byte buffer[80];
      if (len > 80) len = 80;
      client.read(buffer, len);
      if (printWebData) 
      {
        file.write(buffer,len); // show in the serial monitor (slows some boards)
      }
      byteCount = byteCount + len;
    }
  }
  file.close();
  Serial.println("SAVE FILE SPIFFS DONE");  
  // client.stop();
}

// int FirmwareVersionCheck_ETH(void,SSLClient &client)
// {
//   if (client.connect(server, 443)) 
//   {
//     // Serial.print("connected to ");
//     // Make a HTTP request:
//     client.println("GET /Danh555/file_code/main/test.txt HTTP/1.1");
//     client.println("Host: raw.githubusercontent.com");
//     client.println("Connection: close");
//     client.println();
//   } 
//   else 
//   {
//     // if you didn't get a connection to the server:
//     Serial.println("connection failed");
//   }
// }

void listDir(const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = SPIFFS.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void readFile(const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = SPIFFS.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}
