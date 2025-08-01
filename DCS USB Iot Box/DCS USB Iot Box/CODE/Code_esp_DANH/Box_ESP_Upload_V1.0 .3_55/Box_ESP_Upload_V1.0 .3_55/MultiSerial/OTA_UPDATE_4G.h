#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include "SPIFFS.h"
#include <EthernetLarge.h>
#include <SSLClient.h>
#include <SPI.h>
#include <Update.h>
#include <ArduinoJson.h>
// #include "trust_anchors_github.h"
// #include "hardware_ESP.h"
// #define bin "DatMinhHo/OTA/main/Blink_Led.ino.esp32.bin"
// String host = "raw.githubusercontent.com"; // Host => bucket-name.s3.region.amazonaws.com
// int port = 443;

// const int rand_pin = A5;
// EthernetClient myclient;
// SSLClient client(myclient, TAs, (size_t)TAs_NUM_github, rand_pin);

// const char* file_update_eth_esp32 = "/file_update_eth_esp32.bin";
// const char* file_update_eth_stm32 = "/file_update_eth_stm32.bin";

int printWebData_4g = 1;
unsigned long byteCount_4g = 0;
// unsigned long buffer[1024];
long contentLength_4g = 0;
bool isValidContentType_4g = false;
uint8_t ui8_update_fw_4G = 0;
uint8_t ui8_update_fw_4G_STM32 = 0;
uint8_t ui8_update_fw_4G_ESP32 = 0;
uint8_t ui8_update_fw_4G_STM32_barcode = 0;
char check_fw_printer[20];
char check_fw_upbarcode[20];
char check_fw_server[20];
String kt_fwversion="";
// String getHeaderValue(String header, String headerName) 
// {
//   return header.substring(strlen(headerName.c_str()));
// }

void firmwareUpdate_4G(SSLClient &client, String host, int port, String bin) 
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

void save_fileOTA_4G(SSLClient &client,  const char* CONFIG_FILE)
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
      if (printWebData_4g) 
      {
        file.write(buffer,len); // show in the serial monitor (slows some boards)
      }
      byteCount_4g = byteCount + len;
    }
  }
  file.close();
  Serial.println("SAVE FILE SPIFFS DONE");  
  // client.stop();
}

void update_ESP32_SPIFFS_4G(SSLClient &client,  const char* CONFIG_FILE)
{
  File file = SPIFFS.open(CONFIG_FILE, "r");
  // long giatri;
  // giatri=file.size();
  // Update.begin(giatri,U_FLASH);
  // Serial.println(giatri);
  uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
   if (!Update.begin(maxSketchSpace, U_FLASH)) { //start with max available size
    Update.printError(Serial);
    Serial.println("ERROR");
  }
  while (file.available()) 
  {
    uint8_t ibuffer[128];
    file.read((uint8_t *)ibuffer, 128);
    // Serial.println((char *)ibuffer);
    Update.write(ibuffer, sizeof(ibuffer));

  }
  Serial.print(Update.end(true));
  file.close();
  // ESP.restart();
  // if (Update.end())
  // {
  //   Serial.println("update esp32 done");
  //   
  // }
  
}

void update_ESP32_4G(SSLClient &myclient, String host, int port, String bin)
{
  Serial.println("Connecting to: " + String(host));
  // Connect to S3
  if (myclient.connect(host.c_str(), port)) {
    // Connection Succeed.
    // Fecthing the bin
    Serial.println("CONNECT THANH CONG");
    Serial.println("Fetching Bin: " + String(bin));

    // Get the contents of the bin file
    myclient.print(String("GET ") + bin + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Cache-Control: no-cache\r\n" +
                 "Connection: close\r\n\r\n");

    // Check what is being sent
      //  Serial.print(String("GET ") + bin + " HTTP/1.1\r\n" +
      //               "Host: " + host + "\r\n" +
      //               "Cache-Control: no-cache\r\n" +
      //               "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (myclient.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println("Client Timeout !");
        myclient.stop();
        return;
      }
    }
    // Once the response is available,
    // check stuff

    /*
       Response Structure
        HTTP/1.1 200 OK
        x-amz-id-2: NVKxnU1aIQMmpGKhSwpCBh8y2JPbak18QLIfE+OiUDOos+7UftZKjtCFqrwsGOZRN5Zee0jpTd0=
        x-amz-request-id: 2D56B47560B764EC
        Date: Wed, 14 Jun 2017 03:33:59 GMT
        Last-Modified: Fri, 02 Jun 2017 14:50:11 GMT
        ETag: "d2afebbaaebc38cd669ce36727152af9"
        Accept-Ranges: bytes
        Content-Type: application/octet-stream
        Content-Length: 357280
        Server: AmazonS3
                                   
        {{BIN FILE CONTENTS}}

    */
    while (myclient.available()) {
      // read line till /n
      String line = myclient.readStringUntil('\n');
      // remove space, to check if the line is end of headers
      line.trim();
      // Serial.println(line);

      if (!line.length()) {
        //headers ended
        break; // and get the OTA started
      }

      if (line.startsWith("HTTP/1.1")) {
        if (line.indexOf("200") < 0) {
          Serial.println("Got a non 200 status code from server. Exiting OTA Update.");
          break;
        }
      }

      if (line.startsWith("Content-Length: ")) {
        contentLength_4g = atol((getHeaderValue(line, "Content-Length: ")).c_str());
        Serial.println("Got " + String(contentLength_4g) + " bytes from server");
      }
  
      // Next, the content type
      if (line.startsWith("Content-Type: ")) {
        String contentType = getHeaderValue(line, "Content-Type: ");
        Serial.println("Got " + contentType + " payload.");
        if (contentType == "application/octet-stream") {
          isValidContentType_4g = true;
        }
      }
    }
  } else {

    Serial.println("Connection to " + String(host) + " failed. Please check your setup");

  }

  // Check what is the contentLength and if content type is `application/octet-stream`
  Serial.println("contentLength : " + String(contentLength_4g) + ", isValidContentType : " + String(isValidContentType_4g));

  // check contentLength and content type
  if (contentLength_4g && isValidContentType_4g) {
    // Check if there is enough to OTA Update
    bool canBegin = Update.begin(contentLength_4g);
    // If yes, begin
    if (canBegin) {
      Serial.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");
      // No activity would appear on the Serial monitor
      // So be patient. This may take 2 - 5mins to complete
      size_t written = Update.writeStream(myclient);
      // testFileIO(SPIFFS,"/calendar.text", int(written));
      if (written == contentLength_4g) {
        Serial.println("Written : " + String(written) + " successfully");
      } 
      else {
        Serial.println("Written only : " + String(written) + "/" + String(contentLength_4g) + ". Retry?" );
        // retry??
        // execOTA();
      }

      if (Update.end()) {
        Serial.println("OTA done!");
        if (Update.isFinished()) {
          Serial.println("Update successfully completed. Rebooting.");
          ESP.restart();
          return;
        } else {
          Serial.println("Update not finished? Something went wrong!");
        }
      } else {
        Serial.println("Error Occurred. Error #: " + String(Update.getError()));
      }
    }
     else {
      // not enough space to begin OTA
      // Understand the partitions and
      // space availability
      Serial.println("Not enough space to begin OTA");
      myclient.flush();
      return;
    }
  } else {
    Serial.println("There was no content in the response");
    myclient.flush();
  }
}

/*----------------HAM KIEM TRA VERSION SU DUNG 4G----------------------*/
int FirmwareVersionCheck_4G(SSLClient &mysclient_gsm, String host, int port, String bin, char *mynamedevice,const char *versionname, char *myprinter, char *mybarcode) 
{
  uint8_t ui8_check_ketnoiversion = 0;
  int kq =0;
  String payload="";
  if (mysclient_gsm.connect(host.c_str(), 443)) 
  {
  Serial.println("truy cap duoc vo github!!!");
  mysclient_gsm.print(String("GET ") + bin + " HTTP/1.1\r\n" +
                  "Host: " + host + "\r\n" +
                  "Cache-Control: no-cache\r\n" +
                  "Connection: close\r\n\r\n");

  while (mysclient_gsm.connected()) 
    {
      String line = mysclient_gsm.readStringUntil('\n');
      if (line.startsWith(F("Content-Length: ")))
      {
        int contentLength = line.substring(15).toInt();
    // Serial.println(contentLength);
      }
      if (line == "\r") {
        // Serial.println("headers received");
        break;
      } // than tru 
    }
  while (mysclient_gsm.available())
  {
    char c = mysclient_gsm.read();
    payload += c;
    // ui8_check_ketnoiversion=1;
  }
  // Serial.print("payload la: ");
  // Serial.println(payload);

  // Serial.println(payload);
    
  /* so sánh version to update*/
  /* 
  {"SVR": "1.0.3", "PRT": "1.0.4", "BCD": "1.0.1"}
  */
  StaticJsonDocument<1024> doc;
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, payload);
  if (error)
  {
    Serial.println(F("This is a JDaughter string, not a JSON string"));
    return 0;
  }	
  char chuoi[20];
  /* kiểm tra ID: đúng mới cập nhật */
  strlcpy(chuoi,                  // <- destination
  doc["ID"] | "erro",  // <- source
  sizeof(chuoi));         // <- destination's capacity
  // Serial.println("ID  =" + String(chuoi));

  if(String(chuoi)=="ALL")
  {
    Serial.printf("Allow update \n");
  }
  else if(String(chuoi)==String(mynamedevice))
  {
    Serial.printf("Allow update \n");
  }
  else
  {  
    Serial.printf("Not update \n");
    return 0;
  }


  /* kiểm tra update cho server */

  strlcpy(chuoi,                  // <- destination
  doc["SVR"] | "erro",  // <- source
  sizeof(chuoi));         // <- destination's capacity
  // Serial.println("Version wifi board =" + String(chuoi));

  if(String(chuoi)=="erro")
  {
    Serial.printf("Firmware: check erro\n");
  }
  else if(String(chuoi)==String(versionname))
  {
    Serial.printf("latest fw server:%s\n", chuoi);
    sprintf(check_fw_server,"%s",chuoi);
  }
  else
  {
    Serial.printf("new firmware server is %s\n",chuoi);
    sprintf(check_fw_server,"%s",chuoi);
    kq =1;
  }

  if(myprinter[2] == '1'  || kt_fwversion=="1")
  {
    strlcpy(chuoi,                  // <- destination
    doc["PRT"] | "erro",  // <- source
    sizeof(chuoi));         // <- destination's capacity
    // Serial.println("Version 4G LMD =" + String(chuoi));
    // Serial.println("Version device_status_printer" + String(device_status.vs_printer));
    // if ( String (device_status.vs_printer) != "NOT")
    // {	
    if(String(chuoi)=="erro")
    {
      Serial.printf("printer version erro check");
       sprintf(check_fw_printer,"%s",myprinter);
    }
    else if(String(chuoi)==String(myprinter))
    {
      Serial.printf("Firmware printer latest :%s\n", String(myprinter));
      sprintf(check_fw_printer,"%s",chuoi);
    }
    else
    {
      Serial.printf("new firmware printer is %s\n",chuoi);
      sprintf(check_fw_printer,"%s",chuoi);
      kq =2;
    }
  }


  else if(myprinter[2] == '2' || kt_fwversion=="2")
  {
    strlcpy(chuoi,                  // <- destination
    doc["PRT2"] | "erro",  // <- source
    sizeof(chuoi));         // <- destination's capacity
    // Serial.println("Version 4G LMD =" + String(chuoi));
    // Serial.println("Version device_status_printer" + String(device_status.vs_printer));
    // if ( String (device_status.vs_printer) != "NOT")
    // {	
    if(String(chuoi)=="erro")
    {
      Serial.printf("printer version erro check");
      sprintf(check_fw_printer,"%s",myprinter);
    }
    else if(String(chuoi)==String(myprinter))
    {
      Serial.printf("Firmware printer latest :%s\n", String(myprinter));
      sprintf(check_fw_printer,"%s",chuoi);
    }
    else
    {
      Serial.printf("new firmware printer is %s\n",chuoi);
      sprintf(check_fw_printer,"%s",chuoi);
      kq =2;
    } 
  }

  else if(myprinter[2] == '3'||kt_fwversion=="3")
  {
    strlcpy(chuoi,                  // <- destination
    doc["PRT3"] | "erro",  // <- source
    sizeof(chuoi));         // <- destination's capacity
    // Serial.println("Version 4G LMD =" + String(chuoi));
    // Serial.println("Version device_status_printer" + String(device_status.vs_printer));
    // if ( String (device_status.vs_printer) != "NOT")
    // {	
    if(String(chuoi)=="erro")
    {
      Serial.printf("printer version erro check");
      sprintf(check_fw_printer,"%s",myprinter);
    }
    else if(String(chuoi)==String(myprinter))
    {
      Serial.printf("Firmware printer latest :%s\n", String(myprinter));
      sprintf(check_fw_printer,"%s",chuoi);
    }
    else
    {
      Serial.printf("new firmware printer is %s\n",chuoi);
      sprintf(check_fw_printer,"%s",chuoi);
      kq =2;
    } 
  }

  else if(myprinter[2] == '4'||kt_fwversion=="4")
  {
    strlcpy(chuoi,                  // <- destination
    doc["PRT4"] | "erro",  // <- source
    sizeof(chuoi));         // <- destination's capacity
    // Serial.println("Version 4G LMD =" + String(chuoi));
    // Serial.println("Version device_status_printer" + String(device_status.vs_printer));
    // if ( String (device_status.vs_printer) != "NOT")
    // {	
    if(String(chuoi)=="erro")
    {
      Serial.printf("printer version erro check");
    }
    else if(String(chuoi)==String(myprinter))
    {
      Serial.printf("Firmware printer latest :%s\n", String(myprinter));
      sprintf(check_fw_printer,"%s",chuoi);
    }
    else
    {
      Serial.printf("new firmware printer is %s\n",chuoi);
      sprintf(check_fw_printer,"%s",chuoi);
      kq =2;
    } 
  }

  else if(myprinter[2] == '5'||kt_fwversion=="5")
  {
    strlcpy(chuoi,                  // <- destination
    doc["PRT5"] | "erro",  // <- source
    sizeof(chuoi));         // <- destination's capacity
    // Serial.println("Version 4G LMD =" + String(chuoi));
    // Serial.println("Version device_status_printer" + String(device_status.vs_printer));
    // if ( String (device_status.vs_printer) != "NOT")
    // {	
    if(String(chuoi)=="erro")
    {
      Serial.printf("printer version erro check");
    }
    else if(String(chuoi)==String(myprinter))
    {
      Serial.printf("Firmware printer latest :%s\n", String(myprinter));
      sprintf(check_fw_printer,"%s",chuoi);
    }
    else
    {
      Serial.printf("new firmware printer is %s\n",chuoi);
      sprintf(check_fw_printer,"%s",chuoi);
      kq =2;
    } 
  }

  else if(myprinter[2] == '6'||kt_fwversion=="6")
  {
    strlcpy(chuoi,                  // <- destination
    doc["PRT6"] | "erro",  // <- source
    sizeof(chuoi));         // <- destination's capacity
    // Serial.println("Version 4G LMD =" + String(chuoi));
    // Serial.println("Version device_status_printer" + String(device_status.vs_printer));
    // if ( String (device_status.vs_printer) != "NOT")
    // {	
    if(String(chuoi)=="erro")
    {
      Serial.printf("printer version erro check");
    }
    else if(String(chuoi)==String(myprinter))
    {
      Serial.printf("Firmware printer latest :%s\n", String(myprinter));
      sprintf(check_fw_printer,"%s",chuoi);
    }
    else
    {
      Serial.printf("new firmware printer is %s\n",chuoi);
      sprintf(check_fw_printer,"%s",chuoi);
      kq =2;
    } 
  }

  strlcpy(chuoi,                  // <- destination
  doc["BCD"] | "erro",  // <- source
  sizeof(chuoi));         // <- destination's capacity
  // Serial.println("Version 4G LMD =" + String(chuoi));
  // Serial.println("version barcode=" + String(mybarcode));
  // Serial.println("Version device_status_printer" + String(device_status.vs_printer));
  // if ( String (device_status.vs_printer) != "NOT")
  // {	
  if(String(chuoi)=="erro")
  {
    Serial.printf("printer version erro check");
  }
  else if(String(chuoi)==String(mybarcode))
  {
    Serial.printf("Firmware barcode latest :%s\n", String(mybarcode));
    sprintf(check_fw_upbarcode,"%s",chuoi);
  }
  else
  {
    Serial.printf("new firmware barcode is %s\n",chuoi);
    sprintf(check_fw_upbarcode,"%s",chuoi);
    kq =3;
  }
  } 
  else 
  {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  return kq;
}
/*----------------END HAM KIEM TRA VERSION SU DUNG 4G----------------------*/

