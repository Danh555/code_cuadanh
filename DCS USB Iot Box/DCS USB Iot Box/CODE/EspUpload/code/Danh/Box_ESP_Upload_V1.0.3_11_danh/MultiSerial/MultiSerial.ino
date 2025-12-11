/*
Emergency !!!!!!!!!!!!!
-S3: khi upload liên tục lên: bản tin sau có hiện trạng không up được, sau đó bị tràn buffer-->reset chip ESP : cần kiểm tra 

To Do:
-test  lại up mqtt theo đường internet
-test  lại up mqtt theo đường 4G
-lộn model và brand
-có truyền status printer lên bị sai (0/1: mà lại truyền 0,1,2)
-sau khi ESP xử lý xong: gửi 1 tín hiệu qua STM: STM sẽ reset lại timer check busy
-Tăng thời gian gửi từ STM32 sang ESP
-Xử lý lại qcode: tránh bị gửi trùng lệnh-->sẽ gửi lên server liên tục
-đang bị lỗi quét qcode liên tục 2 lần (trong 5S) : thì chỉ chấp nhận code lần 1
- khác ngày format data
-trên 130 file sẽ format data

-mqtt theo đường internet
-mqtt theo đường 4G

-in 2 file liên tiếp: file thứ 2 không thông báo lên PC biết đã in xong

-ERROR: Control error: Device not responding
USBH_FAIL
USB Device Connected
USB Device Reset Completed
rơi vào trường hợp này thì thiết bị không khởi tạo kết nối máy in với PC được

#PRT: start print data: ngưng việc upload lên server-->vì printer chuẩn bị gửi data qua


-Tách qcode sang đường MQTT: mới qua đường wifi: done


-In file hơn 128k là bị tràn bufferz
- xử lý mode upload file lên server
-tăng dung lượng upload lên (hiện đang đang giới hạn ~15k/ lần up)

thử theo cách này xem??? std::unique_ptr<char[]> buf(new char[size]);
-reconnect mode connection
*/

/*
Done:
- thêm flash memory
- thêm lệnh format disk
-last bill đang bị sai
-Đang giới hạn in 2 file liên tiếp: phải có khoảng chờ:  khắc phục bằng cách đưa sensor vào trạng thái bận
- timout modem: kiểm tra init lại: Waiting for network
- timeout cổng ethernet ngắn lại
- Tạo file setting, cấu hình các thông số
- nut button config wifi
- xử lý lại thời gian trong hàm request s3-->chuyển thành getlocal time
-  thời gian đang chuyển sang server này:
 // const char* timeServer = "aws.amazon.com";
    const char* timeServer = "s3.altacloud.biz";
-thêm OTA cho ESP, BARCODE, PRINTER : local webserver
-Đang up mà có file từ STM: thì dừng up, ưu tiên nhận file từ STM trước
*/

/*
  MultiSerial:
  -Server process


  The circuit:
  - Board : ESP32 DEV Module
  - Partition scheme: " 8Mhz with spiffs (3MB APP/1.5MB SPIFFS)"  

  created 30 Otc 2023
  by MrCong
  
*/

/*--------------------------------PHẦN DANH THÊM----------------------------*/
#include "OTA_UPDATE_ETHERNET.h"
#include "OTA_UPDATE_WIFI.h"
#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include "SPIFFS.h"
#define FORMAT_SPIFFS_IF_FAILED true
#define INTERRUPT_ATTR IRAM_ATTR

IPAddress IP_LOCAL (192,168,80,175);
IPAddress IP_GATEWAY (192,168,80,254);
IPAddress SUBNET (255,255,255,0);
IPAddress DNS1 (8,8,8,8);
IPAddress DNS2 (8,8,4,4);
// #include <time.h>
/* start ethernet */
WiFiClientSecure sclient;
/*--------------------------------PHẦN DANH THÊM----------------------------*/


#include "STM32F107.h"

#include <SPI.h>
// #include <Ethernet.h>
#include <EthernetLarge.h>
#include <SSLClient.h>

// https://github.com/OPEnSLab-OSU/SSLClient/blob/master/TrustAnchors.md

#include "config.h"
#include "trust_anchors.h"
#include "hardware_ESP.h"

/*--------------Extern Flash memory----------------*/
#include<SPIMemory.h>
SPIFlash flash(CS_EXTROM);
#include "mymemory.h"

/*--------------End Extern Flash memory----------------*/

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(54,85,55,79);  // numeric IP for Google (no DNS)


// Choose the analog pin to get semi-random data from for SSL
// Pick a pin that's not connected or attached to a randomish voltage source
// const int rand_pin = 33;
// đã chỉnh lại thư viện dùng random-->nên chân này đưa vào cho đủ chức năng
// không có tác dụng gì

// Initialize the SSL client library
// We input an EthernetClient, our trust anchors, and the analog pin
EthernetClient base_client;
SSLClient mysclient(base_client, TAs, (size_t)TAs_NUM, rand_pin);


/* start wifi */
// #include <WiFiClient.h>
#include <HTTPClient.h> //ESP32 case
#include <AmazonIOTClient.h> 
#include "ESPAWSImplementations.h"
#include <ArduinoJson.h>
EspHttpClient httpClient;
EspDateTimeProvider dateTimeProvider;

AmazonIOTClient iotClient;
ActionError actionError;



//thư viện dùng OTA
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
// #include "cert.h"
#include <Update.h>

/*---------------------------------MQTT start-----------------------------------------------------*/
// #define DF_UPFILE_USING_MQTT 1

#include <MQTT.h>

WiFiClient net;
EthernetClient netethernet;


#if (DF_UPFILE_USING_MQTT)
	MQTTClient clientmqtt(17*1024);//chiều dài buffer 1
#else
	MQTTClient clientmqtt(1*1024);//chiều dài buffer 1k: đủ để upload barcode: không up data printer
#endif

/*---------------------------------MQTT end-----------------------------------------------------*/


/*---------------------------------OTA STM start-----------------------------------------------------*/
#include <WebServer.h>
#include "stm32ota.h"
#include <ESPmDNS.h>

#include "SettingServer.h"
/*---------------------------------OTA STM end-----------------------------------------------------*/

/*------------------------------modem 4G-------------------------*/
#define TINY_GSM_MODEM_SIM7600  // SIM7600 AT 
#define SerialMon Serial
#define SerialAT Serial1

// Increase RX buffer to capture the entire response
// Chips without internal buffering (A6/A7, ESP8266, M590)
// need enough space in the buffer for the entire response
// else data will be lost (and the http library will fail).
#if !defined(TINY_GSM_RX_BUFFER)
#define TINY_GSM_RX_BUFFER 650
#endif

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon
// #define LOGGING  // <- Logging is for the HTTP library

// Range to attempt to autobaud
#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 115200

// Add a reception delay - may be needed for a fast processor at a slow baud rate
// #define TINY_GSM_YIELD() { delay(2); }

// Define how you're planning to connect to the internet
/*
 * Tests enabled
 */
#define TINY_GSM_TEST_GPRS          true
#define TINY_GSM_TEST_TCP           true

#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// set GSM PIN, if any
#define GSM_PIN ""


// flag to force SSL client authentication, if needed
// #define TINY_GSM_SSL_CLIENT_AUTHENTICATION

// Your GPRS credentials, if any
// const char apn[]  = "v-internet";
const char apn[]  = "m-wap";
const char gprsUser[] = "";
const char gprsPass[] = "";

// Your WiFi connection credentials, if applicable
// const char wifiSSID[]  = "YourSSID";
// const char wifiPass[] = "YourWiFiPass";

// Server details
// const char server[]   = "vsh.pp.ua";
// const char resource[] = "/TinyGSM/logo.txt";
// const int  port       = 443;

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

// Just in case someone defined the wrong thing..
#if TINY_GSM_USE_GPRS && not defined TINY_GSM_MODEM_HAS_GPRS
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS false
#define TINY_GSM_USE_WIFI true
#endif
#if TINY_GSM_USE_WIFI && not defined TINY_GSM_MODEM_HAS_WIFI
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#endif

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm        modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

// HTTPS Transport
// TinyGsmClient base_client(modem, 0);
// SSLClient secure_layer(&base_client);
// HttpClient client = HttpClient(secure_layer, server, port);
TinyGsmClient base_client_gsm(modem, 0);
SSLClient mysclient_gsm(base_client_gsm, TAs, (size_t)TAs_NUM, rand_pin);

#if (DF_mqtt_modem)
// TinyGsmClient net4G(modem,0);
#endif 
// Initialize the SSL client library
// We input an EthernetClient, our trust anchors, and the analog pin
// EthernetClient base_client;
// SSLClient mysclient(base_client, TAs, (size_t)TAs_NUM, rand_pin);



/*------------------------------End modem 4G-------------------------*/
//OTA ethernet	

// #include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager


#define URL_fw_Version "https://raw.githubusercontent.com/DaikCong/DCSPrinter/main/bin_version.txt"
#define URL_fw_Bin "https://raw.githubusercontent.com/DaikCong/DCSPrinter/main/fw_printer_esp.bin"
const char *versionname="1.0.0";

#define df_time_upgrade_ms 600000 //10minute*60second*1000ms
uint32_t u32check_firmware=0;

// const char *ssid="tuantuan";
// const char *password="12345678";

const char *ssid="RD-2.4Ghz";
const char *password="@12345678";



// const char *ssid="mrCong";
// const char *password="0938477008";

// const char *ssid="Unknow";
// const char *password="congluxubu";

// const char *ssid="MIXIE-4G-53B2";
// const char *password="12345678";

// const char *ssid="AM-IT";
// const char *password="AMit@123";
/* end wifi */

/*---------------------------------------------------------------------------------------------------------------*/
/*---------------------------------Realtime Clock DS1307 start-----------------------------------------------------*/

#include <Wire.h>
#include "RTClib.h"

// Create RTC object
RTC_DS1307 rtc;
const char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


//mặc định khởi động chương trình là cấu hình lại thời gian
uint8_t ui8_ds1307_need_update =1;

/*---------------------------------Realtime Clock DS1307 end-----------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------*/


/*---------------------------------OTA STM start-----------------------------------------------------*/

const String STM32_CHIPNAME[47] = {
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
WebServer server(80);
const char* serverIndex = "<h1>Upload STM32 BinFile</h1><h2>Please use STM32FW.bin as the file name<br><br><form method='POST' action='/upload' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Upload'></form></h2>";


const char* serverIndexesp =
			"<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>Update Server Controller</title>"
			"<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;}</style>"
			"</head><body text=#ffffff bgcolor=##4da5b9><div style='text-align:left;display:inline-block;min-width:260px;'>"
			"<h3>Update Firmware to Server Controller</h3>"
			"<br/>Note: You are about to update the program for the Server Controller<br/>"
			"Please choose the correct file format as:<br/>"
			"<h3>ICB_ServerVx.x.bin</h3><br/>"
			"<form method='POST' action='/updateesp' enctype='multipart/form-data'><input type='file' name='update'><br/><br/><br/><input type='submit' value='Update'></form>";

const char* serverIndexprinter =
			"<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>Update Server Controller</title>"
			"<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;}</style>"
			"</head><body text=#ffffff bgcolor=##4da5b9><div style='text-align:left;display:inline-block;min-width:260px;'>"
			"<h3>Update Firmware to Printer Controller</h3>"
			"<br/>Note: You are about to update the program for the Printer Controller<br/>"
			"Please choose the correct file format as:<br/>"
			"<h3>ICB_PrinterVx.x.bin</h3><br/>"
			"<form method='POST' action='/upload' enctype='multipart/form-data'><input type='file' name='update'><br/><br/><br/><input type='submit' value='Next step'></form>";

const char* serverIndexbarcode =
			"<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>Update Server Controller</title>"
			"<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;}</style>"
			"</head><body text=#ffffff bgcolor=##4da5b9><div style='text-align:left;display:inline-block;min-width:260px;'>"
			"<h3>Update Firmware to Barcode Controller</h3>"
			"<br/>Note: You are about to update the program for the Barcode Controller<br/>"
			"Please choose the correct file format as:<br/>"
			"<h3>ICB_BarcodeVx.x.bin</h3><br/>"
			"<form method='POST' action='/upload' enctype='multipart/form-data'><input type='file' name='update'><br/><br/><br/><input type='submit' value='Next step'></form>";

const char* serverIndexsetting =
			"<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>Update Server Controller</title>"
			"<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;}</style>"
			"</head><body text=#ffffff bgcolor=##4da5b9><div style='text-align:left;display:inline-block;min-width:260px;'>"
			"<h3>System Setting</h3>"
			"<br/>Note: You are about to config the parametters for Main Controller<br/>"
			"Please choose the correct file format as:<br/>"			
			"<form method='POST' action='/upload' enctype='multipart/form-data'><input type='file' name='update'><br/><br/><br/><input type='submit' value='Next step'></form>";


File fsUploadFile;

uint8_t binread[256];
int lastbuf = 0;
int bini = 0;
int rdtmp;
int stm32ver;
String stringtmp;
//bool initflag = 0;
bool Runflag = 0;
bool ESP_OTA_Flag = 1;

enum chip_select
{
	UPDATE_NONE = 0,
	UPDATE_PRINTER,
	UPDATE_BARCODE	
};

struct str_update_manager
{
	uint8_t		deviceselect=UPDATE_NONE;
	String filename ="";
};

str_update_manager device_upgrade;

/*---------------------------------OTA STM end-----------------------------------------------------*/




/* private functions*/

/* end private functions*/

#define max_packagesize_upload (7*1024 + 512)  //4096 = 4*1024 = 4k số byte mỗi part upload lên server MAX = 8*512

#define DF_NUM_OF_BYTES_READ 512		//số bytes mỗi lần đọc ra từ flash


//15 = (max_packagesize_upload / 512) , 512 =  mỗi lần đọc ra 512 bytes flash
#define DF_NUM_OF_SAPERATE_PACK (max_packagesize_upload/DF_NUM_OF_BYTES_READ)	

#define size_buffer (2*max_packagesize_upload +5)		//số byte gửi lên server

/*-------------------------mqtt--------------------------------*/
/*
16k: up ok
49k: báo up ok: nhưng không thấy file trên app mqtt

*/

// #define max_packagesize_uploadmqtt (16*1024)  //4096 = 4*1024 = 4k số byte mỗi part upload lên server MAX = 8*512
#define max_packagesize_uploadmqtt (16*1024)  //4096 = 4*1024 = 4k số byte mỗi part upload lên server MAX = 8*512

#define DF_NUM_OF_BYTES_READmqtt 512		//số bytes mỗi lần đọc ra từ flash

// #define DF_NUM_OF_SAPERATE_PACKmqtt (max_packagesize_uploadmqtt/DF_NUM_OF_BYTES_READmqtt)
/*-------------------------mqtt--------------------------------*/


// char VALUE [size_buffer] = {'\0'}; // Nhận tối đa 0.5Mb là 76569 // 45000 30*1024
// char VALUE [50*1024] = {'\0'}; // Nhận tối đa 0.5Mb là 76569 // 45000 30*1024
char VALUE [17*1024] = {'\0'}; // Nhận tối đa 0.5Mb là 76569 // 45000 30*1024


#define DF_LEN_PRINTER (4096+50)

int bien =0;
uint32_t tempus=0;
// const int led_sign=2;
int status_led;
int counter_flash =0;

uint16_t ui16_index_printed =0; 
uint16_t ui16_index_cmd =0;
uint16_t ui16_len_package =0;
int is_save_package =0;

uint8_t ui8_ask_resend =0;
// uint8_t batdebug =0;

int is_data =0;
uint32_t ui32_timechar =0;
uint32_t totalkichthuoc =0;
uint32_t		totalup=0;
// uint32_t		last_len_upload=0;
uint8_t		updatepart=0;
uint8_t		totalpart=0;
uint16_t index_buffer_printed =0;
uint8_t ui8_data_rec [DF_LEN_PRINTER];
uint16_t  ui16_counter_up =25; /* time upload = 30 - ui16_counter_up*/
uint16_t ui16_counterfail =0;
uint8_t have_header =0;
int have_cmd =0;
uint8_t ui8_cmd[50];
uint8_t ui8_need_upload =0 ;
uint8_t ui8_have_newfile =0;
uint8_t stt_storing_file =0;
uint8_t ui8_busy_rec_printer =0; //=1: đang nhận dữ liệu in, ưu tiên nhận trước, dừng up lên server
uint32_t u32_timer_rec_printer_data =0;
uint32_t ui32_time_allow_upfile =0;
uint32_t u32_timer_test =0;

#define dflabel_header_length 4

const uint8_t au8Header[4] = {'C', 'M', 'D', ' '};
uint8_t au8BufferHeader[dflabel_header_length];

struct mqtt_OPH_MEDIA_HTTP
{    
	uint8_t mid;	//id của bill đang xử lý
	uint8_t tot;	//total bill da luu
	uint8_t part;	//part upload		
	uint8_t mode;	//tình trạng upload file =0: không up, =1: đang upload	
	uint32_t duration;//số lượng bytes đã up được
	uint32_t sizedata;//kích thước file in
	uint32_t next_addr_data;//chứa địa chỉ tiếp theo của data cần lưu
	char nameofbill[24];
};

mqtt_OPH_MEDIA_HTTP upload_manager_info;

char lastbil[24];

struct STR_SSD_INFO
{
    uint8_t 	last_day;	
    uint8_t 	new_day;	
    uint8_t 	daystatus;	
};

STR_SSD_INFO SSD_info;//buffer de xu ly du lieu

struct mqtt_BIll_HEADDER
{
    uint8_t 	status;			//trạng thái bill =0: new, =1: đã/đang upload
	uint32_t  sizedata;		//kích thước file in
	char nameofbill[24];	// BIL20231127101417
	String nameofqcode;		//chứa mã qcode
};

mqtt_BIll_HEADDER bill_media_info;//buffer chung de xu ly thông tin file hiện tại, cho việc lưu bill mới

String nameofqcode;		//chứa mã qcode

// const char *mynamedevice="ITB_1234567890";
char mynamedevice[20];

struct str_time_manager
{
	uint8_t		status;
	uint8_t		mode=0;
	uint8_t		errotimecounter;
	uint32_t	u32check_system_time=0;
	uint32_t	u32checkrespone=0;
	int32_t 	timeset=0;
	uint8_t 	process	=0;
};
str_time_manager info_time_system;

struct str_regetlost_chunk
{
    uint8_t 	status =0;
	uint8_t 	chunk;
	uint8_t 	index = 0;
	char name_tofind[24];
};

str_regetlost_chunk inforeget;



enum device_status
{
	DEV_IDLE = 0,
	DEV_INIT,
	DEV_WAITDATA
};

struct str_device_manager
{
	uint8_t		status=0;
	uint8_t		stage=0;
	uint8_t		resend=0;
	uint32_t	time=0;
};

str_device_manager devicebarcode;

str_device_manager deviceprinter;

/*trạng thái kết nối ngoại vi*/
struct str_device_status
{
	uint8_t		printer=2; /*chưa cập nhật thông tin*/
	uint8_t		barcode=2; /*chưa cập nhật thông tin*/
};

str_device_status device_status;

enum server_mode_status
{
	DEVICE_ENABLE = 0,
	DEVICE_DISABLE
};

struct str_server_manager
{
	uint8_t		enable=0; 
	uint8_t		status=0;	
	uint8_t		busy=0;	
};

enum my_device_status
{
	MY_ERRO = 0,
	MY_OK
};

str_server_manager server_ethernet;
str_server_manager server_wifi;
str_server_manager server_modem;




/* str_system_info systeminfo; */

uint8_t system_running_mode =0; /*=0: normal, 1: setup mode*/
uint32_t u32_time_recheck_nw =0;
uint32_t u32_time_resend_opu =0;
uint32_t u32_time_resend_barcode =0;


enum system_time_status
{
	TIME_NORMAL = 0,
	TIME_ERRO
};

enum system_time_control
{	
	BYETHERNET = 0,
	BYWIFI,
	BYSIM4G
};

// long timezone = 7; 
long timezone = 0; //GMT time

uint8_t system_connection_mode =BYETHERNET;
uint8_t network_status;

byte daysavetime = 0;//=0 hay bằng 1?????????????????

String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
uint8_t ui8_qcode_complete =0;
uint8_t ui8_need_pub_barcode =0;
uint8_t ui8_chuanbinhanbarcode =0;
uint8_t ui8_newfile_wait_check_qcode =0;
uint32_t ui32_timecheck_qcode =0;
uint32_t ui32_timerec_qcode =0;

uint32_t u32_time_force_format =0;
uint8_t ui8_force_format =0;

uint8_t ui8_barcode_index_last =20;
uint8_t ui8_barcode_index_new =0;

int lastButtonState=0;
int flash_led=0;
uint32_t lastDebounceTime =0;

void  setup() {
  // initialize both serial ports:
  Serial.begin(115200);
	/*init RTC*/
 	// Wire.begin();
	Wire.begin(42, 41);
 	Wire.setClock(100000);
	WiFi.config(IP_LOCAL,IP_GATEWAY,SUBNET,DNS1,DNS2);
// Serial1.begin(115200);
// Serial2.begin(115200);

//   Serial1.begin(9600,SERIAL_8N1,26,27); /* test board*/
//   Serial2.begin(230400,SERIAL_8N1,17,16);
//   pinMode(led_sign,OUTPUT);

	hardware_init();
	harware_int_test();
	/*----------------extern flash---------------*/
	flash.begin();

    // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  delay(10);
  
    if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
	{
        Serial.println("SPIFFS Mount Failed");
        return;
    }

  Serial.println("-----------------------------------");
  Serial.println("Test HW Interceptor box");
  Serial.printf("firmware version: %s\n", versionname);  
  Serial.println("-----------------------------------\n");
  

	/*load values*/
	deleteallmdeia(SPIFFS, "/", 0);

	uint32_t maxAddr = flash.getCapacity();
	Serial.print(F("Flash memory capacity = "));
	Serial.println(maxAddr);
	
	system_running_mode = 0;

	load_values();
	
	snprintf(mynamedevice, 20, "ITB_%lld", ESP.getEfuseMac());
	Serial.print("Name of box: ");
	Serial.println(mynamedevice);
	
  	
	setup_realtimeclock();
	/*thông báo khởi động*/
	for(int i=0;i<5;i++)
	{
		digitalWrite( led_sign, LOW);
		delay(200);
		digitalWrite( led_sign, HIGH);
		delay(200);
	}
	
	/* setup ethernet */
    // start Ethernet and UDP

	if(server_ethernet.enable == DEVICE_ENABLE)
	{
		//RESET chip internet

		digitalWrite(RS_ETHERNET,0);		
		digitalWrite( led_sign, LOW);
		delay(200);			
		digitalWrite(RS_ETHERNET,1);
		digitalWrite( led_sign, HIGH);
		delay(200);	
		digitalWrite( led_sign, LOW);
		delay(200);			
		digitalWrite( led_sign, HIGH);
		delay(200);	
		
		Serial.println("Init Ethernet...");
		Ethernet.init(CS_ETHERNET);
		if (Ethernet.begin(mac) == 0)  
		{
			Serial.println("Failed to configure Ethernet using DHCP");
			// Check for Ethernet hardware present
			if (Ethernet.hardwareStatus() == EthernetNoHardware) {
				Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
			} 
			else if (Ethernet.linkStatus() == LinkOFF)
			{
				Serial.println("Ethernet cable is not connected.");
			}
			// no point in carrying on, so do nothing forevermore:
			// while (true) {
			//   delay(1);
			// }
			server_ethernet.status = MY_ERRO;
		}
		else
		{
			server_ethernet.status = MY_OK;
			Serial.println("Ethernet connected, using DHCP");
		}
	}
	else
	{
		server_ethernet.status = MY_ERRO;
		Serial.println("Ethernet: disable");
	}
    
  	/* setup wifi */
  	if( server_wifi.enable == DEVICE_ENABLE)
  	{
		Serial.println(systeminfo.wifi_name);
		WiFi.begin(systeminfo.wifi_name, systeminfo.wifi_pass);	
		connect_bywifi();		
	}
	else
	{
		/* khởi tạo chỗ này để sử dụng wifi config */
		Serial.println(systeminfo.wifi_name);
		WiFi.begin(systeminfo.wifi_name, systeminfo.wifi_pass);		
		connect_bywifi();		
		server_wifi.enable = MY_ERRO;
		Serial.println("wifi: disable");
	}

		
	if (system_connection_mode == BYETHERNET)
	{
		clientmqtt.begin("mqtt.altacloud.biz",1883,netethernet);
	}
#if (DF_mqtt_modem)
	else if (system_connection_mode == BYSIM4G)
	{
		clientmqtt.begin("mqtt.altacloud.biz",1883,base_client_gsm);
	}
#endif 
	else
	{
		clientmqtt.begin("mqtt.altacloud.biz",1883,net);			
	}

	clientmqtt.setHost("mqtt.altacloud.biz",1883);
	clientmqtt.setKeepAlive(300);//5 minutes
	clientmqtt.setTimeout(30000);
	// clientmqtt.onMessageAdvanced(messageReceived);
	clientmqtt.onMessage(messageReceived);


  /* setup s3 */
	iotClient.setAWSRegion("us-east-1"); 
	iotClient.setAWSEndpoint("s3.altacloud.biz:443");
	iotClient.setAWSDomain("s3.altacloud.biz"); 	 
	iotClient.setAWSKeyID("esp-iot-2023");
	iotClient.setAWSSecretKey("secret-esp-iot-2023-alta");
    iotClient.setHttpClient(&httpClient);
	iotClient.setDateTimeProvider(&dateTimeProvider);
  
    if( server_modem.enable == DEVICE_ENABLE)
	{
		setup_modem();
	}
	else
	{
		Serial.println("4G: disable");
	}
	
	server_init(); /*mode config wifi*/

	/*sum kq init*/

	/* load upload info*/
    spiff_readfilemedia(name_up_manager, &upload_manager_info);
    
	totalup=upload_manager_info.duration;
	updatepart=upload_manager_info.part;
	ui8_need_upload =upload_manager_info.mode;
	stt_storing_file =upload_manager_info.tot;
	
	
	stt_storing_file++;//chọn file tiếp theo để lưu
	
	if (stt_storing_file > max_file_save)stt_storing_file=0;
	
	if (ui8_need_upload)
	{
		ui8_qcode_complete = 1;
	}
	Serial.print("mid : ");
	Serial.println(upload_manager_info.mid);	
	Serial.print("tot : ");
	Serial.println(upload_manager_info.tot);	
	Serial.print("up to part : ");
	Serial.println(updatepart);
	Serial.print("bytes up : ");
	Serial.println(totalup);
	Serial.print("mode : ");
	Serial.println(ui8_need_upload);
	Serial.print("next addr : ");
	Serial.println(upload_manager_info.next_addr_data);
	Serial.print("billname : ");
	Serial.println(upload_manager_info.nameofbill);

	spiff_getSSD_info(name_disk_info, &SSD_info);
	
	Serial.print("lastday : ");
	Serial.println(SSD_info.last_day);

	SSD_info.daystatus =0;
	
	get_lastbill_name(upload_manager_info.tot);
	
	/* end load upload info*/
 	 printhelp();


	 info_time_system.mode = BYETHERNET;

	/*reset other device*/
	digitalWrite(RESET_BARCODE,0);
	delay(100);
	digitalWrite(RESET_BARCODE,1);
	
  
  #if (DF_DEBUG_NOUPLOAD_TO_SERVER > 0 )
  	Serial.println("\n !!!!!!!! DF_DEBUG_NOUPLOAD_TO_SERVER!!!!!!!!!!!!!!!\n");
  #endif 


  	#if (DF_USING_BARCODE)
	barcode_process(); /*cần xử lý lại*/
	#else
	Serial.println("\n !!!!!!!! DISABLE BARCODE!!!!!!!!!!!!!!!\n");
	#endif
 
  u32_time_resend_opu = millis() + 10*60*1000;/*sau 10' up len*/

  deviceprinter.time = millis() + 120000;// 2 phút = 2*60*1000

  Serial.println("\n --------application start-------");
  
}
// int m =0;
void loop() {
 
	/* Đây là chế độ chạy bình thường, và chưa nhấn nút setting */
	if(flash_led==0 && system_running_mode == 0)
	{		
		// process_upfile();			/*S3*/
		
		process_upfile_mqtt();	/*MQTT: mới test wifi, ethernet*/
		
		process_get_chunk_mqtt();

		process_upbarcode();

		#if DF_USING_CHECKTIME
		check_system_time();
		#endif 

		send_opu_status();

		check_format_disk();
		
		checkfirmware();
		
		
		// select_uart(UART_4G);
		/*mqtt process*/
		
		
		clientmqtt.loop();
				
		if (!clientmqtt.connected()) {
			if (ui8_busy_rec_printer != 1)
			{
				connect_mqtt();
			}
		}
	}
		
	if(system_running_mode ==0)
	{
		/* đây là chế độ chạy bình thường */
		
		#if (DF_USING_BARCODE)
		select_uart(UART_BARCODE);
		barcode_process(); /*cần xử lý lại*/	
		#endif 
			
		check_serial_debug();
		check_serial_printer();	
				
		heart_beat();
		check_button();
		check_status_device();
	}
	else
	{
		/* đây là chế độ cấu hình qua wifi */
		flashflash();
		server.handleClient();
	}

	/*------------------------SAVE FILE SU DUNG ETHERNET--------------------*/
	if(ui8_update_fw_eth==1 && ui8_update_eth_esp32 == 1)
	{
		save_fileOTA_ETHERNET(mysclient,file_update_eth_esp32);
		// listDir("/",0);
		// readFile(file_update_esp32);
		ui8_update_fw_eth=0;   
		ui8_update_eth_esp32=0;
	}

	else if(ui8_update_fw_eth==1 && ui8_update_eth_STM32 == 1)
	{
		save_fileOTA_ETHERNET(mysclient,file_update_eth_stm32);
		// listDir("/",0);
		// readFile(file_update_stm32);
		ui8_update_fw_eth=0;   
		ui8_update_eth_STM32=0;
	}
	/*------------------------SAVE FILE SU DUNG ETHERNET--------------------*/

	/*------------------------SAVE FILE SU DUNG WIFI--------------------*/

	if(ui8_update_fw_wifi==1 && ui8_update_fw_ESP32==1)
	{
		Serial.println("dang luu file cho ESP32");
		save_file(sclient,file_update_wifi_esp32);
		ui8_update_fw_wifi=0;
		ui8_update_fw_ESP32=0;
	}

	else if(ui8_update_fw_wifi==1 && ui8_update_fw_STM32==1)
	{
		Serial.println("dang luu file cho STM32");
		save_file(sclient,file_update_wifi_stm32);
		upload_code_STM32();
		reset_chip();
		ui8_check_kiemtraupcode = 1;
		ui8_update_fw_wifi = 0;
		ui8_update_fw_STM32 = 0;
	}
	
	// if(ui8_check_kiemtraupcode==1)
	// {
	// 	if(millis() - ui32_timeout_resetchip >= 10000)
	// 	{
	// 		Serial.println("vào chỗ reset chip");
	// 		reset_chip();
	// 	}
	// 	ui8_check_kiemtraupcode=0;
	// }

	// napcode();

	/*------------------------SAVE FILE SU DUNG WIFI--------------------*/
}

void printhelp()
{
	Serial.println("\n --------access STM32--------------------");  
	// Serial.println("1. transmit file from STM RAM");  
	// Serial.println("2. test write flash");  
	Serial.println("1. test load file ram from STM");
	Serial.println("2. test write file to flash: expired");
	
	Serial.println("3. Read flash memory expired");  
	Serial.println("4. Read header");  
	Serial.println("5. Simulate a complete print_test_page");
	Serial.println("6. reset barcode controller");
	Serial.println("7. reset esp controller");
	Serial.println("8. reset ethernet controller");
	Serial.println("9. reset printer controller");
	
	Serial.println("\n --------access ESP32--------------------");  
	
	Serial.println("b. debug use ethernet ");
	Serial.println("c. debug use wifi ");
	Serial.println("d. debug use modem 4G ");
	Serial.println("e. debug use ALL ");
	Serial.println("r. Read file from spiffs");  
	Serial.println("d. Delete file from spiffs"); 
	Serial.println("u. test upload file"); 
	Serial.println("v. test upload file printed file");
}

void load_values()
{
	Serial.println("Load values..."); 
			
	spiff_readsettingfile("/sys.ini", &systeminfo);

    /*in debug */
/*
	SerialDebug.print("ssid = ");
	SerialDebug.println(systeminfo.wifi_name);
	SerialDebug.print("pass = ");
	SerialDebug.println(systeminfo.wifi_pass);
	SerialDebug.print("apn = ");
	SerialDebug.println(systeminfo.apn);
	SerialDebug.print("use_ethernet = ");
	SerialDebug.println(systeminfo.use_ethernet);
	SerialDebug.print("use_wifi = ");
	SerialDebug.println(systeminfo.use_wifi);
	SerialDebug.print("use_modem = ");
	SerialDebug.println(systeminfo.use_modem);
*/
	server_modem.enable = DEVICE_DISABLE;
	server_ethernet.enable = DEVICE_DISABLE;
	
	systeminfo.use_network[0]='1';
	
	if(systeminfo.use_network[0] == '0')
	{
		system_connection_mode = BYETHERNET;
		server_ethernet.enable = DEVICE_ENABLE;
		SerialDebug.println("ethernet mode");
	}	
	else if(systeminfo.use_network[0] == '2')
	{
		system_connection_mode = BYSIM4G;
		server_modem.enable = DEVICE_ENABLE;
		SerialDebug.println("4G mode");
	}
	else
	{
		system_connection_mode = BYWIFI;
		server_wifi.enable = DEVICE_ENABLE;
		SerialDebug.println("wifi mode");
	}
	server_wifi.enable = DEVICE_ENABLE;

	//load values test
	
	SerialDebug.println("\n\n !!!!!!!!!!!!!!! debug here!!!!!!!!!!!!!!!");
	// SerialDebug.println("\n\n !!!!!!!!!!!!!!! debug here!!!!!!!!!!!!!!!");
	// SerialDebug.println("\n\n !!!!!!!!!!!!!!! debug here!!!!!!!!!!!!!!!");

	server_modem.enable = DEVICE_DISABLE;
	server_ethernet.enable = DEVICE_DISABLE;
	server_wifi.enable = DEVICE_ENABLE;
	system_connection_mode = BYWIFI;

	// system_running_mode =1;
}
/*---------------------------------------------------------------------------------------------------------------*/
/*---------------------------------Realtime Clock DS1307 start-----------------------------------------------------*/
void read_time_RTC()
{	
	// return;
	#ifdef use_debug_serial
		SerialDEBUG.print("RTC : ");
	#endif 
	DateTime now = rtc.now();
	
	//debug
	#ifndef use_debug_serial
		
	SerialDEBUG.print("ICB RTC: ");
	SerialDEBUG.print(now.year(), DEC);
    SerialDEBUG.print('/');
    SerialDEBUG.print(now.month(), DEC);
    SerialDEBUG.print('/');
    SerialDEBUG.print(now.day(), DEC);
    SerialDEBUG.print(" (");
    SerialDEBUG.print(daysOfTheWeek[now.dayOfTheWeek()]);
    SerialDEBUG.print(") ");
    SerialDEBUG.print(now.hour(), DEC);
    SerialDEBUG.print(':');
    SerialDEBUG.print(now.minute(), DEC);
    SerialDEBUG.print(':');
    SerialDEBUG.print(now.second(), DEC);       
    SerialDEBUG.print(" unixtime = ");
    SerialDEBUG.println(now.unixtime());	
	#endif 
	if(now.unixtime() < 1640998800)//Date and time (your time zone): thứ bảy, 1 tháng 1 năm 2022 08:00:00 GMT+07:00
	{
		//thời gian bị chạy sai
		ui8_ds1307_need_update=1;
	}
	else
	{
		//lưu thời gian để tự động format khi khác ngày
		SSD_info.new_day =now.day();
		SSD_info.daystatus =1;
	}
}

void scan_rtc()
{
  byte error, address;
  int nDevices;

  #ifndef use_debug_serial
		SerialDEBUG.println("Scanning...");
#endif 
  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      #ifndef use_debug_serial
		SerialDEBUG.print("I2C device found at address 0x");
		#endif 
      if (address<16) 
		  {
			#ifndef use_debug_serial
			SerialDEBUG.print("0");		  
		  #endif 
		}
		#ifndef use_debug_serial			
		  SerialDEBUG.print(address,HEX);
		  SerialDEBUG.println("  !");
		  #endif 
      nDevices++;
    }
    else if (error==4) 
    {
      #ifndef use_debug_serial
		SerialDEBUG.print("Unknown error at address 0x");
		#endif 
      if (address<16) 
		  {	
			#ifndef use_debug_serial
				SerialDEBUG.print("0");
			  SerialDEBUG.println(address,HEX);
			  #endif 
		}
    }    
  }
  #ifndef use_debug_serial
  if (nDevices == 0)
    
		SerialDEBUG.println("No I2C devices found\n");		
  else    
		SerialDEBUG.println("done\n");
#endif 

  delay(5000);           // wait 5 seconds for next scan
}

void setup_realtimeclock()
{  
   scan_rtc();
   
   for(int i=0;i<5;i++)
  { 
	  if (! rtc.begin()) {
		#ifndef use_debug_serial
		SerialDEBUG.println("Couldn't find RTC @" + String (i));
		#endif 
		delay(100);
	  }
	  else
	  {
		  break;
	  }		
  }

  if (! rtc.isrunning()) {
    #ifndef use_debug_serial
		SerialDEBUG.println("RTC is NOT running!");
		#endif 
	
	// SerialDEBUG.println("Debug Test--> set this var to 1");
	// SerialDEBUG.println("Remember comment this line");
	ui8_ds1307_need_update=1;
	
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  else
  {
	  #ifndef use_debug_serial
		SerialDEBUG.println("Reading Time RTC");
		#endif 
		DateTime now = rtc.now();
		
		//debug
		#ifndef use_debug_serial
		SerialDEBUG.print(now.year(), DEC);
		SerialDEBUG.print('/');
		SerialDEBUG.print(now.month(), DEC);
		SerialDEBUG.print('/');
		SerialDEBUG.print(now.day(), DEC);
		SerialDEBUG.print(" (");
		SerialDEBUG.print(daysOfTheWeek[now.dayOfTheWeek()]);
		SerialDEBUG.print(") ");
		SerialDEBUG.print(now.hour(), DEC);
		SerialDEBUG.print(':');
		SerialDEBUG.print(now.minute(), DEC);
		SerialDEBUG.print(':');
		SerialDEBUG.print(now.second(), DEC);
				
		SerialDEBUG.print(" unixtime= ");
		SerialDEBUG.println(now.unixtime());
		#endif 
		
		if(now.unixtime() < 1640998800)//Date and time (your time zone): thứ bảy, 1 tháng 1 năm 2022 08:00:00 GMT+07:00
		{
			//thời gian bị chạy sai
			ui8_ds1307_need_update=1;
		}

  }
  // SerialDEBUG.println("***test nen co dong nay 4104");
  // ui8_ds1307_need_update=1;
}
/*---------------------------------Realtime Clock DS1307 end-----------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------*/


void check_serial_debug()
{
	  // read from port 0, send to port 2:
  if (Serial.available()) {
    int inByte = Serial.read();
    
	if(inByte =='1')
    {
      Serial.println("mode 1: get info STM");
	  Serial.println("-------");
	  init_chip_STM32();
	  
    }

	else if(inByte =='2')
    {
      Serial.println("\n mode 2: test write file to flash: expired");
	  Serial.println("-------");
	  inByte ='a';
	  check_stmErase_test();
    }
	else if(inByte =='3')
    {
      Serial.println("\n mode 3: expired");
	  Serial.println("-------");
	  inByte ='a';
	  reset_chip();
    }

	// else if(inByte =='d' || inByte =='D')
	// {
	// 	Serial.println("\n delete file");
	// 	Serial.println("-------");
	// 	deleteFile(SPIFFS, "/storage.dat");
	// }
	else if(inByte =='5')
    {      
      /* yêu cầu STM gửi vùng flash chứa data in qua */
	  Serial.println("\n get flash data printed");
	  Serial.println("-------");
	  SerialQR4G.begin(115200, SERIAL_8N1, 5, 4);
      digitalWrite(SEL_QR_4G, HIGH);
      digitalWrite(RST_F107, LOW);
      delay(50);
      digitalWrite(Boot_F107, LOW);
      delay(50);
      digitalWrite(RST_F107, HIGH);
      Serial.println("OK SEL_QR_4G = HIGH");
    }
	else if(inByte =='6')
    {      
      /* yêu cầu STM gửi vùng flash chứa data in qua */
	  Serial.println("\n reset barcode board");
	  Serial.println("-------");
	  inByte ='a';
	  	/*reset other device*/
		// digitalWrite(RS_ETHERNET,0);
		digitalWrite(RESET_BARCODE,0);
		delay(100);
		// digitalWrite(RS_ETHERNET,1);
		digitalWrite(RESET_BARCODE,1);
    }
	else if(inByte =='7')
    {      
      /* yêu cầu STM gửi vùng flash chứa data in qua */
	  Serial.println("\n reset esp controller");
	  Serial.println("-------");
		ESP.restart();
		delay(2000);

	  inByte ='a';

    }
	else if(inByte =='8')
    {      
      /* yêu cầu STM gửi vùng flash chứa data in qua */
	  Serial.println("\n reset ethernet controller");
	  Serial.println("-------");
	  inByte ='a';
	  	/*reset other device*/
		digitalWrite(RS_ETHERNET,0);
		// digitalWrite(RESET_BARCODE,0);
		delay(100);
		digitalWrite(RS_ETHERNET,1);
		// digitalWrite(RESET_BARCODE,1);
    }
	else if(inByte =='9')
    {      
      /* yêu cầu STM gửi vùng flash chứa data in qua */
	  Serial.println("\n reset printer controller");
	  Serial.println("-------");
	  inByte ='a';
	  	/*reset other device*/
		digitalWrite(RESET_PRINTER,0);
		delay(100);
		digitalWrite(RESET_PRINTER,1);		
    }
	else if(inByte =='b')
	{
		Serial.println("\n debug use ethernet ");
		server_ethernet.status	= MY_OK;
		server_wifi.status	= MY_ERRO;
		server_modem.status	= MY_ERRO;
	}
	else if(inByte =='c')
	{
		Serial.println("\n debug use wifi ");
		server_ethernet.status	= MY_ERRO;
		server_wifi.status	= MY_OK;
		server_modem.status	= MY_ERRO;
	}
	else if(inByte =='d')
	{
		Serial.println("debug use modem 4G ");
		server_ethernet.status	= MY_ERRO;
		server_wifi.status	= MY_ERRO;
		server_modem.status	= MY_OK;
	}
	else if(inByte =='e')
	{
		Serial.println("debug use ALL ");
		server_ethernet.status	= MY_OK;
		server_wifi.status	= MY_OK;
		server_modem.status	= MY_OK;
	}
	else if(inByte =='f')
	{
		// u32check_firmware  = 0;
		Serial.println("connect update esp32");
		firmwareUpdate_Ethernet(mysclient,"raw.githubusercontent.com", 443, "DatMinhHo/OTA/main/Blink_Led.ino.esp32.bin");
		ui8_update_eth_esp32=1;
		ui8_update_fw_eth=1;
	}

	else if(inByte=='F')
	{
		Serial.println("connect update wifi");
		Serial.println("save file bin for ESP32");
		connected_server(sclient, test_root_ca, "raw.githubusercontent.com", 443, "DatMinhHo/OTA/main/Blink_Led.ino.esp32.bin");
		ui8_update_fw_wifi=1;
		ui8_update_fw_ESP32=1;
	}
	else if(inByte == 'm')
	{
		listDir("/", 0);
	}
	else if(inByte =='g')
	{
		Serial.println("test updatetimebyModem_func");
		updatetimebyModem_func();
	}	
	else if(inByte =='h')
	{
		Serial.println("test upbarcode");
		ui8_need_pub_barcode=1;
		nameofqcode ="QCODE12345678";		
	}
	else if(inByte =='i')
	{
		Serial.println("test public mqtt");
		updatemqtt_func();
	}
	else if(inByte =='j')
	{
		Serial.println("format disk");
		formatdisk_func();
	}
	else if(inByte =='k')
	{
		Serial.println("update file BIN de update cho STM32");
		firmwareUpdate_Ethernet(mysclient,"raw.githubusercontent.com", 443, "Danh555/file_code/main/STM32F107RCT6_new.bin");
		ui8_update_eth_STM32=1;
		ui8_update_fw_eth=1;
	}
	else if(inByte == 'K')
	{
		Serial.println("connect update wifi");
		Serial.println("save file bin for STM32");
		connected_server(sclient, test_root_ca, "raw.githubusercontent.com", 443, "Danh555/file_code/main/STM32F107RCT6_new.bin");
		ui8_update_fw_wifi=1;
		ui8_update_fw_STM32=1;
	}
	else if(inByte =='r' || inByte =='R')
	{
		Serial.println("\n read file");
		Serial.println("-------");
		readInSPIFFS();
	}
	else if(inByte =='u')
	{
		Serial.println("\n Test upload file to server");
		// char VALUE[10];
		// for(int i =0;i<10;i++)
		// {
			// VALUE[i]='0' + i;
		// }
		// VALUE[9]='\0';
		char ten[200];
		int index_up =0;
		// sprintf(ten,"/esp-iot-2023/printer_device_test/Printerdata_%d.txt",index_up);
		sprintf(ten,"/esp-iot-2023/CongtestUpload/demo_%d.txt",index_up);
		upload_files(VALUE, ten);	
	}
	else if(inByte =='y')
	{
		Serial.println("\n Test upload large file ");
		// char VALUE[10];
		// for(int i =0;i<10;i++)
		// {
			// VALUE[i]='0' + i;
		// }
		// VALUE[9]='\0';
		char ten[200];
		int index_up =1;
		// sprintf(ten,"/esp-iot-2023/printer_device_test/Printerdata_%d.txt",index_up);
		sprintf(ten,"/esp-iot-2023/ethernet/demo_%d.txt",index_up);
		upload_files1(VALUE, ten);	
	}
	else if(inByte =='v')
	{
		Serial.println("------Debug creat file print----");
		creatnamebill();
		save_bill_info();
		
		
		ui8_qcode_complete =false;
		stringComplete = false;		
		// ui8_newfile_wait_check_qcode = 1;								
		process_savefile();
		ui32_timecheck_qcode = millis()+60000;		
		Serial.println("Wait for Qcode in 60S...");
	}
	else if(inByte =='x' || inByte =='X')
	{
		Serial.print("Cancel upload file");
		totalkichthuoc =0;
		
		updatepart=0;
		totalup=0;
		ui8_need_upload =0;

		upload_manager_info.part=updatepart;
		upload_manager_info.duration=totalup;
		upload_manager_info.mode = ui8_need_upload;
        strcpy(upload_manager_info.nameofbill,"BILLCANCEL");
		// spiff_updatemediafile("/up_manage.h", &upload_manager_info,1);
		spiff_updatemediafile(name_up_manager, &upload_manager_info,1);

	}	
	else if(inByte =='t')
	{
		//save 1st upload
		totalkichthuoc =0;
		updatepart=0;
		totalup=0;
		ui8_need_upload =1;/* thông báo upload file */

		bill_media_info.nameofqcode ="";
		upload_manager_info.part=updatepart;
		upload_manager_info.duration=totalup;
		upload_manager_info.mode = ui8_need_upload;	

		// spiff_updatemediafile("/up_manage.h", &upload_manager_info,1);
		spiff_updatemediafile(name_up_manager, &upload_manager_info,1);
		
		/*cần kiểm tra thêm quét mã qcode*/
		Serial.println("--------------------------------------");
		Serial.println("Need check qcode scanner");
		
		ui8_qcode_complete = 0;
		stringComplete = false;
		
		ui32_timecheck_qcode = millis()+60000;
		Serial.println("Wait for Qcode in 60S...");
	}


	
	else if(inByte =='\r')
	{
		
	}
	else if(inByte =='\n')
	{
		
	}
	else
	{
		printhelp();
	}
    
    Serial2.write(inByte);
  }
}

/* Serial2 */
void respone_ACK()
{
	Serial2.write('O');
}
void respone_Idle()
{
	Serial2.write('M');
}
void respone_Erro()
{
	Serial2.write('E');
}


uint32_t atoui32(char* chuoi)
{
	/* 98765432 */
	uint32_t tong=0;

	for(int i =0;i<8;i++)
	{		
		if (isdigit(chuoi[i]))
		{
			tong*=10;						
			tong += (uint32_t)(chuoi[i] - '0');
		}
		else
		{
			break;
		}
	}
	return tong;
}

/*
check_status_device
-kiểm tra hoạt động thiết bị
-kiểm tra các timeout

*/
void check_status_device()
{
	/* kiểm tra trạng thái máy in */
	uint32_t time_ = millis();
	if (time_ > deviceprinter.time)
	{
		/* time out, need reseet printer*/
		Serial.println("Reset printer board, cause no respone");
		deviceprinter.time = millis() + 120000; //2 phút = 2*60*1000
		digitalWrite(RESET_PRINTER,0);
		delay(100);
		digitalWrite(RESET_PRINTER,1);
	}


	/* kiểm tra time out BUSY mode*/
	if(ui8_busy_rec_printer == 1)
	{
		if ( millis() > u32_timer_rec_printer_data)
		{			
			Serial.println("exit Busy mode");
			ui8_busy_rec_printer =0;
		}		
	}

	/* test 10s truyền 1 đoạn data qua stm*/	
	#if (0) 
	safdfd
	if ( millis() > u32_timer_test)
	{			
		Serial.println("\n\n --------------------------Test------------------------------------------------");	
		Serial2.println("#123456789$");
		u32_timer_test = millis() + 20000;//10
	}
	#endif 
}

void check_serial_printer()
{
	
	if (is_data ==1) /* printed data*/
	{
		while (Serial2.available()) {
			// get the new byte:
			char inChar = (char)Serial2.read();
			ui32_timechar = millis();
					
			if(ui16_index_printed < ui16_len_package)
			{
			
				ui8_data_rec[ui16_index_printed] = inChar;
				
				ui16_index_printed++;
				
				if(ui16_index_printed == ui16_len_package)
				{							
					is_save_package = 1;
					is_data =0;
				}
			}
			else
			{
				Serial.write(inChar);
			}
		}
	}
	else /* cmd data*/
	{
		while (Serial2.available()) {
			
			/* CMD I*/
			// get the new byte:
			char inChar = (char)Serial2.read();
			ui32_timechar = millis();
			Serial.write(inChar);
			
			if(have_header)
			{				
				if (inChar == '\n')
				{
					ui8_cmd[ui16_index_cmd] = '\0';
					have_cmd =1;
					have_header=0;
					ui16_index_cmd=0;
					break;
				}
				else
				{
					ui8_cmd[ui16_index_cmd] = inChar;
					
					ui16_index_cmd++;
					if(ui16_index_cmd>50)
					{
						ui16_index_cmd=0;
						have_header=0;
					}
				}
			}
			else			
			{
				if (finding_header(inChar)) {
					have_header =1;
					ui16_index_cmd=0;
				}
			}
		}
	}
	
	if ((millis() - ui32_timechar) > 500) {		
		//reset
		is_data =0;
		have_header =0;
		#if (0)
			if(is_data ==1)
			{		
				is_data =0;
			}
			else if (have_header ==1)
			{			
				have_header =0;		
			}
		#endif 
	}
	
	if(is_save_package ==1)
	{		
		if (ui8_data_rec[ui16_index_printed -3] =='E' && ui8_data_rec[ui16_index_printed -2] =='O' && ui8_data_rec[ui16_index_printed -1] =='P')
		{
			// Serial.println("package ok");
			ui8_ask_resend =0;

			ui16_index_printed -= 3;		
			uint32_t a= millis(); 
			
			storeInSPIFFS(ui8_data_rec,ui16_index_printed);
			
			uint32_t b= millis();
			
			b-=a;
			Serial.print(" \t take time = ");
			Serial.println((uint16_t)b);
			Serial.println("");
			respone_Idle();

		}
		else
		{
			Serial.println("pkg fail");
			/*yêu cầu gửi lại: nếu gửi 3 lần mà bị lỗi, thì bỏ qua luôn*/
			ui8_ask_resend ++;
			if (ui8_ask_resend < 3)
			{
				respone_Erro();
			}
			else
			{
				respone_Idle();
				ui8_ask_resend =0;
			}
		}
				
		ui16_index_printed =0;
		is_save_package =0;
	}
	
	if(have_cmd ==1)
	{
		have_cmd =0;
		deviceprinter.time = millis() + 50000;/* reset timout printer*/
		/* #CMD P011234 */
		/* #CMD 11 */
		if (ui8_cmd[0] == '1')
		{			
			if(ui8_cmd[1] == '1')
			{			
				//get total buffer
				Serial.println("start rec data print");
				uint32_t tong = atoui32((char*)(ui8_cmd+2));
				
				Serial.print("totalbuffer =");
				Serial.println(tong);								
				bill_media_info.sizedata = tong;
				if(tong > 0)
				{													
					creatnamebill();
					save_bill_info();//save header
				}
				
				//xóa file cũ
				// deleteFile(SPIFFS, "/storage.dat");
				#if (DF_USING_EXT_FLASH)

				#else
					deleteoldFile();
				#endif 

				ui16_index_printed =0;
				ui8_busy_rec_printer = 1;
				u32_timer_rec_printer_data = millis() + 60000;// 1 phút
				respone_ACK();												
			}
			else if(ui8_cmd[1] == '0')
			{
				is_data = 0;
				Serial.println("End of package printed");			
				/*cần kiểm tra thêm quét mã qcode*/
			
				// ui8_qcode_complete = 0;				
				// stringComplete = false;
				// bill_media_info.nameofqcode ="";
				// ui8_newfile_wait_check_qcode = 1;

				process_savefile();

				// ui32_timecheck_qcode = millis()+60000;
				// Serial.println("Wait for Qcode in 60S...");
			}				
		}
		else if (ui8_cmd[0] == 'S') 
		{					
			/* đang in file, dừng các hoạt động khác để chuẩn bị nhận file */
			ui8_busy_rec_printer = 1;
			u32_timer_rec_printer_data = millis() + 60000;// 1 phút
			Serial.println("Busy mode");
		}
		else if (ui8_cmd[0] == 'I') 
		{					
			if (ui8_cmd[1] == '1') 
			{
				if(device_status.printer != 1 && device_status.printer != 2)
				{					
					u32_time_resend_opu = millis() + 1000;
				}
				
				device_status.printer = 1;
				Serial.println("Printer Ready");
			}
			else
			{
				
				if(device_status.printer != 0 && device_status.printer != 2)
				{
					u32_time_resend_opu = millis() + 1000;
				}

				device_status.printer = 0;
				Serial.println("Printer Not Ready");
			}						
		}
		else if (ui8_cmd[0] == '4')
		{
			/* 4479765 */
			//get len
			uint32_t tong = atoui32((char*)(ui8_cmd+2));	
			
			Serial.print("tong =");
			Serial.println(tong);							
		}
		else if (ui8_cmd[0] == 'R' && ui8_cmd[1] == 'D') /* dữ liệu check ACK*/
		{					
			/* #CMD RD are you ready???*/
			// Serial.println("ready???");
			respone_ACK();
			is_data =0;
		}
		else if (ui8_cmd[0] == 'P') /* dữ liệu in*/
		{					
			//P[index]L[len]
			//	#CMD P0L0256
			//get lenght
			int vitrilen =0;
			for(int i =0; i < sizeof(ui8_cmd); i++)
			{
				if(ui8_cmd[i] == 'L')
				{
					vitrilen = i;
				}
			}

			if(vitrilen)			
			{
				char m[5];
				vitrilen++;
				ui16_len_package = atoui32((char*)(ui8_cmd+vitrilen));
				if (ui16_len_package < 4097)
				{						
					is_data =1;
					ui8_busy_rec_printer = 1;
					u32_timer_rec_printer_data = millis() + 60000;// 1 phút
					ui16_len_package+=3; /*thêm 3 kí tự kết thúc END*/
				}
			}
		}
		else
		{
			Serial.print("Unknow CMD ");
			Serial.println(ui8_cmd[0]);
		}
	}
}


boolean finding_header(uint8_t input) {
  boolean the_same = true;

  uint8_t i = 0;
  for (i = 0; i < dflabel_header_length - 1; i++) {

    au8BufferHeader[i] = au8BufferHeader[i + 1]; //ghi dịch

    if (au8BufferHeader[i] != au8Header[i])
    {
      the_same = false;
    }
  }
  au8BufferHeader[i] = input; //i đã cộng thêm 1 khi kết thúc for

  if (au8BufferHeader[i] != au8Header[i])
  {
    the_same = false;
  }//kiểm tra phần tử cuối

  return the_same;
}

void select_uart(int uart_index_)
{
	/* barcode*/
	digitalWrite( CS_UART, uart_index_);

}

/*
check_serial_qcode_Scanner
=1: hoàn thành
=0: chưa hoàn thành
*/
int check_serial_qcode_Scanner()
{	
	if (server_modem.busy == 1) return 1;
	int kq =0;
	select_uart(UART_BARCODE);

	// read from port 1, send to port 0:
  while (SerialToBarcodeModem.available()) {
    char inChar = (char)SerialToBarcodeModem.read();
    Serial.write(inChar);
	//nếu quét liên tiếp 2 lần liền: sẽ bị chồng mã
	// add it to the inputString:
    
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n' || inChar == '\r') {

		// Serial.println("have enter");
/*
	#BC CODE
	8935106211321
	END

	new package:
	#CODE [index]
	8935106211321
	END
*/
		if(ui8_chuanbinhanbarcode ==1)
		{
			/*đây là mã code*/
			ui8_chuanbinhanbarcode =0;						
			
			bill_media_info.nameofqcode = inputString;
			Serial.print("Code rec = ");
		  	Serial.println(bill_media_info.nameofqcode);
						
			if(ui8_barcode_index_last != ui8_barcode_index_new || millis() > ui32_timerec_qcode)
			{
				ui8_barcode_index_last =ui8_barcode_index_new;
				Serial.println("Accept this code");
				nameofqcode = inputString;
				
				ui8_need_pub_barcode =1;
				u32_time_resend_barcode = millis() + 1000;
				ui8_qcode_complete =true;
				stringComplete = true;
				ui32_timerec_qcode = millis()+45000;
			}
			kq =1;
			// ui32_timecheck_qcode = millis()+60000;
		}
		
		kq =1;
		if (inputString.startsWith("#CODE "))
		{
			String s =inputString.substring(6,inputString.length());
			ui8_barcode_index_new = s.toInt();
			// SerialDEBUG.println("index: " + s);
			// SerialDEBUG.println("= " + ui8_barcode_index_new);
			kq =0;
			ui8_chuanbinhanbarcode =1;			
		}
		else if (inputString == "#BC I1")
		{
			if(device_status.barcode != 1 && device_status.barcode != 2)
			{
				u32_time_resend_opu = millis() + 1000;
			}
			
			device_status.barcode = 1;						
		}		
		else if (inputString == "#BC I0")
		{
			if(device_status.barcode != 0 && device_status.barcode != 2)
			{
				u32_time_resend_opu = millis() + 1000;
			}
			device_status.barcode = 0;						
		}		
		inputString = "";
    }
	else
	{
		if(inChar =='\0')
		{
			Serial.print(".");
		}
		else
		{
			inputString += inChar;
		}
	}
  }

  if (millis() > ui32_timecheck_qcode)
  {
	if (stringComplete ==true)
	{		
		Serial.println("qCode expired");
	}
		
	stringComplete = false;
	bill_media_info.nameofqcode ="";
	ui8_qcode_complete =true;
	ui32_timecheck_qcode = millis()+600000;
  }

  if (kq ==1)
  {
		devicebarcode.time = millis() + 5000;/*thời gian check status barcode processor*/
		devicebarcode.stage = DEV_IDLE;
  }
return kq;
	//   select_uart(UART_4G);
}

void barcode_process()
{
	if (server_modem.busy == 1) return ;
		
	switch (devicebarcode.stage)
	{
		case DEV_INIT/* constant-expression */:
		{
			/* code */			
			select_uart(UART_BARCODE);
			Serial.println("GET CODE");
			SerialToBarcodeModem.println("#GET$");
			// SerialToBarcodeModem.write('#');
			devicebarcode.time = millis() + 5000;
			devicebarcode.stage = DEV_WAITDATA;
			break;
		}
		case DEV_WAITDATA/* constant-expression */:
		{
			/* code */
			check_serial_qcode_Scanner();
			
			// if (check_serial_qcode_Scanner())
			// {
			// 	select_uart(UART_4G);
			// }
			
			if(millis() > devicebarcode.time)
			{
				Serial.println("Warning barcode no responed");
				
				devicebarcode.resend++;
				if(devicebarcode.resend > 1)
				{
					/*reset barcode here*/
					digitalWrite(RESET_BARCODE,0);
					delay(100);
					digitalWrite(RESET_BARCODE,1);

					devicebarcode.resend = 0;
					devicebarcode.time = millis() + 5000;/*thời gian check status barcode processor*/
					devicebarcode.stage = DEV_IDLE;

					// select_uart(UART_4G);
				}
				else
				{
					devicebarcode.stage = DEV_INIT;
				}									
			}
			break;	
		}
		default:
		{	
			if(millis() > devicebarcode.time)
			{
				devicebarcode.resend = 0;
				devicebarcode.stage = DEV_INIT;
			}
			
			break;
		}
	}
}

int flashflash() {
  int kq=0;
  if (millis() > tempus) {
    counter_flash++;
	if(counter_flash ==1)
	{
		digitalWrite( led_sign, HIGH );
		tempus = millis() + 50;
	}
	else if(counter_flash ==2)
	{
		digitalWrite( led_sign, LOW );
		tempus = millis() + 200;
	}
	else if(counter_flash ==3)
	{
		digitalWrite( led_sign, HIGH );
		tempus = millis() + 50;
	}
	else
	{
		digitalWrite( led_sign, LOW );
		tempus = millis() + 800;
		counter_flash =0;
	}	
  }
  return kq;
}

int heart_beat() {
  int kq=0;
  if (millis() > tempus) {
    if (status_led == 1) {
      digitalWrite( led_sign, HIGH );      
      status_led = 0;
	  
	  if(flash_led || system_running_mode ==1)
	   {
			tempus = millis() + 100;
	   }
	  else if(network_status == MY_ERRO)
	  {
			tempus = millis() + 500;			
	  }
	  else
	  {
			tempus = millis() + 50;
	  }	  	  	  	  
	  kq=1;
	  ui16_counter_up++;
    }
    else {
      digitalWrite( led_sign, LOW );
      status_led = 1;
		if(flash_led || system_running_mode ==1)
	   {
		tempus = millis() + 100;
	   }
	   else if(network_status == MY_ERRO)
	  {
			tempus = millis() + 500;	
	  }
	  else
	  {
			tempus = millis() + 950;
	  }
    }
  }
  return kq;
}

int flashled(int mode_) 
{
  int kq=0;
  if (millis() > tempus) 
  {
    if (status_led == 1) 
	{
      digitalWrite( led_sign, HIGH );      
      status_led = 0;
	  
	  if(mode_ ==1)
	  {
			tempus = millis() + 100;
	  }
	  else
	  {
			tempus = millis() + 500;
	  }
    }
    else 
	{
      digitalWrite( led_sign, LOW );
      status_led = 1;
	  if(mode_ == 1)
	  {
			tempus = millis() + 100;	
	  }
	  else
	  {
			tempus = millis() + 500;
	  }
    }
  }
}

void check_button()
{
	int reading = digitalRead(INPUT_BUTTON);
	int kq = 0;
	
	
	// If the switch changed, due to noise or pressing:
	if (reading != lastButtonState) {
		// reset the debouncing timer
		lastDebounceTime = millis();
		flash_led=0;
  	}

	if ((millis() - lastDebounceTime) > 1000) {
		// whatever the reading is at, it's been there for longer than the debounce
		// delay, so take it as the actual current state:
		// Vẫn đang nhấn nút
		if (reading == LOW) {
			flash_led=1;
		}		
  	}

	if ((millis() - lastDebounceTime) > 5000) {
		// whatever the reading is at, it's been there for longer than the debounce
		// delay, so take it as the actual current state:

		// Vẫn đang nhấn nút
		if (reading == LOW) {
			kq=1;
		}		
  	}

	
	if(kq ==1)
	{
		SerialDebug.println(F("Enter configmode"));

		delay(500); // Without delay I've seen the IP address blank

		WiFi.mode(WIFI_AP_STA);
		// WiFi.softAP(mynamedevice, "12345678");//password option
		WiFi.softAP(mynamedevice, "");//password option
		delay(500); // Without delay I've seen the IP address blank
		SerialDebug.println(F("AP IP address: "));
  		SerialDebug.println(WiFi.softAPIP());

		system_running_mode =1;
	}

	lastButtonState = reading;

}

long storeheaderSPIFFS(String file, mqtt_OPH_MEDIA_HTTP *regs, int ms) {

	#ifdef use_debug_serial
		SerialDEBUG.println(F("\n\r[FILE]:spiff_updatemediafile Start"));
	#endif
	
	//Open file for write
    File f = SPIFFS.open(file,  "w");
  

  int s = 0;
//if File Exists
  if (f) {
    s = f.size();
	// Allocate the JSON document
	//
	// Inside the brackets, 200 is the RAM allocated to this document.
	// Don't forget to change this value to match your requirement.
	// Use arduinojson.org/v6/assistant to compute the capacity.
	DynamicJsonDocument doc(1024);					
	// StaticJsonObject allocates memory on the stack, it can be
	// replaced by DynamicJsonDocument which allocates in the heap.
	//
	// DynamicJsonDocument  doc(200);
	doc["mid"]=regs->mid;
	doc["tot"]=regs->tot;
	doc["part"]=regs->part;	
	doc["mode"]=regs->mode;
	doc["duration"]=regs->duration;
	doc["name"] = regs->nameofbill;

	#ifdef use_debug_serial
	SerialDEBUG.println(F("[FILE]:lmd_msg= "));
    SerialDEBUG.println(regs->lmd_msg);
	SerialDEBUG.println(F("[FILE]:end lmd_msg= "));
	#endif

	
	//create file		
	// Generate the minified JSON and send it to the SerialDEBUG port.
	#ifdef use_debug_serial
	serializeJson(doc,SerialDEBUG);
	#endif 
	//create file
	serializeJson(doc,f);
	f.close();		
	
	// SerialDEBUG.println(F("\n\r[FILE]:spiff_updatemediafile Done"));		
	
    //Return the size of file
    return (s);
	
  } else {
    #ifdef use_debug_serial
	SerialDEBUG.print(F("[FILE]:Unable to open file "));
    SerialDEBUG.println(file);
	#endif 
  }
  return (1);
}

void deleteoldFile()
{
	char namefile[30];
	sprintf(namefile,"/bill_%d.dat",stt_storing_file);

    Serial.printf("Deleting file: %s\r\n", namefile);
    if(SPIFFS.remove(namefile)){
        Serial.println("- file deleted");
    }
	else 
	{
        Serial.println("- delete failed");
    }
}

void deleteallmdeia(fs::FS &fs, const char * dirname, uint8_t levels){
    int index_=0;
	
	#ifndef use_debug_serial
	SerialDEBUG.printf("[*FILE]: Delete All Media File: %s\n", dirname);
	#endif 
	
	File root = fs.open(dirname);
    if(!root){
        #ifndef use_debug_serial
	SerialDEBUG.println("[*FILE]: Failed to open directory");
	#endif 
        return;
    }
    if(!root.isDirectory()){
        #ifndef use_debug_serial
	SerialDEBUG.println("[*FILE]: Not a directory");
	#endif 
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            #ifndef use_debug_serial
			SerialDEBUG.print("[*FILE]: \tDIR : ");
            SerialDEBUG.print (file.name());
			#endif 

            //bỏ qua không xử lý folder
			// if(levels){
                // getmediafilename(fs, file.name(), levels -1);
            // }
        } else {
            #ifndef use_debug_serial
			SerialDEBUG.print("[*FILE]: NAME: ");
            SerialDEBUG.print(file.name());
            SerialDEBUG.print("  SIZE: ");
            SerialDEBUG.println(file.size());
			#endif 

			  String s =String(file.name());
			//   if (s.endsWith(".ini") || s.endsWith(".dat")|| s.endsWith(".h")) {
			  if (s.endsWith(".ini") || s.endsWith(".sys")) {
				#ifndef use_debug_serial
				SerialDEBUG.println("[*FILE]: \tright file-->keep");
				#endif 								
			  }
			  else
			  {
				//delete file
				#ifndef use_debug_serial
				SerialDEBUG.println("[*FILE]: \twrong file-->delete");
				#endif 
				deleteFile(fs,file.name());
			  }
        }
        file = root.openNextFile();
    }
}

#if (DF_USING_EXT_FLASH)
void storeInSPIFFS(byte* payload, unsigned int length){
	// Serial.println(F("Save package"));
	// write_data(upload_manager_info.next_addr_data, &values[0],length);
	
	int erro = write_data(upload_manager_info.next_addr_data, &(*payload),length);
	upload_manager_info.next_addr_data +=length;

	if (erro != 0)
	{
		/*need format*/
		u32_time_force_format = millis() + 30*1000;
		ui8_force_format =1;
	}
}
#else
void storeInSPIFFS(byte* payload, unsigned int length){
	
	if (!length)return;
 
 	char namefile[30];
	sprintf(namefile,"/bill_%d.dat",stt_storing_file);

	Serial.print(F("Save data file = "));
    Serial.println(namefile);
 
//  File storage = SPIFFS.open("/storage.dat", "ab");/*wb= write binary, ab=apend binary */
 File storage = SPIFFS.open(namefile, "ab");/*wb= write binary, ab=apend binary */
 if (storage)
 {   
   Serial.print(F("Save len = "));
   Serial.println(length);
	/* debug */
	#if (0)
	int m=0;
   for (int i = 0; i < length; i++) {
     Serial.printf("%02x ",(byte)payload[i]);
    m++;
	if (m >19)
	{
	  m=0;
	  Serial.println("");
	}
	
	if(payload[i] == 0xff)
	{
		m=0;
		Serial.println("\n");
	}
   }
   Serial.println(""); 
   #endif
   
   storage.write(payload, length);
 }
 storage.close();
}
#endif 

void readInSPIFFS(){
 
	Serial.println(F("---------Going to read storage -----------------------"));
 
 int total=0;

	char namefile[30];
	sprintf(namefile,"/spiffs/bill_%d.dat",upload_manager_info.mid);

	// FILE* readStorage=fopen("/spiffs/storage.dat", "rb");
	FILE* readStorage=fopen(namefile, "rb");
 
	uint8_t dataBuffer[4096];
 
	if (readStorage == NULL) {			
			Serial.println("Failed to open file for reading");
	}
	else
	{
		/*read header */
		
		int len;
		
		
		while(!feof(readStorage)) {
			len = fread(dataBuffer, 1, sizeof(dataBuffer), readStorage);
			total += len;
			   
			   Serial.print("Read @len =");
			   Serial.println(len);
			   int aa =0;
			   for (int i = 0; i < len; i++) {
					Serial.printf("%02x ",(byte)dataBuffer[i]);
					aa++;
					if (aa > 19)
					{
						aa = 0;
						Serial.println("");						
					}
			   }
			   Serial.println("----");						
		}
		Serial.print("\n Total @len =");
		Serial.println(total);
		
		fclose(readStorage);
		
	}

 
 #if (0)
	 
  File readStorage = SPIFFS.open("/storage.dat", "rb");
 if (readStorage) {
   Serial.print("size of file = ");
   Serial.println(readStorage.size());
   Serial.println("--------------");
   
   byte buffer[readStorage.size()];
   
   Serial.println("line 363");
   
   size_t bufferSize = readStorage.readBytes((char*)buffer,readStorage.size());
      
  
   	int m=0;
   for (int i = 0; i < bufferSize; i++) {
     Serial.printf("%02x ",(byte)buffer[i]);
    m++;
	if (m >19)
	{
	  m=0;
	  Serial.println("");
	}
	
	if(buffer[i] == 0xff)
	{
		m=0;
		Serial.println("\n");
	}
   }
   Serial.println(""); 

   readStorage.close();
 }
 #endif
 
 Serial.println(F("---------End going to read storage -----------------------"));
}


void deleteFile(fs::FS &fs, String path){
    Serial.printf("Deleting file: %s\r\n", path);

	if (!path.startsWith("/")) path = "/" + path;


    if(fs.remove(path)){
        Serial.println("- file deleted");
    }
	else 
	{
        Serial.println("- delete failed");
    }
	
	//tạo file mới
/*	
	for (int i =0;i<128;i++)
	{
		ui8_data_rec[i] = 'a';
	}
	storeInSPIFFS(ui8_data_rec,128);
	*/
}



const char* testsend(const char* request, const char* serverUrl, int port)
{
    //port = 443;
#if !defined ARDUINO_ARCH_SAM && !defined ARDUINO_ARCH_SAMD
	// WiFiClientSecure sclient;
	// sclient.setInsecure();
#else
	// WiFiSSLClient sclient;
#endif

    // EthernetClient sclient;

	Serial.println("testsend");
    Serial.print("serverUrl = ");
    Serial.println(serverUrl);
    Serial.print("port = ");
    Serial.println(port);
    // Serial.println(request);
    Serial.println("");
    Serial.println("");

    String response = "";
    
    if (mysclient.connect(serverUrl, port)) {
#ifdef ESP8266
        if(mysclient.verify(fingerprint,serverUrl)){
            Serial.println("Certificate Matches");
        } else {
            Serial.println("Certificate Does Not Match");
        }
#endif
        Serial.println("Connected to server");
        // Send the request
        mysclient.print(request);
        // WiFiClientSecure sclient;
        // sclient.setInsecure();
        // keep reading the response until it's finished
        while(mysclient.connected()) {
            bool availableSeen = false;
            while(mysclient.available()){
                availableSeen = true;
                char c = mysclient.read();
                // Serial.print(c);
                response.concat(c);
            }
            if(availableSeen)
            {
                mysclient.stop(); // disconnect any open connections
                Serial.println("STOP");
            }
        }
        Serial.println("disconneted server");

    } else {
        // mysclient.setInsecure();
        Serial.println("Connection Unsuccessful");
        // connection was unsuccessful
        // mysclient.stop();
        return "Connection Unsuccessful";
    }

    // convert the string into a char and return
    int len = response.length();
    char* response_char = new char[len + 1]();
    response.toCharArray(response_char, len + 1);
    return response_char;
}

const char* testsend4G(const char* request, const char* serverUrl, int port)
{
    //port = 443;
#if !defined ARDUINO_ARCH_SAM && !defined ARDUINO_ARCH_SAMD
	// WiFiClientSecure sclient;
	// sclient.setInsecure();
#else
	// WiFiSSLClient sclient;
#endif

    // EthernetClient sclient;

	Serial.println("testsend 4G");
    Serial.print("serverUrl = ");
    Serial.println(serverUrl);
    Serial.print("port = ");
    Serial.println(port);
    // Serial.println(request);
    Serial.println("");
    Serial.println("");

    String response = "";
    
    if (mysclient_gsm.connect(serverUrl, port)) {
#ifdef ESP8266
        if(mysclient.verify(fingerprint,serverUrl)){
            Serial.println("Certificate Matches");
        } else {
            Serial.println("Certificate Does Not Match");
        }
#endif
        Serial.println("Connected to server");
        // Send the request
        mysclient_gsm.print(request);
        // WiFiClientSecure sclient;
        // sclient.setInsecure();
        // keep reading the response until it's finished
        while(mysclient_gsm.connected()) {
            bool availableSeen = false;
            while(mysclient_gsm.available()){
                availableSeen = true;
                char c = mysclient_gsm.read();
                // Serial.print(c);
                response.concat(c);
            }
            if(availableSeen)
            {
                mysclient_gsm.stop(); // disconnect any open connections
                Serial.println("STOP");
            }
        }
        Serial.println("disconneted server");

    } else {
        // mysclient.setInsecure();
        Serial.println("Connection Unsuccessful");
        // connection was unsuccessful
        // mysclient.stop();
        return "Connection Unsuccessful";
    }

    // convert the string into a char and return
    int len = response.length();
    char* response_char = new char[len + 1]();
    response.toCharArray(response_char, len + 1);
    return response_char;
}

/*
upload_file_to_server()
base on: testsend, testsend4G

*/

const char* upload_file_to_server(const char* request, const char* serverUrl, int port)
{
	Serial.println("upload_file_to_server");
    Serial.print("serverUrl = ");
    Serial.println(serverUrl);
    Serial.print("port = ");
    Serial.println(port);
    Serial.println("");
    Serial.println("");

    String response = "";
	const char* res ="";
	int viawifi =0;
	//kiểm tra upload theo đường nào
	if(server_ethernet.status	== MY_OK)
	{
		Serial.println("by ethernet");
		if (mysclient.connect(serverUrl, port)) {
			Serial.println("Connected to server");
			// Send the request
			mysclient.print(request);
			// WiFiClientSecure sclient;
			// sclient.setInsecure();
			// keep reading the response until it's finished
			while(mysclient.connected()) {
				bool availableSeen = false;
				while(mysclient.available()){
					availableSeen = true;
					char c = mysclient.read();
					// Serial.print(c);
					response.concat(c);
				}
				if(availableSeen)
				{
					mysclient.stop(); // disconnect any open connections
					Serial.println("STOP");
				}
			}
			Serial.println("disconneted server");

		} else {
			// mysclient.setInsecure();
			Serial.println("Connection Unsuccessful");
			// connection was unsuccessful
			// mysclient.stop();
			return "Connection Unsuccessful";
		}


	}
	else if(server_wifi.status	== MY_OK)
	{
		Serial.println("by wifi");

		WiFiClientSecure sclient;
    	sclient.setInsecure();
		
		if (sclient.connect(serverUrl, port)) {
			Serial.println("Connected to server");
			// Send the request
			sclient.print(request);
			// WiFiClientSecure sclient;
			// sclient.setInsecure();
			// keep reading the response until it's finished
			while(sclient.connected()) {
				bool availableSeen = false;
				while(sclient.available()){
					availableSeen = true;
					char c = sclient.read();
					// Serial.print(c);
					response.concat(c);
				}
				if(availableSeen)
				{
					sclient.stop(); // disconnect any open connections
					Serial.println("STOP");
				}
			}
			Serial.println("disconneted server");

		} else {
			// sclient.setInsecure();
			Serial.println("Connection Unsuccessful");
			// connection was unsuccessful
			// sclient.stop();
			return "Connection Unsuccessful";
		}

	}
	else if(server_modem.status	== MY_OK)
	{
		Serial.println("by modem");
		//chuyển mode
		server_modem.busy = 1;
		select_uart(UART_4G);
		delay(1);
				
		if (mysclient_gsm.connect(serverUrl, port)) {
			Serial.println("Connected to server");
			// Send the request
			mysclient_gsm.print(request);
			// WiFiClientSecure sclient;
			// sclient.setInsecure();
			// keep reading the response until it's finished
			while(mysclient_gsm.connected()) {
				bool availableSeen = false;
				while(mysclient_gsm.available()){
					availableSeen = true;
					char c = mysclient_gsm.read();
					// Serial.print(c);
					response.concat(c);
				}
				if(availableSeen)
				{
					mysclient_gsm.stop(); // disconnect any open connections
					Serial.println("STOP");
				}
			}
			Serial.println("disconneted server");

		} else {
			// mysclient.setInsecure();
			Serial.println("Connection Unsuccessful");
			// connection was unsuccessful
			// mysclient.stop();
			return "Connection Unsuccessful";
		}
		server_modem.busy = 0;
	}
	else
	{
		Serial.println("erro: internet available");
	}

    // convert the string into a char and return


	int len = response.length();
	char* response_char = new char[len + 1]();
	response.toCharArray(response_char, len + 1);
	return response_char;

}


/*
upload_files
dùng để test chức năng upload file

*/

const char* upload_files(char* data, const char* path){
  const char* res = "";
  const char* res2 = "";

  //
    //long image_buf_size = 4000 * 1000;                                                  
	//char *image = (char *)ps_calloc(image_buf_size, sizeof(char));
  
  //40000
  Serial.println("Load data");

  int u = 0;
  int j = 'a';
  for(int i =0; i< 40*1024; i++)
  {	  	  
	  VALUE[u] = j; 
	  u++;
	  j++;
	  if( j >'z') j ='a';	  
  }  
  VALUE[u] = '\0';

	if(server_ethernet.status	== MY_OK)
	{
		VALUE[0] ='E';
		VALUE[1] ='T';
		VALUE[2] ='H';
		VALUE[3] ='E';
		VALUE[4] ='R';
		VALUE[5] ='N';
		VALUE[6] ='E';
		VALUE[7] ='T';
		VALUE[8] =' ';
		VALUE[9] =' ';
		VALUE[10] =' ';
	}
	else if(server_wifi.status	== MY_OK)
	{
		VALUE[0] ='W';
		VALUE[1] ='I';
		VALUE[2] ='F';
		VALUE[3] ='I';
		VALUE[4] =' ';
		VALUE[5] =' ';
		VALUE[6] =' ';
		VALUE[7] =' ';
	}
	else if(server_modem.status	== MY_OK)
	{
		VALUE[0] ='4';
		VALUE[1] ='G';
		VALUE[2] =' ';
		VALUE[3] =' ';
		VALUE[4] =' ';
		VALUE[5] =' ';
		VALUE[6] =' ';
		VALUE[7] =' ';
	}

  Serial.print("total byte = ");
  Serial.println(u);
  
  iotClient.setAWSPath(path);

  Serial.print("File name ");
  Serial.println(path);
  
  /* AmazonIOTClient.cpp / ESPAWSImplementations.cpp*/
  
  res = iotClient.update_shadow(VALUE, actionError); 

//   Serial.println("\n --------Response From server-------\n");
  Serial.println("\n --------Request-------\n");
  Serial.println(res);
  Serial.println("\n --------end Request-------");
  
//   res2 = testsend(res,"s3.altacloud.biz",443);
  res2 = upload_file_to_server(res,"s3.altacloud.biz",443);
  
    
  Serial.println("\n --------Response From server-------\n");
  Serial.println(res2);
  Serial.println("\n --------end response-------");
  
  Serial.println("end");
  return res; 
}
const char* upload_files4G(char* data, const char* path){
  const char* res = "";
  const char* res2 = "";

  //
    //long image_buf_size = 4000 * 1000;                                                  
	//char *image = (char *)ps_calloc(image_buf_size, sizeof(char));
  
  //40000
  Serial.println("Load data");

  int u = 0;
  int j = 'a';
  for(int i =0; i< 15*1024; i++)
  {	  	  
	  VALUE[u] = j; 
	  u++;
	  j++;
	  if( j >'z') j ='a';	  
  }  
  VALUE[u] = '\0';

  Serial.print("total byte = ");
  Serial.println(u);
  
  iotClient.setAWSPath(path);

  Serial.print("File name ");
  Serial.println(path);
  
  /* AmazonIOTClient.cpp / ESPAWSImplementations.cpp*/
  
  res = iotClient.update_shadow(VALUE, actionError); 

//   Serial.println("\n --------Response From server-------\n");
  Serial.println("\n --------Request-------\n");
  Serial.println(res);
  Serial.println("\n --------end Request-------");
  
  res2 = testsend4G(res,"s3.altacloud.biz",443);
  
    
  Serial.println("\n --------Response From server-------\n");
  Serial.println(res2);
  Serial.println("\n --------end response-------");
  
  Serial.println("end");
  return res; 
}

const char* upload_files1(char* data, const char* path){
  const char* res = "";
  const char* res2 = "";

  //
    //long image_buf_size = 4000 * 1000;                                                  
	//char *image = (char *)ps_calloc(image_buf_size, sizeof(char));
  
  //40000
  Serial.println("Load data q");
  int u = 0;
  int j = 'a';
  for(int i =0; i< 15*1024; i++)
  {	  	  
	  VALUE[u] = j; 
	  u++;
	  j++;
	  if( j >'z') j ='a';	  
  }  
  VALUE[u] = '\0';
  
  Serial.print("total byte = ");
  Serial.println(u);
  
  iotClient.setAWSPath(path);

  Serial.print("File name ");
  Serial.println(path);
  
  /* AmazonIOTClient.cpp / ESPAWSImplementations.cpp*/
  
  res = iotClient.update_shadow(VALUE, actionError); 

//   Serial.println("\n --------Response From server-------\n");
  Serial.println("\n --------Request-------\n");
  Serial.println(res);
  Serial.println("\n --------end Request-------");
  
  res2 = testsend(res,"s3.altacloud.biz",443);
  
    
  Serial.println("\n --------Response From server-------\n");
  Serial.println(res2);
  Serial.println("\n --------end response-------");
  
  Serial.println("end");
  return res; 
}

int upload_files_new(char* data, const char* path, int len){
  const char* res = "";
  const char* res2 = "";
	int answer=0;
  int u = 0;

  u = strlen(data);
  Serial.print("size to upload = ");
  Serial.println(u);
  
  iotClient.setAWSPath(path);

  Serial.print("File name ");
  Serial.println(path);
  
  /* AmazonIOTClient.cpp / ESPAWSImplementations.cpp*/
  
  res = iotClient.update_shadow(data, actionError);
  
  
//   res2 = testsend(res,"s3.altacloud.biz",443);
  res2 = upload_file_to_server(res,"s3.altacloud.biz",443);
  
    
  Serial.println("\n --------Response From server-------\n");
  Serial.println(res2);
  Serial.println("\n --------end response-------");
  
  
  Serial.println("end");

  
  if (strstr(res2, "HTTP/1.1 200 OK") != NULL) {
        answer = 1;
      }
  res ="";

  return answer;
}


#if (0) 
int upload_files_new(char* data, const char* path, int len){
  const char* res = "";
	int answer=0;
  int u = 0;

  u = strlen(data);
  Serial.print("size to upload = ");
  Serial.println(u);
  
  iotClient.setAWSPath(path);

  Serial.print("File name ");
  Serial.println(path);
  
  /* AmazonIOTClient.cpp / ESPAWSImplementations.cpp*/
  
  res = iotClient.update_shadow(data, actionError);
//   res = iotClient.update_shadow_kichthuoc(data, actionError,len);
  
  
  Serial.println("\n --------Response From server-------\n");
  Serial.println(res);
  Serial.println("\n --------end response-------");
  Serial.println("end");

  
  if (strstr(res, "HTTP/1.1 200 OK") != NULL) {
        answer = 1;
      }
  res ="";

  return answer;
}
#endif 

void debug_print_data(char* data_, size_t leng_)
{
   	int m=0;
   Serial.print("**---debug print data, len = ");
   Serial.println(leng_);
#if (0) 
   for (int i = 0; i < leng_; i+=2) {
     Serial.printf("%c%c",(byte)data_[i],(byte)data_[i+1]);
    m++;
	if (m >19)
	{
	  m=0;
	  Serial.println("");
	}	
   }
#endif 
   Serial.println("\n -----end debug print data--------");
}
void debug_raw_data(char* data_, size_t leng_)
{
   	int m=0;
   Serial.print("**---debug print raw data, len = ");
   Serial.println(leng_);
#if (0) 
   for (int i = 0; i < leng_; i++) {
     Serial.printf("%02X",data_[i]);
    m++;
	if (m >19)
	{
	  m=0;
	  Serial.println("");
	}	
   }
#endif 
   Serial.println("\n -----end debug print raw data--------");
}

#if(DF_USING_EXT_FLASH)
uint32_t getkichthuoc(uint8_t stt_)
{
	Serial.println(F("--------- getkichthuoc ext flash-----------------------"));
	int err_ = read_header_index(upload_manager_info.mid, &header_file_doing);
	inval(&header_file_doing);
	
	if (err_ ==1 )
	{
		header_file_doing.length =0;
		Serial.println("size get : ERRO");
	}
	if(header_file_doing.length > 200*1024)
	{
		Serial.println("size get : Too large");
		header_file_doing.length =0;
	}
	
	Serial.print("size get = ");
	Serial.println(header_file_doing.length);
	return header_file_doing.length;
}
#else
uint32_t getkichthuoc(uint8_t stt_)
{
	uint32_t  t = 0;
	Serial.println(F("--------- getkichthuoc -----------------------"));
	char namefile[30];
	sprintf(namefile,"/spiffs/bill_%d.dat",stt_);
	
	Serial.print("name file = ");
	Serial.println(namefile);
	
	// FILE* readStorage=fopen("/spiffs/storage.dat", "rb");
	FILE* readStorage=fopen(namefile, "rb");
	
	if (readStorage == NULL) {
		Serial.println("Failed to open file for reading");
	}
	else
	{
		fseek(readStorage,0,SEEK_END);
		t = ftell(readStorage);
	}
	Serial.print("size get = ");
	Serial.println(t);
	return t;
}
#endif 

uint32_t getszie(String name_)
{
	uint32_t  t = 0;
	Serial.println(F("--------- getszie -----------------------"));
	// char namefile[100];
	char namefile[100];
	
	Serial.print("name input = ");
	Serial.println(name_);
	
	sprintf(namefile,"/spiffs%s",name_);
	
	Serial.print("name output = ");
	Serial.println(namefile);
	
	// FILE* readStorage=fopen("/spiffs/storage.dat", "rb");
	FILE* readStorage=fopen(namefile, "rb");
	
	if (readStorage == NULL) {
		Serial.println("Failed to open file for reading");
	}
	else
	{
		fseek(readStorage,0,SEEK_END);
		t = ftell(readStorage);
	}
	Serial.print("size get = ");
	Serial.println(t);
	return t;
}


int get_files_part(char* dataBuffer, int part_)
{
 
	Serial.println(F("--------- get_files_part -----------------------"));

	int index_up =0;
	int len;
/*
part_	start	len
0		0		15
1		15		15
2		30		15
.
*/
	
	int start = DF_NUM_OF_SAPERATE_PACK*part_;
	int end =start + DF_NUM_OF_SAPERATE_PACK -1;
	int reslen =0;
	
	char buff[3];
	memset(dataBuffer,'\0',(max_packagesize_upload*2));//8192
	// char buf_temp_[max_packagesize_upload +2];//4098
	
	char buf_temp_[DF_NUM_OF_BYTES_READ + 2];//
	size_t number_data_get = DF_NUM_OF_BYTES_READ;//mỗi lần đọc 512 byte

	char namefile[30];
	sprintf(namefile,"/spiffs/bill_%d.dat",upload_manager_info.mid);

	Serial.print("file =");
	Serial.println(namefile);
	
	// FILE* readStorage=fopen("/spiffs/storage.dat", "rb");
	FILE* readStorage=fopen(namefile, "rb");

	// size_t number_data_get = max_packagesize_upload;//4096
	

	if (readStorage == NULL) {
			Serial.println("Failed to open file for reading");
	}
	else
	{
		while(!feof(readStorage)) {

			len = fread(buf_temp_, 1, number_data_get, readStorage);
			buf_temp_[len]='\0';			
			
			if (index_up > end)
			{
				Serial.print("Total @len =");
				Serial.println(reslen);
				break;
			}
			else if (index_up >= start)
			{				
				// Serial.print("Get @len =");
				// Serial.println(len);
				//convert to text file
				for(int i =0;i<len;i++)
				{
					sprintf(buff,"%02X",buf_temp_[i]);		
					dataBuffer[reslen + 2*i]=buff[0];
					dataBuffer[reslen + 2*i +1 ]=buff[1];
				}
				len = len*2;
				reslen +=len;
				dataBuffer[reslen] = '\0';
			}

			index_up++;
		}
		
		fclose(readStorage);
	}
	
	
	Serial.print("\n return @len =");
	Serial.println(reslen);
  
	Serial.println(F("---------End get_files_part -----------------------"));
	return reslen;
}

int get_lost_chunk(char* dataBuffer, int part_, int tong_, HEADER_FILE hdtam)
{
 
	Serial.println(F("\n --------- get_lost_chunk-----------------------"));

	uint32_t start = max_packagesize_uploadmqtt*part_;
	uint32_t end =start + max_packagesize_uploadmqtt -1;
	int reslen =0;
	
	uint8_t buf_temp_[256];
	memset(dataBuffer,'\0',(max_packagesize_uploadmqtt));

	/* mở file*/
	uint32_t len_remain = hdtam.length - start;
	
	Serial.print("\n file size = ");
	Serial.println(hdtam.length);
	
	start += hdtam.addr;
	Serial.print("start addr = ");
	Serial.println(start);
	Serial.print("len_remain = ");
	Serial.println(len_remain);
	
	
	if(len_remain > max_packagesize_uploadmqtt -1)
	{
		len_remain = max_packagesize_uploadmqtt;
	}

	Serial.print("len_remain new= ");	
	Serial.println(len_remain);

	/*xử lý đọc file*/

	
	uint32_t number_data_get =0;
	uint32_t number_data_hientai =0;
	Serial.println("\n --------------Get data start ----------------------");

while (number_data_hientai < len_remain)
{
	number_data_get = len_remain - number_data_hientai;
	if (number_data_get > 256)number_data_get =256;

	if(number_data_get)
	{
		read_data(start, &(buf_temp_[0]),number_data_get);
		start += number_data_get;
		for(int i =0;i<number_data_get;i++)
		{	
			dataBuffer[number_data_hientai + i]=buf_temp_[i];
			// dataBuffer[reslen + 2*i +1 ]=buff[1];
		}

		#if (DF_DEBUG_PRINT_EXT_FLASH)
		_printPageBytes(buf_temp_, 1, number_data_get);
		#endif 
		number_data_hientai +=number_data_get;
	}
}
  	
	reslen = len_remain;
	Serial.print("\n return @len =");
	Serial.println(reslen);
  
	Serial.println(F("---------End get_lost_chunk-----------------------"));
	return reslen;
}


#if (DF_UPFILE_USING_MQTT)

#if (DF_USING_EXT_FLASH)

int get_files_part_mqtt(char* dataBuffer, int part_, int tong_)
{
 
	Serial.println(F("\n --------- get_files_part mqtt ext flash-----------------------"));

	/*kiểm tra sơ bộ*/
	if (header_file_doing.length==0) return 0;
	
	uint32_t start = max_packagesize_uploadmqtt*part_;
	uint32_t end =start + max_packagesize_uploadmqtt -1;
	int reslen =0;
	
	uint8_t buf_temp_[256];
	memset(dataBuffer,'\0',(max_packagesize_uploadmqtt));

	/* mở file*/
	if (header_file_doing.length < start)
	{
		return 0;
	}
	
	uint32_t len_remain = header_file_doing.length - start;
	
	Serial.print("\n file size = ");
	Serial.println(header_file_doing.length);
	
	
	start += header_file_doing.addr;
	
	Serial.print("start addr = ");
	Serial.println(start);


	Serial.print("len_remain = ");
	Serial.println(len_remain);
	
	
	if(len_remain > max_packagesize_uploadmqtt -1)
	{
		len_remain = max_packagesize_uploadmqtt;
	}

	Serial.print("len_remain new= ");	
	Serial.println(len_remain);

	/*xử lý đọc file*/

	
	uint32_t number_data_get =0;
	uint32_t number_data_hientai =0;
Serial.println("\n --------------Get data start ----------------------");

while (number_data_hientai < len_remain)
{
	number_data_get = len_remain - number_data_hientai;
	if (number_data_get > 256)number_data_get =256;

	if(number_data_get)
	{
		read_data(start, &(buf_temp_[0]),number_data_get);
		start += number_data_get;/*next data addr*/

		for(int i =0;i<number_data_get;i++)
		{	
			dataBuffer[number_data_hientai + i]=buf_temp_[i];
			// dataBuffer[reslen + 2*i +1 ]=buff[1];
		}

		#if (DF_DEBUG_PRINT_EXT_FLASH)
		_printPageBytes(buf_temp_, 1, number_data_get);
		#endif 

		number_data_hientai +=number_data_get;
	}
	else
	{
		break;
	}
}
  	
	reslen = len_remain;
	Serial.print("\n return @len =");
	Serial.println(reslen);
  
	Serial.println(F("---------End get_files_part mqtt-----------------------"));
	return reslen;
}
#else
int get_files_part_mqtt(char* dataBuffer, int part_, int tong_)
{
 
	Serial.println(F("\n --------- get_files_part mqtt-----------------------"));

	int index_up =0;
	int len =0 ;
	// last_len_upload =0;
	/*
	part_	start	len
	0		0		15
	1		15		15
	2		30		15
	.
	*/
	
	int start = DF_NUM_OF_SAPERATE_PACKmqtt*part_;
	int end =start + DF_NUM_OF_SAPERATE_PACKmqtt -1;
	int reslen =0;
	
	char buff[3];
	memset(dataBuffer,'\0',(max_packagesize_uploadmqtt));

#if (0) 
	int t =0;
	int num_part =0;
	for (int i =0; i<100;i++)
	{		
		 if(t < tong_)
		 {
			num_part++;
			t += max_packagesize_uploadmqtt;
			// SerialDEBUG.print(F("@i =  "));
    		// SerialDEBUG.print(i);
			// SerialDEBUG.print(F(", num_part =  "));
			// SerialDEBUG.print(num_part);    		
		 }
		 else
		 {
			break;
		 }
	}



	//sprintf(ten,"/esp-iot-2023/printer_device_test/%s_%d.txt",upload_manager_info.nameofbill, updatepart);	

	DynamicJsonDocument doc(1024);
	doc["part"]	= (part_ +1 );
	doc["total"] = num_part;
	doc["bill"] = upload_manager_info.nameofbill;
	doc["data"] = 1;
	
	serializeJson(doc,Serial);
	Serial.println("");
	// char dataBuffer[1024];
	serializeJson(doc,dataBuffer,1024);

	reslen = strlen(dataBuffer);
	/*
	
	{"code":"0056","time":"20231219123639","data":1}
	
	*/

	reslen --;/*giảm 1: trỏ tới dấu } */
	reslen --;/*giảm 1: trỏ tới số 1 */
#endif 

	// char buf_temp_[max_packagesize_upload +2];//4098
	
	char buf_temp_[DF_NUM_OF_BYTES_READ + 2];//
	size_t number_data_get = DF_NUM_OF_BYTES_READ;//mỗi lần đọc 512 byte

	char namefile[30];
	sprintf(namefile,"/spiffs/bill_%d.dat",upload_manager_info.mid);

	Serial.print("file =");
	Serial.println(namefile);
	
	// FILE* readStorage=fopen("/spiffs/storage.dat", "rb");
	FILE* readStorage=fopen(namefile, "rb");

	// size_t number_data_get = max_packagesize_upload;//4096
	

	if (readStorage == NULL) {
			Serial.println("Failed to open file for reading");
	}
	else
	{
		while(!feof(readStorage)) {

			len = fread(buf_temp_, 1, number_data_get, readStorage);
			buf_temp_[len]='\0';			
			
			if (index_up > end)
			{
				Serial.print("Total @len =");
				Serial.println(reslen);
				break;
			}
			else if (index_up >= start)
			{				
				// Serial.print("Get @len =");
				// Serial.println(len);
				//convert to text file
				for(int i =0;i<len;i++)
				{
					// sprintf(buff,"%02X",buf_temp_[i]);		
					dataBuffer[reslen + i]=buf_temp_[i];
					// dataBuffer[reslen + 2*i +1 ]=buff[1];
				}
				// len = len*2;
				// last_len_upload+=len;
				reslen +=len;
				dataBuffer[reslen] = '\0';
			}

			index_up++;
		}
		
		fclose(readStorage);
	}
	// dataBuffer[reslen] = '}';
	// reslen++;
	dataBuffer[reslen] = '\0';
	
	// if (last_len_upload ==0)reslen =0;

	Serial.print("\n return @len =");
	Serial.println(reslen);
  
	Serial.println(F("---------End get_files_part mqtt-----------------------"));
	return reslen;
}

#endif 
#endif 

#if (0) 
int get_files_part(char* dataBuffer, int part_)
{
 
	Serial.println(F("--------- get_files_part -----------------------"));

	int index_up =0;
	int len;
	const char* res = "";
	// char buf_temp_[max_packagesize_upload +2];//4098

	char namefile[30];
	sprintf(namefile,"/spiffs/bill_%d.dat",upload_manager_info.mid);
	
	// FILE* readStorage=fopen("/spiffs/storage.dat", "rb");
	FILE* readStorage=fopen(namefile, "rb");

	size_t number_data_get = max_packagesize_upload;//4096

	if (readStorage == NULL) {
			Serial.println("Failed to open file for reading");
	}
	else
	{
		while(!feof(readStorage)) {

			len = fread(buf_temp_, 1, number_data_get, readStorage);
			buf_temp_[len]='\0';			
			if (index_up == part_)
			{
				Serial.print("Get @len =");
				Serial.println(len);
				break;
			}

			index_up++;
		}
		
		fclose(readStorage);
	}
	//convert to text file
	char buff[3];
	memset(dataBuffer,'\0',(max_packagesize_upload*2));//8192
	for(int i =0;i<len;i++)
	{
		sprintf(buff,"%02X",buf_temp_[i]);		
		dataBuffer[2*i]=buff[0];
		dataBuffer[2*i +1 ]=buff[1];
	}
	len =len*2;
	dataBuffer[len] = '\0';
	Serial.print("\n return @len =");
	Serial.println(len);
  
 Serial.println(F("---------End get_files_part -----------------------"));
 return len;
}
#endif 


int get_files_headder(char* dataBuffer, int part_)
{
 
	Serial.println(F("--------- get_files_headder -----------------------"));

	int index_up =0;
	int len;
	const char* res = "";
	// char buf_temp_[4098];
	mqtt_BIll_HEADDER bill_media_info_temp; 
	char namefile[30];
	sprintf(namefile,"/bill_%d.h",upload_manager_info.mid);
	Serial.print("file =");
	Serial.println(namefile);
	//Reading file
    File f = SPIFFS.open(namefile,  "r");  //r+

  int s = 0;
//if File Exists
  if (f) {
    s = f.size();

    // SerialDEBUG.printf("File Opened , Size=%d\r\n", s);
	//Read    
      // *value = f.readString();
	// SerialDEBUG.println(F("[FILE SYS]:Read file"));  
	size_t size = f.size();
	// Allocate a buffer to store contents of the file.
	std::unique_ptr<char[]> buf(new char[size]);
	f.readBytes(buf.get(), size);
		
	DynamicJsonDocument  jsonBuffer(1024);
	deserializeJson(jsonBuffer,buf.get());
	#ifdef use_debug_serial
	SerialDEBUG.print(F("[*FILE]: \t"));
	serializeJson(jsonBuffer,SerialDEBUG);  	
	SerialDEBUG.println(F(""));
	#endif 		
	if (!jsonBuffer.isNull()) {

		bill_media_info_temp.sizedata = jsonBuffer["sizedata"];
		
		strlcpy(bill_media_info_temp.nameofbill,                  // <- destination
		jsonBuffer["name"] | "Unknow",  // <- source
		sizeof(bill_media_info_temp.nameofbill));         // <- destination's capacity

		bill_media_info_temp.nameofqcode = jsonBuffer["code"].as<String>();
		
		f.close();
	} else {
		#ifdef use_debug_serial
	SerialDEBUG.println(F("[FILE SYS]:File incorrect format"));		
	#endif 
		// SerialDEBUG.println(F("[FILE SYS]:Load default"));
		f.close();
		// load_oph_media_default(file,regs);		
		#ifdef use_debug_serial
	SerialDEBUG.println(F("\n\r[FILE]:Done"));
	#endif 
	}  
  } 
  else 
  {
    #ifdef use_debug_serial
	SerialDEBUG.print(F("[FILE SYS]:Failed to open file for reading "));
    SerialDEBUG.println(file);
	#endif 	

  }

	//tạo json file

	// Allocate the JSON document
	//
	// Inside the brackets, 200 is the RAM allocated to this document.
	// Don't forget to change this value to match your requirement.
	// Use arduinojson.org/v6/assistant to compute the capacity.
	DynamicJsonDocument doc(1024);					
	// StaticJsonObject allocates memory on the stack, it can be
	// replaced by DynamicJsonDocument which allocates in the heap.
	//
	// DynamicJsonDocument  doc(200);
	doc["size"]=bill_media_info_temp.sizedata;
	doc["name"]=String(bill_media_info_temp.nameofbill);
	doc["code"]=String(bill_media_info_temp.nameofqcode);

	//cal nuber of part
	uint32_t t=0;
	uint8_t num_part=0;

	SerialDEBUG.println(F("cal number of part "));
	SerialDEBUG.println(F("Total size =  "));
    SerialDEBUG.println(bill_media_info_temp.sizedata);

	for (int i =0; i<100;i++)
	{
		
		 if(t < bill_media_info_temp.sizedata)
		 {
			num_part++;
			t += max_packagesize_upload;
			// SerialDEBUG.print(F("@i =  "));
    		// SerialDEBUG.print(i);
			// SerialDEBUG.print(F(", num_part =  "));
			// SerialDEBUG.print(num_part);    		
		 }
		 else
		 {
			break;
		 }
	}

	num_part++;
	totalpart= num_part;
	// SerialDEBUG.print(F(", final part =  "));
	// SerialDEBUG.print(num_part); 

	doc["part"]=num_part; /*tính luôn cả part 0 header*/

	#ifdef use_debug_serial
	SerialDEBUG.println(F("[FILE]:lmd_msg= "));
    SerialDEBUG.println(regs->lmd_msg);
	SerialDEBUG.println(F("[FILE]:end lmd_msg= "));
	#endif

	
	//create file		
	// Generate the minified JSON and send it to the SerialDEBUG port.
	//memset(dataBuffer,'\0',4096);
	serializeJson(doc,Serial);
	Serial.println("");

	serializeJson(doc,dataBuffer,1024);

	len = strlen(dataBuffer);
	
	Serial.print("\n header send = ");
	Serial.println(len);	
	Serial.println(dataBuffer);	
	
 
 #if (0)
	 
  File readStorage = SPIFFS.open("/storage.dat", "rb");
 if (readStorage) {
   Serial.print("size of file = ");
   Serial.println(readStorage.size());
   Serial.println("--------------");
   
   byte buffer[readStorage.size()];
   
   Serial.println("line 363");
   
   size_t bufferSize = readStorage.readBytes((char*)buffer,readStorage.size());
      
  
   	int m=0;
   for (int i = 0; i < bufferSize; i++) {
     Serial.printf("%02x ",(byte)buffer[i]);
    m++;
	if (m >19)
	{
	  m=0;
	  Serial.println("");
	}
	
	if(buffer[i] == 0xff)
	{
		m=0;
		Serial.println("\n");
	}
   }
   Serial.println(""); 

   readStorage.close();
 }
 #endif
 
 Serial.println(F("---------End get_files_headder -----------------------"));
 return len;
}
#if (DF_USING_EXT_FLASH)
int get_files_headder_mqtt(char* dataBuffer, int part_)
{
	Serial.println(F("--------- get_files_headder_mqtt ext flash-----------------------"));

	int len;
	
	int err_ = read_header_index(upload_manager_info.mid, &header_file_doing);
	inval(&header_file_doing);

	if (err_ ==1 ) return 0;


	//tạo json file

	// Allocate the JSON document
	//
	// Inside the brackets, 200 is the RAM allocated to this document.
	// Don't forget to change this value to match your requirement.
	// Use arduinojson.org/v6/assistant to compute the capacity.
	DynamicJsonDocument doc(1024);					
	// StaticJsonObject allocates memory on the stack, it can be
	// replaced by DynamicJsonDocument which allocates in the heap.
	//
	// DynamicJsonDocument  doc(200);
	// doc["size"]=bill_media_info_temp.sizedata;
	// doc["code"]=String(bill_media_info_temp.nameofqcode);
	doc["code"]="";
	doc["name"]=String(header_file_doing.name);	
	doc["time"]=String(header_file_doing.name+3);	
	doc["printerModel"] = String(header_file_doing.model);	//Product
	doc["printerBrand"] = String(header_file_doing.brand_name);	//Manufacturer

	/*
	cal number of part mqtt
	-không bao gồm part 0
	-chỉ gồm part data printer	
	*/
	
	uint32_t t=0;
	uint8_t num_part=0;

	SerialDEBUG.println(F("cal number of part "));
	SerialDEBUG.println(F("Total size =  "));
    SerialDEBUG.println(header_file_doing.length);

	for (int i =0; i<100;i++)
	{		
		 if(t < header_file_doing.length)
		 {
			num_part++;
			t += max_packagesize_uploadmqtt;
			// SerialDEBUG.print(F("@i =  "));
    		// SerialDEBUG.print(i);
			// SerialDEBUG.print(F(", num_part =  "));
			// SerialDEBUG.print(num_part);    		
		 }
		 else
		 {
			break;
		 }
	}

	// num_part++; /*S3 tính luôn cả part 0 header*/	
	/*mqtt không bao gồm header*/

	totalpart= num_part;
	// SerialDEBUG.print(F(", final part =  "));
	// SerialDEBUG.print(num_part); 

	doc["part"]=num_part; 

	strncpy(upload_manager_info.nameofbill,header_file_doing.name, sizeof(header_file_doing.name));

	#ifdef use_debug_serial
	SerialDEBUG.println(F("[FILE]:lmd_msg= "));
    SerialDEBUG.println(regs->lmd_msg);
	SerialDEBUG.println(F("[FILE]:end lmd_msg= "));
	#endif

	
	//create file		
	// Generate the minified JSON and send it to the SerialDEBUG port.
	//memset(dataBuffer,'\0',4096);
	serializeJson(doc,Serial);
	Serial.println("");

	serializeJson(doc,dataBuffer,1024);

	len = strlen(dataBuffer);
	
	Serial.print("\n header send = ");
	Serial.println(len);	
	Serial.println(dataBuffer);	

	Serial.println(F("---------End get_files_headder -----------------------"));
	return len;

}
#else
int get_files_headder_mqtt(char* dataBuffer, int part_)
{
 
	Serial.println(F("--------- get_files_headder_mqtt -----------------------"));

	int index_up =0;
	int len;
	const char* res = "";
	// char buf_temp_[4098];
	mqtt_BIll_HEADDER bill_media_info_temp; 
	char namefile[30];
	sprintf(namefile,"/bill_%d.h",upload_manager_info.mid);
	Serial.print("file =");
	Serial.println(namefile);
	//Reading file
    File f = SPIFFS.open(namefile,  "r");  //r+

  int s = 0;
//if File Exists
  if (f) {
    s = f.size();

    // SerialDEBUG.printf("File Opened , Size=%d\r\n", s);
	//Read    
      // *value = f.readString();
	// SerialDEBUG.println(F("[FILE SYS]:Read file"));  
	size_t size = f.size();
	// Allocate a buffer to store contents of the file.
	std::unique_ptr<char[]> buf(new char[size]);
	f.readBytes(buf.get(), size);
		
	DynamicJsonDocument  jsonBuffer(1024);
	deserializeJson(jsonBuffer,buf.get());
	#ifdef use_debug_serial
	SerialDEBUG.print(F("[*FILE]: \t"));
	serializeJson(jsonBuffer,SerialDEBUG);  	
	SerialDEBUG.println(F(""));
	#endif 		
	if (!jsonBuffer.isNull()) {

		bill_media_info_temp.sizedata = jsonBuffer["sizedata"];
		
		strlcpy(bill_media_info_temp.nameofbill,                  // <- destination
		jsonBuffer["name"] | "Unknow",  // <- source
		sizeof(bill_media_info_temp.nameofbill));         // <- destination's capacity

		bill_media_info_temp.nameofqcode = jsonBuffer["code"].as<String>();
		
		f.close();
	} else {
		#ifdef use_debug_serial
	SerialDEBUG.println(F("[FILE SYS]:File incorrect format"));		
	#endif 
		// SerialDEBUG.println(F("[FILE SYS]:Load default"));
		f.close();
		// load_oph_media_default(file,regs);		
		#ifdef use_debug_serial
	SerialDEBUG.println(F("\n\r[FILE]:Done"));
	#endif 
	}  
  } 
  else 
  {
    #ifdef use_debug_serial
	SerialDEBUG.print(F("[FILE SYS]:Failed to open file for reading "));
    SerialDEBUG.println(file);
	#endif 	

  }

	//tạo json file

	// Allocate the JSON document
	//
	// Inside the brackets, 200 is the RAM allocated to this document.
	// Don't forget to change this value to match your requirement.
	// Use arduinojson.org/v6/assistant to compute the capacity.
	DynamicJsonDocument doc(1024);					
	// StaticJsonObject allocates memory on the stack, it can be
	// replaced by DynamicJsonDocument which allocates in the heap.
	//
	// DynamicJsonDocument  doc(200);
	// doc["size"]=bill_media_info_temp.sizedata;
	doc["code"]=String(bill_media_info_temp.nameofqcode);
	doc["name"]=String(bill_media_info_temp.nameofbill);	
	doc["time"]=String(bill_media_info_temp.nameofbill+3);
	doc["printerModel"] = "TM-T88IV";	//Product
	doc["printerBrand"] = "EPSON";	//Manufacturer

	/*
	cal number of part mqtt
	-không bao gồm part 0
	-chỉ gồm part data printer	
	*/
	
	uint32_t t=0;
	uint8_t num_part=0;

	SerialDEBUG.println(F("cal number of part "));
	SerialDEBUG.println(F("Total size =  "));
    SerialDEBUG.println(bill_media_info_temp.sizedata);

	for (int i =0; i<100;i++)
	{		
		 if(t < bill_media_info_temp.sizedata)
		 {
			num_part++;
			t += max_packagesize_uploadmqtt;
			// SerialDEBUG.print(F("@i =  "));
    		// SerialDEBUG.print(i);
			// SerialDEBUG.print(F(", num_part =  "));
			// SerialDEBUG.print(num_part);    		
		 }
		 else
		 {
			break;
		 }
	}

	// num_part++; /*S3 tính luôn cả part 0 header*/	
	/*mqtt không bao gồm header*/

	totalpart= num_part;
	// SerialDEBUG.print(F(", final part =  "));
	// SerialDEBUG.print(num_part); 

	doc["part"]=num_part; 

	#ifdef use_debug_serial
	SerialDEBUG.println(F("[FILE]:lmd_msg= "));
    SerialDEBUG.println(regs->lmd_msg);
	SerialDEBUG.println(F("[FILE]:end lmd_msg= "));
	#endif

	
	//create file		
	// Generate the minified JSON and send it to the SerialDEBUG port.
	//memset(dataBuffer,'\0',4096);
	serializeJson(doc,Serial);
	Serial.println("");

	serializeJson(doc,dataBuffer,1024);

	len = strlen(dataBuffer);
	
	Serial.print("\n header send = ");
	Serial.println(len);	
	Serial.println(dataBuffer);	
	
 
 #if (0)
	 
  File readStorage = SPIFFS.open("/storage.dat", "rb");
 if (readStorage) {
   Serial.print("size of file = ");
   Serial.println(readStorage.size());
   Serial.println("--------------");
   
   byte buffer[readStorage.size()];
   
   Serial.println("line 363");
   
   size_t bufferSize = readStorage.readBytes((char*)buffer,readStorage.size());
      
  
   	int m=0;
   for (int i = 0; i < bufferSize; i++) {
     Serial.printf("%02x ",(byte)buffer[i]);
    m++;
	if (m >19)
	{
	  m=0;
	  Serial.println("");
	}
	
	if(buffer[i] == 0xff)
	{
		m=0;
		Serial.println("\n");
	}
   }
   Serial.println(""); 

   readStorage.close();
 }
 #endif
 
 Serial.println(F("---------End get_files_headder -----------------------"));
 return len;
}
#endif 


void process_savefile()
{
		
		// save_bill_info();//save header
		
		//reset info
		bill_media_info.status =0;
		bill_media_info.sizedata =0;
		bill_media_info.nameofbill[0] ='\0';
		bill_media_info.nameofqcode ="";

		//save total file status
		upload_manager_info.tot = stt_storing_file;

		// spiff_updatemediafile("/up_manage.h", &upload_manager_info,1);
		spiff_updatemediafile(name_up_manager, &upload_manager_info,1);

		stt_storing_file++; //next to save file
		if(stt_storing_file > max_file_save) stt_storing_file =0;

		u32_timer_rec_printer_data = millis() + 5000;//sau 5s cho upload// tránh 2 file in liên tiếp
}


/*
kiểm tra lại chỗ này:
nếu đang up, mà bị dừng giữa chừng
*/
void check_file_to_upload()
{
	if(ui8_need_upload ==1)//đang trong giai đoạn upfile
	{		
		return;
	}

	//kiểm tra xem có file cần up không
	if(upload_manager_info.mid < upload_manager_info.tot)
	{
		// save 1st upload
		totalkichthuoc =0;
		updatepart=0;
		totalup=0;
		ui8_need_upload =1;

		upload_manager_info.mid++;
				
		upload_manager_info.part=updatepart;
		upload_manager_info.duration=totalup;
		upload_manager_info.mode = ui8_need_upload;
		
		get_name_bill(upload_manager_info.mid);

		// spiff_updatemediafile("/up_manage.h", &upload_manager_info,1);
		spiff_updatemediafile(name_up_manager, &upload_manager_info,1);
		
		ui16_counter_up =100;
	}
	
	/*else if(upload_manager_info.mid > upload_manager_info.tot)
	{
		// scanfile
		upload_manager_info.mid++;
		if(upload_manager_info.mid > max_file_save)upload_manager_info.mid =0;
		
		// save 1st upload
		totalkichthuoc =0;
		updatepart=0;
		totalup=0;
		ui8_need_upload =1;

		upload_manager_info.part=updatepart;
		upload_manager_info.duration=totalup;
		upload_manager_info.mode = ui8_need_upload;
		get_name_bill(upload_manager_info.mid);		

		spiff_updatemediafile("/up_manage.h", &upload_manager_info,1);
		
		ui16_counter_up =100;
	}
	*/
}

void process_upfile()
{
	//check busy
	if(ui8_busy_rec_printer == 1) return;
	
	//kiểm tra qcode
	// if (ui8_newfile_wait_check_qcode == 1) return;
		
	check_file_to_upload();

	if(ui8_need_upload ==0)
	{
		//check file to upload
		return;
	}

	if (ui16_counter_up <30)return;

	//kiểm tra kết nối

	// if(WiFi.status() != WL_CONNECTED) return;

	int len;	
	
	char ten[200];

	if (totalkichthuoc ==0 )
	{		
		totalkichthuoc = getkichthuoc(upload_manager_info.mid);
		upload_manager_info.sizedata = totalkichthuoc;
	}
		
	Serial.print("get part");
	Serial.println(updatepart);

	int kq =1;
 
	if (updatepart==0)
	{
		len = get_files_headder(VALUE,updatepart);
	}
	else
	{	
		len = get_files_part(VALUE,(updatepart -1));
	}

	debug_print_data(VALUE,len);
	// SerialDebug.println("\n\n-----------debug nen co dong nay 3380------------\n\n");
	// len =0;
	
	#if (DF_DEBUG_NOUPLOAD_TO_SERVER > 0 )
		SerialDebug.println("\n\n!!!!!!!!!!!!!!!!!debug no update to server 3466!!!!!!!!!!!!!\n\n");
		len =0;
	#endif 
	
	
	/* start upload file to s3 */
	if (len >0)
	{
		// sprintf(ten,"/esp-iot-2023/printer_device_test/%s_%d.txt",upload_manager_info.nameofbill, updatepart);		
		sprintf(ten,"/esp-iot-2023/%s/%s_%d.txt",mynamedevice, upload_manager_info.nameofbill, updatepart);
		
		kq = upload_files_new(VALUE, ten, len);

		// kq =1 ; debug
		
		if (kq ==1)
		{
			Serial.println("up ok");
			ui16_counterfail =0;
			
			len/=2;
			
			if(updatepart == 0)
			{
				Serial.println("header: no increase data up");
			}
			else
			{
				totalup +=len;
			}

			updatepart ++;
#if (0) 
			SerialDebug.printf("updatepart = %d \n", updatepart);
			SerialDebug.printf("total part = %d \n", totalpart);
			SerialDebug.printf("totalup = %d \n", totalup);
			SerialDebug.printf("totalkichthuoc = %d \n", totalkichthuoc);
#endif 

			if (totalup >= totalkichthuoc)
			{
				Serial.print("Finish");
				totalkichthuoc =0;
				updatepart=0;
				totalup=0;
				ui8_need_upload =0;
				ui8_qcode_complete = 0;			
				// strcpy(upload_manager_info.nameofbill,"BILLFINISHED");
			}
			
			upload_manager_info.part=updatepart;
			upload_manager_info.duration=totalup;
			upload_manager_info.mode = ui8_need_upload;

			
			// spiff_updatemediafile("/up_manage.h", &upload_manager_info,1);
			spiff_updatemediafile(name_up_manager, &upload_manager_info,1);
			ui16_counter_up = 20;
		}
		else
		{
			Serial.println("up fail");
			ui16_counter_up = 0;
			ui16_counterfail++;
			if(ui16_counterfail > 1)
			{
				//reset
				Serial.println("-------------restart here------");
				ESP.restart();
				delay(2000);
			}
		}
	}
	else
	{		
		//finish upload
		Serial.println("finish here");
		Serial.print("totalup = ");
		Serial.println(totalup);
		Serial.print("totalkichthuoc = ");
		Serial.println(totalkichthuoc);

		ui16_counterfail =0;
		Serial.print("Finish");
		totalkichthuoc =0;
		updatepart=0;
		totalup=0;
		ui8_need_upload =0;
		ui8_qcode_complete = 0;	
		// strcpy(upload_manager_info.nameofbill,"BILLFINISHED");			
		upload_manager_info.part=updatepart;
		upload_manager_info.duration=totalup;
		upload_manager_info.mode = ui8_need_upload;	

		
		// spiff_updatemediafile("/up_manage.h", &upload_manager_info,1);
		spiff_updatemediafile(name_up_manager, &upload_manager_info,1);
		ui16_counter_up = 25;
	}

}
#if (DF_UPFILE_USING_MQTT)
void process_upfile_mqtt()
{
	/*check server*/
	if (!clientmqtt.connected()) return;

	if (ui8_force_format ==1) return;
	
	//check busy
	if(ui8_busy_rec_printer == 1) return;
	
	//kiểm tra qcode
	// if (ui8_newfile_wait_check_qcode == 1) return;
		
	check_file_to_upload();

	if(ui8_need_upload ==0)
	{
		//check file to upload
		return;
	}

	// if (ui16_counter_up <30)return;
	if ( millis() <  ui32_time_allow_upfile) return;

	int len;	
	
	char ten[200];

	if (totalkichthuoc ==0 )
	{		
		totalkichthuoc = getkichthuoc(upload_manager_info.mid);
		upload_manager_info.sizedata = totalkichthuoc;
	}
		
	Serial.print("get part : ");
	Serial.println(updatepart);

	int kq =1;
	/*len = chiều dài public: không phải là chiều dài dữ liệu in*/
	if (updatepart==0)
	{
		len = get_files_headder_mqtt(VALUE,updatepart);
	}
	else
	{	
		len = get_files_part_mqtt(VALUE,(updatepart -1),totalkichthuoc);
	}
 
	// debug_print_data(VALUE,len);
	debug_raw_data(VALUE,len);
	// Serial.println("\n\n !!!!!!!!!!!!!!!!!!!!!!!!!!!!!debug uncomment this line 3558\n\n");
	// len =0;
	/* start upload file to s3 */
	if (len >0)
	{
		
		char channel_[100];  
		// sprintf(channel_,"PRINTER/%s/BILL",mynamedevice);
		uint32_t tam = millis();
		if (updatepart==0)
		{
			sprintf(channel_,"PRINTER/%s/%s/INFO",mynamedevice, upload_manager_info.nameofbill);
			#if (DF_DEBUG_NOUPLOAD_TO_SERVER)
				kq =1;
			#else
				kq = clientmqtt.publish(channel_,VALUE,1,2);
			#endif 
		}
		else
		{	
			sprintf(channel_,"PRINTER/%s/%s/%d/CHUNK",mynamedevice, upload_manager_info.nameofbill, (updatepart -1));
			
			#if (DF_DEBUG_NOUPLOAD_TO_SERVER)
				kq =1;
			#else
				// bool MQTTClient::publish(const char topic[], const char payload[], int length, bool retained, int qos)
				kq = clientmqtt.publish(channel_,VALUE,len,1,2);
			#endif 			
		}


		// Serial.println("debug 3902");
		// kq =1; /*debug*/

		Serial.print("Topic : ");
		Serial.println(channel_);
	
		// bool publish(const char topic[], const char payload[], int length, bool retained, int qos);
		
		#if (DF_DEBUG_NOUPLOAD_TO_SERVER)
			Serial.println("debug no pub");				
		#else
			tam = millis() - tam;
			Serial.print("\n pub result = ");
			Serial.println(kq);
			Serial.print("\n Pub: take time = ");
			Serial.println(tam);
		#endif

		/* cộng thêm timeout cho kiểm tra thiết bị */
		deviceprinter.time += tam;
		
		if (kq ==1)
		{
			Serial.println("up ok");
			ui16_counterfail =0;
			// len/=2;
			

			if(updatepart == 0)
			{
				Serial.println("header: no increase data up");
			}
			else
			{
				// totalup +=last_len_upload;
				totalup +=len;			
			}
			
			updatepart ++;

			// SerialDebug.printf("updatepart = %d \n", updatepart);
			// SerialDebug.printf("total part = %d \n", totalpart);
			// SerialDebug.printf("totalup = %d \n", totalup);
			// SerialDebug.printf("totalkichthuoc = %d \n", totalkichthuoc);


			if (totalup >= totalkichthuoc)
			{
				Serial.print("Finish");
				totalkichthuoc =0;
				updatepart=0;
				totalup=0;
				ui8_need_upload =0;
				ui8_qcode_complete = 0;			
				// strcpy(upload_manager_info.nameofbill,"BILLFINISHED");
			}
			
			upload_manager_info.part=updatepart;
			upload_manager_info.duration=totalup;
			upload_manager_info.mode = ui8_need_upload;

			
			// spiff_updatemediafile("/up_manage.h", &upload_manager_info,1);
			spiff_updatemediafile(name_up_manager, &upload_manager_info,1);
			ui16_counter_up = 25;
			ui32_time_allow_upfile = millis() + 1000; /*sau 1s up part tiếp theo */
		}
		else
		{
			Serial.println("up fail");
			ui16_counter_up = 25;
			ui16_counterfail++;
			if(ui16_counterfail > 1)
			{
				//reset
				Serial.println("-------------restart here------");
				ESP.restart();
				delay(2000);
			}
		}
	}
	else
	{		
		//finish upload
		Serial.println("finish here");
		Serial.print("totalup = ");
		Serial.println(totalup);
		Serial.print("totalkichthuoc = ");
		Serial.println(totalkichthuoc);

		ui16_counterfail =0;
		Serial.print("Finish");
		totalkichthuoc =0;
		updatepart=0;
		totalup=0;
		ui8_need_upload =0;
		ui8_qcode_complete = 0;			
		// strcpy(upload_manager_info.nameofbill,"BILLFINISHED");			
		upload_manager_info.part=updatepart;
		upload_manager_info.duration=totalup;
		upload_manager_info.mode = ui8_need_upload;	

		
		// spiff_updatemediafile("/up_manage.h", &upload_manager_info,1);
		spiff_updatemediafile(name_up_manager, &upload_manager_info,1);
		ui16_counter_up = 25;
		ui32_time_allow_upfile = millis() + 2000;
	}
}
#endif 
long spiff_readfilemedia(String file, mqtt_OPH_MEDIA_HTTP *regs) {
 
	//Reading file
    File f = SPIFFS.open(file,  "r");  //r+

  int s = 0;
//if File Exists
  if (f) {
    s = f.size();

    // SerialDEBUG.printf("File Opened , Size=%d\r\n", s);
	//Read    
      // *value = f.readString();
	// SerialDEBUG.println(F("[FILE SYS]:Read file"));  
	size_t size = f.size();
	// Allocate a buffer to store contents of the file.
	std::unique_ptr<char[]> buf(new char[size]);
	f.readBytes(buf.get(), size);
		
	DynamicJsonDocument  jsonBuffer(1024);
	deserializeJson(jsonBuffer,buf.get());
	#ifdef use_debug_serial
	SerialDEBUG.print(F("[*FILE]: \t"));
	serializeJson(jsonBuffer,SerialDEBUG);  	
	SerialDEBUG.println(F(""));
	#endif 		
	if (!jsonBuffer.isNull()) {

		regs-> mid= jsonBuffer["mid"];
		regs-> tot= jsonBuffer["tot"];
		regs-> part= jsonBuffer["part"];		
		regs-> mode= jsonBuffer["mode"];
		regs-> duration= jsonBuffer["duration"];
		regs-> next_addr_data= jsonBuffer["nextdata"];
		strlcpy(regs->nameofbill,                  // <- destination
		jsonBuffer["name"] | "Unknow",  // <- source
		sizeof(regs->nameofbill));         // <- destination's capacity

		f.close();
	} else {
		#ifdef use_debug_serial
	SerialDEBUG.println(F("[FILE SYS]:File incorrect format"));		
	#endif 
		// SerialDEBUG.println(F("[FILE SYS]:Load default"));
		f.close();
		// load_oph_media_default(file,regs);		
		#ifdef use_debug_serial
	SerialDEBUG.println(F("\n\r[FILE]:Done"));
	#endif 
	}  

    //Return the size of file
    return (s);
	
  } else {
    #ifdef use_debug_serial
	SerialDEBUG.print(F("[FILE SYS]:Failed to open file for reading "));
    SerialDEBUG.println(file);
	#endif 
  }
  return (1);
}

long spiff_putSSD_info(String file, STR_SSD_INFO *regs) {
	#ifdef use_debug_serial
		SerialDEBUG.println(F("\n\r[FILE]:spiff_updatemediafile Start"));
	#endif
	
	//Open file for write
    File f = SPIFFS.open(file,  "w");
  

  int s = 0;
//if File Exists
  if (f) {
    s = f.size();
	// Allocate the JSON document
	//
	// Inside the brackets, 200 is the RAM allocated to this document.
	// Don't forget to change this value to match your requirement.
	// Use arduinojson.org/v6/assistant to compute the capacity.
	DynamicJsonDocument doc(1024);					
	// StaticJsonObject allocates memory on the stack, it can be
	// replaced by DynamicJsonDocument which allocates in the heap.
	//
	// DynamicJsonDocument  doc(200);
	doc["last_day"]=regs->last_day;

	#ifdef use_debug_serial
	SerialDEBUG.println(F("[FILE]:lmd_msg= "));
    SerialDEBUG.println(regs->lmd_msg);
	SerialDEBUG.println(F("[FILE]:end lmd_msg= "));
	#endif

	
	//create file		
	// Generate the minified JSON and send it to the SerialDEBUG port.
	Serial.println(F(""));
	serializeJson(doc,Serial);
	Serial.println(F(""));
	
	//create file
	serializeJson(doc,f);
	f.close();		
	
	// SerialDEBUG.println(F("\n\r[FILE]:spiff_updatemediafile Done"));		
	
    //Return the size of file
    return (s);
	
  } else {
    #ifdef use_debug_serial
	SerialDEBUG.print(F("[FILE]:Unable to open file "));
    SerialDEBUG.println(file);
	#endif 
  }
  return (1);
}


long spiff_getSSD_info(String file, STR_SSD_INFO *regs) {
 
	//Reading file
    File f = SPIFFS.open(file,  "r");  //r+

  int s = 0;
//if File Exists
  if (f) {
    s = f.size();

    // SerialDEBUG.printf("File Opened , Size=%d\r\n", s);
	//Read    
      // *value = f.readString();
	// SerialDEBUG.println(F("[FILE SYS]:Read file"));  
	size_t size = f.size();
	// Allocate a buffer to store contents of the file.
	std::unique_ptr<char[]> buf(new char[size]);
	f.readBytes(buf.get(), size);
		
	DynamicJsonDocument  jsonBuffer(1024);
	deserializeJson(jsonBuffer,buf.get());
	#ifdef use_debug_serial
	SerialDEBUG.print(F("[*FILE]: \t"));
	serializeJson(jsonBuffer,SerialDEBUG);  	
	SerialDEBUG.println(F(""));
	#endif 		
	if (!jsonBuffer.isNull()) {
		
		regs-> last_day= jsonBuffer["last_day"];

		f.close();
	} else {
		#ifdef use_debug_serial
	SerialDEBUG.println(F("[FILE SYS]:File incorrect format"));		
	#endif 
		// SerialDEBUG.println(F("[FILE SYS]:Load default"));
		f.close();
		// load_oph_media_default(file,regs);		
		#ifdef use_debug_serial
	SerialDEBUG.println(F("\n\r[FILE]:Done"));
	#endif 
	}  

    //Return the size of file
    return (s);
	
  } else {
    #ifdef use_debug_serial
	SerialDEBUG.print(F("[FILE SYS]:Failed to open file for reading "));
    SerialDEBUG.println(file);
	#endif 
  }
  return (1);
}
void process_get_chunk_mqtt()
{
	if(inforeget.status == 0) return;
	
	if (!clientmqtt.connected()) return;

	//tìm chunk lost
	int tam = find_bill(inforeget.name_tofind);
	int stat =0;
	HEADER_FILE head_file_tempread;
	
	if (tam != -1)
	{		
		Serial.println("---------result find file--------------------");
		Serial.print(F("find @"));
		Serial.println(tam);
		Serial.println("---------result--------------------");
		
		read_header_index(tam, &head_file_tempread);
		inval(&head_file_tempread);
		Serial.println("---------end result--------------------");
		stat =1;
	}
	
	if (inforeget.index ==0)
	{		
		Serial.println(F("Period 1"));

		//creat respone
		StaticJsonDocument<500> doc;
		doc["status"] =stat;

		if(stat ==1)
		{
			doc["mesage"] ="File ok";
		}
		else
		{
			doc["mesage"] ="File not found";
		}
		doc["name"] = String(inforeget.name_tofind);
		doc["chunk"] =inforeget.chunk;

		char msg[500];
		serializeJson(doc, msg);
		serializeJson(doc,Serial);
		Serial.println("");

		char channel_[100];  
		sprintf(channel_,"PRINTER/%s/RESP",mynamedevice);

		Serial.print("Topic : ");
		Serial.println(channel_);

		// bool publish(const char topic[], const char payload[], int length, bool retained, int qos);				

		#if (DF_DEBUG_NOUPLOAD_TO_SERVER)
			int tt =1;
			Serial.println("debug no upload");
		#else
			int tt  = clientmqtt.publish(channel_,msg,0,2);
		#endif

		if (tt ==1 )
		{
			Serial.println("sent");
			
			if (tam == -1)
			{
				inforeget.index =0;	/*end*/
				inforeget.status = 0;
			}
			else
			{
				inforeget.index =1;
			}
		}
		else
		{
			Serial.println("fail");
		}
	}

	if (inforeget.index == 1)
	{
		/*up chunk*/
		int len = get_lost_chunk(VALUE,inforeget.chunk,totalkichthuoc,head_file_tempread);
		debug_raw_data(VALUE,len);
		if (len >0)
		{
			char channel_[100];
			sprintf(channel_,"PRINTER/%s/%s/%d/CHUNK",mynamedevice, inforeget.name_tofind, inforeget.chunk);
			#if (DF_DEBUG_NOUPLOAD_TO_SERVER)
				int kq =1;
			#else
				int kq = clientmqtt.publish(channel_,VALUE,len,1,2);
			#endif
			Serial.print("Topic : ");
			Serial.println(channel_);

			if (kq ==1)
			{
				inforeget.index =0;	/*end*/
				inforeget.status = 0;
				Serial.println("up ok finish");
			}
			else
			{
				//đặt thời gian gửi lại
				delay(100);
				Serial.println("up fail");
			}

		}

	}
}

long spiff_updatemediafile(String file, mqtt_OPH_MEDIA_HTTP *regs, int ms) {

	#ifdef use_debug_serial
		SerialDEBUG.println(F("\n\r[FILE]:spiff_updatemediafile Start"));
	#endif
	
	//Open file for write
    File f = SPIFFS.open(file,  "w");
  

  int s = 0;
//if File Exists
  if (f) {
    s = f.size();
	// Allocate the JSON document
	//
	// Inside the brackets, 200 is the RAM allocated to this document.
	// Don't forget to change this value to match your requirement.
	// Use arduinojson.org/v6/assistant to compute the capacity.
	DynamicJsonDocument doc(1024);					
	// StaticJsonObject allocates memory on the stack, it can be
	// replaced by DynamicJsonDocument which allocates in the heap.
	//
	// DynamicJsonDocument  doc(200);
	doc["mid"]=regs->mid;
	doc["tot"]=regs->tot;
	doc["part"]=regs->part;	
	doc["mode"]=regs->mode;
	doc["duration"]=regs->duration;
	doc["sizedata"]=regs->sizedata;
	doc["nextdata"]=regs->next_addr_data;
	doc["name"] = regs->nameofbill;

	#ifdef use_debug_serial
	SerialDEBUG.println(F("[FILE]:lmd_msg= "));
    SerialDEBUG.println(regs->lmd_msg);
	SerialDEBUG.println(F("[FILE]:end lmd_msg= "));
	#endif

	
	//create file		
	// Generate the minified JSON and send it to the SerialDEBUG port.
	Serial.println(F(""));
	serializeJson(doc,Serial);
	Serial.println(F(""));
	
	//create file
	serializeJson(doc,f);
	f.close();		
	
	// SerialDEBUG.println(F("\n\r[FILE]:spiff_updatemediafile Done"));		
	
    //Return the size of file
    return (s);
	
  } else {
    #ifdef use_debug_serial
	SerialDEBUG.print(F("[FILE]:Unable to open file "));
    SerialDEBUG.println(file);
	#endif 
  }
  return (1);
}

void load_setting_default(String file, str_system_info *regs){

	StaticJsonDocument<1024> json1;

	json1["ssid"] = "RD-2.4Ghz";
	json1["pass"] = "@12345678";
	json1["apn"] = "m-wap";	
	json1["ethernet"] = "0";
	json1["wifi"] = "1";
	json1["modem"] = "0";

	sprintf(regs->wifi_name,"%s","RD-2.4Ghz");
	sprintf(regs->wifi_pass,"%s","@12345678");
	sprintf(regs->apn,"%s","m-wap");
	sprintf(regs->use_network,"%s","1"); /* wifi*/
/*
	sprintf(regs->use_ethernet,"%s","0");
	sprintf(regs->use_wifi,"%s","1");
	sprintf(regs->use_modem,"%s","0");
*/
	//create file
	File f2 = SPIFFS.open(file,  "w");
	#ifdef use_debug_serial
	serializeJson(json1,SerialDEBUG);
	#endif 
	serializeJson(json1,f2);
	f2.close();	
}

long spiff_readsettingfile(String file, str_system_info *regs) {
 
	//Reading file
    File f = SPIFFS.open(file,  "r");  //r+

  int s = 0;
//if File Exists
  if (f) {
    s = f.size();

    // SerialDEBUG.printf("File Opened , Size=%d\r\n", s);
	//Read    
      // *value = f.readString();
	// SerialDEBUG.println(F("[FILE SYS]:Read file"));  
	size_t size = f.size();
	// Allocate a buffer to store contents of the file.
	std::unique_ptr<char[]> buf(new char[size]);
	f.readBytes(buf.get(), size);
		
	DynamicJsonDocument  jsonBuffer(1024);
	deserializeJson(jsonBuffer,buf.get());

	SerialDEBUG.print(F("[*FILE]: \t"));
	serializeJson(jsonBuffer,SerialDEBUG);  	
	SerialDEBUG.println(F(""));
	
	if (!jsonBuffer.isNull()) {

		strlcpy(regs->wifi_name,                  // <- destination
		jsonBuffer["ssid"] | "ssid",  // <- source
		sizeof(regs->wifi_name));         // <- destination's capacity

		strlcpy(regs->wifi_pass,                  // <- destination
		jsonBuffer["pass"] | "pass",  // <- source
		sizeof(regs->wifi_pass));         // <- destination's capacity
		
		strlcpy(regs->apn,                  // <- destination
		jsonBuffer["apn"] | "apn",  // <- source
		sizeof(regs->apn));         // <- destination's capacity
		
		strlcpy(regs->use_network,                  // <- destination
		jsonBuffer["network"] | "1",  // <- source
		sizeof(regs->use_network));         // <- destination's capacity
		
		// strlcpy(regs->use_ethernet,                  // <- destination
		// jsonBuffer["ethernet"] | "0",  // <- source
		// sizeof(regs->use_ethernet));         // <- destination's capacity
		
		// strlcpy(regs->use_wifi,                  // <- destination
		// jsonBuffer["wifi"] | "1",  // <- source
		// sizeof(regs->use_wifi));         // <- destination's capacity
		
		// strlcpy(regs->use_modem,                  // <- destination
		// jsonBuffer["modem"] | "0",  // <- source
		// sizeof(regs->use_modem));         // <- destination's capacity

		f.close();
	} else {
		#ifdef use_debug_serial
		SerialDEBUG.println(F("[FILE SYS]:File incorrect format"));		
		#endif 
		SerialDEBUG.println(F("[FILE SYS]:Load default"));
		f.close();
		load_setting_default(file,regs);		
		#ifdef use_debug_serial
		SerialDEBUG.println(F("\n\r[FILE]:Done"));
		#endif 
	}  

    //Return the size of file
    return (s);
	
  } else {
    #ifdef use_debug_serial
	SerialDEBUG.print(F("[FILE SYS]:Failed to open file for reading "));
    SerialDEBUG.println(file);
	#endif 
  }
  return (1);
}
long spiff_updatesettingfile(String file, str_system_info *regs) {

	#ifdef use_debug_serial
		SerialDEBUG.println(F("\n\r[FILE]:spiff_updatemediafile Start"));
	#endif
	
	//Open file for write
    File f = SPIFFS.open(file,  "w");
  

  int s = 0;
//if File Exists
  if (f) {
    s = f.size();
	// Allocate the JSON document
	//
	// Inside the brackets, 200 is the RAM allocated to this document.
	// Don't forget to change this value to match your requirement.
	// Use arduinojson.org/v6/assistant to compute the capacity.
	DynamicJsonDocument doc(1024);					
	// StaticJsonObject allocates memory on the stack, it can be
	// replaced by DynamicJsonDocument which allocates in the heap.
	//
	// DynamicJsonDocument  doc(200);
	doc["ssid"]=regs->wifi_name;
	doc["pass"]=regs->wifi_pass;
	doc["apn"]=regs->apn;	
	doc["network"]=regs->use_network;
	// doc["ethernet"]=regs->use_ethernet;
	// doc["wifi"]=regs->use_wifi;
	// doc["modem"]=regs->use_modem;

	#ifdef use_debug_serial
	SerialDEBUG.println(F("[FILE]:lmd_msg= "));
    SerialDEBUG.println(regs->lmd_msg);
	SerialDEBUG.println(F("[FILE]:end lmd_msg= "));
	#endif

	
	//create file		
	// Generate the minified JSON and send it to the SerialDEBUG port.
	Serial.println(F(""));
	serializeJson(doc,Serial);
	Serial.println(F(""));
	
	//create file
	serializeJson(doc,f);
	f.close();		
	
	// SerialDEBUG.println(F("\n\r[FILE]:spiff_updatemediafile Done"));		
	
    //Return the size of file
    return (s);
	
  } else {
    #ifdef use_debug_serial
	SerialDEBUG.print(F("[FILE]:Unable to open file "));
    SerialDEBUG.println(file);
	#endif 
  }
  return (1);
}

void check_system_time()
{
	/*
    struct tm {
        int8_t          tm_sec; 	<seconds after the minute - [ 0 to 59 ]
        int8_t          tm_min; 	<minutes after the hour - [ 0 to 59 ]
        int8_t          tm_hour; 	< hours since midnight - [ 0 to 23 ]
        int8_t          tm_mday; 	< day of the month - [ 1 to 31 ]
        int8_t          tm_wday; 	< days since Sunday - [ 0 to 6 ]
        int8_t          tm_mon; 	< months since January - [ 0 to 11 ]
        int16_t         tm_year; 	< years since 1900
        int16_t         tm_yday; 	< days since January 1 - [ 0 to 365 ]
        int16_t         tm_isdst; 	< Daylight Saving Time flag 
    };	
	*/
	
	//đọc thời gian hiện tại
	if(ui8_busy_rec_printer == 1) return;

	if (millis() < info_time_system.u32check_system_time) return;
	
	struct tm tmstruct ;		
	tmstruct.tm_year = 0;
	getLocalTime(&tmstruct, 5000);
	
	Serial.printf("\nICB : %d-%02d-%02d %02d:%02d:%02d GMT\n",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
	// SerialDEBUG.println("");
	// SerialDEBUG.print("Time get is : ");
	// SerialDEBUG.println(&tmstruct, "%A, %B %d %Y %H:%M:%S");
	// SerialDEBUG.print("tm_year is : ");
	// SerialDEBUG.println(tmstruct.tm_year);
		
	//Thursday, January 01 1970 00:02:26
	if(tmstruct.tm_year < 100)
	{		

		SerialDEBUG.print("Time get is : ");
		SerialDEBUG.println(&tmstruct, "%A, %B %d %Y %H:%M:%S");
		SerialDEBUG.println("Wrong time-->need update");

		info_time_system.status=TIME_ERRO;
		
		if(system_connection_mode==BYETHERNET)
		{
			SerialDEBUG.println("mode ethernet");
			updatetimebyEthernet_func();
		}
		else if(system_connection_mode==BYSIM4G)
		{						
			SerialDEBUG.println("mode 4G");
			updatetimebyModem_func();
		}
		else
		{
			SerialDEBUG.println("mode wifi");
			if(system_running_mode==0)
			{
				updatetimebyPool_func();
				info_time_system.errotimecounter++;
			}
		}
				
		getLocalTime(&tmstruct, 5000);

		Serial.printf("\nICB set : %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);

		if(info_time_system.errotimecounter > 5)
		{
			/*cập nhật thời gian từ RTC qua*/
			updatetimebyrtc_func();
		}
	}
	else
	{
		info_time_system.errotimecounter =0;
		//kiểm tra nếu RTC bị chạy sai, thì cập nhật cho RTC
		if(ui8_ds1307_need_update==1)
		{
			ui8_ds1307_need_update =0;
			#ifndef use_debug_serial
			SerialDEBUG.println("routine time: update time for RTC");
			#endif 
			// This line sets the RTC with an explicit date & time, for example to set
			// January 21, 2014 at 3am you would call:
			// rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
			rtc.adjust(DateTime(((tmstruct.tm_year)+1900), (( tmstruct.tm_mon)+1), tmstruct.tm_mday, tmstruct.tm_hour, tmstruct.tm_min, tmstruct.tm_sec));
			//read time RTC
			// read_time_RTC();			
		}


		//lưu thời gian để tự động format khi khác ngày
		SSD_info.new_day =tmstruct.tm_mday;
		SSD_info.daystatus =1;

		
		//nếu trước đó thời gian đang bị lỗi thì load lại lịch phát	
		if(info_time_system.status==TIME_ERRO)
		{
			#ifdef use_debug_serial
			SerialDEBUG.print("check_system_time: update new time-->update playlist");
			#endif
					
		}
		info_time_system.status=TIME_NORMAL;
		info_time_system.u32check_system_time = millis()+50000;
	}	
	read_time_RTC();		
}
void updatetimebyPool_func()
{
	if (server_wifi.status == MY_ERRO) return;
	
	Serial.println("\n --------updatetimebyWifi_func-------\n");

	//pool-->RTC-->MQTT
	#ifdef use_debug_serial
		SerialDEBUG.println("Contacting Time Server");
	#endif 
	configTime(3600*timezone, daysavetime*3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
	info_time_system.u32check_system_time = millis() + 2000;


	struct tm tmstruct ;
	delay(2000);
	tmstruct.tm_year = 0;
	getLocalTime(&tmstruct, 5000);

	//kiểm tra nếu RTC bị chạy sai, thì cập nhật cho RTC
	if(ui8_ds1307_need_update==1 && tmstruct.tm_year > 80)
	{
		ui8_ds1307_need_update =0;
		#ifndef use_debug_serial
		SerialDEBUG.println("update time for RTC");
		#endif 
		// This line sets the RTC with an explicit date & time, for example to set
		// January 21, 2014 at 3am you would call:
		// rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
		rtc.adjust(DateTime(((tmstruct.tm_year)+1900), (( tmstruct.tm_mon)+1), tmstruct.tm_mday, tmstruct.tm_hour, tmstruct.tm_min, tmstruct.tm_sec));
		//read time RTC
		read_time_RTC();			
	}
}
/*
mode ethernet
*/

void updatetimebyModem_func()
{
	if (server_modem.status == MY_ERRO) return;
	select_uart(UART_4G);
	delay(1);	
	Serial.println("\n --------updatetimebyModem_func-------\n");
	
	struct tm tmstruct ;
	float timezone_;
	int kq =modem.getNetworkTime(&tmstruct.tm_year, &tmstruct.tm_mon, &tmstruct.tm_mday, &tmstruct.tm_hour, &tmstruct.tm_min, &tmstruct.tm_sec, &timezone_);
	if(kq)
	{
		Serial.print("year = ");	
		Serial.println(tmstruct.tm_year);
		tmstruct.tm_year-=1900;
		Serial.print("tm_mon = ");	
		Serial.println(tmstruct.tm_mon);
		tmstruct.tm_mon-=1;
		Serial.print("tm_mday = ");	
		Serial.println(tmstruct.tm_mday);
		Serial.print("tm_hour = ");	
		Serial.println(tmstruct.tm_hour);
		Serial.print("tm_min = ");	
		Serial.println(tmstruct.tm_min);
		Serial.print("tm_sec = ");	
		Serial.println(tmstruct.tm_sec);
		Serial.print("time zone = ");	
		Serial.println(timezone_);

		time_t  t = mktime(&tmstruct);//đây là thời gian UNIX
		Serial.print("UNIX = ");
		Serial.println(t);

		int tz = (int)timezone_;

		if (timezone_ >= 0)
		{
			t -= tz*3600;
		}
		else
		{
			t += tz*3600;
		}
		
		Serial.print("UNIX new= ");
		Serial.println(t);
		          
		if(t > 1640998800)//Date and time (your time zone): thứ bảy, 1 tháng 1 năm 2022 08:00:00 GMT+07:00
		{

			Serial.println("Set local time");
			// t += 7*60*60; //convert to timezone =7

			timeval epoch = {t, 0};
			settimeofday((const timeval*)&epoch, 0);
			
			//kiểm tra nếu RTC bị chạy sai, thì cập nhật cho RTC
			if(ui8_ds1307_need_update==1 && tmstruct.tm_year > 80)
			{
				ui8_ds1307_need_update =0;
				#ifndef use_debug_serial
				SerialDEBUG.println("update time for RTC");
				#endif 
				// This line sets the RTC with an explicit date & time, for example to set
				// January 21, 2014 at 3am you would call:
				// rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
				
				// rtc.adjust(DateTime(((tmstruct.tm_year)+1900), (( tmstruct.tm_mon)+1), tmstruct.tm_mday, tmstruct.tm_hour, tmstruct.tm_min, tmstruct.tm_sec));

				rtc.adjust(DateTime(t));

				//read time RTC
				read_time_RTC();
			}
		}
	}
	else
	{
		Serial.println("\n get time fail\n");
	}
	
	info_time_system.u32check_system_time = millis() + 2000;

	//   Serial.println("\n --------Response From server-------\n");

	Serial.println("\n --------end updatetimebyModem_func-------");
}
void updatetimebyEthernet_func()
{
	if (server_ethernet.status == MY_ERRO) return;
	Serial.println("\n --------updatetimebyEthernet_func-------\n");
	
	const char* res = "";	
	res = iotClient.get_timeshadow();
	
	Serial.println(res);

	if(res == "")
	{
		Serial.println("\n get time fail\n");
	}
	else
	{
		//2023 12 04 07 00 49		
		struct tm tmstruct ;
		uint32_t tam= 0;

		tam = (res[0]-'0')*1000;
		tam += (res[1]-'0')*100;
		tam += (res[2]-'0')*10;
		tam += (res[3]-'0')*1;
		tmstruct.tm_year=tam-1900;
		// Serial.print("year = ");	
		// Serial.println(tmstruct.tm_year);

		tam =0;
		tam += (res[4]-'0')*10;
		tam += res[5]-'0';
		tmstruct.tm_mon=tam-1;
		// Serial.print("tm_mon = ");	
		// Serial.println(tmstruct.tm_mon);

		tam =0;
		tam += (res[6]-'0')*10;
		tam += res[7]-'0';
		tmstruct.tm_mday=tam;
		// Serial.print("tm_mday = ");	
		// Serial.println(tmstruct.tm_mday);


		tam =0;
		tam += (res[8]-'0')*10;
		tam += res[9]-'0';
		tmstruct.tm_hour=tam;
		// Serial.print("tm_hour = ");	
		// Serial.println(tmstruct.tm_hour);

		tam =0;
		tam += (res[10]-'0')*10;
		tam += res[11]-'0';
		tmstruct.tm_min=tam;
		// Serial.print("tm_min = ");	
		// Serial.println(tmstruct.tm_min);

		tam =0;
		tam += (res[12]-'0')*10;
		tam += res[13]-'0';
		tmstruct.tm_sec=tam;
		// Serial.print("tm_sec = ");	
		// Serial.println(tmstruct.tm_sec);

		time_t  t = mktime(&tmstruct);//đây là thời gian UNIX

		Serial.print("UNIX = ");	
		Serial.println(t);
		
		if(t > 1640998800)//Date and time (your time zone): thứ bảy, 1 tháng 1 năm 2022 08:00:00 GMT+07:00
		{

			Serial.println("\t Set local time");
			// t += 7*60*60; //convert to timezone =7

			timeval epoch = {t, 0};
			settimeofday((const timeval*)&epoch, 0);
			
			//kiểm tra nếu RTC bị chạy sai, thì cập nhật cho RTC
			if(ui8_ds1307_need_update==1 && tmstruct.tm_year > 80)
			{
				ui8_ds1307_need_update =0;
				#ifndef use_debug_serial
				SerialDEBUG.println("\t update time for RTC");
				#endif 
				// This line sets the RTC with an explicit date & time, for example to set
				// January 21, 2014 at 3am you would call:
				// rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
				rtc.adjust(DateTime(((tmstruct.tm_year)+1900), (( tmstruct.tm_mon)+1), tmstruct.tm_mday, tmstruct.tm_hour, tmstruct.tm_min, tmstruct.tm_sec));
				//read time RTC
				read_time_RTC();
			}
		}
	}
	
	info_time_system.u32check_system_time = millis() + 2000;

	//   Serial.println("\n --------Response From server-------\n");

	Serial.println("\n --------end updatetimebyEthernet_func-------");
}

void updatetimebyrtc_func()
{
	DateTime now = rtc.now();
	
	//debug
	#ifdef use_debug_serial
		SerialDEBUG.print(now.year(), DEC);
    SerialDEBUG.print('/');
    SerialDEBUG.print(now.month(), DEC);
    SerialDEBUG.print('/');
    SerialDEBUG.print(now.day(), DEC);
    SerialDEBUG.print(" (");
    SerialDEBUG.print(daysOfTheWeek[now.dayOfTheWeek()]);
    SerialDEBUG.print(") ");
    SerialDEBUG.print(now.hour(), DEC);
    SerialDEBUG.print(':');
    SerialDEBUG.print(now.minute(), DEC);
    SerialDEBUG.print(':');
    SerialDEBUG.print(now.second(), DEC);
    SerialDEBUG.println();
    
    SerialDEBUG.print(" since midnight 1/1/1970 = ");
    SerialDEBUG.print(now.unixtime());
    SerialDEBUG.print("s = ");
    SerialDEBUG.print(now.unixtime() / 86400L);
    SerialDEBUG.println("d");
	#endif 
	
	if(now.unixtime() > 1640998800)//Date and time (your time zone): thứ bảy, 1 tháng 1 năm 2022 08:00:00 GMT+07:00
	{

		SerialDEBUG.println("config time via RTC");
 
		struct tm tmstruct ;
		
		tmstruct.tm_year=now.year()-1900;
		tmstruct.tm_mon=now.month()-1;
		tmstruct.tm_mday=now.day();
		tmstruct.tm_hour=now.hour();
		tmstruct.tm_min=now.minute();
		tmstruct.tm_sec=now.second();
		time_t  t = mktime(&tmstruct);//đây là thời gian UNIX
		#ifdef use_debug_serial
		SerialDEBUG.print("UNIX set : ");
		SerialDEBUG.println(t);
		#endif 
		timeval epoch = {t, 0};
		settimeofday((const timeval*)&epoch, 0);
				
		delay(2000);
		tmstruct.tm_year = 0;
		getLocalTime(&tmstruct, 5000);
		// SerialDEBUG.printf("\nNow is : %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
		// SerialDEBUG.println("");
		#ifdef use_debug_serial
		SerialDEBUG.print("Now is : ");
		SerialDEBUG.println(&tmstruct, "%A, %B %d %Y %H:%M:%S");
		#endif 
	}	
}


void creatnamebill()
{
	struct tm tmstruct ;		
	tmstruct.tm_year = 0;
	getLocalTime(&tmstruct, 5000);

	if(tmstruct.tm_year < 100)
	{
		//thời gian chạy sai, lấy thời gian RTC xài tạm
		Serial.println("time of RTC");
		DateTime now = rtc.now();
		sprintf(bill_media_info.nameofbill,"BIL%d%02d%02d%02d%02d%02d",now.year(),now.month(), now.day(),now.hour() , now.minute(), now.second());		
	}
	else
	{
		sprintf(bill_media_info.nameofbill,"BIL%d%02d%02d%02d%02d%02d",(tmstruct.tm_year+1900),( tmstruct.tm_mon+1), tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
	}

	// strcpy(bill_media_info.nameofbill,upload_manager_info.nameofbill);
	
	Serial.print("creat name of bill = ");
	Serial.println(bill_media_info.nameofbill);

	strncpy(lastbil,bill_media_info.nameofbill,sizeof(lastbil));

	info_time_system.u32check_system_time = millis()+10000;
}

void get_name_bill(uint8_t stt_)
{	
	Serial.println(F("--------- get_name_bill -----------------------"));
	char namefile[30];
	sprintf(namefile,"/bill_%d.h",stt_);

	//Reading file
    File f = SPIFFS.open(namefile,  "r");  //r+

  int s = 0;
//if File Exists
  if (f) {
    s = f.size();

    // SerialDEBUG.printf("File Opened , Size=%d\r\n", s);
	//Read    
      // *value = f.readString();
	// SerialDEBUG.println(F("[FILE SYS]:Read file"));  
	size_t size = f.size();
	// Allocate a buffer to store contents of the file.
	std::unique_ptr<char[]> buf(new char[size]);
	f.readBytes(buf.get(), size);
		
	DynamicJsonDocument  jsonBuffer(1024);
	deserializeJson(jsonBuffer,buf.get());
	#ifdef use_debug_serial
	SerialDEBUG.print(F("[*FILE]: \t"));
	serializeJson(jsonBuffer,SerialDEBUG);  	
	SerialDEBUG.println(F(""));
	#endif 		
	if (!jsonBuffer.isNull()) {

		strlcpy(upload_manager_info.nameofbill,                  // <- destination
		jsonBuffer["name"] | "Unknow",  // <- source
		sizeof(upload_manager_info.nameofbill));         // <- destination's capacity

		f.close();
	} else {
		#ifdef use_debug_serial
	SerialDEBUG.println(F("[FILE SYS]:File incorrect format"));		
	#endif 
		// SerialDEBUG.println(F("[FILE SYS]:Load default"));
		f.close();
		// load_oph_media_default(file,regs);		
		#ifdef use_debug_serial
	SerialDEBUG.println(F("\n\r[FILE]:Done"));
	#endif 
	}  
  } 
  else {
    #ifdef use_debug_serial
	SerialDEBUG.print(F("[FILE SYS]:Failed to open file for reading "));
    SerialDEBUG.println(file);
	#endif 	
	strlcpy(upload_manager_info.nameofbill,                  // <- destination
	"nofile",  // <- source
	sizeof(upload_manager_info.nameofbill));         // <- destination's capacity
  }

	Serial.print("get name of bill = ");
	Serial.println(upload_manager_info.nameofbill);
}



/* 
save_bill_info()

*/

#if (DF_USING_EXT_FLASH)

/*
save header

*/
void save_bill_info()
{
	Serial.println("save_bill_info_flash");
	
	HEADER_FILE head_file_temp;
	
	/*init values*/
	sprintf(head_file_temp.SOF,"SOF");
	sprintf(head_file_temp.brand_name,"EPSON");
	sprintf(head_file_temp.model,"TM-T88IV");
	strncpy(head_file_temp.name, bill_media_info.nameofbill, sizeof(bill_media_info.nameofbill));	
	head_file_temp.length = bill_media_info.sizedata;

	head_file_temp.addr = upload_manager_info.next_addr_data;

	int erro = write_header_index(stt_storing_file, head_file_temp);
	
	if (erro != 0)
	{
		/*need format*/		
		u32_time_force_format = millis() + 30*1000;
		ui8_force_format =1;
	}
}
#else

void save_bill_info()
{
	char file[30];
	sprintf(file,"/bill_%d.h",stt_storing_file);

	Serial.print("save to file = ");
	Serial.println(file);

	mqtt_BIll_HEADDER* regs = &bill_media_info;
	
	//Open file for write
    File f = SPIFFS.open(file,  "w");
  
  int s = 0;
//if File Exists
  if (f) {
    s = f.size();
	// Allocate the JSON document
	//
	// Inside the brackets, 200 is the RAM allocated to this document.
	// Don't forget to change this value to match your requirement.
	// Use arduinojson.org/v6/assistant to compute the capacity.
	DynamicJsonDocument doc(1024);
	// StaticJsonObject allocates memory on the stack, it can be
	// replaced by DynamicJsonDocument which allocates in the heap.
	//
	// DynamicJsonDocument  doc(200);

	doc["status"]=regs->status;
	doc["sizedata"]=regs->sizedata;
	doc["name"] = regs->nameofbill;
	doc["code"]=String(regs->nameofqcode);
	
	//create file		
	// Generate the minified JSON and send it to the SerialDEBUG port.

	serializeJson(doc,Serial);
	Serial.println("");
	//create file
	serializeJson(doc,f);
	f.close();		
	
  } else {

	Serial.print(F("[save_bill_info]:Unable to open file "));
    Serial.println(file);
  }
}
#endif 
//----------------------------------------------------------firmware updare process--------------------------------------------------------------------------------
/*

*/

int FirmwareVersionCheck(void) 
{
  String payload;
  int httpCode;
  String fwurl = "";
  fwurl += URL_fw_Version;
  fwurl += "?";
  fwurl += String(rand());
  Serial.println(fwurl);
  WiFiClientSecure * client = new WiFiClientSecure;

  if (client) 
  {
    client -> setInsecure();//mới thêm
	// client -> setCACert(rootCACertificate);//bỏ dòng này đi mới chạy
	
    // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
    HTTPClient https;

    if (https.begin( * client, fwurl)) 
    { // HTTPS      
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      delay(100);
      httpCode = https.GET();
      delay(100);
      if (httpCode == HTTP_CODE_OK) // if version received
      {
        payload = https.getString(); // save received version
      } 
	  else {
        Serial.print("error in downloading version file:");
        Serial.println(httpCode);
      }
      https.end();
    }
    delete client;
  }
      
  if (httpCode == HTTP_CODE_OK) // if version received
  {
    payload.trim();
    if (payload.equals(versionname)) {
      Serial.printf("\nDevice already on latest firmware version:%s\n", versionname);
      return 0;
    } 
    else 
    {
      Serial.println(payload);
      Serial.println("New firmware detected");
      return 1;
    }
  } 
  return 2;  //have erro
}

/**checkfirmware
hàm tự động kiểm tra version và cập nhật


*/

void checkfirmware(){
	

	if(millis()<u32check_firmware) return;
	
	if (FirmwareVersionCheck()==1) {
      	// firmwareUpdate();
		Serial.println("connect update wifi");
		Serial.println("save file bin for STM32");
		connected_server(sclient, test_root_ca, "raw.githubusercontent.com", 443, "Danh555/file_code/main/STM32F107RCT6_new.bin");
		ui8_update_fw_wifi=1;
		ui8_update_fw_STM32=1;
    }
	
	u32check_firmware=millis() + df_time_upgrade_ms;
}

void firmwareUpdate(void) {
  WiFiClientSecure client;
  client.setInsecure();//mới thêm
	  // client.setCACert(rootCACertificate);//bỏ dòng này đi
	//   httpUpdate.setLedPin(2, LOW);

 	Serial.println("processing upgrade fw...");

  t_httpUpdate_return ret = httpUpdate.update(client, URL_fw_Bin);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
    {  
	  Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
      break;
	}
    case HTTP_UPDATE_NO_UPDATES:
	{		
	  Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;
	}
    case HTTP_UPDATE_OK:
    {		
	  Serial.println("HTTP_UPDATE_OK");
      break;
	}
  }
}

// void firmwareUpdate_Ethernet(void) {
// 	  WiFiClientSecure client;
// 	  client.setInsecure();//mới thêm
	  
// 	// SSLClient client(base_client, TAs, (size_t)TAs_NUM, rand_pin);


//  	Serial.println("processing upgrade fw via Ethernet...");

//   t_httpUpdate_return ret = httpUpdate.update(client, URL_fw_Bin);

//   switch (ret) {
//     case HTTP_UPDATE_FAILED:
//     {  
// 	  Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
//       break;
// 	}
//     case HTTP_UPDATE_NO_UPDATES:
// 	{		
// 	  Serial.println("HTTP_UPDATE_NO_UPDATES");
//       break;
// 	}
//     case HTTP_UPDATE_OK:
//     {		
// 	  Serial.println("HTTP_UPDATE_OK");
//       break;
// 	}
//   }
// }

void connect_mqtt()
{
	// Serial.print("Connecting to ");
	// Serial.println(ssid);
	// WiFi.begin(ssid, password);
	// Serial.println(systeminfo.wifi_name);
	// WiFi.begin(systeminfo.wifi_name, systeminfo.wifi_pass);
	
	int count =0;
	network_status =MY_ERRO;
	switch (system_connection_mode)
	{
		case BYETHERNET:
		{
			/* kiểm tra kết nối internet */
			
			network_status = server_ethernet.status;
			break;
		}
		case BYSIM4G:
		{			
			network_status = server_modem.status;
			break;
		}
		default:
		{
			if (server_wifi.status == MY_ERRO) 
			{
				connect_bywifi();
			}
			network_status = server_wifi.status;
			break;
		}
	}

	if (network_status == MY_ERRO) return;
		
	char datastatus[] = "offline";
	char au_topic_pub[100];

	sprintf(au_topic_pub, "PRINTER/%s/STATUS", mynamedevice);	
	clientmqtt.setWill(au_topic_pub, datastatus , 1, 2);

	
	// if (clientmqtt.connect(mqtt.client_id, mqtt.username , mqtt.pwdMqtt))
	if (clientmqtt.connect(mynamedevice, "altamedia" , "Altamedia"))
	{
		char datastatus[] = "online";
		sprintf(au_topic_pub, "PRINTER/%s/STATUS", mynamedevice);
		clientmqtt.publish(au_topic_pub, datastatus, 1, 2);
		Serial.println("Online");

		sprintf(au_topic_pub, "PRINTER/%s/QUERY", mynamedevice);
		
		// sprintf(au_topic_pub, "PRINTER/ITB_5079184315124/QUERY");
        clientmqtt.subscribe(au_topic_pub, 2);
		ui32_time_allow_upfile = millis() + 1000; /*sau 1s up part tiếp theo */
	}
}

void connect_bywifi()
{
	// Serial.print("Connecting to ");
	// Serial.println(ssid);
	// WiFi.begin(ssid, password);
	// Serial.println(systeminfo.wifi_name);
	// WiFi.begin(systeminfo.wifi_name, systeminfo.wifi_pass);
	int count =0;
	if (millis() < u32_time_recheck_nw) return;
	u32_time_recheck_nw = millis() + 1000;
	
	if(WiFi.status() == WL_CONNECTED)
	{
		server_wifi.status = MY_OK;
		Serial.println("");
		Serial.println("WiFi connected");
		Serial.println("IP address: ");
		Serial.println(WiFi.localIP());
	}
	else
	{
		server_wifi.status = MY_ERRO;
		Serial.print(".");
	}

}

void setup_modem ()
{
	    /*
    MODEM_PWRKEY IO:4 The power-on signal of the modulator must be given to it,
    otherwise the modulator will not reply when the command is sent
    */
   digitalWrite( led_sign, 1);//ON led
   
   select_uart(UART_4G);

    digitalWrite(PWRKEY_MODEM, HIGH);
    delay(300); //Need delay
    digitalWrite(PWRKEY_MODEM, LOW);

	TESTMODEM();
}
void light_sleep(uint32_t sec )
{
    esp_sleep_enable_timer_wakeup(sec * 1000000ULL);
    esp_light_sleep_start();
}
void TESTMODEM()
{
    bool res ;

    // Restart takes quite some time
    // To skip it, call init() instead of restart()
    DBG("Initializing modem...");
    if (!modem.init()) {
      if (!modem.restart()) {
        DBG("Failed to restart modem, delaying 10s and retrying");
        // restart autobaud in case GSM just rebooted
        return;
      }
    }

#if TINY_GSM_TEST_GPRS
    /*  Preferred mode selection : AT+CNMP
          2 – Automatic
          13 – GSM Only
          14 – WCDMA Only
          38 – LTE Only
          59 – TDS-CDMA Only
          9 – CDMA Only
          10 – EVDO Only
          19 – GSM+WCDMA Only
          22 – CDMA+EVDO Only
          48 – Any but LTE
          60 – GSM+TDSCDMA Only
          63 – GSM+WCDMA+TDSCDMA Only
          67 – CDMA+EVDO+GSM+WCDMA+TDSCDMA Only
          39 – GSM+WCDMA+LTE Only
          51 – GSM+LTE Only
          54 – WCDMA+LTE Only
      */
    String ret;
    do {
        ret = modem.setNetworkMode(54);
        delay(500);
    } while (!ret);

    String name = modem.getModemName();
    DBG("Modem Name:", name);

    String modemInfo = modem.getModemInfo();
    DBG("Modem Info:", modemInfo);

    // Unlock your SIM card with a PIN if needed
    if (GSM_PIN && modem.getSimStatus() != 3) {
        modem.simUnlock(GSM_PIN);
    }
	
	if (modem.getSimStatus() == SIM_READY)
	{		
		DBG("Waiting for network...");
		int kq =0;
		int tt=1;
		digitalWrite( led_sign, tt);
		for(int i =0;i<30;i++)
		{
			Serial.printf("i = %d \n",i);
			if (!modem.waitForNetwork(1000L)) {//100 = 10s
				// light_sleep(10);
				// return;
			}
			else
			{
				kq =1;
				break;
			}		

			tt=!tt;
			digitalWrite( led_sign, tt); 
		}
		if (kq ==0) return;
		

		if (modem.isNetworkConnected()) {
			DBG("Network connected");
		}
	}
	else
	{
		DBG("Simcard erro: exit");
		return;
	}
#endif


#if TINY_GSM_TEST_GPRS
    // DBG("Connecting to", apn);
    DBG("Connecting to", systeminfo.apn);
    if (!modem.gprsConnect(systeminfo.apn, gprsUser, gprsPass)) {
        light_sleep(10);
        return;
    }

    res = modem.isGprsConnected();
    DBG("GPRS status:", res ? "connected" : "not connected");

    String ccid = modem.getSimCCID();
    DBG("CCID:", ccid);

    String imei = modem.getIMEI();
    DBG("IMEI:", imei);

    String imsi = modem.getIMSI();
    DBG("IMSI:", imsi);

    String cop = modem.getOperator();
    DBG("Operator:", cop);

      /*  Network mode : AT+CNSMOD
        0 – no service
        1 - GSM
        2 - GPRS
        3 - EGPRS (EDGE)
        4 - WCDMA
        5 - HSDPA only(WCDMA)
        6 - HSUPA only(WCDMA)
        7 - HSPA (HSDPA and HSUPA, WCDMA)
        8 - LTE
        9 - TDS-CDMA
        10 - TDS-HSDPA only
        11 - TDS- HSUPA only
        12 - TDS- HSPA (HSDPA and HSUPA)
        13 - CDMA
        14 - EVDO
        15 - HYBRID (CDMA and EVDO)
        16 - 1XLTE(CDMA and LTE)
        23 - eHRPD
        24 - HYBRID(CDMA and eHRPD)
    */
    modem.sendAT(GF("+CNSMOD?"));
    if (modem.waitResponse(GF(GSM_NL "+CNSMOD:")) != 1) { }
    int nmodec = modem.stream.readStringUntil(',').toInt() != 0;
    int nmode = modem.stream.readStringUntil('\n').toInt();
    modem.waitResponse();
    DBG("Network Mode:", nmode);

    IPAddress local = modem.localIP();
    DBG("Local IP:", local);

    int csq = modem.getSignalQuality();
    DBG("Signal quality:", csq);
#endif

// #if TINY_GSM_TEST_TCP && defined TINY_GSM_MODEM_HAS_TCP
#if TINY_GSM_TEST_TCP
    
    // Retrieve Time
    String time;
      do {
        time = modem.getGSMDateTime(DATE_FULL).substring(0, 17);
        delay(100);
    } while (!time);
    DBG("Current Network Time:", time);

	int year_;
	int month_;
	int day_;
	int hour_;
	int minute_;
	int second_;
	float timezone_;

	modem.getNetworkTime(&year_, &month_, &day_, &hour_, &minute_,&second_, &timezone_);
	DBG("year:", year_);
	DBG("month:", month_);
	DBG("day:", day_);
	DBG("hour:", hour_);
	DBG("minute:", minute_);
	DBG("second:", second_);
	DBG("timezone:", timezone_);


    // Retrieve Temperature
    float temp;
    do {
        temp = modem.getTemperature();
        delay(100);
    } while (!temp);
    DBG("Modem Temp:", temp);

    if (!modem.isGprsConnected()) {
        server_modem.status = MY_ERRO;
		DBG("... not connected");
    } else {
        DBG("4G init OK");
		server_modem.status = MY_OK;

		// Serial.println("\n Test upload file ");
		// char VALUE[10];
		// for(int i =0;i<10;i++)
		// {
			// VALUE[i]='0' + i;
		// }
		// VALUE[9]='\0';
		// char ten[200];
		// int index_up =0;
		// sprintf(ten,"/esp-iot-2023/printer_device_test/Printerdata_%d.txt",index_up);
		// sprintf(ten,"/esp-iot-2023/Congtest4G/modem_%d.txt",index_up);
		// upload_files4G(VALUE, ten);

		#if (0) 
		DBG("Connecting to ", server);
        // Make a HTTPS POST request:
        Serial.println("Making POST request securely");
        String contentType = "Content-Type: application/json";
        String postData = "{\"time\": \""+ time + "\", \"temp\": \"" + temp + "\"}";        
        mysclient_gsm.post(resource, contentType, postData);
        int status_code = mysclient_gsm.responseStatusCode();
        String response = mysclient_gsm.responseBody();
        
        Serial.print("Status code: ");
        Serial.println(status_code);
        Serial.print("Response: ");
        Serial.println(response);

        mysclient_gsm.stop();      
		#endif   
    }
#endif
    
#if TINY_GSM_TEST_GPRS
    // modem.gprsDisconnect();
    // light_sleep(5);
    // if (!modem.isGprsConnected()) {
    //     DBG("GPRS disconnected");
    // } else {
    //     DBG("GPRS disconnect: Failed.");
    // }
#endif

#if TINY_GSM_POWERDOWN
    // Try to power-off (modem may decide to restart automatically)
    // To turn off modem completely, please use Reset/Enable pins
    // modem.poweroff();
    // DBG("Poweroff.");
#endif

    SerialMon.printf("End of init \n");
	

    //Wait moden power off
    // light_sleep(5);

    // esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    // delay(200);
    // esp_deep_sleep_start();

    // while (1);
}

#if (0) 
void test_downloadfile() {
  // Create a client connection
  EthernetClient client = server.available();

	  WiFiClientSecure client;
	  client.setInsecure();//mới thêm


  if (client) {
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        //read char by char HTTP request
        if (readString.length() < 100) {
          //store characters to string
          readString += c;
          Serial.print(c);
        }
        //if HTTP request has ended
        if (c == '\n' && currentLineIsBlank) {
          // open requested web page file
          client.println();
          if (HTTP_req, "GET /TEST.txt") {
            client.println("HTTP/1.1 200 OK"); //send new page
            client.println("Content-Disposition: attachment; filename=TEST.txt");
            client.println("Connection: close");
            client.println();
            Serial.println(readString); //print to serial monitor for debuging
            if (myFile) {
              byte clientBuf[64];
              int clientCount = 0;
              while (myFile.available()) {
                clientBuf[clientCount] = myFile.read();
                clientCount++;
                if (clientCount > 63) {
                  client.write(clientBuf, 64);
                  clientCount = 0;
                }
              }
              //final <64 byte cleanup packet
              if (clientCount > 0) client.write(clientBuf, clientCount);
              // close the file:
              myFile.close();
            }
            delay(1);
            //stopping client
            client.stop();
            readString = "";
          }
        }
      }
    }
  }
}
#endif 

/*---------------------------------OTA STM start-----------------------------------------------------*/

String makePage(String title, String contents) {
  String s = "<!DOCTYPE html><html><head>";
  s += "<meta name=\"viewport\" content=\"width=device-width,user-scalable=0\"/>";
  s += "<title >";
  s += title;
  s += "</title></head><body text=#ffffff bgcolor=##4da5b9 align=\"center\">";
  s += contents;
  s += "</body></html>";
  return s;
}

// Upload STM32 Bin file to STM32 MCU from the flash of ESP32 and shown on HTTPserver
void handleFlash()
{
  String FileName, flashwr;
  uint8_t cflag, fnum = 256;
  
//   File dir = SPIFFS.open("/");
//   File file = dir.openNextFile();
//   while (file)
//   {
//     FileName = file.name();
//     file = dir.openNextFile();
//   }

//   FileName = "/STM32FW.dat";

//   FileName = device_upgrade.filename;
FileName=file_update_wifi_stm32;
  
//   int t= getszie(device_upgrade.filename);
  
//   Serial.print("Open file = ");
//   Serial.println(FileName);

  fsUploadFile = SPIFFS.open(FileName, "rb");
  if (fsUploadFile) {
    
	// Serial.print("Size = ");
  	// Serial.println(fsUploadFile.size());	
	// bini = fsUploadFile.size() / 256;
    // lastbuf = fsUploadFile.size() % 256;
    int t = fsUploadFile.size();
	Serial.print("Size = ");
  	Serial.println(t);	
	bini = t / 256;
    lastbuf = t % 256;
	
	flashwr = String(bini) + "-" + String(lastbuf) + "<br>";
    for (int i = 0; i < bini; i++) {
      fsUploadFile.read(binread, 256);
      stm32SendCommand(STM32WR);
      while (!Serial1.available()) ;
      cflag = Serial1.read();
      if (cflag == STM32OK)
        if (stm32Address(STM32STADDR + (256 * i)) == STM32OK) {
          if (stm32SendData(binread, 255) == STM32OK)
            flashwr += ".";
          else flashwr = "Error";
        }
    }
    fsUploadFile.read(binread, lastbuf);
    stm32SendCommand(STM32WR);
    while (!Serial1.available()) ;
    cflag = Serial1.read();
    if (cflag == STM32OK)
      if (stm32Address(STM32STADDR + (256 * bini)) == STM32OK) {
        if (stm32SendData(binread, lastbuf) == STM32OK)
          flashwr += "<br>Finished<br>";
        else flashwr = "Error";
      }
    //flashwr += String(binread[0]) + "," + String(binread[lastbuf - 1]) + "<br>";
    fsUploadFile.close();
	Serial.println("up xong roi");
    String flashhtml = "<h1>Programming</h1><h2>" + flashwr +  "<br><br><a style=\"color:white\" href=\"/flash\">Return </a><br><br></h2>";
    server.send(200, "text/html", makePage("Flash Page", flashhtml));
  }
  else
  {
	Serial.println("erro open file");
  }
}

void handlesetting()
{

String page = FPSTR(HTTP_HEAD_CONG);
char parLength[2];

page.replace("{v}", "Setting Parameters");
page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
//   page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);

  page += FPSTR(HTTP_FORM_START);
		
		page += "<br/><h3>ID Box</h3>";
		page += "<br/><h3>" + String(mynamedevice) + "</h3>";
		page += "<br/><h3>SSID</h3>";
String pitem = FPSTR(HTTP_FORM_PARAM);
	  pitem.replace("{i}", "p1");/*ID*/
      pitem.replace("{n}", "p1");/*Name*/
      pitem.replace("{p}", "SSID");/*place holder*/
      snprintf(parLength, 2, "%d", sizeof(systeminfo.wifi_name));
      pitem.replace("{l}", parLength);/*length*/
      pitem.replace("{v}", systeminfo.wifi_name);/*values init*/
	  page += pitem;

	  page += "<br/><h3>Pass Wifi</h3>";
	  pitem = FPSTR(HTTP_FORM_PARAM);
      pitem.replace("{i}", "p2");
      pitem.replace("{n}", "p2");
      pitem.replace("{p}", "PASS CODE");
      snprintf(parLength, 2, "%d", sizeof(systeminfo.wifi_pass));
      pitem.replace("{l}", parLength);
      pitem.replace("{v}", systeminfo.wifi_pass);    
	  page += pitem;

	  page += "<br/><h3>APN</h3>";
	  pitem = FPSTR(HTTP_FORM_PARAM);
      pitem.replace("{i}", "p3");
      pitem.replace("{n}", "p3");
      pitem.replace("{p}", "APN SIM mobile");
      snprintf(parLength, 2, "%d", sizeof(systeminfo.wifi_pass));
      pitem.replace("{l}", parLength);
      pitem.replace("{v}", systeminfo.apn);    
	  page += pitem;

	  page += "<br/><h3>Network connection by</h3>";

	  page += "<br/><h3>Just support WIFI</h3>";

	  pitem = FPSTR(HTTP_NW_MODE_SELECTION);
	  page += pitem;
#if (0)
	  page += "<br/><h3>Ethernet mode</h3>";
	  pitem = FPSTR(HTTP_FORM_PARAM);
      pitem.replace("{i}", "p4");
      pitem.replace("{n}", "p4");
      pitem.replace("{p}", "Ethernet = 1 Enable, =0: disable");
      snprintf(parLength, 2, "%d", sizeof(systeminfo.use_ethernet));
      pitem.replace("{l}", parLength);
      pitem.replace("{v}", systeminfo.use_ethernet);    
	  page += pitem;

	  page += "<br/><h3>Wifi mode</h3>";
	  pitem = FPSTR(HTTP_FORM_PARAM);
      pitem.replace("{i}", "p5");
      pitem.replace("{n}", "p5");
      pitem.replace("{p}", "Wifi = 1 Enable, =0: disable");
      snprintf(parLength, 2, "%d", sizeof(systeminfo.use_wifi));
      pitem.replace("{l}", parLength);
      pitem.replace("{v}", systeminfo.use_wifi);
	  page += pitem;

	  page += "<br/><h3>4G mode</h3>";
	  pitem = FPSTR(HTTP_FORM_PARAM);
      pitem.replace("{i}", "p6");
      pitem.replace("{n}", "p6");
      pitem.replace("{p}", "4G = 1 Enable, =0: disable");
      snprintf(parLength, 2, "%d", sizeof(systeminfo.use_modem));
      pitem.replace("{l}", parLength);
      pitem.replace("{v}", systeminfo.use_modem);
	  page += pitem;
#endif 

	  page += "<br/>";

	page += FPSTR(HTTP_FORM_END);
	page += FPSTR(HTTP_HOME_LINK);
	page += FPSTR(HTTP_END);

	//   server->sendHeader("Content-Length", String(page.length()));
  	server.send(200, "text/html", page);
}

void handleWifiSave()
{
//   systeminfo.wifi_name = server.arg("s");
//   systeminfo.wifi_pass = server.arg("PASS");
//   systeminfo.mode = server.arg("Mode");

// strcpy(systeminfo.wifi_name, server.arg("p1"));
// strcpy(systeminfo.wifi_pass, server.arg("p2"));
// strcpy(systeminfo.mode, server.arg("p3"));

// http://192.168.4.1/wifisave?p1=Unknow&p2=congluxubu&p3=m-wap&p4=0&modef1=0&p5=1&p6=0

server.arg("p1").toCharArray(systeminfo.wifi_name, sizeof(systeminfo.wifi_name));
server.arg("p2").toCharArray(systeminfo.wifi_pass, sizeof(systeminfo.wifi_pass));
server.arg("p3").toCharArray(systeminfo.apn, sizeof(systeminfo.apn));
// server.arg("p4").toCharArray(systeminfo.use_ethernet, sizeof(systeminfo.use_ethernet));
// server.arg("p5").toCharArray(systeminfo.use_wifi, sizeof(systeminfo.use_wifi));
// server.arg("p6").toCharArray(systeminfo.use_modem, sizeof(systeminfo.use_modem));
server.arg("modef1").toCharArray(systeminfo.use_network, sizeof(systeminfo.use_network));

  systeminfo.use_network[0] = '1';
  
  SerialDebug.print("ssid = ");
  SerialDebug.println(systeminfo.wifi_name);
  SerialDebug.print("pass = ");
  SerialDebug.println(systeminfo.wifi_pass);
  SerialDebug.print("apn = ");
  SerialDebug.println(systeminfo.apn);
  SerialDebug.print("use_network = ");
  SerialDebug.println(systeminfo.use_network);

//   SerialDebug.print("use_ethernet = ");
//   SerialDebug.println(systeminfo.use_ethernet);
//   SerialDebug.print("use_wifi = ");
//   SerialDebug.println(systeminfo.use_wifi);
//   SerialDebug.print("use_modem = ");
//   SerialDebug.println(systeminfo.use_modem);

  //save file  
  spiff_updatesettingfile("/sys.ini", &systeminfo);
  SerialDebug.println("Reload values..");
  load_values();

  
	server.send(200, "text/html", makePage("Run", "<h2> saved setting <br><br><a style=\"color:white\" href=\"/\">Home </a></h2>"));
}
// String filenametam;

// Upload STM32 Bin files to the flash of ESP32 from local machine and shown on HTTPserver
void handleFileUpload()
{
  if (server.uri() != "/upload") return;
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) filename = "/" + filename;
    // filenametam = filename;

	device_upgrade.filename = filename;
    Serial.print("filename to upload= ");
    Serial.println(device_upgrade.filename);
    fsUploadFile = SPIFFS.open(device_upgrade.filename, "wb");/* "w"*/
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile)
      fsUploadFile.close();
  }
}

// Delete STM32 Bin file from the flash of ESP32 and shown on HTTPserver
void handleFileDelete() {
  int binhigh = 0;
  String FileList = "File: ";
  String FName;
  File dir = SPIFFS.open("/");
  File file = dir.openNextFile();
  while (file) {
    FName = file.name();
	FileList += FName + "<br>";
	if (!FName.startsWith("/")) FName = "/" + FName;	
	SPIFFS.remove(FName);
    file = dir.openNextFile();
  }
//   FileList += FName;
//   if (SPIFFS.exists(FName)) {
    server.send(200, "text/html", makePage("Deleted", "<h2>" + FileList + " be deleted!<br><br><a style=\"color:white\" href=\"/list\">Return </a></h2>"));
    // SPIFFS.remove(FName);
//   }
//   else
//   {
//     return server.send(404, "text/html", makePage("File Not found", "404"));
//   }
//    if (SPIFFS.exists(FName)) {
//    server.send(200, "text/html", makePage("Deleted", "<h2>" + FileList + " be deleted!<br><br><a style=\"color:white\" href=\"/list\">Return </a></h2>"));
//    SPIFFS.remove(FName);
//  }
}


// List STM32 Bin files in the flash of ESP32 and shown on HTTPserver
void handleListFiles()
{
  String FileList = "Bootloader Ver: ";
  String Listcode;
  char blversion = 0;
  File dir = SPIFFS.open("/");
  blversion = stm32Version();
  FileList += String((blversion >> 4) & 0x0F) + "." + String(blversion & 0x0F) + "<br> MCU: ";
//   FileList += STM32_CHIPNAME[stm32GetId()];
  FileList += "<br><br> File: ";
  File file = dir.openNextFile();
  while (file)
  {
    String FileName = file.name();
    File f = SPIFFS.open(file.name());
    String FileSize = String(f.size());
    int whsp = 6 - FileSize.length();
    while (whsp-- > 0)
    {
      FileList += " ";
    }
    FileList +=  FileName + "   Size:" + FileSize + "<br>";
    file = dir.openNextFile();
  }
  Listcode = "<h1>List STM32 BinFile</h1><h2>" + FileList + "<br><br><a style=\"color:white\" href=\"/flash\">Flash Menu</a><br><br><a style=\"color:white\" href=\"/delete\">Delete BinFile </a><br><br><a style=\"color:white\" href=\"/up\">Upload BinFile</a></h2>";
  server.send(200, "text/html", makePage("FileList", Listcode));
}

void server_init()
{

	MDNS.begin("stm32-ota");
  server.on("/up", HTTP_GET, []() {
    server.send(200, "text/html", makePage("Select file", serverIndex));
  });
  server.on("/list", HTTP_GET, handleListFiles);

  server.on("/upesp", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndexesp);
  });

  server.on("/upbarcode", HTTP_GET, []() {
    
	device_upgrade.deviceselect = UPDATE_BARCODE;
	server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndexbarcode);
  });

   server.on("/upprinter", HTTP_GET, []() {    
	device_upgrade.deviceselect = UPDATE_PRINTER;
	server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndexprinter);
  });
   server.on("/exit", HTTP_GET, []() {    
	server.sendHeader("Connection", "close");
    server.send(200, "text/html", "I've gone out!!!");
	ESP.restart();
  });
    
  server.on("/updateesp", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
	server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      // SerialDEBUG.setDebugOutput(true);
      // WiFiUDP::stopAll();
      #ifdef use_debug_serial
	  SerialDEBUG.printf("Update: %s\n", upload.filename.c_str());
	  #endif 
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if (!Update.begin(maxSketchSpace)) { //start with max available size
        #ifdef use_debug_serial
		Update.printError(SerialDEBUG);
		#endif 
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        #ifdef use_debug_serial
		Update.printError(SerialDEBUG);
		#endif 
      }
	  #if (1)
	  SerialDebug.println("c/t" + String(upload.currentSize) + "\t" + String(upload.totalSize));
	  #endif
	  
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        #ifdef use_debug_serial
		SerialDEBUG.println("Update Success: " + String(upload.totalSize));
		#endif 
      } else {
        #ifdef use_debug_serial
		Update.printError(SerialDEBUG);		
		SerialDEBUG.println(F("Update Error"));
		#endif 
      }
      // SerialDEBUG.setDebugOutput(false);
    }
    // yield();
  });


  server.on("/programm", HTTP_GET, handleFlash);

  server.on("/run", HTTP_GET, []() {
    String Runstate = "Chip Restart and runing!<br><br>Check if the device is running properly";

      RunMode();

    server.send(200, "text/html", makePage("Run", "<h2>" + Runstate + "<br><br><a style=\"color:white\" href=\"/\">Home </a></h2>"));
  });
  
  server.on("/setting", HTTP_GET, handlesetting);

  server.on("/wifisave", HTTP_GET, handleWifiSave);

  server.on("/initchip", HTTP_GET, []() {

    FlashMode();
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
	stringtmp = "ERROR";
		
	while (Serial1.available())
	{
		rdtmp = Serial1.read();

		if (rdtmp == STM32ACK)
		{
			stringtmp = STM32_CHIPNAME[stm32GetId()];
			res =1;
			SerialDebug.print("stringtmp = ");
	  		SerialDebug.println(stringtmp);
		}		
	}

	String starthtml ="";
	if (res == 1)
      starthtml = "<h1>Init OK</h1><h2><a style=\"color:white\" href=\"/flash\">Return </a></h2>";
    else
      starthtml = "<h1>Init failure</h1><h2><a style=\"color:white\" href=\"/initchip\">ReInint chip </a><br><br><a style=\"color:white\" href=\"/flash\">Return </a></h2>";

    server.send(200, "text/html", makePage("Start Page", starthtml + "- Init MCU -<br> " + stringtmp));
  });


  server.on("/erase", HTTP_GET, []() {
    if (stm32Erase() == STM32ACK)
      stringtmp = "<h1>Erase OK</h1><h2><a style=\"color:white\" href=\"/flash\">Return </a></h2>";
    else if (stm32Erasen() == STM32ACK)
      stringtmp = "<h1>Erase OK</h1><h2><a style=\"color:white\" href=\"/flash\">Return </a></h2>";
    else
      stringtmp = "<h1>Erase failure</h1><h2><a style=\"color:white\" href=\"/flash\">Return </a></h2>";
    server.send(200, "text/html", makePage("Erase page", stringtmp));
  });
  server.on("/flash", HTTP_GET, []() {
    stringtmp = "<h1>FLASH MENU</h1><h2><a style=\"color:white\" href=\"/initchip\">Init Chip</a><br><br><a style=\"color:white\" href=\"/erase\">Erase Chip</a><br><br><a style=\"color:white\" href=\"/programm\">Flash Chip</a><br><br><a style=\"color:white\" href=\"/run\">Run Chip</a><br><br><a style=\"color:white\" href=\"/\">Home </a></h2>";
    server.send(200, "text/html", makePage("Flash page", stringtmp));
  });
  server.on("/esp-ota", HTTP_GET, []() {
    stringtmp = "<h1>Update ESP32 </h1>";
    server.send(200, "text/html", makePage("ESP-OTA-Mode", stringtmp));
      digitalWrite(LED, 1);    
    for ( int j = 0; j < 3; j++) {
      digitalWrite(LED, !digitalRead(LED));
      delay(100);
    }
  });
  server.on("/delete", HTTP_GET, handleFileDelete);
  server.onFileUpload(handleFileUpload);
  server.on("/upload", HTTP_POST, []() {
    server.send(200, "text/html", makePage("FileList", "<h1> Upload file Finished </h1><br><br><h2><a style=\"color:white\" href=\"/flash\">Next step </a></h2>"));
  });
  server.on("/", HTTP_GET, []() {
    
    device_upgrade.deviceselect = UPDATE_NONE;	
	String starthtml = "<h1>Intercept Box Menu</h1><h2>" + String(versionname) + "<br><br><a style=\"color:white\" href=\"/setting\">Setting Parameters</a><br><br><a style=\"color:white\" href=\"/upesp\">Update Server Controller</a><br><br><a style=\"color:white\" href=\"/upprinter\">Update Printer Controller </a><br><br><a style=\"color:white\" href=\"/upbarcode\">Update Barcode Controller </a><br><br><br/><div class=\"c\"><a href=\"/exit\">Exit</a></div></h2>";
    server.send(200, "text/html", makePage("Start Page", starthtml));
  });
  server.begin();

}


void FlashMode()  {    //Tested  Change to flashmode
	
	if (device_upgrade.deviceselect == UPDATE_PRINTER)
	{
		Serial.println("Enter FlashMode Printer");
		SerialToBarcodeModem.begin(115200,SERIAL_8E1,SerialToPrinter_tx,SerialToPrinter_rx);
		//    SerialToPrinter.begin(115200,SERIAL_8E1,SerialToBarcodeModem_tx,SerialToBarcodeModem_rx);

		digitalWrite(BOOT_PRINTER, HIGH);
		delay(100);
		digitalWrite(RESET_PRINTER, LOW);
		digitalWrite(LED, LOW);
		delay(50);
		digitalWrite(RESET_PRINTER, HIGH);
		delay(200);
	}
	else if (device_upgrade.deviceselect == UPDATE_BARCODE)
	{
		Serial.println("Enter FlashMode Barcode");
		SerialToBarcodeModem.begin(115200,SERIAL_8E1,SerialToBarcodeModem_tx,SerialToBarcodeModem_rx);
		digitalWrite(BOOT_BARCODE, HIGH);
		delay(100);
		digitalWrite(RESET_BARCODE, LOW);
		digitalWrite(LED, LOW);
		delay(50);
		digitalWrite(RESET_BARCODE, HIGH);
		delay(200);
	}
	else
	{
		Serial.println("FlashMode Unknow");
	}

}

void RunMode()  {    //Tested  Change to runmode

	if (device_upgrade.deviceselect == UPDATE_PRINTER)
	{
		Serial.println("Enter RunMode Printer");
		SerialToBarcodeModem.begin(115200,SERIAL_8N1,SerialToBarcodeModem_tx,SerialToBarcodeModem_rx);
		SerialToPrinter.begin(115200,SERIAL_8N1,SerialToPrinter_tx,SerialToPrinter_rx);

		digitalWrite(BOOT_PRINTER, LOW);
		delay(100);
		digitalWrite(RESET_PRINTER, LOW);
		digitalWrite(LED, LOW);
		delay(50);
		digitalWrite(RESET_PRINTER, HIGH);
		delay(200);
	}
	else if (device_upgrade.deviceselect == UPDATE_BARCODE)
	{
		Serial.println("Enter RunMode Barcode");
		SerialToBarcodeModem.begin(115200,SERIAL_8N1,SerialToBarcodeModem_tx,SerialToBarcodeModem_rx);
		SerialToPrinter.begin(115200,SERIAL_8N1,SerialToPrinter_tx,SerialToPrinter_rx);
		digitalWrite(BOOT_BARCODE, LOW);
		delay(100);
		digitalWrite(RESET_BARCODE, LOW);
		digitalWrite(LED, LOW);
		delay(50);
		digitalWrite(RESET_BARCODE, HIGH);
		delay(200);
	}
	else
	{
		Serial.println("RunMode Unknow");
	}
}
/*---------------------------------OTA STM end-----------------------------------------------------*/


/*------------------------------------MQTT FUNTIONS START------------------------------------------------*/

#if (DF_USING_EXT_FLASH)
int get_lastbill_name(uint8_t index_)
{
	Serial.println(F("--------- get_lastbill_name -----------------------"));
	
	HEADER_FILE head_file_tempread;
	
	/*đọc file theo số thứ tự file*/
	int err0 = read_header_index(index_, &head_file_tempread);
	inval(&head_file_tempread);
	if (err0 == 0)
	{
		/*copy last bill*/
		strncpy(lastbil,head_file_tempread.name,sizeof(lastbil));
		Serial.print("lastbill =");	
		Serial.println(lastbil);
		return 0;
	}	
	else
	{
		Serial.print("cannot get lastbil");	
		return 1;
	}
return 1;
}

#else
int get_lastbill_name(uint8_t index_)
{
 
	Serial.println(F("--------- get_lastbill_name -----------------------"));

	int index_up =0;
	int len;
	const char* res = "";

	char namefile[30];
	char dataBuffer[24];

	sprintf(namefile,"/bill_%d.h",index_);
	Serial.print("file =");
	Serial.println(namefile);
	//Reading file
    File f = SPIFFS.open(namefile,  "r");  //r+

  int s = 0;
//if File Exists
  if (f) {
    s = f.size();

    // SerialDEBUG.printf("File Opened , Size=%d\r\n", s);
	//Read    
      // *value = f.readString();
	// SerialDEBUG.println(F("[FILE SYS]:Read file"));  
	size_t size = f.size();
	// Allocate a buffer to store contents of the file.
	std::unique_ptr<char[]> buf(new char[size]);
	f.readBytes(buf.get(), size);
		
	DynamicJsonDocument  jsonBuffer(1024);
	deserializeJson(jsonBuffer,buf.get());
	#ifndef use_debug_serial
	SerialDEBUG.print(F("[*FILE]: \t"));
	serializeJson(jsonBuffer,SerialDEBUG);  	
	SerialDEBUG.println(F(""));
	#endif 		
	if (!jsonBuffer.isNull()) {

		strlcpy(lastbil,                  // <- destination
		jsonBuffer["name"] | "Unknow",  // <- source
		sizeof(lastbil));         // <- destination's capacity
		
		Serial.print("name last =");	
		Serial.println(lastbil);

		f.close();
	} else {
		#ifdef use_debug_serial
	SerialDEBUG.println(F("[FILE SYS]:File incorrect format"));		
	#endif 
		// SerialDEBUG.println(F("[FILE SYS]:Load default"));
		f.close();
		// load_oph_media_default(file,regs);		
		#ifdef use_debug_serial
	SerialDEBUG.println(F("\n\r[FILE]:Done"));
	#endif 
	}  
  } 
  else 
  {
    #ifdef use_debug_serial
	SerialDEBUG.print(F("[FILE SYS]:Failed to open file for reading "));
    SerialDEBUG.println(file);
	#endif 	

  }

	Serial.print("name last =");	
	Serial.println(lastbil);	
 Serial.println(F("---------End get_lastbill_name -----------------------"));
 return len;
}
#endif 

int updatebarcode_func()
{
	int kq =0;
	char timeofcode[24];
	struct tm tmstruct ;		
	tmstruct.tm_year = 0;
	getLocalTime(&tmstruct, 5000);

	if(tmstruct.tm_year < 100)
	{
		//thời gian chạy sai, lấy thời gian RTC xài tạm
		Serial.println("time of RTC");
		DateTime now = rtc.now();
		sprintf(timeofcode,"%d%02d%02d%02d%02d%02d",now.year(),now.month(), now.day(),now.hour() , now.minute(), now.second());		
	}
	else
	{
		sprintf(timeofcode,"%d%02d%02d%02d%02d%02d",(tmstruct.tm_year+1900),( tmstruct.tm_mon+1), tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
	}
	
	Serial.print("timeofcode = ");
	Serial.println(timeofcode);

	DynamicJsonDocument doc(1024);	

	
	doc["code"]=String(nameofqcode);
	doc["time"] = timeofcode;
	
	/*gán với bill gần nhất, cho dù có up hay chưa*/
	
	/*mỗi lần khởi động  sẽ load last bill*/
	// get_lastbill_name(upload_manager_info.tot);
	
	doc["lastbill"] = lastbil;

	serializeJson(doc,Serial);
	Serial.println("");
	char dataBuffer[1024];
	serializeJson(doc,dataBuffer,1024);

	int len = strlen(dataBuffer);
	
	// Serial.print("\n buffer send = ");
	// Serial.println(len);	
	// Serial.println(dataBuffer);
	
	char channel_[100];  
	sprintf(channel_,"PRINTER/%s/BARCODE",mynamedevice);

	// bool publish(const char topic[], const char payload[], int length, bool retained, int qos);
	
	#if (DF_DEBUG_NOUPLOAD_TO_SERVER)	
		kq =1;
		Serial.println("debug no upload barcode");	
	#else 
		kq = clientmqtt.publish(channel_,dataBuffer,len,1,2);
	#endif 

	// Serial.println("\n\n!!!!!!!!!!!!!!!!!!!!!!!!!debug nen co dong nay 5596\n");
	// kq =1;
	Serial.print("pub barcode = ");
	Serial.println(kq);
	return kq;
}

void process_upbarcode()
{
	if (ui8_need_pub_barcode == 0) return;

	if (millis() < u32_time_resend_barcode) return;
	
	// select_uart(UART_4G);
	// kiểm tra kết nối
	if (!clientmqtt.connected()) return;

	if(ui8_busy_rec_printer == 1) return;

	int kq =updatebarcode_func();
	if (kq ==1)
	{
		ui8_need_pub_barcode =0;
	}
	else
	{
		//sau 2 giay up lai
		u32_time_resend_barcode = millis()+2000;
	}
}
void updatemqtt_func()
{
	// select_uart(UART_4G);
	Serial.println("Load data");

	int u = 0;
	uint8_t j = 0;
	/*
	16k không publish được
	
	*/
	for(int i =0; i< 8*1024; i++)
	{	  	  
		VALUE[u] = j; 
		u++;
		j++;
		// if( j >'z') j ='a';	  
	}
	VALUE[u] = '\0';
	Serial.print("\n buffer pub = ");
	Serial.println(u);	
	
	char channel_[100];  
	sprintf(channel_,"PRINTER/%s/BILL",mynamedevice);

	// bool publish(const char topic[], const char payload[], int length, bool retained, int qos);
	uint32_t tam = millis();
	int kq = clientmqtt.publish(channel_,VALUE,u,0,2);

	tam = millis() - tam;
	Serial.print("\n pub result = ");
	Serial.println(kq);
	Serial.print("\n take time = ");
	Serial.println(tam);
}

void send_opu_status()
{			
	//kiểm tra
	if (millis() < u32_time_resend_opu) return;
	
	if (!clientmqtt.connected()) return;
		
	// publish a message roughly every minutes
	// if (millis() - lastMillis > df_time_send_opu) {
		// lastMillis = millis();
		// check_firmware++;
		// char msg[100];

		// Allocate the JSON document
		//
		// Inside the brackets, 200 is the RAM allocated to this document.
		// Don't forget to change this value to match your requirement.
		// Use arduinojson.org/v6/assistant to compute the capacity.
		StaticJsonDocument<500> doc;

		// StaticJsonObject allocates memory on the stack, it can be
		// replaced by DynamicJsonDocument which allocates in the heap.
		//
		// DynamicJsonDocument  doc(200);

		// Add values in the document
		/*
		{
		"code": "<DEVICE_ID>",
		"printerModel": "<PRINTER MODEL>",
		"printerBand": "<PRINTER BRAND>",
		"network":"4G|LAN|WIFI",
		"ip": "<ip của thiết bị nếu có>",
		"printerSerial": "<serial may in>",
		"barcodeModel": "<barcode model>",
		"barcodeBrand": "<barcodeBrand>",
		"barcodeStatus": 1|0,
		"printerStatus": 1|0,
		"mode":"S3|MQTT"
		}
		*/
		doc["code"] = String (mynamedevice);
		doc["printerModel"] = "TM-T88IV";	//Product
		doc["printerBrand"] = "EPSON";	//Manufacturer
		doc["printerSerial"] = "L9LF023112";	//Manufacturer
		
		if (device_status.printer == 2)
		{
			doc["printerStatus"] = 0;
		}
		else
		{
			doc["printerStatus"] = device_status.printer;
		}

		doc["network"] = "WIFI";	//Manufacturer
		doc["ip"] = "192.168.80.121";	//Manufacturer
		// doc["mode"] = "S3";
		doc["mode"] = "MQTT";
		
		doc["barcodeModel"] = "";	//Manufacturer
		doc["barcodeBrand"] = "";	//Manufacturer
		doc["barcodeSerial"] = "";
		
		if (device_status.barcode == 2)
		{
			doc["barcodeStatus"] = 0;
		}
		else
		{
			doc["barcodeStatus"] = device_status.barcode;
		}
		
		char msg[500];
		serializeJson(doc, msg);
		
		char channel_[100];  
		sprintf(channel_,"PRINTER/%s/INFO",mynamedevice);

		// bool publish(const char topic[], const char payload[], int length, bool retained, int qos);
		
		int need_upload_opu  = clientmqtt.publish(channel_,msg,1,2);

		if(need_upload_opu == 1)
		{
			/*ok: init long time*/
			u32_time_resend_opu = millis() + 20*60*60*1000; /*20h*/
		}
		else
		{
			/*fail: init short time*/
			u32_time_resend_opu = millis() + 5000;
		}

  	// }	
}


// void messageReceived(MQTTClient *client, char topic[], char bytes[], int length)
void messageReceived(String &topic, String &payload)
{
  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
	// Serial.print(F(">>>>************>>>IN\n"));
	SerialDEBUG.println("incoming: ");
	SerialDEBUG.println(topic);
	SerialDEBUG.println(payload);
	StaticJsonDocument<1024> doc;
    // Deserialize the JSON document
  	DeserializationError error = deserializeJson(doc, payload);
    if (error)
	{
		// #ifdef use_debug_serial
		SerialDEBUG.println(F("This is a JDaughter string, not a JSON string"));
		// #endif 
		return;
	}
	
	//{"name":"BIL20231218043438","chunk":5,"time":1703316283}
	
	if(inforeget.status ==0)
	{
		String strtam = doc["name"].as<String>();
		int chunk_tam		= doc["chunk"] | 1000;

		strtam.toCharArray(inforeget.name_tofind, 24);
		inforeget.chunk = chunk_tam;
		inforeget.status =1;
		inforeget.index =0;
		
		SerialDEBUG.print(F("\n"));
		SerialDEBUG.print(F("name reget ="));
		SerialDEBUG.println(String(inforeget.name_tofind));
		SerialDEBUG.print(F("chunk reget ="));
		SerialDEBUG.println(inforeget.chunk);
	}
	else
	{
		SerialDEBUG.print(F("busy, ignore this cmd \n"));
	}

}

/*------------------------------------MQTT FUNTIONS END------------------------------------------------*/

void formatdisk_func()
{
	Serial.print("formatdisk_func");
	totalkichthuoc =0;
	
	updatepart=0;
	totalup=0;
	ui8_need_upload =0;

	upload_manager_info.mid=0;
	upload_manager_info.tot=0;
	upload_manager_info.next_addr_data=DF_ADDR_DATA;
	upload_manager_info.part=updatepart;
	upload_manager_info.duration=totalup;
	upload_manager_info.mode = ui8_need_upload;
	stt_storing_file =1;//chứa file tiếp theo
	strcpy(upload_manager_info.nameofbill,"FORMAT DISK");
	// spiff_updatemediafile("/up_manage.h", &upload_manager_info,1);
	spiff_updatemediafile(name_up_manager, &upload_manager_info,1);
	if (flash.eraseChip())
	{
		Serial.println(F("Chip erased"));
	}		
	else
	{			
		Serial.println(F("Error erasing chip"));
	}
	Serial.println("formatdisk_func finish");
}


/*
check_format_disk
điều kiện format disk:
	-khác ngày
	-không còn file để upload
	
	-trên 130 file
*/

void check_format_disk()
{
	if (ui8_force_format == 1)
	{
		if (millis() > u32_time_force_format)
		{
			Serial.println(F("process cause corupt memory"));
			
		}
		ui8_force_format =0;/* mục đích :nếu erro thì vẫn cho ghi, đọc file bình thường*/
	}
		
	if (upload_manager_info.mid < upload_manager_info.tot) return;
	if (upload_manager_info.mode == 1) return;
	
	if (upload_manager_info.tot > max_file_save -1)
	{
		Serial.println(F("Format cause file reach memory"));
		formatdisk_func();		
	}

	if (SSD_info.daystatus)
	{
		if(SSD_info.last_day != SSD_info.new_day)
		{
			Serial.println(F("Format cause different day"));
			formatdisk_func();
			SSD_info.last_day = SSD_info.new_day;			
			spiff_putSSD_info(name_disk_info, &SSD_info);
		}
	}
}