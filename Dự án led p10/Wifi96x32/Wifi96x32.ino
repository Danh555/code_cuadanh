/**
 * Led Text Mobiphone HUB75E Project
 * Communication Control board
 * Hardware: Esp32 Wroom
 * 
 * Config:
 * Board ESP32 DEV MODULE
 * Upload Speed 921600
 * CPU Frequency 240MHZ (Wifi/BT)
 * Flash Frequency 80MHZ
 * Flash mode QIO
 * Flash size 4Mb (32Mb)
 * Partition Scheme: Default 4Mb with spiffs (1.2MB APP/1.5MB SPIFFS)
 * Jun072021:
*/
/**
Test bằng phần mềm MQTTX:
Name			bất kỳ
Client ID		bất kỳ
Host			mqtt://		mbs.mobifone.vn
Port			1883
Username		any/empty
Password		any/empty
SSL/TLS			off
MQTT Version	3.1


*/

/**
Thêm xử lý nút nhấn bị kẹt

V MQTT_Matrix_Mobiphone_4G_36betta:

v.x.4.3

1. fix lỗi chạy theo lịch nội dung hiển thị null
	nguyên nhân: do bị tràn buffer json

v.2.4.2

1. Thêm chức năng tự động đăng nhập trang config wifi nếu không có tín hiệu phản hồi từ board matrix trong vòng 2.5p

v.2.4.1

1. Thêm phần cứng V2

V1.3.5:

1. Thêm chức năng cấu hình cho bảng led matrix
2. Chỉnh lại đọc thời gian/ cập nhật RTC, dùng thư viện RTClib-master

V1.3.3:

1. Xử lý thêm đọc thời gian từ realtime clock-->chưa được, thư việc RTClib-master đang bị lỗi năm nhảy lên liên tục
2. Cập nhật update firmware vào mục setting wifi.

V1.3.2:

1. Qui định lại cách đặt tên version
2. Chỉnh lại lệnh update firmware từ server xuống board

V2.1.10:

0. "Wname":"" khi không kết nối được wifi, thông tin wifi name bị xóa--> đã xử theo mục 1
1. Chỉnh lại cách kết nối wifi: ép kết nối vào wifi truyền qua, không để kết nối theo kiểu lưu trước đó của wifimanager
2. Điều chỉnh lại truyền qua bảng led: (do bị lỗi Guru Meditation Error: Core  1 panic'ed (Cache disabled but cached memory region accessed))

V2.1.8:

1. Cấu hình này đang nạp cho bảng led ngoài Hà Nội V0, @ Mar152022

V2.1.6:

1.Thêm config ngay lúc khởi động, để reset lại hệ thống: trong trường hợp bị lỗi file...doing
2. Thêm chức năng update firmware server manual lúc khởi động, code nằm trong wifimanager.
3. Chỉnh lại giao diện config lúc khởi động

V2.1.5:

1. Chỉnh lại nút nhấn vào chế độ config
2.	Thêm chế độ load lại data config nếu thay đổi tên file config cũ 

V2.1.1:

1. Dùng modem Sim4G SIM7600CE
2. Thêm chức năng cập nhật thời gian cho esp
3. Thêm chọn mode chạy: wifi hay 4G

V1.1.21:

1. Dùng modem Sim4G SIM7600CE


V1.1.19:

1. Thêm lệnh reset index
2. Thêm các trạng thái báo qua led: Khi khởi động thì chớp nhanh liên tục


V1.1.18:

V1.1.17:

1. Thêm chức năng OTA qua bảng led control: //OTA 1
2. Tự tạo Device code mới
3. Bỏ chức năng update tự động: vẫn giữ update bằng lệnh MQTT

V1.1.16:
1. Thêm chức năng upgrade frirmware bằng wifi do chip esp phát ra

V1.1.15:
1. Thêm wifi manager để cấu hình thông số bảng Led

V1.1.14:
1. fix lỗi so sánh đúng file txt: so sánh url có chứa .txt là xử lý

V1.1.13 Beta:
1. Thêm so sánh đúng file txt mới xử tiếp


V1.1.13:
1. Thêm chỉnh thông số bằng APP


V1.1.10:

1. Đổi server, user,pass
2. Bỏ không kiểm tra ID code nhận được


V1.1.9:

1. Mode =0: không cần load file ok
2. Mode =0: phải cập nhật tất cả file có ID trong list lại ok
3. Delete: phải cập nhật tất cả file có ID trong list lại ok
4. Có file mới vào: xóa hết danh sách, load lại từ đầu, trừ những slot tin khẩn cấp ra ok
5. Thêm thời gian 30s gửi lại lệnh qua bảng led chưa ok
6. Đang chạy mất điện/chip bị reset: chưa lưu tin loại khẩn cấp.--------------------------

V1.1.8:

1. FIXED Hết giờ phát: tắt ngang bản tin
2. fixed Cập nhật sẵn danh sách phát
3. fixed Tin có mode 0: dừng phát liền (nếu tin đó đang phát)
4. fixed Delete: xóa, và dừng phát nếu tin đó đang phát
5. Empty duration: thay bằng dấu hiệu khác
6. Tin bị lỗi không phát
7. fixed Tin khẩn cấp: phát nối tiếp, phát xong xóa tin đó luôn.
8. fixed Gửi lệnh qua bảng led bị delay 5s: fixed cho lệnh STOP

V1.1.7:

1. Xử lý nhiều tin xuống cùng lúc. 

V1.1.5:

1. Chỉnh lại format gói data OPU_STATUS, OPU_GENERIC

V1.1.3:

1. Sửa lại channel h2/d/id thành h2/t/id, và h2/g/111 thành h2/gt/111

Dec012021_22h16: v1.1.0

1. Thêm tự động cập nhật firmware

Dec012021_22h16:

1.	Thêm lệnh xóa file từ server
2.	Tự động Xóa file đã expired
3.	Cập nhật dung lượng còn lại: chưa

Nov292021_22h58:

1.	Thêm chức năng OTA: cập nhật firmware từ xa.

Nov292021_10h07:

1.	Thêm gói cập nhật send_opu_generic()
2.	Sửa lại nội dung gói send_opu_status()
*/

/*	example:  Examples/wifi/WifiClient
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */
 
// by Joël Gähwiler
// https://github.com/256dpi/arduino-mqtt

#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include "SPIFFS.h"

#include "config_hw_LMD_wifi_gloabl.h"

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */
#define FORMAT_SPIFFS_IF_FAILED true
#define INTERRUPT_ATTR IRAM_ATTR
#include <time.h>
long timezone = 7; 
byte daysavetime = 0;//=0 hay bằng 1?????????????????
#include <WiFi.h>
#include <MQTT.h>
//thư viện dùng OTA
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include "cert.h"
#include <Update.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

//-------------Realtime Clock DS1307--------------------------
#include <Wire.h>
#include "RTClib.h"

// Create RTC object
RTC_DS1307 rtc;
const char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//-------------modem 4G--------------------------
// Select your modem:
#define TINY_GSM_MODEM_SIM7600
// Set serial for debug console (to the SerialDEBUG Monitor, default speed 115200)

#if defined (hwv0)
	#define SerialLMD Serial2
#elif defined (hwv1)
	#include "SoftwareSerial.h"
	SoftwareSerial SerialLMD;
	
	#define SerialMon SerialDEBUG
	// Set serial for AT commands (to the module)
	// Use Hardware SerialDEBUG on Mega, Leonardo, Micro
	#define SerialAT Serial2
	// Define the serial console for debug prints, if needed
	#define TINY_GSM_DEBUG SerialMon	
	#define SerialDEBUG Serial
	#define use_debug_serial
	
#elif defined (hwv2)
	#define SerialLMD Serial
	#define SerialAT Serial2	
#else
	#error "Please define one of hwv in config_hw_local.h"
#endif

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS


// Range to attempt to autobaud
#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 115200

// Define how you're planning to connect to the internet
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// set GSM PIN, if any
#define GSM_PIN ""
#include <TinyGsmClient.h>
// #include <ArduinoHttpClient.h>
//test values
// Server details
// const char servertest[]   = "vsh.pp.ua";
// const char resource[] = "/TinyGSM/logo.txt";
// const int  port       = 443;
uint8_t landautien=0;

//test values

// Your GPRS credentials, if any
const char apn[] = "v-internet";
const char gprsUser[] = "";
const char gprsPass[] = "";

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
  TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

TinyGsmClient GSM1(modem);
MQTTClient clientGSM(1024);//chiều dài buffer

/*
#if defined (hwv0)
	#define SerialLMD Serial2
#elif defined (hwv1)
	#include "SoftwareSerial.h"
	SoftwareSerial SerialLMD;
#endif
*/


#include <ArduinoJson.h>

#include "user_frame.h"
#include "Globalvariables.h"
#include "Globalvariables_4G.h"

#include "esp_process_file.h"

#include "SIM7600HTTPS.h"

//-----------------------OTA------------------------------------------------------

WebServer server(80);
const char* serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";
//-----------------------End OTA------------------------------------------------------


//-----------------eeprom--------------------
#include "EEPROM.h"
#define EEPROM_SIZE 64

// char rname[32];
//-----------------end eeprom--------------------
//-------------------wifi--------------------

char* ssid     = "Nokia 1202 S+";
char* pass = "0938477008";

// const char ssid[] = "LED 1";
// const char pass[] = "123456789";


WiFiClient net;
MQTTClient client(1024);//chiều dài buffer


// const char* host = "https://alta-s3.dev-altamedia.com";
// const char* host = "mbs.mobifone.vn";
// const char* streamId   = "....................";
// const char* privateKey = "....................";


int hieuung=0;
String line2= "Welcome to ALTA Media! 2021";
String strtest= "Hello the world!";
//-------------------end wifi--------------------

//-----------------Variables--------------------
int heart_beat();
//-----------------End Variables--------------------
/** connect()
Kết nối mqtt bằng wifi

*/
void connect()
{
	int m;
	
	#ifdef use_debug_serial
		SerialDEBUG.println("checking wifi...");
	#endif 
	hieuung=0;
	while (WiFi.status() != WL_CONNECTED) {
	
		system_status=SYS_ERR0;//co loi
		hieuung++;		
		
		/*
		m=heart_beat();		
		while(m==heart_beat())
		{
			check_button();			
		}
		*/

		delay_reset();
		while(delay_withoutdelay())
		{
			check_button();
			heart_beat();			
		}
		
        if(check_key_press())
		{
			//có nhấn nút nên reset hieuung để tăng thời gian đọc nút nhấn.
			hieuung=0;
		}

		
		#ifdef use_debug_serial
		SerialDEBUG.println(hieuung);
		#endif 
						
		if(hieuung>4)
		{									
			break;		
		}		
    }

	if(hieuung>4)
	{
		need_reset_chip++;
		#ifdef use_debug_serial
		SerialDEBUG.print("WiFi connect fail @");
		SerialDEBUG.println(need_reset_chip);
		#endif 
		if(need_reset_chip>60)///~240s
		{
			#ifdef use_debug_serial
			SerialDEBUG.println("Reset cause WiFi connect fail");
			#endif 
			delay(2000);
			ESP.restart();
			delay(2000);		
		}
	}
	else 
	{
		need_reset_chip=0;
		system_status=SYS_NORMAL;	    
		#ifdef use_debug_serial
		SerialDEBUG.println("WiFi connected");
		SerialDEBUG.println("IP address: ");
		SerialDEBUG.println(WiFi.localIP());
		#endif 
		
		char Zone_Alarm[100];
	  
		  //Will Message: {‘mgw_id’ : ‘id’}.	  
		  sprintf(Zone_Alarm,"{\"mgw_id\": %s}",mqtt.device_code);
		  #ifdef use_debug_serial
		  SerialDEBUG.printf("\nwill msg: %s",Zone_Alarm);
		  #endif 
		  
		  //setWill(const char topic[], const char payload[], bool retained, int qos);
		  client.setWill("h2/will", Zone_Alarm , 0, 2);
		  
		  #ifdef use_debug_serial
		  // SerialDEBUG.println(F("\nThêm timeout để thoát khỏi vòng lặp"));
		  SerialDEBUG.print("\nconnecting to server...");
		  #endif 
		  hieuung=0;
		while (!client.connect(mqtt.client_id, mqtt.username, mqtt.pwdMqtt)) {								
			system_status=SYS_ERR_SERVER;
			hieuung++;			
			delay_reset();
			while(delay_withoutdelay())
			{
				check_button();
				heart_beat();	
			}
			if(check_key_press())
			{
				//có nhấn nút nên reset hieuung để tăng thời gian đọc nút nhấn.
				hieuung=0;
			}		
			#ifdef use_debug_serial
			SerialDEBUG.println(hieuung);
			#endif 
			
			if(hieuung>4)
			{									
				break;		
			}
		}
			
			if(hieuung>4)
			{
				#ifdef use_debug_serial
				SerialDEBUG.println("\nconnect server FAIL!");
				#endif 
			}
			else
			{
				system_status=SYS_NORMAL;
				#ifdef use_debug_serial
				SerialDEBUG.println("\nconnected!");
				#endif 


				// bool publish(const String &topic, const String &payload, bool retained, int qos);
				#ifdef use_debug_serial
				SerialDEBUG.println("publish h2/will: online");		
				#endif 

				if(mqtt.mode_data_is_wifi)
				{
					client.publish("h2/will", "online",0,2);	
				}
				else
				{
					clientGSM.publish("h2/will", "online",0,2);		
				}
				
				
				delay(1000);
				#ifdef use_debug_serial
				SerialDEBUG.println("send_opu_generic");
				#endif 
				send_opu_generic();
				delay(1000);
				
				sprintf(Zone_Alarm,"h2/t/%s",mqtt.device_code);
				#ifdef use_debug_serial
				SerialDEBUG.println(Zone_Alarm);
				#endif 
				client.subscribe(Zone_Alarm, 2); //2 la QoS
				
				//sub group				
				client.subscribe("h2/gt/111", 2);
				client.subscribe("h2/gt/222", 2);
				for(int i=0; i<20;i++)
				{
					if(opu_generic_system.group[i]==0)break;
					sprintf(Zone_Alarm,"h2/gt/%d",opu_generic_system.group[i]);
					#ifdef use_debug_serial
					SerialDEBUG.println(Zone_Alarm);
					#endif 
					client.subscribe(Zone_Alarm, 2); //2 la QoS
				}						
			}		
	}
}

bool mqttConnect()
{
	//SerialDEBUG.print("Reconnect to GPRS_MQTT 179");
  //clientGSM.onMessageAdvanced(messageReceived);/// *////
  if (modem.isGprsConnected())
  {
    need_reset_chip=0;
	system_status=SYS_NORMAL;

    // CheckKetNoiGPRS = 1;
		char Zone_Alarm[100];

		//Will Message: {‘mgw_id’ : ‘id’}.	  
		sprintf(Zone_Alarm,"{\"mgw_id\": %s}",mqtt.device_code);
		#ifdef use_debug_serial
			SerialDEBUG.printf("\nwill msg: %s",Zone_Alarm);
		#endif 

		//setWill(const char topic[], const char payload[], bool retained, int qos);
		clientGSM.setWill("h2/will", Zone_Alarm , 0, 2);

		// SerialDEBUG.println(F("\nThêm timeout để thoát khỏi vòng lặp"));
		#ifdef use_debug_serial
		SerialDEBUG.print("\nconnecting to server via 4G...");
		#endif 
		hieuung=0;
    if (clientGSM.connect(mqtt.client_id, mqtt.username , mqtt.pwdMqtt)) //clientGSM.connect(CLIENT_ID,mqtt_user, mqtt_pwd)) // Thực hiện kết nối với mqtt user và pass
    {
      			system_status=SYS_NORMAL;
				#ifdef use_debug_serial
					SerialDEBUG.println("\nconnected!");		
					// bool publish(const String &topic, const String &payload, bool retained, int qos);
					SerialDEBUG.println("publish h2/will: online");		
				#endif 
				clientGSM.publish("h2/will", "online",0,2);
				delay(1000);
				#ifdef use_debug_serial
		SerialDEBUG.println("send_opu_generic");
		#endif 
				send_opu_generic();
				delay(1000);
				
				sprintf(Zone_Alarm,"h2/t/%s",mqtt.device_code);
				#ifdef use_debug_serial
		SerialDEBUG.println(Zone_Alarm);
		#endif 
				clientGSM.subscribe(Zone_Alarm, 2); //2 la QoS
				
				//sub group				
				clientGSM.subscribe("h2/gt/111", 2);
				clientGSM.subscribe("h2/gt/222", 2);
				for(int i=0; i<20;i++)
				{
					if(opu_generic_system.group[i]==0)break;
					sprintf(Zone_Alarm,"h2/gt/%d",opu_generic_system.group[i]);
					#ifdef use_debug_serial
		SerialDEBUG.println(Zone_Alarm);
		#endif 
					clientGSM.subscribe(Zone_Alarm, 2); //2 la QoS
				}
    }
    else
    {
      //SerialDEBUG.println(F(" Connect MQTT fail 231"));
      //watchdogStart();
      // wdt_reset();
	  system_status=SYS_ERR_SERVER;
      // ketnoiGPRS = ketnoiGPRS + 1;
      //SerialDEBUG.print(F(" So lan ket noi MQTT GPRS FAIL 431:"));
      //SerialDEBUG.println(ketnoiGPRS);

    }
  } 
  else
  {
    if (modem.isNetworkConnected())
    {
       if (!modem.gprsConnect(apn, gprsUser, gprsPass))
      {
        // digitalWrite(gsm_status, 0);
        #ifdef use_debug_serial
		SerialDEBUG.println(F(" fail"));
		#endif 
		delay(1000);
        //delay(10000);
        //return;
        // ketnoiGPRS = ketnoiGPRS + 1;
        // CheckKetNoiGPRS = 0;
        // SerialDEBUG.print(F(" So lan ket noi MQTT GPRS FAIL 443:"));
        //  SerialDEBUG.println(ketnoiGPRS);
      }
      else
      {
        #ifdef use_debug_serial
		SerialDEBUG.println(F(" success"));		
		#endif 
        // digitalWrite(gsm_status, 1);
        // CheckKetNoiGPRS = 1;
      }
    } else
    {
       need_reset_chip++;
	  #ifdef use_debug_serial
		SerialDEBUG.println(F(" not conect to tleco"));
	 
	  SerialDEBUG.println(need_reset_chip);
	  #endif 
		if(need_reset_chip>10)
		{
			#ifdef use_debug_serial
		SerialDEBUG.println("Reset cause tleco connect fail");
		#endif 
			delay(2000);
			ESP.restart();
			delay(2000);		
		}
	  
      // digitalWrite(gsm_status, 0);
      // CheckKetNoiGPRS = 0;
    }
    if (modem.isGprsConnected())
    {
      #ifdef use_debug_serial
		SerialDEBUG.println(F("GPRS connected"));
		#endif 
      // digitalWrite(gsm_status, 1);
      // CheckKetNoiGPRS = 1;
    }

  }
}
void resubfunc()
{
	#ifdef use_debug_serial
		SerialDEBUG.println("resubfunc:");
		#endif 
	
	char Zone_Alarm[50];
	
	for(int i=0; i<20;i++)
	{
		if(opu_generic_system.group[i]==0)break;
		sprintf(Zone_Alarm,"h2/gt/%d",opu_generic_system.group[i]);
		#ifdef use_debug_serial
		SerialDEBUG.println(Zone_Alarm);
		#endif 
		client.subscribe(Zone_Alarm, 2); //2 la QoS
	}
}
void messageReceived(String &topic, String &payload) {
  #ifdef use_debug_serial
		SerialDEBUG.println("incoming: \n" + topic + " - " + payload);
		#endif 

uint32_t tset;
char name[30];
  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
  
  //AppData(N) = {“id”: id, “opcode”: opcode, “data”: Interface}
  //process data
  StaticJsonDocument<1024> doc;
    // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, payload);
    if (error)
	{
		#ifdef use_debug_serial
		SerialDEBUG.println(F("This is a JDaughter string, not a JSON string"));
		#endif 
		return;
	}
	
	//bỏ qua không cần so sánh ID
	/*
	char id_received[20];

		strlcpy(id_received,                  // <- destination
			doc["id"] | "100",  // <- source
			sizeof(id_received));         // <- destination's capacity	
	
	// uint32_t id_received = doc["id"] | 1;
	//so sánh id
	//0 or main
	SerialDEBUG.println("ID =" + String(id_received));
	if(id_received=="0")
	{
		SerialDEBUG.print(F("Broadcast message\n\r"));
	}
	else if(mqtt.device_code != id_received) 
	{	
		SerialDEBUG.print(F("Wrong ID: "));
		SerialDEBUG.println(String(mqtt.device_code) + " : " + String(id_received));		
		SerialDEBUG.print(F("Debug mode: continue process************************\\n\r"));
		SerialDEBUG.print(F("****Remember to uncomment this line RETURN\n\r"));
		return;
	}
	*/	
	int opcode_received = doc["opcode"] | 1000;
	  
	
	#ifdef use_debug_serial
		
		SerialDEBUG.println("opcode_received =" + String(opcode_received));
		#endif 
	
	uint32_t u32_value_;
	uint16_t u32_index_;
	switch (opcode_received)
    {   case OPH_GROUP_UPDATE: 
			// SerialDEBUG.println(F("Cập nhật danh sách các Group cho MGW"));
			u32_index_ =  doc["data"].size();
		
			if(u32_index_>20)u32_index_=20;
			
			for (int i = 0; i< u32_index_; i++){
 
			   opu_generic_system.group[i]=doc["data"][i];
			   			 
			}

			//save
			spiff_updatefile(oph_generic_filename, &opu_generic_system);
			u8_needreub_channle=1;
			//resub channel=group
			break;
        case OPH_ACTIVATE:
			
			opu_generic_system.disabled=doc["data"] | 0;
			
			//cần lưu lại data này
			// SerialDEBUG.print(F("XEM LẠI CHỖ NÀY************************\\n\r"));
			
			spiff_updatefile(oph_generic_filename, &opu_generic_system);

			if(opu_generic_system.disabled==0)
			{
				// SerialDEBUG.print(F("Enable MGW************************\n\r"));
				index_send++;
				index_lmd=100;
			}
			else if (opu_generic_system.disabled==1)
			{
				// SerialDEBUG.print(F("Disable MGW************************\\n\r"));
				index_send++;
				index_lmd=101;
			}			
					
			break;

		case OPH_MEDIA_HTTP:						
			/*

					"id": 12111,
					"opcode" : 67,
					"data":{"mid":12345678,"mode": 2,"ts":[49560],"ds":[300],"duration":300,"created":1638779409,"size":0,"codec":1,"format":1,"cs":0,"url":"https://raw.githubusercontent.com/DaikCong/Hello-World/master/hello.txt","auth":"","loop":0,"prio":0,"expired":1670365040,"start":1638829040,"repeat":0,"days":255,"header":255}
			*/				
			#ifdef use_debug_serial
		SerialDEBUG.print(F("Case OPH_MEDIA_HTTP:\n\r"));			
		#endif 
			
			//đúng file txt mới xử lý tiếp, không thì biến
			oph_media_temp.url= doc["data"]["url"].as<String>();
						
			  // int uuu=oph_media_temp.url.indexOf("txt");
			  
			  if (oph_media_temp.url.indexOf(".txt")>0) {
				#ifdef use_debug_serial
		SerialDEBUG.println("Got file txt");
		#endif 
			  }
			  else
			  {
				  #ifdef use_debug_serial
		SerialDEBUG.println("Not txt file ");
		#endif 
				  break;
			  }
			
			oph_media_temp.mid=doc["data"]["mid"];
			oph_media_temp.prio=doc["data"]["prio"];
			oph_media_temp.created=doc["data"]["created"];
			oph_media_temp.expired=doc["data"]["expired"];
			oph_media_temp.mode=doc["data"]["mode"];
			if(oph_media_temp.mode==2)phatngaychotoi=1;
			oph_media_temp.start=doc["data"]["start"];
			oph_media_temp.repeat=doc["data"]["repeat"];
			oph_media_temp.days=doc["data"]["days"];
			oph_media_temp.looop=doc["data"]["loop"];
			oph_media_temp.duration=doc["data"]["duration"];

			u32_index_ =  doc["data"]["ts"].size();
		
			if(u32_index_>24)u32_index_=24;
			
			for (int i = 0; i< u32_index_; i++){
 
			   oph_media_temp.ts[i]=doc["data"]["ts"][i];
			 
			}
			
			u32_index_ =  doc["data"]["ds"].size();
		
			if(u32_index_>24)u32_index_=24;
			for (int i = 0; i< u32_index_; i++){
			   oph_media_temp.ds[i]=doc["data"]["ds"][i];

			}
			
			oph_media_temp.url= doc["data"]["url"].as<String>();						
			oph_media_temp.auth= doc["data"]["auth"].as<String>();
				
			//lưu vào bộ nhớ để xử lý sau
			//tránh trường hợp message xuống nhiều quá bị đè lên nhau

			sprintf(name,"/F%d.lmd",oph_media_temp.mid);		
			spiff_updatemediafile(name, &oph_media_temp,0);
			
			//kiểm tra stop file
			//nếu stop thì không cần xử lý file nữa
			if(oph_media_temp.mode ==0) 
			{
				stop_playlist(oph_media_temp.mid);
				break;
			}
			
			
			//lưu lại tên file
			
			if(index_storing>store_index_max-1)index_storing=store_index_max-1;
			
			for (int y=0;y<index_storing;y++)
			{			
				if(store_name[0] != dfnofile)break;
				
				for(int i=0; i <index_storing;i++)				
				{
					store_name[i]=store_name[i+1];														
				}
				//clear end of file
				store_name[store_index_max-1]=dfnofile;
			}
			
			//search empty memory			
			
			for(int m=0; m <store_index_max;m++)				
			{
				if (store_name[m]==dfnofile)
				{
					store_name[m]=oph_media_temp.mid;
					index_storing++;
					#ifdef use_debug_serial
		SerialDEBUG.println("Insert file: " + String (store_name[m]) + " @ " + String(m));
		#endif 
					
					break;
				}					
			}
			
			havedata=1;
			
			break;
		case  OPH_MEDIA_DELETE:
			u32_value_=doc["data"];
			#ifdef use_debug_serial
		SerialDEBUG.println("Delete this msg =" + String(u32_value_));
		#endif 
			// int t=sizeof(uint32_t)+10;
			// char name[30];
			sprintf(name,"/F%d.lmd",u32_value_);
			deleteFile(SPIFFS,name);

			//stop led nếu msg này đang phát			
			stop_playlist(u32_value_);

			break;
			
		case  OPH_MEDIA_DELETE_ALL:
			u32_value_=doc["data"];
			#ifdef use_debug_serial
		SerialDEBUG.println("Delete all msg: " + String(u32_value_));			
		#endif 
			if(u32_value_==0)
			{			
				deleteallmdeia(SPIFFS, "/", 0);
				//stop toan bo danh sach
				for (int i=0;i<max_file_process-1;i++)
				{							
					media_manager[i].ts=0;
					media_manager[i].ds=0;
					media_manager[i].mode=0;
				}
				for(int m=0; m <store_index_max;m++)				
				{
					store_name[m]=dfnofile;
				}
								
				//gửi qua bảng led liền
				index_send++;								
				lmd_command=LMD_STOP;
				lmd_repeat=0;
				u32_lmd_resend=0;//gửi liền không cần chờ
				
				// send_command_screen();				
			}

			break;
		case  OPH_TIME:
			info_time_system.timeset=doc["data"];
			#ifdef use_debug_serial
		SerialDEBUG.println("Time from server: " + String(info_time_system.timeset));
		#endif 
			//cập nhật thời gian
			info_time_system.process=TIME_FINISH;
			break;
		case  OPH_REBOOT:
			u32_value_=doc["data"];
			#ifdef use_debug_serial
		SerialDEBUG.println("OPH_REBOOT: " + String(u32_value_));
		#endif 
			ESP.restart();			
			while(1){
				delay(1);
			}			
			break;
		case  OPH_UDF_2:
			u32_value_=doc["data"];
			#ifdef use_debug_serial
		SerialDEBUG.println("Check upgrade now: " + String(u32_value_));
		#endif 
			
			if(u32_value_==UPDATE_WIFI){
				firmwareUpdate();
			}			
			else if(u32_value_==UPDATE_LED)
			{
				#ifdef use_debug_serial
		SerialDEBUG.println("Check upgrade UPDATE_LED");
		#endif 
				index_lmd=102;
			}
			else if(u32_value_==UPDATE_WIFI_OTA)
			{
				#ifdef use_debug_serial
		SerialDEBUG.println("Check upgrade UPDATE_WIFI_OTA");
		#endif 
				setup_reset_fatory_wifi();
			}
			else if(u32_value_==UPDATE_LED_OTA)
			{
				#ifdef use_debug_serial
		SerialDEBUG.println("Check upgrade UPDATE_LED_OTA");
		#endif 
				index_lmd=103;
			}
			
			
			break;			
    }
}
void stop_playlist(uint32_t mid_)
{

	#ifdef use_debug_serial
		SerialDEBUG.print("stop_playlist mid = " + String(mid_) +"\n");
		#endif 
	for (int i=0;i<max_file_process-1;i++)
	{					
		if(media_manager[i].mid==mid_)
		{
			media_manager[i].ts=0;
			media_manager[i].ds=0;
			media_manager[i].mode=0;//trường hợp là tin khẩn cấp: phải xóa mode =0
		}
	}				
}

void print_playlist()
{
	#ifdef use_debug_serial
		SerialDEBUG.println("print_playlist");
		#endif 
	empty_playlist=1;
	for (int i=0;i<max_file_process-1;i++)
	{							
		if (media_manager[i].playing==PLAYER_STOP)
		{
			break;
		}
		else
		{
			#ifdef use_debug_serial
		
			if(media_manager[i].mode==0)SerialDEBUG.print("\t @" + String(i) + " Mode khong phat");
			else if(media_manager[i].mode==1)SerialDEBUG.print("\t @" + String(i) + " Mode theo lich");
			else if(media_manager[i].mode==2)SerialDEBUG.print("\t @" + String(i) + " Mode khan cap");
			else SerialDEBUG.print("\t @" + String(i) + " Mode Unknow" +String(media_manager[i].mode));
			
			if(media_manager[i].playing==PLAYER_READY)SerialDEBUG.print(", " + String(i) + " PLAYER_READY");
			if(media_manager[i].playing==PLAYER_INTIME)SerialDEBUG.print(", " + String(i) + " PLAYER_INTIME");
			if(media_manager[i].playing==PLAYER_PLAYING)SerialDEBUG.print(", " + String(i) + " PLAYER_PLAYING");			

			SerialDEBUG.print(", " + String(i) + " mid= " + media_manager[i].mid);
			SerialDEBUG.print(", " + String(i) + " ts= " + media_manager[i].ts);
			SerialDEBUG.print(", " + String(i) + " ds= " + media_manager[i].ds);
			SerialDEBUG.print(", " + String(i) + " ms= " + media_manager[i].lmd_msg + "\n");	
		#endif 	
			empty_playlist=0;
		}
	}
	if(empty_playlist)
	{
		#ifdef use_debug_serial		
		SerialDEBUG.println("STOP cause empty playlist");
		#endif 
		//gửi qua bảng led					
		index_send++;								
		lmd_command=LMD_STOP;
		lmd_repeat=0;
		u32_lmd_resend=0;//gửi liền không cần chờ		
		// send_command_screen();
	}
}

void shift_playlist()
{
	#ifdef use_debug_serial
		SerialDEBUG.print("shift_playlist\n");
		#endif 
	for (int i=0;i<max_file_process-1;i++)
	{					
		media_manager[i].mid=media_manager[i+1].mid;
		media_manager[i].ts=media_manager[i+1].ts;
		media_manager[i].ds=media_manager[i+1].ds;
		media_manager[i].mode=media_manager[i+1].mode;
		media_manager[i].playing=media_manager[i+1].playing;
		media_manager[i].looop=media_manager[i+1].looop;
		media_manager[i].index=media_manager[i+1].index;
		media_manager[i].lmd_msg=media_manager[i+1].lmd_msg;
		// SerialDEBUG.print(String(i) + "=" + String (i+1) + "\n");
	}
	//sau shift
	print_playlist();
}
int insert_playlist()
{
	int tim=100;//chứa vị trí trong danh sách
	// tìm ô còn trống
	// SerialDEBUG.print("insert_playlist\n");
		
	for (int i=0;i<max_file_process-1;i++)
	{							
		// SerialDEBUG.print("\t@" + String (i) + "\n");
		
		if (media_manager[i].mode!=2)
		{
			tim=i;
			// SerialDEBUG.print("\t find@" + String (i) + "\n");
			break;
		}
	}
	if(tim==100){
		// SerialDEBUG.print("Khong con o nao trong de them vao: bo qua\n");
	}
	else
	{
		//tien hanh chen vao
		// SerialDEBUG.print("Tien hanh chen vao @" + String (tim) + "\n");
		if(tim==0)
		{	
			//nếu chèn vào ô đầu tiên
			// chuyển trạng thái ô đầu tiên thành ready nếu đang phát
			if(media_manager[0].playing==PLAYER_PLAYING)media_manager[0].playing=PLAYER_READY;
			
		}
		
		
		//dịch danh sách sang phải
		for (int i=max_file_process-2;i>=tim;i--)
		{					
			media_manager[i+1].mid=media_manager[i].mid;
			media_manager[i+1].ts=media_manager[i].ts;
			media_manager[i+1].ds=media_manager[i].ds;
			media_manager[i+1].mode=media_manager[i].mode;
			media_manager[i+1].playing=media_manager[i].playing;
			media_manager[i+1].looop=media_manager[i].looop;
			media_manager[i+1].index=media_manager[i].index;
			media_manager[i+1].lmd_msg=media_manager[i].lmd_msg;
			// SerialDEBUG.print(String(i+1) + "=" + String (i) + "\n");
		}
		//chèn thằng chen ngang vào
		media_manager[tim].mid=oph_media_buffer.mid;
		media_manager[tim].ts=0;//khi nào phát sẽ set lại thời gian bắt đầu
		media_manager[tim].ds=oph_media_buffer.duration;//tin =2: lấy duration phát
		media_manager[tim].mode=oph_media_buffer.mode;
		media_manager[tim].playing=PLAYER_READY;
		media_manager[tim].looop=oph_media_buffer.looop;
		media_manager[tim].index=0;
		media_manager[tim].lmd_msg=oph_media_buffer.lmd_msg;		
	}
	print_playlist();
	return tim;
}
void test_function()
{
	
	#ifdef use_debug_serial
		SerialDEBUG.println("********************************");
	SerialDEBUG.println("********************************");
	SerialDEBUG.println("TEST FUNCTION*******************");
	#endif 
	
	testmessage();
	#ifdef use_debug_serial
		SerialDEBUG.println("Get content 1");
		#endif 
	get_content();	
	#ifdef use_debug_serial
		SerialDEBUG.println("Get content 2");
		#endif 
	get_content();
	#ifdef use_debug_serial
		SerialDEBUG.println("insert");
		#endif 
	insert_playlist();
	#ifdef use_debug_serial
		SerialDEBUG.println("shift");
		#endif 
	shift_playlist();
	while(1)
	{
		delay(1);
	}
	
}
void testmessage() {
  
  String payload = "{\"id\":0,\"opcode\":67,\"data\":{\"summary\":null,\"created\":1638779409,\"size\":0,\"codec\":1,\"format\":1,\"cs\":0,\"url\":\"https://alta-s3.dev-altamedia.com/lalova/chaomung.txt\",\"auth\":\"\",\"loop\":0,\"mid\":21,\"prio\":4,\"mode\":2,\"expired\":1638925216,\"start\":1638833411,\"ts\":[0,1,3],\"repeat\":0,\"days\":0,\"duration\":18001,\"ds\":[],\"header\":255}}";
  String payload1 = "{\"id\":0,\"opcode\":67,\"data\":{\"summary\":null,\"created\":1638779409,\"size\":0,\"codec\":1,\"format\":1,\"cs\":0,\"url\":\"https://alta-s3.dev-altamedia.com/lalova/chaomung.txt\",\"auth\":\"\",\"loop\":0,\"mid\":22,\"prio\":4,\"mode\":2,\"expired\":1638925216,\"start\":1638833411,\"ts\":[0,1,3],\"repeat\":0,\"days\":0,\"duration\":18001,\"ds\":[],\"header\":255}}";
  String payload2 = "{\"id\":0,\"opcode\":67,\"data\":{\"summary\":null,\"created\":1638779409,\"size\":0,\"codec\":1,\"format\":1,\"cs\":0,\"url\":\"https://alta-s3.dev-altamedia.com/lalova/chaomung.txt\",\"auth\":\"\",\"loop\":0,\"mid\":23,\"prio\":4,\"mode\":2,\"expired\":1638925216,\"start\":1638833411,\"ts\":[0,1,3],\"repeat\":0,\"days\":0,\"duration\":18001,\"ds\":[],\"header\":255}}";
  String payload3 = "{\"id\":0,\"opcode\":67,\"data\":{\"summary\":null,\"created\":1638779409,\"size\":0,\"codec\":1,\"format\":1,\"cs\":0,\"url\":\"https://alta-s3.dev-altamedia.com/lalova/chaomung.txt\",\"auth\":\"\",\"loop\":0,\"mid\":24,\"prio\":4,\"mode\":2,\"expired\":1638925216,\"start\":1638833411,\"ts\":[0,1,3],\"repeat\":0,\"days\":0,\"duration\":18001,\"ds\":[],\"header\":255}}";
  
  
  // "data":{"ts":[49560],"ds":[300],"mid":12345678,"created":1638779409,"size":0,"codec":1,"format":1,"cs":0,"url":"https://alta-s3.dev-altamedia.com/lalova/chaomung.txt","auth":"","loop":0,"prio":0,"mode":1,"expired":1670365040,"start":1638829040,"repeat":0,"days":255,"duration":300,"header":255}
  
  String channel_ ="test";
  
  #ifdef use_debug_serial
		SerialDEBUG.println("Enter test\n");
		#endif 
	
	messageReceived (channel_,payload);
	messageReceived (channel_,payload1);
	messageReceived (channel_,payload2);
	messageReceived (channel_,payload3);	
	#ifdef use_debug_serial
		SerialDEBUG.println("Exit test\n");
		#endif 
}
void testmessage1() {
  
  String payload = "{\"id\":0,\"opcode\":67,\"data\":{\"summary\":null,\"created\":1638779409,\"size\":0,\"codec\":1,\"format\":1,\"cs\":0,\"url\":\"https://alta-s3.dev-altamedia.com/lalova/chaomung.txt\",\"auth\":\"\",\"loop\":0,\"mid\":25,\"prio\":1,\"mode\":1,\"expired\":1639501238,\"start\":1638833411,\"ts\":[49200],\"repeat\":0,\"days\":255,\"duration\":60,\"ds\":[60],\"header\":255}}";
  String payload1 = "{\"id\":0,\"opcode\":67,\"data\":{\"summary\":null,\"created\":1638779409,\"size\":0,\"codec\":1,\"format\":1,\"cs\":0,\"url\":\"https://alta-s3.dev-altamedia.com/lalova/chaomung.txt\",\"auth\":\"\",\"loop\":0,\"mid\":26,\"prio\":1,\"mode\":1,\"expired\":1639501238,\"start\":1638833411,\"ts\":[49260],\"repeat\":0,\"days\":255,\"duration\":60,\"ds\":[60],\"header\":255}}";
  String payload2 = "{\"id\":0,\"opcode\":67,\"data\":{\"summary\":null,\"created\":1638779409,\"size\":0,\"codec\":1,\"format\":1,\"cs\":0,\"url\":\"https://alta-s3.dev-altamedia.com/lalova/chaomung.txt\",\"auth\":\"\",\"loop\":0,\"mid\":27,\"prio\":2,\"mode\":1,\"expired\":1639501238,\"start\":1638833411,\"ts\":[49320],\"repeat\":0,\"days\":255,\"duration\":60,\"ds\":[60],\"header\":255}}";
  
  
  
  // "data":{"ts":[49560],"ds":[300],"mid":12345678,"created":1638779409,"size":0,"codec":1,"format":1,"cs":0,"url":"https://alta-s3.dev-altamedia.com/lalova/chaomung.txt","auth":"","loop":0,"prio":0,"mode":1,"expired":1670365040,"start":1638829040,"repeat":0,"days":255,"duration":300,"header":255}
  
  String channel_ ="test";
  

  #ifdef use_debug_serial
		SerialDEBUG.println("Enter test\n");
		#endif 
	
	messageReceived (channel_,payload);
	messageReceived (channel_,payload1);
	messageReceived (channel_,payload2);	
	#ifdef use_debug_serial
		SerialDEBUG.println("Exit test\n");
		#endif 
}
void testchen() {
  
  String payload = "{\"id\":0,\"opcode\":67,\"data\":{\"summary\":null,\"created\":1638779409,\"size\":0,\"codec\":1,\"format\":1,\"cs\":0,\"url\":\"https://alta-s3.dev-altamedia.com/lalova/chaomung.txt\",\"auth\":\"\",\"loop\":0,\"mid\":1,\"prio\":1,\"mode\":2,\"expired\":1639501238,\"start\":1638833411,\"ts\":[49200],\"repeat\":0,\"days\":255,\"duration\":60,\"ds\":[60],\"header\":255}}";
  
  // "data":{"ts":[49560],"ds":[300],"mid":12345678,"created":1638779409,"size":0,"codec":1,"format":1,"cs":0,"url":"https://alta-s3.dev-altamedia.com/lalova/chaomung.txt","auth":"","loop":0,"prio":0,"mode":1,"expired":1670365040,"start":1638829040,"repeat":0,"days":255,"duration":300,"header":255}
  
  String channel_ ="test";
  

  #ifdef use_debug_serial
		SerialDEBUG.println("Enter test chen 1\n");
		#endif 
	
	messageReceived (channel_,payload);
	#ifdef use_debug_serial
		SerialDEBUG.println("Exit test\n");
		#endif 
}
void testchen2() {
  
  String payload = "{\"id\":0,\"opcode\":67,\"data\":{\"summary\":null,\"created\":1638779409,\"size\":0,\"codec\":1,\"format\":1,\"cs\":0,\"url\":\"https://alta-s3.dev-altamedia.com/lalova/chaomung.txt\",\"auth\":\"\",\"loop\":0,\"mid\":2,\"prio\":1,\"mode\":2,\"expired\":1639501238,\"start\":1638833411,\"ts\":[49200],\"repeat\":0,\"days\":255,\"duration\":60,\"ds\":[60],\"header\":255}}";
  
  // "data":{"ts":[49560],"ds":[300],"mid":12345678,"created":1638779409,"size":0,"codec":1,"format":1,"cs":0,"url":"https://alta-s3.dev-altamedia.com/lalova/chaomung.txt","auth":"","loop":0,"prio":0,"mode":1,"expired":1670365040,"start":1638829040,"repeat":0,"days":255,"duration":300,"header":255}
  
  String channel_ ="test";
  

  #ifdef use_debug_serial
		SerialDEBUG.println("Enter test chen 2\n");
		#endif 
	
	messageReceived (channel_,payload);
	#ifdef use_debug_serial
		SerialDEBUG.println("Exit test\n");
		#endif 
}
void print_oph_media_buffer()
{
	#ifdef use_debug_serial		
	int t=0;

		SerialDEBUG.println("***print_oph_media_buffer***");
	SerialDEBUG.println("mid =" + String(oph_media_buffer.mid));
	SerialDEBUG.println("prio =" + String(oph_media_buffer.prio));
	SerialDEBUG.println("created =" + String(oph_media_buffer.created));
	SerialDEBUG.println("expired =" + String(oph_media_buffer.expired));
	SerialDEBUG.println("mode =" + String(oph_media_buffer.mode));
	SerialDEBUG.println("start =" + String(oph_media_buffer.start));
	SerialDEBUG.println("repeat =" + String(oph_media_buffer.repeat));
	SerialDEBUG.println("days =" + String(oph_media_buffer.days));
	SerialDEBUG.println("url =" + String(oph_media_buffer.url));
	SerialDEBUG.println("auth =" + String(oph_media_buffer.auth));
	SerialDEBUG.println("loop =" + String(oph_media_buffer.looop));
	SerialDEBUG.println("duration =" + String(oph_media_buffer.duration));	
	SerialDEBUG.println("lmd =" + String(oph_media_buffer.lmd_msg));

	for(int i=0;i<24;i++)
	{
		if(oph_media_buffer.ts[i])
		{		
			SerialDEBUG.println("ts[" + String(i) + "]=" + String(oph_media_buffer.ts[i]));
		}
	}
	
	for(int i=0;i<24;i++)
	{
		if(oph_media_buffer.ds[i])
		{
			SerialDEBUG.println("ds[" + String(i) + "]=" + String(oph_media_buffer.ds[i]));		
		}
	}
	
	SerialDEBUG.println("***end print_oph_media_buffer***");
	#endif 
}
//----------------------wifi----------------------
int value = 0;
int readdata=0;
static byte c1;  // Character count to remove

byte utf8ascii(byte ascii) {
    if ( ascii<128 )   // Standard ASCII-set 0..0x7F handling  
    {   c1=0;
        return( ascii );
    }

    // get previous input
    byte last = c1;   // get last char
    c1=ascii;         // remember actual character

    switch (last)     // conversion depending on first UTF8-character
    {   case 0xC2: return  (ascii);  break;
        case 0xC3: return  (ascii | 0xC0);  break;
        case 0x82: if(ascii==0xAC) return(0x80);       // special case Euro-symbol
    }

    return  (0);                                     // otherwise: return zero, if character has to be ignored
}

String utf8toascii(String s)
{      
        String r="";
        char c;
        // SerialDEBUG.println( "Start convert: ");
		for (int i=0; i<s.length(); i++)
        {
				// SerialDEBUG.print( s.charAt(i), HEX);
				// SerialDEBUG.print( "-->");
				c = utf8ascii(s.charAt(i));
				// SerialDEBUG.println( c, HEX);				
				
				if (c!=0) r+=c;
        }
		// SerialDEBUG.println( "End convert: ");
        return r;
}

int getvia4G()
{
	#ifdef use_debug_serial
		SerialDEBUG.println("************getvia4G******************");
		#endif 

	// String payload;
	int kq=0;
	int httpCode;

	#ifdef use_debug_serial
		SerialDEBUG.print("url=");
	SerialDEBUG.println(oph_media_buffer.url);
	#endif 
	oph_media_buffer.lmd_msg=test4(oph_media_buffer.url,&kq);
	
	if (kq==200) 
    { 
		kq=1;
		oph_media_buffer.lmd_msg.replace("\n", "  ");
		oph_media_buffer.lmd_msg.replace("\r", "#");
		#ifdef use_debug_serial
		SerialDEBUG.println(oph_media_buffer.lmd_msg);
		#endif 
    }
	else
	{
		kq=0;
		#ifdef use_debug_serial
		SerialDEBUG.print("error in downloading file in 4G mode");
		#endif 
	}

  return kq;
	
}


void test_getgetdatavia4G()
{
	
	return;
	if(landautien==1)return;
	landautien=1;
	
	String url = "https://raw.githubusercontent.com/DaikCong/Hello-World/master/welcomeDongNai.txt";
	
	int kq;
	String atad=test4(url, &kq);
	#ifdef use_debug_serial
		SerialDEBUG.print("chuoi nhan=");
	SerialDEBUG.println(atad);
	SerialDEBUG.println("Pause");
	#endif 
	
	/*
// WiFiClientSecure * client = new WiFiClientSecure;	
	
TinyGsmClient * clienttest(modem)= new TinyGsmClient;
HttpClient          http(clienttest, servertest, port);

	SerialDEBUG.print(F("Performing HTTPS GET request... "));
    http.connectionKeepAlive();  // Currently, this is needed for HTTPS
  int err = http.get(resource);
  if (err != 0) {
    SerialDEBUG.println(F("failed to connect"));
    delay(10000);
    return;
  }

  int status = http.responseStatusCode();
  SerialDEBUG.print(F("Response status code: "));
  SerialDEBUG.println(status);
  if (!status) {
    delay(10000);
    return;
  }

  SerialDEBUG.println(F("Response Headers:"));
  while (http.headerAvailable()) {
    String headerName = http.readHeaderName();
    String headerValue = http.readHeaderValue();
    SerialDEBUG.println("    " + headerName + " : " + headerValue);
  }

  int length = http.contentLength();
  if (length >= 0) {
    SerialDEBUG.print(F("Content length is: "));
    SerialDEBUG.println(length);
  }
  if (http.isResponseChunked()) {
    SerialDEBUG.println(F("The response is chunked"));
  }

  String body = http.responseBody();
  SerialDEBUG.println(F("Response:"));
  SerialDEBUG.println(body);

  SerialDEBUG.print(F("Body length is: "));
  SerialDEBUG.println(body.length());

  // Shutdown

  http.stop();
  SerialDEBUG.println(F("Server disconnected"));

#if TINY_GSM_USE_WIFI
    modem.networkDisconnect();
    SerialDEBUG.println(F("WiFi disconnected"));
#endif
#if TINY_GSM_USE_GPRS
    modem.gprsDisconnect();
    SerialDEBUG.println(F("GPRS disconnected"));
#endif
	
	*/
}


int getotherway(void) {
  #ifdef use_debug_serial
		SerialDEBUG.println("************getotherway******************");
  #endif 
  // String payload;
  int kq=0;
  int httpCode;

#ifdef use_debug_serial
		SerialDEBUG.print("url=");
SerialDEBUG.println(oph_media_buffer.url);
#endif 

    // client -> setCACert(rootCACertificate);

    // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
    HTTPClient https;

    if (https.begin(oph_media_buffer.url)) 
    { // HTTPS      
      #ifdef use_debug_serial
		SerialDEBUG.print("[HTTPS] GET...\n");
		#endif 
      // start connection and send HTTP header
      delay(100);
      httpCode = https.GET();
      delay(100);
      if (httpCode == HTTP_CODE_OK) // if version received
      {
        // payload = https.getString(); // save received version
		oph_media_buffer.lmd_msg=https.getString(); // save received version
      } 
	  else {
        #ifdef use_debug_serial
		SerialDEBUG.print("error in downloading version file:");        
		#endif 
      }
      https.end();
    }

  
  if (httpCode == HTTP_CODE_OK) // if version received
  {
    // payload.trim();
	#ifdef use_debug_serial
		SerialDEBUG.println(oph_media_buffer.lmd_msg);
		#endif 
	kq=1;

  }   
  return kq;  
}

int getdata()
{
	//http://mbs.mobifone.vn/media
	// http://mbs.mobifone.com/media
	#ifdef use_debug_serial
		SerialDEBUG.println("\rGetdata:");    
		#endif 
	//get buffer from flash into  oph_media_buffer
	//search buffer name

	if(downdloadingthisfile == dfnofile)
	{	
		#ifdef use_debug_serial
		SerialDEBUG.println("Searching file");
		#endif 
		for(int i =0;i< store_index_max;i++)
		{
			if(store_name[i]!=dfnofile)
			{			
				downdloadingthisfile=store_name[i];
				
				#ifdef use_debug_serial
		SerialDEBUG.println("Downd file: " + String (downdloadingthisfile) + " @ " + String(i));
		#endif 
				
				store_name[i]=dfnofile;
				index_storing--;
				break;		
			}
		}
		#ifdef use_debug_serial
		SerialDEBUG.println("End Searching file");
		#endif 
	}
	else
	{
		#ifdef use_debug_serial
		SerialDEBUG.printf("Re downdloading file\n");
		#endif 
	}
	
	if(downdloadingthisfile == dfnofile)
	{
		index_storing=0;
		havedata=0;
		#ifdef use_debug_serial
		SerialDEBUG.printf("Empty file to get\n");
		#endif 
		return 2;		
	}

	char name[30];
	sprintf(name,"/F%d.lmd",downdloadingthisfile);		
	load_oph_media_http(name);
	
	String in= String(oph_media_buffer.url);
	#ifdef use_debug_serial
		SerialDEBUG.print("url= ");    			
	SerialDEBUG.println(in);    			
	#endif 
	int aa;
	if(mqtt.mode_data_is_wifi)
	{	
		aa=getotherway();
	}
	else
	{
		aa=getvia4G();
	}
	// SerialDEBUG.printf("The dotcom = %d\n",dotcom);
	// https://alta-s3.dev-altamedia.com/lalova/chaomung.txt
	//https://mbs.mobifone.vn/media/vms-mtc/SMediaContentS/testMBF.txt?AWSAccessKeyId=POD_IDENTITY_S3112233&Expires=1640837265&Signature=DY2rWvzAiCiniZuvri2UbJpmvUQ%3D
	// https://mbs.mobifone.vn/media/vms-mtc/SMediaContentS/new23.txt?AWSAccessKeyId=POD_IDENTITY_S3112233&Expires=1640847228&Signature=DqRpeETNibX%2Fv8MzywOqjN1TiTQ%3D

	
	
	if (!aa) {//host
        #ifdef use_debug_serial
		SerialDEBUG.println("\tconnection failed");
		#endif 
        counterget++;
		
		if(counterget>3)
		{
			downdloadingthisfile = dfnofile;
			counterget=0;
			havedata=0;
			#ifdef use_debug_serial
		SerialDEBUG.println("\tCancel file");
		#endif 
			read_playlist=1;
		}
		else havedata=1;

		return 1;
    }
	counterget=0;
	downdloadingthisfile = dfnofile;

    //nội dung đang chứa trong đây: oph_media_buffer.lmd_msg
	#ifdef use_debug_serial
		SerialDEBUG.println("\tFinish ok");
		#endif 
	return 0;
}



int getdata1()
{
	//http://mbs.mobifone.vn/media
	// http://mbs.mobifone.com/media
	#ifdef use_debug_serial
		SerialDEBUG.println("\rGetdata:");    
		#endif 
	//get buffer from flash into  oph_media_buffer
	//search buffer name

	if(downdloadingthisfile == dfnofile)
	{	
		#ifdef use_debug_serial
		SerialDEBUG.println("Searching file");
		#endif 
		for(int i =0;i< store_index_max;i++)
		{
			if(store_name[i]!=dfnofile)
			{			
				downdloadingthisfile=store_name[i];
				
				#ifdef use_debug_serial
		SerialDEBUG.println("Downd file: " + String (downdloadingthisfile) + " @ " + String(i));
		#endif 		
				store_name[i]=dfnofile;
				index_storing--;
				break;		
			}
		}
		#ifdef use_debug_serial
		SerialDEBUG.println("End Searching file");
		#endif 
	}
	else
	{
		#ifdef use_debug_serial
		SerialDEBUG.printf("Re downdloading file\n");
		#endif 
	}
	
	if(downdloadingthisfile == dfnofile)
	{
		index_storing=0;
		havedata=0;
		#ifdef use_debug_serial
		SerialDEBUG.printf("Empty file to get Stop\n");
		#endif 
		return 2;		
	}

	char name[30];
	sprintf(name,"/F%d.lmd",downdloadingthisfile);		
	load_oph_media_http(name);
	
	String in= String(oph_media_buffer.url);
	#ifdef use_debug_serial
		SerialDEBUG.print("url= ");    			
	SerialDEBUG.println(in);    			
	#endif 
	getotherway();
	
	
	//filter host
	int dotcom = String(oph_media_buffer.url).indexOf(".com");
	// SerialDEBUG.printf("The dotcom = %d\n",dotcom);
	// https://alta-s3.dev-altamedia.com/lalova/chaomung.txt
	//https://mbs.mobifone.vn/media/vms-mtc/SMediaContentS/testMBF.txt?AWSAccessKeyId=POD_IDENTITY_S3112233&Expires=1640837265&Signature=DY2rWvzAiCiniZuvri2UbJpmvUQ%3D
	// https://mbs.mobifone.vn/media/vms-mtc/SMediaContentS/new23.txt?AWSAccessKeyId=POD_IDENTITY_S3112233&Expires=1640847228&Signature=DqRpeETNibX%2Fv8MzywOqjN1TiTQ%3D

	int dotvn;
	String s;
	if(dotcom==-1)
	{
		dotvn = String(oph_media_buffer.url).indexOf(".vn");
		// SerialDEBUG.printf("The dotvn = %d\n",dotvn);
		
		s= String(oph_media_buffer.url).substring(0, dotvn+3);
	}
	else
	{	
		// SerialDEBUG.print("dotcom!=0"); 
		s= String(oph_media_buffer.url).substring(0, dotcom+4);
	}		
    
	// s= String(oph_media_buffer.url);//debug
	
	// SerialDEBUG.printf("The dotcom = %d\n",dotcom);
	// SerialDEBUG.print("substring: ");    			
	// SerialDEBUG.println(s);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
	
	int l=s.length();
	char buf[l+1];
	s.toCharArray(buf,l+1);
	
    const int httpPort = 80;
    #ifdef use_debug_serial
		SerialDEBUG.print("\tconnecting to ");    			
	SerialDEBUG.println(buf);
	#endif 
	//---------------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------------
	
	if (!client.connect(buf, httpPort)) {//host
        #ifdef use_debug_serial
		SerialDEBUG.println("\tconnection failed");
		#endif 
        counterget++;
		
		if(counterget>3)
		{
			downdloadingthisfile = dfnofile;
			counterget=0;
			havedata=0;
			#ifdef use_debug_serial
		SerialDEBUG.println("\tCancel file");
		#endif 
			read_playlist=1;
		}
		else havedata=1;

		return 1;
    }
	counterget=0;
	downdloadingthisfile = dfnofile;

    // We now create a URI for the request        
    // String url = "https://alta-s3.dev-altamedia.com/lalova/chaomung.txt";
    
    #ifdef use_debug_serial
		SerialDEBUG.print("\tRequesting URL: ");
    SerialDEBUG.println(oph_media_buffer.url);
#endif  
    // This will send the request to the server

	client.print("GET ");
	client.print(oph_media_buffer.url);
	// client.print(" HTTP/1.1");
	client.print("\r\n");
	
	// client.print("GET https://alta-s3.dev-altamedia.com/lalova/chaomung.txt\r\n");
    
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            #ifdef use_debug_serial
		SerialDEBUG.println("\t>>> Client Timeout !");
		#endif 
            client.stop();
            return 1;
        }
    }

    // Read all the lines of the reply from server and print them to SerialDEBUG
    while(client.available()) {
        String line = client.readStringUntil('\r');
        #ifdef use_debug_serial
		SerialDEBUG.print("\t");
		SerialDEBUG.print(line);
		#endif 
		
		if(line=="")line="None";
		// SerialDEBUG.println("\n\rExtended ASCII-Version:");
		// String temp=utf8toascii(line);
		oph_media_buffer.lmd_msg=	line;
    }

    #ifdef use_debug_serial
		SerialDEBUG.println();
    SerialDEBUG.println("\tclosing connection");
	#endif 
	return 0;
}

int read_button(button_info_t *button_) {
  int kq=0;
  // read the state of the switch into a local variable:
  int reading = digitalRead(button_->pin_sw);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != button_->lastButtonState) {
    // reset the debouncing timer
    button_->lastDebounceTime = millis();
  }

  if ((millis() - button_->lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != button_->buttonStateEncoder) {
      button_->buttonStateEncoder = reading;

      // only toggle the LED if the new button state is HIGH
      if (button_->buttonStateEncoder == HIGH) {
        kq=1;
		//flash led
		
      }
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  button_->lastButtonState = reading;
  return kq;
}
int read_state_button(button_info_t *button_) {
  int kq=0;
  // read the state of the switch into a local variable:
  int reading = digitalRead(button_->pin_sw);

  return reading;  
}

int read_hold_button(button_info_t *button_) {
  int kq=0;
  // read the state of the switch into a local variable:
  int reading = digitalRead(button_->pin_sw);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != button_->lastButtonState) {
    // reset the debouncing timer
    button_->lastDebounceTime = millis();
	flash_led=0;
  }


  if ((millis() - button_->lastDebounceTime) > 100) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // Vẫn đang nhấn nút
    if (reading == LOW) {
       button_->keypressed=1;
    }
	else
	{
		button_->keypressed=0;
	}	
  }

  
  if ((millis() - button_->lastDebounceTime) > 5000) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // Vẫn đang nhấn nút
    if (reading == LOW) {
      flash_led=1;
    }		
  }
  if ((millis() - button_->lastDebounceTime) > 10000) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // Vẫn đang nhấn nút
    if (reading == LOW) {
      kq=1;
    }		
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  button_->lastButtonState = reading;
  return kq;
}
int check_key_press()
{
	
#if defined (hwv0)
	if(button0.keypressed)return 1;
#elif defined (hwv1)
	if(button0.keypressed)return 1;
	if(button1.keypressed)return 1;
	if(button2.keypressed)return 1;
	if(button3.keypressed)return 1;
#elif defined (hwv2)
	if(button3.keypressed)return 1;
#else
	#error "Please define one of use_ in config_hw_local.h"
#endif
	return 0;	
}

void config_setting_mode()
{
	//enter configmode
	//truyền qua bảng led
	flash_led=0;
	// system_status=SYS_WAIT_RESET;
	#ifdef use_debug_serial
	SerialDEBUG.println("Enter config wifi");
	#endif 
	send_messageinfo("Enter config wifi");
	
	//xử lý luôn, khỏi khởi động lại chip
	setupwifi(2);
	//kiểm tra xem có cần khởi động lại chip không
	if(shouldSaveConfig)
	{		
	//kiểm tra xem có cần khởi động lại chip không		
	// system_status=SYS_WAIT_RESET;
		#ifdef use_debug_serial
	SerialDEBUG.println("Reset chip");
	#endif 
		// EEPROM.write(addr_configwifi, 20);
		// EEPROM.commit();
		delay(2000);
		ESP.restart();			
		while(1)
		{
			delay(2000);
		}
	}				
}
void check_button()
{
	int tam;
	uint8_t presssw=0;
	
#if defined (hwv0)
	tam=read_hold_button(&button0);
	if ( tam) {
		presssw=1;
		#ifdef use_debug_serial
		SerialDEBUG.println("Button 0 has pressed");	
		#endif 
	}
#elif defined (hwv1)
	tam=read_hold_button(&button0);
	if ( tam) {
		presssw=1;
		#ifdef use_debug_serial
		SerialDEBUG.println("Button 0 has pressed");	
		#endif 
	}
	
	tam=read_hold_button(&button1);
	if ( tam) {
		presssw=1;
		#ifdef use_debug_serial
		SerialDEBUG.println("Button 1 has pressed");	
		#endif 
	}
	tam=read_hold_button(&button2);
	if ( tam) {
		presssw=1;
		#ifdef use_debug_serial
		SerialDEBUG.println("Button 2 has pressed");	
		#endif 
	}
	tam=read_hold_button(&button3);
	if ( tam) {
		presssw=1;
		#ifdef use_debug_serial
		SerialDEBUG.println("Button 3 has pressed");	
		#endif 
	}
#elif defined (hwv2)
	tam=read_hold_button(&button3);
	if ( tam) {
		presssw=1;
		#ifdef use_debug_serial
		SerialDEBUG.println("Button 3 has pressed");	
		#endif 
	}
#else
	#error "Please define one of use_ in config_hw_local.h"
#endif	
	
	//xử lý nút nhấn
	if(presssw)
	{
		
		config_setting_mode();
				
	}
	
	//check timeout
	
	if (millis() > sw_timeout) {
		count_press=0;
	}
}

void read_serial_LMD()
{
    while (SerialLMD.available())
    {
      	char inChar = (char)SerialLMD.read();
		
		if (inChar == '\n' || inChar == '\r') {
			//xử lý ở đây
			process_data();	
			
			while ( SerialLMD.available() > 0) SerialLMD.read();			
			
			break;		
		}
		else
		{
			inputString += inChar;
		}
    }		
}
void check_connection()
{
	if(millis()>ui32timeoutrx)
	{
		#ifdef use_debug_serial
		SerialDEBUG.println("No reply from communication");	 
		#endif 
		// ui8_communication_status=SYS_DISCONNECT;
		
	 	//Xử lý timeout ở đây
		config_setting_mode();
		//reset timer 
		ui32timeoutrx=millis() + df_time_disconnect;		
	}
}

void read_serial()
{
	#ifdef use_debug_serial
		
	while (SerialDEBUG.available() > 0) {
		int readdata=SerialDEBUG.read();

		if(readdata=='r')
		{
			readdata=0;
			//test reset
			SerialDEBUG.println("Enter reset chip");
			ESP.restart();			
			while(1){
				delay(1);
			}			
		}
		else if(readdata=='o')//test truyen OTA
		{	
			readdata=0;
			SerialDEBUG.println("Test send upgrade firmware from server");
			index_lmd=102;
			update2lmd();			
		}
		else if(readdata=='p')//test truyen OTA
		{	
			readdata=0;
			SerialDEBUG.println("Test send OTA manual to Led");
			index_lmd=103;
			update2lmd();			
		}
		else if(readdata=='d')
		{	
			readdata=0;
			SerialDEBUG.println("List file");
			listDir(SPIFFS, "/", 0);
		}	
		else if(readdata=='u')
		{	
			readdata=0;
			SerialDEBUG.println("Test check upgrade firmware");
			firmwareUpdate();		
		}
		else if(readdata=='s')
		{
			readdata=0;
			SerialDEBUG.println("read ");
			char name[30];
			sprintf(name,"/F%d.lmd",opu_generic_system.mid);		
			load_oph_media_http(name);
		}
		else if(readdata=='c')
		{
			readdata=0;
			SerialDEBUG.println("c: config wifi");
			send_messageinfo("config wifi");
			EEPROM.write(addr_configwifi, 20);
			EEPROM.commit();
			delay(2000);
			ESP.restart();
			delay(2000);
		}
		else if(readdata=='q')
		{		
			//setup 
			EEPROM.write(addr_configwifi, 30);
			EEPROM.commit();
			
			SerialDEBUG.println("Enter update firmware server manual");
			send_messageinfo("Enter update firmware");
			//chờ reset liền
			delay(2000);
			ESP.restart();
			delay(2000);				
		}
		else if(readdata=='t')
		{						
			readdata=0;
			SerialDEBUG.println("getmediafilename");
			get_content();
		}
		else if(readdata=='1')
		{						
			readdata=0;
			SerialDEBUG.println("testmessage1");
			testmessage1();
		}
		else if(readdata=='2')
		{						
			readdata=0;
			SerialDEBUG.println("testmessage1");
			testchen();
		}
		else if(readdata=='3')
		{						
			readdata=0;
			SerialDEBUG.println("get_setting_parameters");
			get_setting_parameters();
		}
		else if(readdata=='k')
		{						
			readdata=0;
			SerialDEBUG.println("deleteallmdeia");
			deleteallmdeia(SPIFFS, "/", 0);	
		}
		else if(readdata=='m')
		{						
			readdata=0;
			SerialDEBUG.println("deleteallfile");
			deleteallfile(SPIFFS, "/", 0);	
		}
		else if(readdata=='?')
		{						
			readdata=0;
			SerialDEBUG.println("Test command");
			SerialDEBUG.println("k: delete all led file");
			SerialDEBUG.println("m: delete all file");
			SerialDEBUG.println("l: testmessage1");
			SerialDEBUG.println("t: get_content");
			SerialDEBUG.println("s: generic_system ");
			SerialDEBUG.println("d: listDir ");
			SerialDEBUG.println("r: reset ");			
			SerialDEBUG.println("c: config wifi ");
			SerialDEBUG.println("o: send Upgrade firmware via server to led ");
			SerialDEBUG.println("p: send OTA manual to led");
			SerialDEBUG.println("q: send OTA manual to server");
		}
	}
	#endif 
}

void get_content()
{
	uint32_t offset_ds=0;
	//tìm ô trống để insert file

	int tim =100;
	for (int i=0;i<max_file_process-1;i++)
	{							
		if (media_manager[i].playing==PLAYER_STOP)
		{
			tim=i;
			break;
		}
		offset_ds=offset_ds+media_manager[i].ds;
		// SerialDEBUG.println("Offset ad: " + String(media_manager[i].ds) + " >> " + String(offset_ds));
	}
	
	if(tim==100){
		#ifdef use_debug_serial
		SerialDEBUG.print("get_content: Full list: ignored\n");
		#endif 
	}
	else
	{
		//tien hanh chen vao
		// SerialDEBUG.println("Nhap noi dung vao vung: " + String(tim));
		//thêm offset duration
		size_t tam=0;
		size_t controng=0;
		
		if(tim)
		{
			getmediaplay(SPIFFS, "/", 0, &media_manager[tim],offset_ds,&tam, media_manager[tim-1].ts);
		}
		else 
		{
			getmediaplay(SPIFFS, "/", 0, &media_manager[tim],offset_ds,&tam,0);
		}
		
		tam=tam/1024;		
		#ifdef use_debug_serial
		SerialDEBUG.println("Use size " + String(tam));
		#endif 
		controng=1536-tam;
		#ifdef use_debug_serial
		SerialDEBUG.println("Free size " + String(controng));
		#endif 
		
		opu_status_system.sdfree=int(controng);	
		
		// rearangeplaylist();
		
		print_playlist();
	}
		
}
void reload_playlist()
{
	#ifdef use_debug_serial
		SerialDEBUG.println("reload_playlist");
		#endif 
	for (int i=0;i<max_file_process-1;i++)
	{							
		if (media_manager[i].mode!=2 && media_manager[i].playing !=  PLAYER_PLAYING)
		{
			media_manager[i].playing=PLAYER_STOP;			
		}
	}
}
/*
void rearangeplaylist()
{	
	SerialDEBUG.print(" rearangeplaylist \n");
	str_media_manager tstam;
	int i,j,vitrimoi;
	uint32_t cur=millis();
	for (i=0;i<max_file_process-1;i++)
	{					
		if(media_manager[i].mode!=2)
		{
			//tim  o  co ts nho nhat chen vao vi tri nay
			tstam.ts=media_manager[i].ts;
			vitrimoi=i;
			for(j=i;j<max_file_process-1;j++)
			{
				if(tstam.ts < media_manager[i].ts)
				{
					tstam.ts = media_manager[i].ts;
					vitrimoi=j;
				}
			}
			
			if(vitrimoi!=i)
			{
				SerialDEBUG.print(" Hoan vi" + String(i) + " va " + String (vitrimoi) + "\n");
				//hoán vị i và vitrimoi
				tstam.mid=media_manager[i].mid;
				media_manager[i].mid=media_manager[vitrimoi].mid;
				media_manager[vitrimoi].mid=tstam.mid;
				
				tstam.ts=media_manager[i].ts;
				media_manager[i].ts=media_manager[vitrimoi].ts;
				media_manager[vitrimoi].ts=tstam.ts;
				
				tstam.ds=media_manager[i].ds;
				media_manager[i].ds=media_manager[vitrimoi].ds;
				media_manager[vitrimoi].ds=tstam.ds;
				
				tstam.mode=media_manager[i].mode;
				media_manager[i].mode=media_manager[vitrimoi].mode;
				media_manager[vitrimoi].mode=tstam.mode;
				
				tstam.playing=media_manager[i].playing;
				media_manager[i].playing=media_manager[vitrimoi].playing;
				media_manager[vitrimoi].playing=tstam.playing;
				
				tstam.looop=media_manager[i].looop;
				media_manager[i].looop=media_manager[vitrimoi].looop;
				media_manager[vitrimoi].looop=tstam.looop;
				
				tstam.index=media_manager[i].index;
				media_manager[i].index=media_manager[vitrimoi].index;
				media_manager[vitrimoi].index=tstam.index;
				
				tstam.lmd_msg=media_manager[i].lmd_msg;
				media_manager[i].lmd_msg=media_manager[vitrimoi].lmd_msg;
				media_manager[vitrimoi].lmd_msg=tstam.lmd_msg;
				
			}
		}

	}

	uint32_t noww=millis()-cur;
			SerialDEBUG.print("time: ");
		SerialDEBUG.println(noww);
}
*/
void load_values()
{	  
	for(int i=0; i <store_index_max;i++)				
	{
		store_name[i]=dfnofile;														
	}
	for (int i=0;i<max_file_process-1;i++)
	{							
		media_manager[i].playing=PLAYER_STOP;

	}
}
void send2lmd(char *chuoi)
{
	int m;
	for(int i=0;i<5;i++){

		#ifdef use_debug_serial
		SerialDEBUG.print("VER ");
		SerialDEBUG.println(chuoi);
		#endif 
		delay(1);
		SerialLMD.print("VER ");
		SerialLMD.print(chuoi);
		SerialLMD.print("\n");
				
		delay_reset();
		while(delay_withoutdelay())
		{
			heart_beat();
		}
	}
	index_lmd=0;	
	
}
void update2lmd()
{
	if (index_lmd==0) return;
	int m;
	for(int i=0;i<5;i++){
		
		if(index_lmd==100)
		{
			#ifdef use_debug_serial
		SerialDEBUG.println("ENA 0");
		#endif 
			delay(1);
			SerialLMD.print("ENA 0\n");
		}
		else if(index_lmd==101)
		{
			#ifdef use_debug_serial
		SerialDEBUG.println("ENA 1");
		#endif 
			delay(1);
			SerialLMD.print("ENA 1\n");
		}
		else if(index_lmd==102)
		{
			// SerialDEBUG.println("wifi_name =" + String(mqtt.wifi_name));
			// SerialDEBUG.println("wifi_pass =" + String(mqtt.wifi_pass));			
			#ifdef use_debug_serial
		SerialDEBUG.println("UGN " + String(mqtt.wifi_name) +"," + String(mqtt.wifi_pass));
		#endif 
			delay(1);
			SerialLMD.print("UGN " + String(mqtt.wifi_name) +"," + String(mqtt.wifi_pass));
			SerialLMD.print("\n");
			// SerialLMD.print("UGN 1\n");
		}
		else if(index_lmd==103)
		{
			#ifdef use_debug_serial
				SerialDEBUG.println("OTA 1");
			#endif 
			delay(1);
			SerialLMD.print("OTA 1\n");
		}
		else if(index_lmd==104)
		{
			#ifdef use_debug_serial
		SerialDEBUG.print("LED ");
			SerialDEBUG.print(ui16ledcommand[0]);
			SerialDEBUG.print(" ");
			SerialDEBUG.print(ui16ledcommand[1]);
			SerialDEBUG.print(" ");
			SerialDEBUG.print(ui16ledcommand[2]);
			SerialDEBUG.print("\n");
		#endif 	
			delay(1);
			SerialLMD.print("LED ");
			SerialLMD.print(ui16ledcommand[0]);
			SerialLMD.print(" ");
			SerialLMD.print(ui16ledcommand[1]);
			SerialLMD.print(" ");
			SerialLMD.print(ui16ledcommand[2]);
			SerialLMD.print("\n");
		}

		delay_reset();
		while(delay_withoutdelay())
		{
			heart_beat();
		}
	}
	index_lmd=0;	
}

void send_messageinfo(char *msg)
{
	//repeat 5 lần
	int m;	
	index_send++;
	for(int i=0;i<5;i++){

		#ifdef use_debug_serial
		SerialDEBUG.print("DSP ");
		SerialDEBUG.print(index_send);
		SerialDEBUG.print(" ");
		SerialDEBUG.print(media_manager[0].mode);		
		SerialDEBUG.print(" ");
		SerialDEBUG.print(media_manager[0].looop);	
		SerialDEBUG.print(" ");
		SerialDEBUG.print(media_manager[0].ds);	
		SerialDEBUG.print(" ");
		SerialDEBUG.println(msg);		
		#endif 
		
		SerialLMD.print("DSP ");
		SerialLMD.print(index_send);
		SerialLMD.print(" ");
		SerialLMD.print(media_manager[0].mode);
		SerialLMD.print(" ");
		SerialLMD.print(media_manager[0].looop);	
		SerialLMD.print(" ");
		SerialLMD.print(media_manager[0].ds);	
		SerialLMD.print(" ");
		SerialLMD.print(msg);
		SerialLMD.print("\n");

		delay_reset();
		while(delay_withoutdelay())
		{
			heart_beat();
		}
		
		// digitalWrite(2,HIGH);
		// delay(50);
		// digitalWrite(2,LOW);
		previousMillis=millis();
	}
}
/**
 * @enum send_message2LMD
 * @brief
 * index_: vị trí chứa thời lượng của bản tin
 * được lưu trữ trong flash
 * cập nhật lên server 1phut/lần/ hoặc khi có thay đổi conn, spkerr
 */
void send_message2LMD(uint8_t index_)
{
	//repeat 5 lần
	int m;
	for(int i=0;i<5;i++){
		if (index_<24)
		{
			#ifdef use_debug_serial
		SerialDEBUG.print("DSP ");
			SerialDEBUG.print(index_send);
			SerialDEBUG.print(" ");
			// SerialDEBUG.print(media_manager[0].mode);
			SerialDEBUG.print(media_manager[0].mode);
			SerialDEBUG.print(" ");
			SerialDEBUG.print(media_manager[0].looop);	
			SerialDEBUG.print(" ");
			SerialDEBUG.print(media_manager[0].ds);	
			SerialDEBUG.print(" ");
			//chỗ này đang không có nội dung
			//trong mode phát theo lịch
			SerialDEBUG.println(media_manager[0].lmd_msg);
		#endif 	
			
			SerialLMD.print("DSP ");
			SerialLMD.print(index_send);
			SerialLMD.print(" ");
			SerialLMD.print(media_manager[0].mode);
			SerialLMD.print(" ");
			SerialLMD.print(media_manager[0].looop);	
			SerialLMD.print(" ");
			SerialLMD.print(media_manager[0].ds);	
			SerialLMD.print(" ");
			SerialLMD.print(media_manager[0].lmd_msg);
			SerialLMD.print("\n");
		}
		delay_reset();
		while(delay_withoutdelay())
		{
			heart_beat();
		}
		
		// digitalWrite(2,HIGH);
		// delay(50);
		// digitalWrite(2,LOW);
		previousMillis=millis();
	}
	//hủy lệnh stop nếu đang gửi
	lmd_command=LMD_PLAY;
	lmd_repeat=10;
}


/*
send_command_screen:

Truyền qua bảng led theo chu kỳ.
	5 giây đầu truyền trong 1s 1  lần
	sau đó 30s truyền 1  lần
*/
void send_command_screen()
{
	unsigned long currentMillis = millis();
	
	// if(lmd_repeat>4) return;
	
	if (currentMillis < u32_lmd_resend) return;

	lmd_repeat++;
	//5 giây đầu truyền trong 1s 1  lần
	//sau đó 30s truyền 1  lần
	if(lmd_repeat<4) u32_lmd_resend=currentMillis+1000;
	else u32_lmd_resend=currentMillis+30000;
		
	switch (lmd_command)
	{
		case LMD_STOP:
			// SerialDEBUG.println(F("TEST NEN CO DONG NAY, NHO BO BREAK DI "));
			// break;
			#ifdef use_debug_serial
			SerialDEBUG.print(F("STOP "));
			SerialDEBUG.print(system_status);
			SerialDEBUG.print(F("\n"));
			#endif 
			delay(1);
			SerialLMD.print(F("STOP "));
			SerialLMD.print(system_status);
			SerialLMD.print(F("\n"));

			break;
		case LMD_PLAY:
			
			#ifdef use_debug_serial
		SerialDEBUG.print("DSP ");
			SerialDEBUG.print(index_send);
			SerialDEBUG.print(" ");
			SerialDEBUG.print(media_manager[0].mode);
			SerialDEBUG.print(" ");
			SerialDEBUG.print(media_manager[0].looop);	
			SerialDEBUG.print(" ");
			SerialDEBUG.print(media_manager[0].ds);	
			SerialDEBUG.print(" ");
			SerialDEBUG.println(media_manager[0].lmd_msg);		
			#endif 
			
			SerialLMD.print("DSP ");
			SerialLMD.print(index_send);
			SerialLMD.print(" ");
			SerialLMD.print(media_manager[0].mode);
			SerialLMD.print(" ");
			SerialLMD.print(media_manager[0].looop);	
			SerialLMD.print(" ");
			SerialLMD.print(media_manager[0].ds);	
			SerialLMD.print(" ");
			SerialLMD.print(media_manager[0].lmd_msg);
			SerialLMD.print("\n");			
			break;
		default:
			SerialLMD.print("CHK\n");
	}			
}
/*
void check_getdata()//không dùng
{

	unsigned long currentMillis = millis();

	  if (currentMillis - timegetdata >= 20000) {
		// save the last time you blinked the LED		
		getdata();//không gọi
		timegetdata = currentMillis;		
	  }

}
*/

//------------------end wifi------------------------
void load_system_info()
{
	#ifdef use_debug_serial
		SerialDEBUG.println("load_system_info");
		#endif 
	spiff_readfilesystem(sys_filename, &mqtt);	
	// print_info_system_info();
}
void print_info_system_info()
{
	
	#ifdef use_debug_serial
		SerialDEBUG.println("***print_info_system_info***");
	SerialDEBUG.println("wifi_name =" + String(mqtt.wifi_name));
	SerialDEBUG.println("wifi_pass =" + String(mqtt.wifi_pass));
	SerialDEBUG.println("port =" + String(mqtt.port));
	SerialDEBUG.println("sever_mqtt =" + String(mqtt.sever_mqtt));
	SerialDEBUG.println("client_id =" + String(mqtt.client_id));
	SerialDEBUG.println("username =" + String(mqtt.username));
	SerialDEBUG.println("pwdMqtt =" + String(mqtt.pwdMqtt));
	SerialDEBUG.println("Mode data =" + String(mqtt.mode_data_is_wifi));
	
	
	SerialDEBUG.println("device_code =" + String(mqtt.device_code));
	#endif 
	
	char channel_[100];  
	sprintf(channel_,"device_code 2 = %s",mqtt.device_code);	
	#ifdef use_debug_serial
		SerialDEBUG.println(channel_);
		#endif 
}
void print_info_oph_status()
{
	
	#ifdef use_debug_serial
		SerialDEBUG.println("***print_info_oph_status***");
	SerialDEBUG.println("fvers =" + String(versionname));
	SerialDEBUG.println("fverl =" + String(fverl));
	SerialDEBUG.println("vcode =" + String(opu_generic_system.vcode));
	SerialDEBUG.println("disabled =" + String(opu_generic_system.disabled));
	SerialDEBUG.println("mid =" + String(opu_generic_system.mid));
	SerialDEBUG.println("txtype =" + String(opu_generic_system.txtype));
	SerialDEBUG.println("ip =" + String(opu_generic_system.ip));
	SerialDEBUG.println("phone =" + String(opu_generic_system.phone));
#endif 

	for(int i=0;i<20;i++)
	{
		if(opu_generic_system.group[i]==0)break;
		#ifdef use_debug_serial
		SerialDEBUG.print("group[" + String(i) + "]=");
		SerialDEBUG.println(opu_generic_system.group[i]);
		#endif 
	}
	
	#ifdef use_debug_serial
		SerialDEBUG.println("***end print_info***");
		#endif 
}


void load_oph_media_http(const char * name_)
{
	#ifdef use_debug_serial
		SerialDEBUG.printf("load_oph_media_http file %s\n\r",name_);
		#endif 
	spiff_readfilemedia(name_, &oph_media_buffer);		
	// print_oph_media_buffer();
	
}
void load_oph_generic()
{
	// SerialDEBUG.println("load_oph_generic");
	spiff_readfileoph(oph_generic_filename, &opu_generic_system);	
	print_info_oph_status();	

}

void load_save_value(String file){	
	
	//
	#ifdef use_debug_serial
		SerialDEBUG.println("Load_save_value");
	SerialDEBUG.print("File name: ");
	SerialDEBUG.println(file);
	#endif 
	spiff_readfile(file, au16outputdata);
	
	uint16_t function_contain;		  
	uint16_t out_function_contain;//OUTPUT FUNCTION==mode
	uint16_t out_loop_contain;//LOOP
	int MODE =0;	
	for(int i=0;i<8;i++){
		#ifdef use_debug_serial
		SerialDEBUG.print("\tOutput ");
		SerialDEBUG.print(i+1);
		SerialDEBUG.print(": ");
		#endif 
		for(int m=0;m<3;m++){															
			if(m==0){
				function_contain		=	au16outputdata[MODE+3*i];
				out_function_contain	=	function_contain >> 8;
				out_loop_contain		=	function_contain & 0x00ff;
				#ifdef use_debug_serial
		SerialDEBUG.print("[");SerialDEBUG.print(m);SerialDEBUG.print("]=");SerialDEBUG.print(au16outputdata[i*3+m]);SerialDEBUG.print("--> ");
				SerialDEBUG.print("Mode=");SerialDEBUG.print(out_function_contain);SerialDEBUG.print(",");
				SerialDEBUG.print("Loop=");SerialDEBUG.print(out_loop_contain);SerialDEBUG.print(", ");
				#endif 
			}
			else{				
				#ifdef use_debug_serial
		SerialDEBUG.print("[");SerialDEBUG.print(m);SerialDEBUG.print("]=");SerialDEBUG.print(au16outputdata[i*3+m]);SerialDEBUG.print(", ");
		#endif 
			}
		}
		#ifdef use_debug_serial
		SerialDEBUG.println("");		
		#endif 
	}
	#ifdef use_debug_serial
		SerialDEBUG.println("----");
		#endif 
}
void test_read_value(String file){	
	#ifdef use_debug_serial
		SerialDEBUG.println("********test_read_value");
	SerialDEBUG.print("File name: ");
	SerialDEBUG.println(file);
	#endif 
	spiff_readfile(file, au16outputdata);
	//
	#ifdef use_debug_serial
		SerialDEBUG.println("********End test_read_value");
		#endif 

}

void delay_reset()
{
	u32_timedelay = millis() + 1000;
}

int delay_withoutdelay() {
  int kq=1;
 
  if (millis() > u32_timedelay) {
    kq=0;
  }
  return kq;
}



void process_data()
{
	#ifdef use_debug_serial
		SerialDEBUG.println("Data: " + inputString);
	#endif 
		

	if (inputString.startsWith("LMD MINF")) {
		String ss =inputString.substring(9, inputString.length());
		if(ss!="")
		{		
			ss.toCharArray(fverl,sizeof(fverl));			
		}
	}
	inputString = "";	
	
	ui32timeoutrx = millis()+df_time_disconnect;
}

int heart_beat() {
  int kq=0;
  if (millis() > tempus) {
    if (status_led == 1) {
      digitalWrite( led_sign, HIGH );      
      status_led = 0;
	  	  
	   if(flash_led)
	   {
		tempus = millis() + 100;
	   }
	   
	   else if(system_status==SYS_ERR0)
	  {
		  tempus = millis() + 500;
	  }
	  else if(system_status==SYS_ERR_SERVER)
	  {
		  tempus = millis() + 900;
	  }
	  else if(system_status==SYS_WAIT_RESET)
	  {
		  tempus = millis() + 900;
	  }
	  else{
		tempus = millis() + 50;
	  }

	  kq=1;
    }
    else {
      digitalWrite( led_sign, LOW );
      status_led = 1;
      if(flash_led)
	   {
		tempus = millis() + 100;
	   }
	   
	   else if(system_status==SYS_ERR0)
	  {
		  tempus = millis() + 500;
	  }
	  else if(system_status==SYS_ERR_SERVER)
	  {
		  tempus = millis() + 100;
	  }
	  else if(system_status==SYS_WAIT_RESET)
	  {
		  tempus = millis() + 900;
	  }
	  else{
		tempus = millis() + 1000;
	  }
    }
  }
  return kq;
}

/**
 * @enum send_opu_generic
 * @brief
 * Gửi bản tin này ngay khi kết nối thành công tới server
 */
void send_opu_generic()
{			
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

		doc["opcode"]=(uint16_t)OPU_GENERIC;
		doc["id"]=mqtt.device_code;
		
		doc["data"]["fvers"]=versionname;
		doc["data"]["fverl"]=String(fverl);

		
		doc["data"]["vcode"]=rand();

		// Create the "group" array
		JsonArray gr = doc["data"].createNestedArray("group");
		for (int pin = 0; pin < 20; pin++) {    
		// Add the value at the end of the array
		if(opu_generic_system.group[pin]==0) break;
			gr.add(opu_generic_system.group[pin]);
		}

		doc["data"]["disabled"]=opu_generic_system.disabled;
		doc["data"]["mid"]=opu_generic_system.mid;
		doc["data"]["txtype"]=opu_generic_system.txtype;
		doc["data"]["ip"]=String(opu_generic_system.ip);
		doc["data"]["phone"]=String(opu_generic_system.phone);

		char msg[500];
		serializeJson(doc, msg);

		char channel_[100];  
		sprintf(channel_,"h2/s/%s",mqtt.device_code);
		
		if(mqtt.mode_data_is_wifi)
		{
			client.publish(channel_,msg,0,2);	
		}
		else
		{
			clientGSM.publish(channel_,msg,0,2);		
		}
}
/**
 * @enum send_opu_status
 * @brief
 * cập nhật lên server 1phut/lần/ hoặc khi có thay đổi conn, spkerr
 */
void send_opu_status()
{			
	// publish a message roughly every minutes
	if (millis() - lastMillis > df_time_send_opu) {
		lastMillis = millis();
		check_firmware++;
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
		doc["opcode"]=(uint16_t)OPU_STATUS;
		doc["id"]=mqtt.device_code;
		
		doc["data"]["conn"]=opu_status_system.conn;
		doc["data"]["csqm"]=opu_status_system.csqm;
		doc["data"]["csqw"]=opu_status_system.csqw;
		doc["data"]["spkerr"]=opu_status_system.spkerr;
		doc["data"]["spksta"]=opu_status_system.spksta;
		doc["data"]["temp"]=opu_status_system.temp;
		doc["data"]["fmsta"]=opu_status_system.fmsta;
		doc["data"]["sdfree"]=opu_status_system.sdfree;
		// doc["data"]["sdtotal"]=opu_status_system.sdtotal;
		doc["data"]["sdtotal"]=1536;//1.5*1024 kbytes
		doc["data"]["fvers"]=versionname;
		doc["data"]["fverl"]=String(fverl);

		char msg[500];
		serializeJson(doc, msg);
		
		char channel_[100];  
		sprintf(channel_,"h2/s/%s",mqtt.device_code);
		
		if(mqtt.mode_data_is_wifi)
		{
			client.publish(channel_,msg,0,2);	
		}
		else
		{
			clientGSM.publish(channel_,msg,0,2);		
		}
  }
	
}

void test_time()
{
	
	struct tm tmstruct ;
	// delay(2000);
	tmstruct.tm_year = 0;
	getLocalTime(&tmstruct, 5000);
	#ifdef use_debug_serial
		SerialDEBUG.printf("\nTime is : %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
	SerialDEBUG.println("");
	#endif 
	
}
int FirmwareVersionCheck(void) {
  String payload;
  int kq=0;
  int httpCode;
  String fwurl = "";
  fwurl += URL_fw_Version;
  fwurl += "?";
  fwurl += String(rand());
  // SerialDEBUG.println(fwurl);
  WiFiClientSecure * client = new WiFiClientSecure;
  
  if (client) 
  {
    client -> setCACert(rootCACertificate);

    // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
    HTTPClient https;

    if (https.begin( * client, fwurl)) 
    { // HTTPS      
      // SerialDEBUG.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      delay(100);
      httpCode = https.GET();
      delay(100);
      if (httpCode == HTTP_CODE_OK) // if version received
      {
        payload = https.getString(); // save received version
      } 
	  // else {
        // SerialDEBUG.print("error in downloading version file:");
        // SerialDEBUG.println(httpCode);
      // }
      https.end();
    }
    delete client;
  }
      
  if (httpCode == HTTP_CODE_OK) // if version received
  {
    payload.trim();
	// SerialDEBUG.println(payload);
	
	StaticJsonDocument<1024> doc;
	// Deserialize the JSON document
	DeserializationError error = deserializeJson(doc, payload);
	if (error)
	{
		// SerialDEBUG.println(F("This is a JDaughter string, not a JSON string"));
		return 0;
	}	
	char chuoi[20];
	strlcpy(chuoi,                  // <- destination
	doc["wifi"] | "erro",  // <- source
	sizeof(chuoi));         // <- destination's capacity
	// SerialDEBUG.println("Version wifi board =" + String(chuoi));
	
	if(String(chuoi)=="erro")
	{
		#ifdef use_debug_serial
		SerialDEBUG.printf("Firmware: check erro\n");
		#endif 
	}
	else if(String(chuoi)==String(versionname))
	{
		#ifdef use_debug_serial
		SerialDEBUG.printf("Firmware already on latest firmware version:%s\n", versionname);
		#endif 
	}
	else{
		// SerialDEBUG.println("New firmware Wifi detected");
		kq=1;
	}
	
	strlcpy(chuoi,                  // <- destination
	doc["lmd"] | "erro",  // <- source
	sizeof(chuoi));         // <- destination's capacity
	// SerialDEBUG.println("Version wifi LMD =" + String(chuoi));
	
	if(String(chuoi)=="erro")
	{
		// SerialDEBUG.printf("\lmd version erro check\n");
		;
	}
	else{
		// SerialDEBUG.printf("\lmd version is %s\n",chuoi);
		send2lmd(chuoi);
	}
	
	/*
    if (payload.equals(FirmwareVer)) {
      SerialDEBUG.printf("\nDevice already on latest firmware version:%s\n", FirmwareVer);
      return 0;
    } 
    else 
    {
      SerialDEBUG.println(payload);
      SerialDEBUG.println("New firmware detected");
      return 1;
    }
	*/
  } 
  return kq;  
}

/**checkfirmware
hàm tự động kiểm tra version và cập nhật
đã tắt tính năng này đi

*/

void checkfirmware(){
	check_firmware=0;
	if (FirmwareVersionCheck()) {
      firmwareUpdate();
    }
}
void firmwareUpdate(void) {
  WiFiClientSecure client;
  

  client.setInsecure();//mới thêm

  // client.setCACert(rootCACertificate);//mới bỏ
  httpUpdate.setLedPin(2, LOW);
  
  t_httpUpdate_return ret = httpUpdate.update(client, URL_fw_Bin);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      #ifdef use_debug_serial
		SerialDEBUG.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
		#endif 
      break;

    case HTTP_UPDATE_NO_UPDATES:
      #ifdef use_debug_serial
		SerialDEBUG.println("HTTP_UPDATE_NO_UPDATES");
		#endif 
      break;

    case HTTP_UPDATE_OK:
      #ifdef use_debug_serial
		SerialDEBUG.println("HTTP_UPDATE_OK");
		#endif 
      break;
  }
}

void selec_mode_mqtt()
{
	info_time_system.status=TIME_ERRO;
	info_time_system.mode = BYMQTT;//chuyển sang server
	info_time_system.process = TIME_START;
}

void selec_mode_rtc()
{
	info_time_system.status=TIME_ERRO;
	info_time_system.mode = BYRTC;//chuyển sang RTC
	info_time_system.process = TIME_START;
}
void read_time_RTC()
{	
	return;
	#ifdef use_debug_serial
		SerialDEBUG.print("RTC : ");
		#endif 
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
    SerialDEBUG.print(" unixtime = ");
    SerialDEBUG.println(now.unixtime());
	#endif 
}
	
void updatetimebyPool_func()
{
	
	//pool-->RTC-->MQTT
	#ifdef use_debug_serial
		SerialDEBUG.println("Contacting Time Server");
		#endif 
	configTime(3600*timezone, daysavetime*3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
	struct tm tmstruct ;
	delay(2000);
	tmstruct.tm_year = 0;
	getLocalTime(&tmstruct, 5000);
	#ifdef use_debug_serial
		SerialDEBUG.printf("\nNow is : %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
	SerialDEBUG.println("");
	#endif 
	if(tmstruct.tm_year<80)
	{
		// info_time_system.status=TIME_ERRO;
		//erro
		// selec_mode_mqtt();
		selec_mode_rtc();
	}
	else
	{			
		
		if(info_time_system.status==TIME_ERRO)
		{
			#ifdef use_debug_serial
			SerialDEBUG.print("updatetimebyPool_func: update new time-->update playlist");
			#endif
			read_playlist=1	;					
		}
		
		info_time_system.status=TIME_NORMAL;
		

		
		//kiểm tra nếu RTC bị chạy sai, thì cập nhật cho RTC
		if(ui8_ds1307_need_update==1)
		{
			#ifdef use_debug_serial
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
}
/**
 * @enum updatetimebyrtc_func
 * @brief
 * Cập nhật thời gian từ RTC qua cho esp
 */
void updatetimebyrtc_func()
{
	//pool-->RTC-->MQTT
	#ifdef use_debug_serial
		SerialDEBUG.println("Contacting Time RTC");
		#endif 
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
	
	if(now.unixtime() < 1640998800)//Date and time (your time zone): thứ bảy, 1 tháng 1 năm 2022 08:00:00 GMT+07:00
	{
		//thời gian bị chạy sai
		ui8_ds1307_need_update=1;
		selec_mode_mqtt();		
	}
	else
	{
		#ifdef use_debug_serial
		SerialDEBUG.println("config time via RTC");
		#endif 

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


/**
 * @enum send_opu_generic
 * @brief
 * Gửi bản tin này ngay khi kết nối thành công tới server
 */
void send_get_time_mqtt()
{			
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

		doc["opcode"]=(uint16_t)OPU_TIME_GET;
		doc["id"]=mqtt.device_code;		
		doc["data"]=0;

		char msg[500];
		serializeJson(doc, msg);

		char channel_[100];  
		sprintf(channel_,"h2/s/%s",mqtt.device_code);
		
		if(mqtt.mode_data_is_wifi)
		{
			client.publish(channel_,msg,0,2);	//không retain gói này
		}
		else
		{
			clientGSM.publish(channel_,msg,0,2); //không retain gói này
		}
}
void updatetimebymqtt_func()
{
	//gửi yêu cầu cập nhật thời gian
	if(info_time_system.process==TIME_START)
	{
		#ifdef use_debug_serial
		SerialDEBUG.println("gui yeu cau get time");
		#endif 
		info_time_system.process=TIME_WAIT;
		send_get_time_mqtt();
		info_time_system.u32checkrespone=millis()+30000;//chờ phản hồi trong 30S
	}
	else if(info_time_system.process==TIME_WAIT)
	{
		//kiểm tra phản hồi
		if (millis() > info_time_system.u32checkrespone){
			#ifdef use_debug_serial
		SerialDEBUG.println("mqtt noT respone get time");
		#endif 
			//RESET
			info_time_system.mode=BYNONE;//chuyển sang server
			info_time_system.process=TIME_START;
		}		
	}
	else if(info_time_system.process==TIME_FINISH)
	{
		//đã có data time
		#ifdef use_debug_serial
		SerialDEBUG.println("Have data time mqtt");
		#endif 
		updatetime_func_via4G(info_time_system.timeset);
		//RESET
		info_time_system.mode=BYNONE;//chuyển sang server
		info_time_system.process=TIME_START;	
	}
	
}
void updatetime_func_via4G(int32_t unixtime)
{
	
	#ifdef use_debug_serial
		SerialDEBUG.println("config time via 4G");
		#endif 
	// configTime(3600*timezone, daysavetime*3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");

	timeval epoch = {unixtime, 0};
	settimeofday((const timeval*)&epoch, 0);
		
	struct tm tmstruct ;
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
	if (millis() < info_time_system.u32check_system_time) return;
	
	struct tm tmstruct ;		
	tmstruct.tm_year = 0;
	getLocalTime(&tmstruct, 5000);
	
	// SerialDEBUG.printf("\nESP : %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
	// SerialDEBUG.println("");
	// SerialDEBUG.print("Time get is : ");
	// SerialDEBUG.println(&tmstruct, "%A, %B %d %Y %H:%M:%S");
	// SerialDEBUG.print("tm_year is : ");
	// SerialDEBUG.println(tmstruct.tm_year);
		
	//Thursday, January 01 1970 00:02:26
	if(tmstruct.tm_year < 100)
	{		
		#ifdef use_debug_serial
		SerialDEBUG.print("Time get is : ");
		SerialDEBUG.println(&tmstruct, "%A, %B %d %Y %H:%M:%S");
		SerialDEBUG.println("Wrong time-->need update");
		#endif 
		info_time_system.status=TIME_ERRO;
		// SerialDEBUG.println("bo dong nay di selec_mode_rtc");
		// selec_mode_rtc();
		
		/*
		if(info_time_system.mode==BYSERVER)
		{
			SerialDEBUG.println("Gettime by server");						
		}
		*/
	}
	else
	{
		//nếu trước đó thời gian đang bị lỗi thì load lại lịch phát				
		if(info_time_system.status==TIME_ERRO)
		{
			#ifdef use_debug_serial
			SerialDEBUG.print("check_system_time: update new time-->update playlist");
			#endif
			read_playlist=1	;					
		}
		
		
		info_time_system.status=TIME_NORMAL;
		info_time_system.mode=BYNONE;
		info_time_system.process=0;		
	}
	
	read_time_RTC();
	
	info_time_system.u32check_system_time = millis()+2000;
}

void getTime1()
{
	unsigned long currentMillis = millis();
	
	// if(lmd_repeat>4) return;
	
	if (currentMillis < u32_gettime) return;
 
  u32_gettime=currentMillis+60000; 
  
  uint8_t HH1, MM1;
  char HH[2];
  char MM[2];

  // const char expected_answer1[] = "+CCLK:";
  if(sendATcommandpro("AT+CCLK?", "OK", 2000))
  {
	//hoạt động bình thường
	ui8_bathuong=0;
  }
  else
  {
	//hoạt động bất thường
	ui8_bathuong++;
	if(ui8_bathuong>2)
	{
		#ifdef use_debug_serial
		SerialDEBUG.println("Repower modem");
		#endif 
		connect_modem();
		return;
	}
  }
  
  if(info_time_system.status==TIME_NORMAL)return;
  
  if(info_time_system.mode==BYMQTT)return;
  
  if(info_time_system.mode==BYRTC)return;
  
  
  
  //+CCLK: "22/03/04,10:23:20+28"

  // SerialDEBUG.println("3002");
	char* ps;
  ps = strstr(u8_response, "+CCLK:");
  //SerialDEBUG.print("Vi:");
  //SerialDEBUG.print(ps);
  if ( ps != NULL)
  {
    // +CCLK: "21/06/27,20:34:15+28"	
	struct tm tmstruct ;
	for ( uint8_t i = 0; i < 30; i++)
    { if ( ps[i] == '"')
      {
        //tm_year
		HH[0] = ps[i + 1];
        HH[1] = ps[i + 2];
		HH1 = (HH[0] - '0') * 10 + (HH[1] - '0');				
		
		if(HH1<20)
		{
			#ifdef use_debug_serial
		SerialDEBUG.println("Sim wrong time-->need update time by MQTT");
		#endif 
			selec_mode_mqtt();
			break;
		}
		
		tmstruct.tm_year=2000+HH1;
		#ifdef use_debug_serial
		SerialDEBUG.print("tm_year=");
		SerialDEBUG.println(tmstruct.tm_year);
		#endif 
		tmstruct.tm_year-=1900;
						
		//tm_mon
		HH[0] = ps[i + 4];
        HH[1] = ps[i + 5];
		HH1 = (HH[0] - '0') * 10 + (HH[1] - '0');				
		tmstruct.tm_mon=HH1;
		
		#ifdef use_debug_serial
		SerialDEBUG.print("tm_mon=");
		SerialDEBUG.println(tmstruct.tm_mon);
		#endif 
		tmstruct.tm_mon--;
		
		//tm_mday
		HH[0] = ps[i + 7];
        HH[1] = ps[i + 8];
		HH1 = (HH[0] - '0') * 10 + (HH[1] - '0');				
		tmstruct.tm_mday=HH1;
		#ifdef use_debug_serial
		SerialDEBUG.print("tm_mday=");
		SerialDEBUG.println(tmstruct.tm_mday);
		#endif 
		
		//tm_hour
		HH[0] = ps[i + 10];
        HH[1] = ps[i + 11];
		HH1 = (HH[0] - '0') * 10 + (HH[1] - '0');				
		tmstruct.tm_hour=HH1;
		#ifdef use_debug_serial
		SerialDEBUG.print("tm_hour=");
		SerialDEBUG.println(tmstruct.tm_hour);
		#endif 
		
		//tm_min
		HH[0] = ps[i + 13];
        HH[1] = ps[i + 14];
		HH1 = (HH[0] - '0') * 10 + (HH[1] - '0');				
		tmstruct.tm_min=HH1;
		#ifdef use_debug_serial
		SerialDEBUG.print("tm_min=");
		SerialDEBUG.println(tmstruct.tm_min);
		#endif 
		
		//tm_sec
		HH[0] = ps[i + 16];
        HH[1] = ps[i + 17];
		HH1 = (HH[0] - '0') * 10 + (HH[1] - '0');				
		tmstruct.tm_sec=HH1;
		#ifdef use_debug_serial
		SerialDEBUG.print("tm_sec=");
		SerialDEBUG.println(tmstruct.tm_sec);
		#endif 
		// SerialDEBUG.printf("\nTime is : day %d %d-%02d-%02d %02d:%02d:%02d",(tmstruct.tm_wday)+1, (tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
		// SerialDEBUG.print(">>>");		
		#ifdef use_debug_serial
		SerialDEBUG.print("Time set : ");
		#endif 
		
		// SerialDEBUG.println(&tmstruct, "%A, %B %d %Y %H:%M:%S");	dòng này gây ra lỗi chip	
		
		time_t  t = mktime(&tmstruct);//đây là thời gian UNIX
		
		#ifdef use_debug_serial
		SerialDEBUG.print("UNIX set : ");
		SerialDEBUG.println(t);
		#endif 
		updatetime_func_via4G(t);

		i = 31;
		break;
      }
    }
  }  
}

void print_time()
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
		
	if (millis() > u32chk) {
		
		u32chk = millis() + df_time_ack;//ms
		// SerialDEBUG.println("CHK");
		SerialLMD.print("CHK\n");		
		
		/*		
		struct tm tmstruct ;		
		tmstruct.tm_year = 0;
		getLocalTime(&tmstruct, 5000);

		SerialDEBUG.printf("\nTime is : day %d %d-%02d-%02d %02d:%02d:%02d",(tmstruct.tm_wday)+1, (tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
		SerialDEBUG.print(">>>");		
		// SerialDEBUG.println(&tmstruct, "%A, %B %d %Y %H:%M:%S");		
		time_t  t = mktime(&tmstruct);
		SerialDEBUG.println(" = " + String(t));		
		*/
	}
}
/**
 * @enum playtext
 * @brief
 * Kiểm tra xem có tới thời điểm hiển thị text không
 */
 
void playtext()
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
	//kiểm tra status
	
	if (opu_generic_system.disabled==1)return;
	
	if (millis() > u32timecheck) {
		
		//in thời gian cho vui mắt
		struct tm tmstruct ;		
		tmstruct.tm_year = 0;
		getLocalTime(&tmstruct, 5000);

		// SerialDEBUG.printf("\nTime is : day %d %d-%02d-%02d %02d:%02d:%02d",(tmstruct.tm_wday)+1, (tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
		// SerialDEBUG.print(">>>");		
		// SerialDEBUG.println(&tmstruct, "%A, %B %d %Y %H:%M:%S");		
		time_t  t = mktime(&tmstruct);//đây là thời gian UNIX
		#ifdef use_debug_serial
		SerialDEBUG.println("Time unix = " + String(t));
		#endif

		///giây so sánh
		uint32_t tsssss=(tmstruct.tm_hour)*3600+(tmstruct.tm_min)*60+(tmstruct.tm_sec);
		
		if(media_manager[0].playing==PLAYER_INTIME)
		{
			
			#ifdef use_debug_serial
				SerialDEBUG.printf("\nTime is : day %d %d-%02d-%02d %02d:%02d:%02d",(tmstruct.tm_wday)+1, (tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
				SerialDEBUG.print(">>>");
				SerialDEBUG.println(" = " + String(t));
									
				SerialDEBUG.println("\tCheck start");
				SerialDEBUG.printf("\tnow= %d",tsssss);
				SerialDEBUG.printf(" , ss= %d\n",media_manager[0].ts);
			#endif 
			
			//so sánh lại thời gian cho dù mày là thằng nào
			
			if(tsssss<media_manager[0].ts)
			{
				#ifdef use_debug_serial
					SerialDEBUG.println("time nho hon->chuyen thanh ready");
				#endif 
				media_manager[0].playing=PLAYER_READY;
			}
			else if(tsssss>=media_manager[0].ts && tsssss< (media_manager[0].ts + media_manager[0].ds))
			{
				#ifdef use_debug_serial
					SerialDEBUG.println("Intime-->send led");
				#endif 
		
			
				index_send++;
				send_message2LMD(media_manager[0].index);
				media_manager[0].playing=PLAYER_PLAYING;
				read_playlist=1;
			}
			else
			{
				//quá lịch phát
				//tìm lịch phát tiếp theo
				//shift lịch
				shift_playlist();
			}
		}
		else if(media_manager[0].playing==PLAYER_PLAYING)
		{
			//đang phát
			//kiểm tra xem phát xong chưa
			if(tsssss>=media_manager[0].ts + media_manager[0].ds)
			{
				#ifdef use_debug_serial
		SerialDEBUG.printf("\nTime is : day %d %d-%02d-%02d %02d:%02d:%02d",(tmstruct.tm_wday)+1, (tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
				SerialDEBUG.print(">>>");
				SerialDEBUG.println(" = " + String(t));
									
				SerialDEBUG.println("\tCheck start");
				SerialDEBUG.printf("\tnow= %d",tsssss);
				SerialDEBUG.printf(" , ss= %d\n",media_manager[0].ts);
								
				//gửi qua bảng led
				SerialDEBUG.println("End time-->End play file ");
				#endif 
				//kiểm tra xem có lịch phát tiếp không
				//nếu có gửi tin nhắn tiếp theo, không cần gửi STOP
				//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				//shift lịch
				shift_playlist();
				
				//check file
				//trường hợp có nhiều file khẩn cấp trong hàng đợi
				if(media_manager[0].mode==2)
				{
					//phat ngay cho toi
					#ifdef use_debug_serial
		SerialDEBUG.println("Phat ngay cho toi in stop mode");
		#endif 
					//lấy duration truyền qua
					
					index_send++;
					send_message2LMD(0);

					//gettime
					struct tm tmstruct ;		
					tmstruct.tm_year = 0;
					getLocalTime(&tmstruct, 5000);
					uint32_t tsssss=(tmstruct.tm_hour)*3600+(tmstruct.tm_min)*60+(tmstruct.tm_sec);
					
					media_manager[0].ts=tsssss;
					media_manager[0].playing=PLAYER_PLAYING;
					//Thêm nội dung hiển thị vào hàng đợi
					read_playlist=1;
				}
				//kiểm tra xem có file nào phát liền không
				//nếu có thì gửi qua bảng led, mà không cần gửi lệnh STOP
				else if(tsssss>=media_manager[0].ts && tsssss< (media_manager[0].ts + media_manager[0].ds) && media_manager[0].playing != PLAYER_STOP)
				{
					
					#ifdef use_debug_serial
		SerialDEBUG.println("\tCheck start");
					SerialDEBUG.printf("\tnow= %d",tsssss);
					SerialDEBUG.printf(" , ss= %d\n",media_manager[0].ts);
					
					//tới thời gian chạy file
					//gửi qua bảng led
					SerialDEBUG.println("Ontime in stop mode-->Play file ");
					#endif 
					char name[30];
					sprintf(name,"/F%d.lmd",media_manager[0].mid);
					//gửi qua bảng led					
					index_send++;
					send_message2LMD(media_manager[0].index);
					media_manager[0].playing=PLAYER_PLAYING;
					//Thêm nội dung hiển thị vào hàng đợi
					read_playlist=1;
				}
				else {
					#ifdef use_debug_serial
		SerialDEBUG.println("Gui qua bang led STOP");
		#endif 
					//gửi qua bảng led					
					index_send++;								
					lmd_command=LMD_STOP;
					lmd_repeat=0;
					u32_lmd_resend=0;//gửi liền không cần chờ
					
					// send_command_screen();	
					// delay(20);					
					//doc lich phat tiep theo
					read_playlist=1;			
				}				
			}
		}
		else if(media_manager[0].playing==PLAYER_READY)
		{
			//Kiểm tra xem tới thời gian phát chưa
			
			#ifdef use_debug_serial
			SerialDEBUG.printf("\tso sanh now= %d",tsssss);
			SerialDEBUG.printf(" , ss= %d\n",media_manager[0].ts);
			#endif
			
			if(tsssss>=media_manager[0].ts)
			{													
				#ifdef use_debug_serial
					SerialDEBUG.printf("\tPLAYER_READY?? now= %d",tsssss);
					SerialDEBUG.printf(" , ss= %d\n",media_manager[0].ts);
				#endif 
				if (tsssss< (media_manager[0].ts + media_manager[0].ds))
				{					
					#ifdef use_debug_serial
						SerialDEBUG.println("Ontime-->Play file ");
					#endif 
				
					index_send++;
					send_message2LMD(media_manager[0].index);
					media_manager[0].playing=PLAYER_PLAYING;
					
				}
				else
				{
					#ifdef use_debug_serial
						SerialDEBUG.println("Over time-->Next file ");
					#endif 
					shift_playlist();
				}
				//Thêm nội dung hiển thị vào hàng đợi
				read_playlist=1;
			}		
		}			
		u32timecheck = millis() + 1000;		
	}
}
//callback notifying us of the need to save config
void saveConfigCallback () {
  #ifdef use_debug_serial
		SerialDEBUG.println("Should save config");
		#endif 
  shouldSaveConfig = true;
  //get command

  #ifdef use_debug_serial
		SerialDEBUG.println("Command receive = " + String (ui16command));
  
  SerialDEBUG.println("command led = " + String (ui16ledcommand[0]) + ", " + String (ui16ledcommand[1]) + ", " + String (ui16ledcommand[2]));
  #endif 
  if(ui16command==2)
  {
	  #ifdef use_debug_serial
		SerialDEBUG.println(F("Send Update led command"));
		#endif 
	  index_lmd=103;
		update2lmd();
	  ui16command=0;
	shouldSaveConfig = false;	  
  }
   else if(ui16command==3)
  {
	  #ifdef use_debug_serial
		SerialDEBUG.println(F("Format SPIFFS function"));
		#endif 
	  ui16command=0;
	  deleteallfile(SPIFFS, "/", 0);		  
	  #ifdef use_debug_serial
		SerialDEBUG.println(F("Format SPIFFS complete"));	  	  
		#endif 
	  shouldSaveConfig = false;	  
  }
  
  if(ui16ledcommand[0]!=0)
  {
	  	index_lmd=104;
		update2lmd();	  
	  ui16command=0;	    	  
	  shouldSaveConfig = false;	  
  }
}
/*
void updatefirmware() {

  
  SerialDEBUG.print("Setting AP mode");

  WiFi.softAP(df_user_up_server.c_str(), df_pass_firm); //TẠO ACCESS POINT ĐỂ PHÁT WIFI

  IPAddress IP = WiFi.softAPIP(); //mặc định là 192.168.4.1
  SerialDEBUG.print("AP IP address: ");
  SerialDEBUG.println(IP);

  //THIẾT KẾ WEBSERVER TRÊN IP CỦA ACCESS POINT VÀ CHỨC NĂNG UPLOAD CODE
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      // SerialDEBUG.setDebugOutput(true);
      // WiFiUDP::stopAll();
      SerialDEBUG.printf("Update: %s\n", upload.filename.c_str());
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if (!Update.begin(maxSketchSpace)) { //start with max available size
        Update.printError(SerialDEBUG);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(SerialDEBUG);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        SerialDEBUG.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(SerialDEBUG);
      }
      // SerialDEBUG.setDebugOutput(false);
    }
    // yield();
  });
  server.begin(); //KHỞI ĐỘNG SERVER

}
*/

/**------setup_reset_fatory_wifi-------------
Tạo AP: 
Nút xác nhận format.
Nút xóa các file media.
...
*/
void setup_reset_fatory_wifi()
{
	//reset values
	shouldSaveConfig=false;
	
	// The extra parameters to be configured (can be either global or just in the setup)
	// After connecting, parameter.getValue() will get you the configured value
	// id/name placeholder/prompt default length

	//WiFiManager
	//Local intialization. Once its business is done, there is no need to keep it around
	WiFiManager wifiManager;

	//set config save notify callback
	wifiManager.setSaveConfigCallback(saveConfigCallback);
	
	//set where to save values
	wifiManager.setsavecommand(&ui16command);
	
	
	//add all your parameters here

	
	//reset saved settings
	//wifiManager.resetSettings();

	//set custom ip for portal
	//wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

	//fetches ssid and pass from eeprom and tries to connect
	//if it does not connect it starts an access point with the specified name
	//here  "AutoConnectAP"
	//and goes into a blocking loop awaiting configuration

	//check config wifi

	wifiManager.setConnectTimeout(20);//120second
	wifiManager.setConfigPortalTimeout(300);
	

	// SerialDEBUG.println("Connect wifi in config mode");
	wifiManager.setConfigwifi(2);//cho phep tao wifi
	wifiManager.setBreakAfterConfig(1);

	//load chip id
	char wifiname_id[23];
	snprintf(wifiname_id, 23, "LMD%lld", ESP.getEfuseMac());
	
	
	int kqketnoi = wifiManager.autoConnect(wifiname_id,df_pass_wifi);
	
	// SerialDEBUG.println("check save");
	// if (shouldSaveConfig) {
		// SerialDEBUG.println("saving config");		
	// }
	// else
	// {
		// SerialDEBUG.println("save none");
		// debug print	
	// }
	// SerialDEBUG.print("\nConnect status: ");
	// SerialDEBUG.println(kqketnoi);
	delay(1000);
}

void get_setting_parameters()
{
	
// int modeconnect=0;
	// The extra parameters to be configured (can be either global or just in the setup)
	// After connecting, parameter.getValue() will get you the configured value
	// id/name placeholder/prompt default length
	shouldSaveConfig = false;

	WiFiManagerParameter custom_devicecode("Devicecode", "Device code", mqtt.device_code, 40);
	WiFiManagerParameter custom_mqtt_server("Server", "mqtt server", mqtt.sever_mqtt, 50);
	WiFiManagerParameter custom_mqtt_port("Port", "mqtt port", mqtt.port, 5);		
	WiFiManagerParameter custom_mqtt_user("User", "User MQTT", mqtt.username, 20);
	WiFiManagerParameter custom_mqtt_pass("Pass", "Pass MQTT", mqtt.pwdMqtt, 20);
	WiFiManagerParameter custom_mqtt_id("ID", "Client ID", mqtt.client_id, 20);
	// WiFiManagerParameter custom_update_server("FirmServer", "code update Server", pass_update_firm, 10);
	// WiFiManagerParameter custom_update_led("FirmLed", "code update Led", pass_update_led, 10);
	
	
	WiFiManagerParameter custom_device_name_text("<small>Check to select Data mode Wifi</small>");
	
	
	char customhtml[24] = "type=\"checkbox\"";

	if (mqtt.mode_data_is_wifi) {
      strcat(customhtml, " checked");
    }
    // WiFiManagerParameter p_sensorDht22("mode_data", "Wifi/4G", "T", 2, customhtml, WFM_LABEL_AFTER);
	WiFiManagerParameter p_sensorDht22("mode_data", "Wifi/4G", "T", 2, customhtml);
	WiFiManagerParameter p_hint("<small>*Hint: if you want to reuse the currently active WiFi credentials, leave SSID and Password fields empty</small>");

	
	char customhtmlsv[24];
	char customhtmlhv[24];
	
	snprintf(customhtmlsv, 24, "<br/>Version %s",versionname);
	snprintf(customhtmlhv, 24, "<br/>Hardware %s",hardwareversionname);
	
	
	WiFiManagerParameter custom_version_(customhtmlsv);
	WiFiManagerParameter custom_hardware_(customhtmlhv);
	
	//WiFiManager
	//Local intialization. Once its business is done, there is no need to keep it around
	WiFiManager wifiManager;

	//set config save notify callback
	wifiManager.setSaveConfigCallback(saveConfigCallback);
	//set where to save values
	wifiManager.setsavecommand(&ui16command);
	wifiManager.setsaveledcommand(ui16ledcommand);
	
	//add all your parameters here
	wifiManager.addParameter(&p_hint);
	wifiManager.addParameter(&custom_devicecode);
	wifiManager.addParameter(&custom_mqtt_server);
	wifiManager.addParameter(&custom_mqtt_port);
	wifiManager.addParameter(&custom_mqtt_user);
	wifiManager.addParameter(&custom_mqtt_pass);
	wifiManager.addParameter(&custom_mqtt_id);
	// wifiManager.addParameter(&custom_update_server);
	// wifiManager.addParameter(&custom_update_led);	
	wifiManager.addParameter(&custom_device_name_text);
	wifiManager.addParameter(&p_sensorDht22);
	wifiManager.addParameter(&custom_version_);
	wifiManager.addParameter(&custom_hardware_);
	
	//reset saved settings
	//wifiManager.resetSettings();

	//set custom ip for portal
	//wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

	//fetches ssid and pass from eeprom and tries to connect
	//if it does not connect it starts an access point with the specified name
	//here  "AutoConnectAP"
	//and goes into a blocking loop awaiting configuration

	//check config wifi

	wifiManager.setConnectTimeout(20);//120second
	wifiManager.setConfigPortalTimeout(300);
		

		#ifdef use_debug_serial
		SerialDEBUG.println("Connect wifi in config mode");
		#endif 
		wifiManager.setConfigwifi(1);//cho phep tao wifi
		wifiManager.setBreakAfterConfig(1);
		
		char wifiname_id[23];
		snprintf(wifiname_id, 23, "LMD%s", mqtt.device_code);
		int kqketnoi = wifiManager.autoConnect(wifiname_id,df_pass_wifi);
		
		#ifdef use_debug_serial
		SerialDEBUG.println("read updated parameters");
		#endif 
		//read updated parameters				
		strcpy(mqtt.device_code, custom_devicecode.getValue());
		
		strcpy(mqtt.sever_mqtt, custom_mqtt_server.getValue());
		strcpy(mqtt.port, custom_mqtt_port.getValue());
		strcpy(mqtt.username, custom_mqtt_user.getValue());
		strcpy(mqtt.pwdMqtt, custom_mqtt_pass.getValue());
		strcpy(mqtt.client_id, custom_mqtt_id.getValue());		
		// strcpy(pass_update_firm, custom_update_server.getValue());
		// strcpy(pass_update_led, custom_update_led.getValue());
				

		mqtt.mode_data_is_wifi = (strncmp(p_sensorDht22.getValue(), "T", 1) == 0);

		//save the custom parameters to FS
		#ifdef use_debug_serial
		SerialDEBUG.println("check save");
		#endif 
		if (shouldSaveConfig) {
			#ifdef use_debug_serial
		SerialDEBUG.println("saving config");
		#endif 

			//kiểm tra update firmware
			/*
			if ( strncmp(pass_update_led, df_pass_firm, 10) == 0)
			{
				//setup 
				SerialDEBUG.println("Send update firmware to Led");				
				index_lmd=103;
				update2lmd();
				//chờ reset liền								
			}
			*/
			/*
			if ( strncmp(pass_update_firm, df_pass_firm, 10) == 0)
			{
				//setup 
				EEPROM.write(addr_configwifi, 30);
				EEPROM.commit();
				
				SerialDEBUG.println("Enter update firmware");
				send_messageinfo("Enter update firmware");
				//chờ reset liền
				delay(2000);
				ESP.restart();
				delay(2000);
				
			}
			*/
			
			DynamicJsonDocument jsonBuffer(1024);

			jsonBuffer["Wname"] =  wifiManager.getSSID();
			jsonBuffer["Wpass"] = wifiManager.getPassword();
			jsonBuffer["Port"] = mqtt.port;
			jsonBuffer["Server"] = mqtt.sever_mqtt;
			jsonBuffer["ID"] = mqtt.client_id;
			jsonBuffer["User"] = mqtt.username;
			jsonBuffer["Pass"] = mqtt.pwdMqtt;
			jsonBuffer["Device_code"] = mqtt.device_code;
			jsonBuffer["Data_mode"] = mqtt.mode_data_is_wifi;
									
			File configFile = SPIFFS.open(sys_filename, FILE_WRITE);
			if (!configFile) {
				#ifdef use_debug_serial
		SerialDEBUG.println("failed to open config file for writing");
		#endif 
			}

			#ifdef use_debug_serial
		serializeJson(jsonBuffer,SerialDEBUG);
		#endif 
			serializeJson(jsonBuffer,configFile);
			configFile.close();		
			
			// jsonBuffer.prettyPrintTo(SerialDEBUG);
			// jsonBuffer.printTo(configFile);
			// configFile.close();
			//end save
		}
		else
		{

			#ifdef use_debug_serial
		SerialDEBUG.println("save none");
		#endif 
			//debug print	
		}

}
void connect_wifi()
{
	WiFiManager wifiManager;
	wifiManager.setConnectTimeout(20);//120second
	wifiManager.setConfigPortalTimeout(300);
		
	wifiManager.setConfigwifi(0);
	wifiManager.setBreakAfterConfig(1);
			
	#ifdef use_debug_serial
		SerialDEBUG.println("Connecting to wifi " + String(mqtt.wifi_name) );
		#endif 
	int kqketnoi = wifiManager.autoConnect(mqtt.wifi_name,mqtt.wifi_pass);
			
	// SerialDEBUG.print("\nConnect status: ");
	// SerialDEBUG.println(kqketnoi);
	if(kqketnoi)
	{
		#ifdef use_debug_serial
		SerialDEBUG.println("Wifi connected  ");
		#endif 
	}
	else{
		#ifdef use_debug_serial
		SerialDEBUG.println("Wifi: fail      ");
		#endif 
		
	}
	delay(1000);
	
	// String ssid_ = wifiManager.getSSID();
	// String pass_ = wifiManager.getPassword();
	
	// SerialDEBUG.print("After connect:\n");
	// SerialDEBUG.print("SSID=");
	// SerialDEBUG.println(ssid_);
	// SerialDEBUG.print("PASS=");
	// SerialDEBUG.println(pass_);

	//convert string to char
	// ssid_.toCharArray(ssid, ssid_.length() + 1);
	// pass_.toCharArray(pass, pass_.length() + 1);
	// SerialDEBUG.print("new SSID=");
	// SerialDEBUG.println(ssid);
	// SerialDEBUG.print("new PASS=");
	// SerialDEBUG.println(pass);
	
	
}

void setupwifi(int mode_)
{
	int modeconnect=0;
	
	/*
	if(mode_==0)
	{
		int m = EEPROM.read(addr_configwifi);
		if(m==30)
		{
			mode_=1;
			//write eeprom
			EEPROM.write(addr_configwifi, 10);
			EEPROM.commit();			
		}
		else if(m!=10)
		{
			mode_=2;
			//write eeprom
			EEPROM.write(addr_configwifi, 10);
			EEPROM.commit();			
		}		
	}
	*/

	if(mode_==2)
	{
		get_setting_parameters();
	}
	

	if(mqtt.mode_data_is_wifi)
	{				
		#ifdef use_debug_serial
		SerialDEBUG.println("Connect wifi in normal mode");
		#endif 
		//sau này nếu có thêm phiên bản 4G, thì phải kết nối vào chỗ này
		//kết nối vào wifi		
		connect_wifi();
		return;
	}
	else
	{
		#ifdef use_debug_serial
		SerialDEBUG.println("4G mode");
		#endif 
		return;
	}

	//kết nối vào wifi		
	connect_wifi();

}

uint16_t conertchar2u16(char *chuoi)
{
	uint16_t kq_;
	String inputString = "";
	for(int i=0;i<5;i++)
	{			
		if (isDigit(chuoi[i])) {
			inputString += (char)chuoi[i];               
		}
		else
		{
			break;
		}
	}
	kq_=inputString.toInt();
	
	   // SerialDEBUG.print("Value:");
      // SerialDEBUG.println(kq_);
      // SerialDEBUG.print("String: ");
      // SerialDEBUG.println(inputString);
	  return kq_;
}
void chopledreset()
{
	for(int i=0;i<3;i++)
	{
		digitalWrite( led_sign, HIGH );
		delay(100);
		digitalWrite( led_sign, LOW );
		delay(100);		
	}
}


void press_power_modem()
{
  // wdt_reset();
  // wdt_disable();

  // digitalWrite(gsm_status, 0);
  //không cho reset
  digitalWrite(pinReset, LOW);

  //nhấn nút nguồn
  digitalWrite(pinPower, HIGH);
  delay(1300);
  digitalWrite(pinPower, LOW);

}
/*
int8_t sendATcommandpro(char* ATcommand, char* expected_answer, unsigned int timeout) // xu ly goi lenh
{

  //in debug
  SerialDEBUG.print(F("Sent2 modem: "));
  SerialDEBUG.println(ATcommand);
  uint8_t x = 0;
  bool answer = 0;
  //char response[200];
  unsigned long previous;
  memset(response, '\0', 200);
  // wdt_reset();
  // watchdogStart();
  delay(100);
  while ( SerialAT.available() > 0) SerialAT.read();
  if (ATcommand[0] != '\0') {
    SerialAT.print(ATcommand);
    delay(1);
    SerialAT.print("\r\n");
  }
  x = 0;
  previous = millis();

  do {
    // wdt_reset();
    // watchdogStart();
    // baodong();
    // CheckSerial2();
    if (SerialAT.available() != 0)
    {
      // wdt_reset();
      // watchdogStart();
      response[x] = SerialAT.read();
      x++;
      if (strstr(response, expected_answer) != NULL) {
        answer = 1;
      }
      if ( x > 198)
      {
        //// SerialDEBUG.print(F(" out"));
        x = 198;
        // break;
      }
    }
    //check_cbrung();
  }
  while ((answer == 0) && ((millis() - previous) < timeout));

  SerialDEBUG.println(response);
  return answer;
}
*/
void initializeSMS()
{
  // actual
  if (sendATcommandpro("AT", "OK", 1000))
  {
    sendATcommandpro("ATE0", "OK", 1000);
    sendATcommandpro("AT+CMGF=1", "OK", 1000); // configuration for sending SMS
    sendATcommandpro("AT+CNMI=2,1,0,0,0", "OK", 1000); // Configuration for receiving SMS
    sendATcommandpro("AT&W", "OK", 1000); // confim setting
    sendATcommandpro("AT+CLTS=1;&W", "OK", 2000);

    // sendATcommandpro("AT+CMGDA=\"DEL ALL\"", "OK", 5000);
    /*
	for ( uint8_t i = 0; i < 5; i++)
    {
      if (sendATcommandpro("AT+CMGDA=\"DEL ALL\"", "OK", 1000))
      {
        break;
      } else
      {
        delay(1000);
      }
    }
	*/
    for ( uint8_t i = 0; i < 5; i++)
    {
      if ( sendATcommandpro("AT+CMGD=90,4", "OK", 1000))
      {
        break;
      } else
      {
        delay(1000);
      }
    }
  }
}
void power_on_modem() {

  uint8_t retry_modem = 0;
  uint8_t reretry_modem = 0;
  int kq = 0;

  #ifdef use_debug_serial
		SerialDEBUG.println(F("Check modem"));
		#endif 

  while (1)
  {
    // wdt_reset();
    // wdt_disable();
    if ( sendATcommandpro("AT", "OK", 1000))
    {

      #ifdef use_debug_serial
		SerialDEBUG.println(F("find modem"));
		#endif 

      break;

    }
    else if (++retry_modem > 3)
    {
      retry_modem = 0;
      reretry_modem++;
      press_power_modem();
      // wdt_reset();
      // wdt_disable();
      delay(6000);
      if (reretry_modem > 2)
      {
        #ifdef use_debug_serial
		SerialDEBUG.println(F("Cannot find modem"));
		#endif 
        kq = 1;
        break;
      }
    }
    #ifdef use_debug_serial
		SerialDEBUG.print(F("Try @"));
    SerialDEBUG.println((retry_modem));
	#endif 
  }

}
void connect_modem()
{
  for(int i=0;i<3;i++)
  {
	uint32_t rate_=TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
	if(rate_)
	{
		#ifdef use_debug_serial
		SerialDEBUG.print("Have respone @");
		SerialDEBUG.println(i);
		#endif 
		break;
	}
	else
	{
		#ifdef use_debug_serial
		SerialDEBUG.print("No respone @");
		SerialDEBUG.println(i);
		#endif 
		press_power_modem();
		delay(6000);
	}
  }
}
#if defined (hwv0)

#elif defined (hwv1)
void check_reset_factory()
{
	#ifdef use_debug_serial
		SerialDEBUG.println("Enter check_reset_factory");
		#endif 
	//1 2 3

	// setup_reset_fatory_wifi();
	// return;
	
	//check button
	int read0_=read_state_button(&button0);
	int read1_=read_state_button(&button1);
	int read2_=read_state_button(&button2);
	int read3_=read_state_button(&button3);
	uint8_t presssw=0;
	int tam;
	flash_led=1;
	
	while(read0_==LOW || read1_==LOW ||read2_==LOW ||read3_==LOW)
	{
		read0_=read_state_button(&button0);
		read1_=read_state_button(&button1);
		read2_=read_state_button(&button2);
		read3_=read_state_button(&button3);

		tam=read_hold_button(&button0);
		if ( tam) {
			presssw=1;
		#ifdef use_debug_serial
		SerialDEBUG.println("Button 0 has pressed");	
		#endif 
		}
		
		tam=read_hold_button(&button1);
		if ( tam) {
			presssw=1;
			#ifdef use_debug_serial
		SerialDEBUG.println("Button 1 has pressed");	
		#endif 
		}
		tam=read_hold_button(&button2);
		if ( tam) {
			presssw=1;
			#ifdef use_debug_serial
		SerialDEBUG.println("Button 2 has pressed");	
		#endif 
		}
		tam=read_hold_button(&button3);
		if ( tam) {
			presssw=1;
			#ifdef use_debug_serial
		SerialDEBUG.println("Button 3 has pressed");	
		#endif 
		}
		
		if(presssw)
		{
			//enter configmode
			//truyền qua bảng led
			flash_led=0;
			// system_status=SYS_WAIT_RESET;
			#ifdef use_debug_serial
		SerialDEBUG.println("Enter config system");
		#endif 
			send_messageinfo("Enter config system");
			setup_reset_fatory_wifi();
			break;
		}
		//flash led
		heart_beat();		
	}	
	flash_led=0;	
}
#elif defined (hwv2)
void check_reset_factory()
{
	#ifdef use_debug_serial
		SerialDEBUG.println("Enter check_reset_factory in V2 mode");
		#endif 
	//1 2 3

	// setup_reset_fatory_wifi();
	// return;
	
	//check button
	int read3_=read_state_button(&button3);
	uint8_t presssw=0;
	int tam;
	flash_led=1;
	
	while(read3_==LOW)
	{
		read3_=read_state_button(&button3);

		tam=read_hold_button(&button3);
		if ( tam) {
			presssw=1;
			#ifdef use_debug_serial
		SerialDEBUG.println("Button 3 has pressed");	
		#endif 
		}
		
		if(presssw)
		{
			//enter configmode
			//truyền qua bảng led
			flash_led=0;
			// system_status=SYS_WAIT_RESET;
			#ifdef use_debug_serial
				SerialDEBUG.println("Enter config system");
			#endif 
			send_messageinfo("Enter config system");
			setup_reset_fatory_wifi();
			break;
		}
		//flash led
		heart_beat();		
	}	
	flash_led=0;	
}
#else
	#error "Please define one of use_ in config_hw_local.h"
#endif


void scan_rtc()
{
  byte error, address;
  int nDevices;

  #ifdef use_debug_serial
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
      #ifdef use_debug_serial
		SerialDEBUG.print("I2C device found at address 0x");
		#endif 
      if (address<16) 
		  {
			#ifdef use_debug_serial
			SerialDEBUG.print("0");
		  SerialDEBUG.print(address,HEX);
		  SerialDEBUG.println("  !");
		  #endif 
		}
      nDevices++;
    }
    else if (error==4) 
    {
      #ifdef use_debug_serial
		SerialDEBUG.print("Unknown error at address 0x");
		#endif 
      if (address<16) 
		  {	
			#ifdef use_debug_serial
				SerialDEBUG.print("0");
			  SerialDEBUG.println(address,HEX);
			  #endif 
		}
    }    
  }
  #ifdef use_debug_serial
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
		#ifdef use_debug_serial
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
    #ifdef use_debug_serial
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
	  #ifdef use_debug_serial
		SerialDEBUG.println("Reading Time RTC");
		#endif 
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
void setup() {

 // Set console baud rate
 Wire.begin();
 Wire.setClock(100000);
 
#ifdef use_debug_serial
	SerialDEBUG.begin(115200);
#endif 
delay(1000);
   
#if defined (hwv1)	
	SerialLMD.begin(9600, SWSERIAL_8N1, 26, 27, false, 256);
#else
	SerialLMD.begin(9600);
#endif
 

  // reserve 200 bytes for the inputString:
  // inputString.reserve(200);

 delay(10);
 pinMode(led_sign,OUTPUT);

 
 pinMode(pinReset,OUTPUT);
 pinMode(pinPower,OUTPUT);

	pinMode (encoder0Click, INPUT_PULLUP);
	pinMode (encoder1Click, INPUT_PULLUP);
	pinMode (encoder2Click, INPUT_PULLUP);
	pinMode (encoder3Click, INPUT_PULLUP);
	
	button0.pin_sw=encoder0Click;
	button1.pin_sw=encoder1Click;
	button2.pin_sw=encoder2Click;
	button3.pin_sw=encoder3Click;
	
	button0.buttonStateEncoder = digitalRead(encoder0Click);
	button0.lastButtonState=button0.buttonStateEncoder;
	
	button1.buttonStateEncoder = digitalRead(encoder1Click);
	button1.lastButtonState=button1.buttonStateEncoder;
	
	button2.buttonStateEncoder = digitalRead(encoder2Click);
	button2.lastButtonState=button2.buttonStateEncoder;
	
	button3.buttonStateEncoder = digitalRead(encoder3Click);
	button3.lastButtonState=button3.buttonStateEncoder;
 
 delay(100);
  digitalWrite(led_sign,HIGH);
  // Define your display layout here, e.g. 1/8 step, and optional SPI pins begin(row_pattern, CLK, MOSI, MISO, SS)

  #ifdef use_debug_serial
		SerialDEBUG.println("-----------------------------");
  SerialDEBUG.println("-----------------------------");
  SerialDEBUG.println("Led Matrix Display Information");
  SerialDEBUG.println("Data updater");
  SerialDEBUG.println(versionname);
  SerialDEBUG.println(hardwareversionname);
  SerialDEBUG.println("-----------------------------");
  SerialDEBUG.println("-----------------------------");
  #endif 
  	#ifdef debug_mode
		#ifdef use_debug_serial
		SerialDEBUG.println("WARNING IN DEBUG MODE-----------------------------");
		SerialDEBUG.println("Comment :debug_mode @file config_hw.h-----------------------------");
		#endif 
	#endif
  
  
  //wifi
	SerialLMD.print("WIFI 1\n");
	digitalWrite(led_sign,HIGH);
	delay(200);
	digitalWrite(led_sign,LOW);
	delay(200);	 
	SerialLMD.print("WIFI 2\n");
	digitalWrite(led_sign,HIGH);
	delay(200);
	digitalWrite(led_sign,LOW);
	delay(200);
	SerialLMD.print("WIFI 3\n");
	digitalWrite(led_sign,HIGH);
	delay(200);
	digitalWrite(led_sign,LOW);
	delay(200);

// testmessage();
	// testsend();
	
	// testsend_opu_generic();
	// while(1)
	// {
		// delay(1);
	// }
	
  //----------eeprom--------------------
	//------------------eeprom----------------------------------------	
if (!EEPROM.begin(EEPROM_SIZE))
  {
    delay(10);
    #ifdef use_debug_serial
		SerialDEBUG.println("failed to initialise EEPROM");
		#endif 

  }
	//innit SPIFFS
	
	if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
        #ifdef use_debug_serial
		SerialDEBUG.println("Card Mount Failed");
		#endif 
        return;
    }
	
	load_values();
	
	check_reset_factory();
	
	load_system_info();
	load_oph_generic();
	
	//lấy name từ opu_generic_system chuyển vào load_oph_media_http
	//save file
	//load_oph_media_http hàm này là gì mà phải đọc lúc khởi động
	char name[30];
	sprintf(name,"/F%d.lmd",opu_generic_system.mid);		
	load_oph_media_http(name);
	// load_save_value(filename[0]);	
  
		// for(int i=0;i<5;i++)
		// {
			// SerialDEBUG.printf("Test time @%d",i);
			// test_time();			
		// }

		//debug nen comment dong nay--------------------------------------------------------------------------------------------------------------------------------------------		
	// test_function();
//------------------Realtime Clock----------------------------------------	
	setup_realtimeclock();
	
	
	chopledreset();
	
	
	// SerialDEBUG.println("\n\n*****Test get_setting_parameters");
	// get_setting_parameters();
	// SerialDEBUG.println("\n\nEnd Test get");
	
//------------------wifi----------------------------------------	
	
	setupwifi(0);
	
//------------------4G modem----------------------------------------		

mqtt.mode_data_is_wifi=1;


if(!mqtt.mode_data_is_wifi)
{

// power_on_modem();
	#ifdef use_debug_serial
		SerialDEBUG.println("Wait to connect modem...");
		#endif 
  // Set GSM module baud rate
	connect_modem();
  
  
  // SerialAT.begin(9600);
  delay(6000);
  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  #ifdef use_debug_serial
		SerialDEBUG.println("Initializing modem...");
		#endif 
  // modem.restart();
    modem.init();
  
	  String modemInfo = modem.getModemInfo();
  #ifdef use_debug_serial
		SerialDEBUG.print("Modem Info: ");
  SerialDEBUG.println(modemInfo);
  #endif 
  
  #if TINY_GSM_USE_GPRS
  // Unlock your SIM card with a PIN if needed
  if ( GSM_PIN && modem.getSimStatus() != 3 ) {
    modem.simUnlock(GSM_PIN);
  }
#endif
  
  #if TINY_GSM_USE_WIFI
    // Wifi connection parameters must be set before waiting for the network
  #ifdef use_debug_serial
		SerialDEBUG.print(F("Setting SSID/password..."));
		#endif 
  if (!modem.networkConnect(wifiSSID, wifiPass)) {
    #ifdef use_debug_serial
		SerialDEBUG.println(" fail");
		#endif 
    delay(10000);
    return;
  }
  #ifdef use_debug_serial
		SerialDEBUG.println(" success");
		#endif 
#endif
  
  #if TINY_GSM_USE_GPRS && defined TINY_GSM_MODEM_XBEE
  // The XBee must run the gprsConnect function BEFORE waiting for network!
  modem.gprsConnect(apn, gprsUser, gprsPass);
#endif




  #ifdef use_debug_serial
		SerialDEBUG.print("Waiting for network...");
		#endif 
  if (!modem.waitForNetwork()) {
    #ifdef use_debug_serial
		SerialDEBUG.println(" fail");
		#endif 
    delay(10000);
    return;
  }
  #ifdef use_debug_serial
		SerialDEBUG.println(" success");
		#endif 

  if (modem.isNetworkConnected()) {
    #ifdef use_debug_serial
		SerialDEBUG.println("Network connected");
		#endif 
  }

#if TINY_GSM_USE_GPRS
  // GPRS connection parameters are usually set after network registration
    #ifdef use_debug_serial
		SerialDEBUG.print(F("Connecting to "));
    SerialDEBUG.print(apn);
	#endif 
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
      #ifdef use_debug_serial
		SerialDEBUG.println(" fail");
		#endif 
      delay(10000);
      return;
    }
    #ifdef use_debug_serial
		SerialDEBUG.println(" success");
		#endif 

  if (modem.isGprsConnected()) {
    #ifdef use_debug_serial
		SerialDEBUG.println("GPRS connected");
		#endif 
  }
#endif
  
  initializeSMS();
}	
	
	//kết nối server ở đây
	if(mqtt.mode_data_is_wifi)
	{
		#ifdef use_debug_serial
		SerialDEBUG.println("Connect server via wifi mode");
		SerialDEBUG.println("wifi begin");
		#endif 
		digitalWrite( led_sign, HIGH );//bat led sang
		

		uint16_t port_= conertchar2u16(mqtt.port);
		
		client.begin(mqtt.sever_mqtt,port_,net);	
		client.setHost(mqtt.sever_mqtt,port_);
		client.setKeepAlive(300);//5 minutes
		client.onMessage(messageReceived);
		
		connect();
	}
	else{
		#ifdef use_debug_serial
		SerialDEBUG.println("Connect server via 4G mode");
		#endif 
		digitalWrite( led_sign, HIGH );//bat led sang
		
		uint16_t port_= conertchar2u16(mqtt.port);
		
		clientGSM.begin(mqtt.sever_mqtt,port_,GSM1);			
		clientGSM.setHost(mqtt.sever_mqtt, port_);
		clientGSM.setKeepAlive(300);//5 minutes
		clientGSM.onMessage(messageReceived);
		
		mqttConnect();
	}
	//reset timer 
	ui32timeoutrx=millis() + df_time_disconnect;
}

void loop() {

//debug nen comment dong nay--------------------------------------------------------------------------------------------------------------------------------------------
if(mqtt.mode_data_is_wifi)
{
	client.loop();
	delay(10);  // <- fixes some issues with WiFi stability
	
	if (!client.connected()) {
		connect();
	}
	else {
		if(info_time_system.status==TIME_ERRO)
		{
			//pool-->RTC-->MQTT
			if(info_time_system.mode==BYMQTT)
			{
				updatetimebymqtt_func();				
			}
			else if(info_time_system.mode==BYRTC)
			{
				updatetimebyrtc_func();				
			}
			else
			{						
				updatetimebyPool_func();			
			}
		}
		if(u8_needreub_channle)
		{
			resubfunc();
			u8_needreub_channle=0;
		}
		send_opu_status();
		//		
	}
	//phải kiểm tra thêm time ở khu vực này
	//phòng trường hợp mất kết nối server là không đọc được thời gian
	
	
	//pool-->RTC-->MQTT
	
	if(info_time_system.status==TIME_ERRO)
	{		
		if(info_time_system.mode==BYMQTT)
		{
			updatetimebymqtt_func();				
		}
		else if(info_time_system.mode==BYRTC)
		{
			updatetimebyrtc_func();				
		}		
		else
		{						
			updatetimebyPool_func();
		}
	}	
}	
else
{
	clientGSM.loop();
	delay(10);  // <- fixes some issues with WiFi stability
	if (!clientGSM.connected()) {		
		
		// Reconnect every 10 seconds
		uint32_t temp_time123 = millis();
		if (temp_time123 > lastReconnectAttempt + 10000L) {
		  #ifdef use_debug_serial
		SerialDEBUG.println("=== MQTT 4G NOT CONNECTED ===");
		#endif 
		  lastReconnectAttempt = temp_time123;
		  if (mqttConnect()) {
			lastReconnectAttempt = 0;
		  }
		}
		delay(100);
		// return;
	}
	else {
		if(info_time_system.mode==BYMQTT)
		{
			updatetimebymqtt_func();
		}
		if(u8_needreub_channle)
		{
			resubfunc();
			u8_needreub_channle=0;
		}
		send_opu_status();
		test_getgetdatavia4G();
		//		
	}
	
	getTime1();
}


	if (index_storing || havedata)
	{
		// havedata=0;		
		
		int erro=getdata();
		
		if (!erro){
			// print_oph_media_buffer();
			//save file
			char name[30];
			//lưu thêm nội dung chứa msg lmd
			if(phatngaychotoi)
			{
				//xóa file, không cần lưu file loại này
				#ifdef use_debug_serial
				SerialDEBUG.println("Xoa file loai khan cap");
				#endif 
				sprintf(name,"/F%d.lmd",oph_media_buffer.mid);
				deleteFile(SPIFFS, name);
			}
			else
			{					
				#ifdef use_debug_serial
				SerialDEBUG.println("file loai khac");
				#endif
				
				
				#ifdef use_debug_serial
				SerialDEBUG.println("\n\r------------Truoc update");
				print_oph_media_buffer();				
				#endif
				
				sprintf(name,"/F%d.lmd",oph_media_buffer.mid);		
				spiff_updatemediafile(name, &oph_media_buffer,1);	
				
				#ifdef use_debug_serial
				SerialDEBUG.println("\n\r------------Sau update");
				print_oph_media_buffer();				
				#endif
			}
						
			opu_generic_system.mid=oph_media_buffer.mid;
			spiff_updatefile(oph_generic_filename, &opu_generic_system);
			
			if(phatngaychotoi)
			{
				phatngaychotoi=0;
				#ifdef use_debug_serial
					SerialDEBUG.println("Phat ngay cho toi");
				#endif 
				//chen vao playlist
								
				int tim=insert_playlist();
				
				if(tim==0)
				{
					index_send++;
					send_message2LMD(0);															
					
					//gettime
					struct tm tmstruct ;		
					tmstruct.tm_year = 0;
					getLocalTime(&tmstruct, 5000);
					uint32_t tsssss=(tmstruct.tm_hour)*3600+(tmstruct.tm_min)*60+(tmstruct.tm_sec);
					
					media_manager[0].ts=tsssss;
					media_manager[0].playing=PLAYER_PLAYING;
				}								
			}
			else{		
				//refresh playlist
				read_playlist=1;
				need_reset_playlist=1;
			}
		}
		// else{
			// SerialDEBUG.println("Erro: get data again");
		// }
	}
	
	check_system_time();
	playtext();
	if(read_playlist==1 && index_storing ==0 && havedata==0)
	{
		//xử lý thêm nội dung hiển thị
		//
		if(need_reset_playlist)
		{
			reload_playlist();
			need_reset_playlist=0;
		}
		get_content();
		read_playlist=0;
	}
	// print_time();
	update2lmd();
	
	#ifdef debug_mode
	read_serial();
	#endif
	heart_beat();
	check_button();
	send_command_screen();
	read_serial_LMD();
	// check_connection();
		
	//chức năng update tự động
	// if(check_firmware>df_time_upgrade_minute)checkfirmware();
}
