/*
 Copyright (C) 2020 Mr. Cong <thanhcong402@gmail.com>
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */
 
/**
 * @file esp_remote_buttonfile.h
 *	
 *	
 */

#ifndef __ESP_PROCESS_FILE_DEFINE__
#define __ESP_PROCESS_FILE_DEFINE__

#include <stdio.h>
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
// #include <ESP8266WiFi.h>
#include <FS.h>
#include <ArduinoJson.h>
	
	
// #define debug_print_read_

//-----------------Define, variables-----------------------


// const char *sys_filename="/system.json"; tương ứng server tại công ty, json1["Server"] = "vernemq.altacloud.biz"; 

// const char *sys_filename="/system12.json";//Hà Nội đang là 12 cho server mbs.mobifone.vn

const char *sys_filename=df_filename;//system8



const char *oph_generic_filename="/sysgnric.json";
const char *oph_media_filename="/sysmedia.json";


const char *outname[8]={"Output1","Output2","Output3","Output4","Output5","Output6","Output7","Output8"};
const char *filename[]={"/button1.json","/button2.json","/button3.json","/button4.json","/button5.json","/button6.json","/button7.json","/button8.json","/input1.json","/input2.json","/input3.json","/input4.json","/input5.json","/input6.json","/input7.json","/input8.json"};

typedef struct
{
    //set
	uint8_t u8pinout_config;      /*!< digital pin*/
    uint8_t u8pinfct_config;      /*!< Pin Function code, effect*/
    uint16_t u16timeon_config;    /*!< Address of the first register to access at slave/s */
    uint16_t u16timeoff_config;   /*!< Number of coils or registers to access */
    uint8_t u8loop_config;     	/*!< Pointer to memory image in master */
	//running
	// uint8_t u8looprunning;
	// uint8_t u8periodrunning;	/*!< doing period */
	// uint32_t u32timerunning;
}
buttonconfig_t;

void deleteFile(fs::FS &fs, const char * path);

long spiff_checkfile(String file) {
	
	#ifdef use_debug_serial
	SerialDEBUG.println("spiff_checkfile");
	#endif 
	//Init the Spiff FS (check if our INIT file exists or not
	File f = SPIFFS.open("init", "r+");

	if (!f) {
		// SerialDEBUG.println("Formatting Spiff...");
		// delay(200);
		//Format
		// SPIFFS.format();

		//Make a Init File
		File f1 = SPIFFS.open("init", "w+");
		f1.println("init");
		f1.close();

		#ifdef use_debug_serial
	SerialDEBUG.println("Done");
	#endif 
		return(1);
		// delay(200);
	}
	return(2);
}


//#####################################################
 //read file and store into *value
//#####################################################
void load_default(String file, uint16_t *regs){
	//load default value
	//{"Output1":[1,2,500,1000,2],"Output2":[2,3,5000,6000,3]}
	DynamicJsonDocument json1(1024);
			
	JsonArray outputValues = json1.createNestedArray(outname[0]);
	int t=regs[NET_OPENNAME];
	if(t>7)t-=8;
	for(int i=0;i<8;i++){
		JsonArray outputValues = json1.createNestedArray(outname[i]);
		if(t==i){
			outputValues.add(i);			
			outputValues.add(1000);
			outputValues.add(1000);			
			
			regs[i*3+0]=i;			
			regs[i*3+1]=1000;
			regs[i*3+2]=1000;			
		}
		else{		
			outputValues.add(0);
			outputValues.add(0);
			outputValues.add(0);											
		}
	}				
	
	//create file
	File f2 = SPIFFS.open(file,  "w");		
	#ifdef use_debug_serial
	serializeJson(json1,SerialDEBUG);
	#endif 
	serializeJson(json1,f2);
	f2.close();	
}

int spiff_readoldfilesystem(String file, mqtt_net_info_t *mqtt_) {
 
	//Reading file
    File f = SPIFFS.open(file,  "r");  //r+

  int s = 0;
  int kq=0;
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
		
		//tồn tại file .json
		
		strlcpy(mqtt_->wifi_name,                  // <- destination
			jsonBuffer["Wname"]| df_wifi_name,  // <- source
			sizeof(mqtt_->wifi_name));         // <- destination's capacity


		#ifdef use_debug_serial
	SerialDEBUG.print(F("[FILE OLD] Wname="));
		SerialDEBUG.println(String(mqtt_->wifi_name));
		#endif 

		strlcpy(mqtt_->wifi_pass,                  // <- destination
			jsonBuffer["Wpass"]| df_wifi_pass,  // <- source
			sizeof(mqtt_->wifi_pass));         // <- destination's capacity
		#ifdef use_debug_serial
	SerialDEBUG.print(F("[FILE OLD] wifi_pass="));
		SerialDEBUG.println(String(mqtt_->wifi_pass));	
		#endif 

		// mqtt_-> port= jsonBuffer["Port"] | 1883;

		strlcpy(mqtt_->port,                  // <- destination
			jsonBuffer["Port"]| df_port,  // <- source
			sizeof(mqtt_->port));         // <- destination's capacity
		#ifdef use_debug_serial	
		SerialDEBUG.print(F("[FILE OLD] port="));
		SerialDEBUG.println(String(mqtt_->port));
		#endif 
		
		strlcpy(mqtt_->sever_mqtt,                  // <- destination
			jsonBuffer["Server"]| df_server,  // <- source
			sizeof(mqtt_->sever_mqtt));         // <- destination's capacity
		#ifdef use_debug_serial
	SerialDEBUG.print(F("[FILE OLD] Server="));
		SerialDEBUG.println(String(mqtt_->sever_mqtt));
		#endif 
		
		strlcpy(mqtt_->client_id,                  // <- destination
			jsonBuffer["ID"] | "df_id_mqtt",  // <- source
			sizeof(mqtt_->client_id));         // <- destination's capacity
		#ifdef use_debug_serial
	SerialDEBUG.print(F("[FILE OLD] client_id="));
		SerialDEBUG.println(String(mqtt_->client_id));
		#endif 
		
		strlcpy(mqtt_->username,                  // <- destination
			jsonBuffer["User"] | "df_user_mqtt",  // <- source
			sizeof(mqtt_->username));         // <- destination's capacity
		#ifdef use_debug_serial
	SerialDEBUG.print(F("[FILE OLD] username="));
		SerialDEBUG.println(String(mqtt_->username));
		#endif 
		
		strlcpy(mqtt_->pwdMqtt,                  // <- destination
			jsonBuffer["Pass"] | "df_pass_mqtt",  // <- source
			sizeof(mqtt_->pwdMqtt));         // <- destination's capacity
		#ifdef use_debug_serial
	SerialDEBUG.print(F("[FILE OLD] pwdMqtt="));
		SerialDEBUG.println(String(mqtt_->pwdMqtt));
		#endif 
		
		strlcpy(mqtt_->device_code,                  // <- destination
			jsonBuffer["Device_code"] | "12345678",  // <- source
			sizeof(mqtt_->device_code));         // <- destination's capacity
		#ifdef use_debug_serial
	SerialDEBUG.print(F("[FILE OLD] device_code="));
		SerialDEBUG.println(String(mqtt_->device_code));
		#endif 
		
		 mqtt_-> mode_data_is_wifi= jsonBuffer["Data_mode"];//mặc định bằng 1 là wifi
		#ifdef use_debug_serial
		SerialDEBUG.print(F("[FILE OLD] mode_data_is_wifi="));
		SerialDEBUG.println(String(mqtt_->mode_data_is_wifi));
		#endif 
		
		if(mqtt_->client_id=="")
		{
			#ifdef use_debug_serial
	SerialDEBUG.println(F("\n\r[FILE SYS]:not find ID"));
	#endif 
		}
		else
		{
			kq=1;
		}
		// SerialDEBUG.println(F("\n\r[FILE SYS]:End parsed json"));
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

  } else {
    #ifdef use_debug_serial
	SerialDEBUG.print(F("[FILE SYS]:Failed to open file for reading "));
    SerialDEBUG.println(file);
	#endif 
  }
  return kq;
}

/**
 * @enum get_olddata_system
 * @brief
 * lấy lại thông tin đã lưu trước đó
 * Tìm ts nhỏ nhất tiếp theo old_ts
 */
 
int get_olddata_system(mqtt_net_info_t *mqtt_){
    int index_=0;

	#ifdef use_debug_serial
	SerialDEBUG.printf("[*FILE]: get_olddata_system\n");
	#endif 
		
	File root = SPIFFS.open("/");
    if(!root){
        #ifdef use_debug_serial
	SerialDEBUG.println("[*FILE]: Failed to open directory");
	#endif 
        return 0;
    }
    if(!root.isDirectory()){
        #ifdef use_debug_serial
	SerialDEBUG.println("[*FILE]: Not a directory");
	#endif 
        return 0;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            //bỏ qua không xử lý folder
        } else {
            #ifdef use_debug_serial
	SerialDEBUG.println("");
			SerialDEBUG.print("[*FILE]: NAME: ");
            SerialDEBUG.print(file.name());
            SerialDEBUG.print("  SIZE: ");            			
			SerialDEBUG.println(file.size());
	#endif 				
			  String s =String(file.name());
			  if (s.endsWith(".json")) {
				//kiểm tra lại tên file
				if(s!=oph_generic_filename && s!=oph_media_filename)
				{
					#ifdef use_debug_serial
	SerialDEBUG.println("[*FILE]: maybe old system file-->get");
	#endif 
					//đọc data cũ
					int i=spiff_readoldfilesystem(file.name(),mqtt_);
					if(i==1)
					{
						#ifdef use_debug_serial
	SerialDEBUG.println("[*FILE]: Find old data");
	#endif 
						return 1;
					}
				}			  			  			  
			  }				
        }
        file = root.openNextFile();
    }
	return 0;
}

void load_systemdefault(String file, mqtt_net_info_t *mqtt_){
	//load default value
	//đọc file cũ
	//nếu không có thì lấy data default
	int i=get_olddata_system(mqtt_);
	
	DynamicJsonDocument jsonBuffer(1024);
	
	if(i==1)
	{
		//chưa data cũ
		//tạo file, lưu data cũ lại và thoát.
		jsonBuffer["Wname"] =  mqtt_->wifi_name;
		jsonBuffer["Wpass"] = mqtt_->wifi_pass;
		jsonBuffer["Port"] = mqtt_->port;
		jsonBuffer["Server"] = mqtt_->sever_mqtt;
		jsonBuffer["ID"] = mqtt_->client_id;
		jsonBuffer["User"] = mqtt_->username;
		jsonBuffer["Pass"] = mqtt_->pwdMqtt;
		jsonBuffer["Device_code"] = mqtt_->device_code;
		jsonBuffer["Data_mode"] = mqtt_->mode_data_is_wifi;
	}
	else
	{
		//load default
		char wifiname_id[20];
		snprintf(wifiname_id, 20, "%lld", ESP.getEfuseMac());
		
		
		jsonBuffer["Wname"] = df_wifi_name;
		jsonBuffer["Wpass"] = df_wifi_pass;

		jsonBuffer["Port"] = df_port;
		jsonBuffer["Server"] = df_server;
		
		// jsonBuffer["ID"] = df_id_mqtt;
		// jsonBuffer["User"] = df_user_mqtt;
		// jsonBuffer["Pass"] = df_pass_mqtt;
		// jsonBuffer["Device_code"] = df_device_code;
		
		
		jsonBuffer["ID"] = String(wifiname_id);
		jsonBuffer["User"] = "user" + String(wifiname_id);
		jsonBuffer["Pass"] = "pass" + String(wifiname_id);
		jsonBuffer["Device_code"] = String(wifiname_id);
		
		
		jsonBuffer["Data_mode"] = df_data_mode;
	}
	
			//create file
		File f2 = SPIFFS.open(file,  FILE_WRITE);

		if (!f2) {
				#ifdef use_debug_serial
	SerialDEBUG.println("failed to open config file for writing");
	#endif 
			}		
		#ifdef use_debug_serial
	SerialDEBUG.print("[*FILE]: creat data=");
	
		
		serializeJson(jsonBuffer,SerialDEBUG);
		#endif 
		serializeJson(jsonBuffer,f2);
		f2.close();	
}

void load_oph_media_default(String file, mqtt_OPH_MEDIA_HTTP *mqtt_){

	StaticJsonDocument<1024> json1;
		

	json1["mid"] = 0;
	json1["prio"] = 0;
	json1["created"] = 0;
	json1["expired"] = 0;
	json1["mode"] = 0;
	json1["start"] = 0;
	
	// Create the "ts" array
	JsonArray gr = json1.createNestedArray("ts");
	gr.add(0);
	
	json1["repeat"] = 0;
	json1["days"] = 0;
	json1["url"] = "https://alta-s3.dev-altamedia.com/lalova/chaomung.txt";
	json1["auth"] = "daiKCong";
	
	json1["loop"] = 0;
	json1["duration"] = 0;
	// Create the "ds" array
	JsonArray dsgr = json1.createNestedArray("ds");
	dsgr.add(0);
	json1["lmd_msg"] = "Hello";

	//create file
	File f2 = SPIFFS.open(file,  "w");		
	#ifdef use_debug_serial
	serializeJson(json1,SerialDEBUG);
	#endif 
	serializeJson(json1,f2);
	f2.close();	
}
void load_oph_generic_default(String file, mqtt_OPU_GENERIC *mqtt_){

	StaticJsonDocument<1024> json1;
		

	json1["fvers"] = versionname;
	json1["vcode"] = rand();
	
	// Create the "group" array
	JsonArray gr = json1.createNestedArray("group");
	gr.add(0);
	
	json1["disabled"] = 0;
	json1["mid"] = 0;
	json1["txtype"] = 0;
	json1["ip"] = "192.168.1.100";
	json1["phone"] = "0909090909";	

	//create file
	File f2 = SPIFFS.open(file,  "w");		
	// serializeJson(json1,SerialDEBUG);
	serializeJson(json1,f2);
	f2.close();	
}
long spiff_readfilesystem(String file, mqtt_net_info_t *mqtt_) {
	//If Reading, Check file  
    if (!SPIFFS.exists(file)) {
		#ifdef use_debug_serial
	SerialDEBUG.println(F("[FILE SYS]:File not exists"));
		SerialDEBUG.println(F("[FILE SYS]:Need to create new file 310"));
		#endif 
		load_systemdefault(file,mqtt_);			
		#ifdef use_debug_serial
	SerialDEBUG.println(F("\n[FILE SYS]:Done"));		
	#endif 
    }

	//Reading file
    File f = SPIFFS.open(file,  "r");  //r+

  int s = 0;
//if File Exists
  if (f) {
    s = f.size();
	// SerialDEBUG.println(F("[*FILE]: Read file"));  
	size_t size = f.size();
	// Allocate a buffer to store contents of the file.
	std::unique_ptr<char[]> buf(new char[size]);
	f.readBytes(buf.get(), size);
		
	DynamicJsonDocument  jsonBuffer(1024);
	deserializeJson(jsonBuffer,buf.get());
	
	#ifdef use_debug_serial
	SerialDEBUG.print(F("[*FILE]: \t"));  
	serializeJson(jsonBuffer,SerialDEBUG);  
	SerialDEBUG.print(F("\n")); 
	#endif 
			
	if (!jsonBuffer.isNull()) {

//wifi ngoài Hà Nội

		strlcpy(mqtt_->wifi_name,                  // <- destination
			jsonBuffer["Wname"] | df_wifi_name,  // <- source
			sizeof(mqtt_->wifi_name));         // <- destination's capacity

		strlcpy(mqtt_->wifi_pass,                  // <- destination
			jsonBuffer["Wpass"] | df_wifi_pass,  // <- source
			sizeof(mqtt_->wifi_pass));         // <- destination's capacity
	

		// mqtt_-> port= jsonBuffer["Port"] | 1883;

		strlcpy(mqtt_->port,                  // <- destination
			jsonBuffer["Port"] | df_port,  // <- source
			sizeof(mqtt_->port));         // <- destination's capacity
			
		strlcpy(mqtt_->sever_mqtt,                  // <- destination
			jsonBuffer["Server"] | df_server,  // <- source
			sizeof(mqtt_->sever_mqtt));         // <- destination's capacity
			
		strlcpy(mqtt_->client_id,                  // <- destination
			jsonBuffer["ID"] | "df_id_mqtt",  // <- source
			sizeof(mqtt_->client_id));         // <- destination's capacity

		strlcpy(mqtt_->username,                  // <- destination
			jsonBuffer["User"] | "df_user_mqtt",  // <- source
			sizeof(mqtt_->username));         // <- destination's capacity

		strlcpy(mqtt_->pwdMqtt,                  // <- destination
			jsonBuffer["Pass"] | "df_pass_mqtt",  // <- source
			sizeof(mqtt_->pwdMqtt));         // <- destination's capacity
		
		strlcpy(mqtt_->device_code,                  // <- destination
			jsonBuffer["Device_code"] | "12345678",  // <- source
			sizeof(mqtt_->device_code));         // <- destination's capacity
			
		 mqtt_-> mode_data_is_wifi= jsonBuffer["Data_mode"] | df_data_mode;//mặc định bằng 1 là wifi		 	 		

		f.close();
	} else {
		#ifdef use_debug_serial
	SerialDEBUG.println(F("[FILE]: \tFile incorrect format"));		
		SerialDEBUG.println(F("[FILE]: \tLoad default"));
		#endif 
		f.close();
		load_systemdefault(file,mqtt_);		
		#ifdef use_debug_serial
	SerialDEBUG.println(F("\n\r[FILE]: \tDone"));
	#endif 
	}  

    //Return the size of file
    return (s);
	
  } else {
    #ifdef use_debug_serial
	SerialDEBUG.print(F("[FILE]: \tUnable to open file: "));
    SerialDEBUG.println(file);
	#endif 
  }
  return (1);
}
long spiff_readfile(String file, uint16_t *regs) {
	//If Reading, Check file  
    if (!SPIFFS.exists(file)) {
		#ifdef use_debug_serial
	SerialDEBUG.println(F("[FILE]:File not exists"));
		SerialDEBUG.println(F("[FILE]:Need to create new file 218"));
		#endif 
		load_default(file,regs);			
		#ifdef use_debug_serial
	SerialDEBUG.println(F("\n[FILE]:Done"));		
	#endif 
    }

	//Reading file
    File f = SPIFFS.open(file,  "r");  //r+

  int s = 0;
//if File Exists
  if (f) {
    s = f.size();

    // SerialDEBUG.printf("File Opened , Size=%d\r\n", s);
	//Read    
      // *value = f.readString();
	#ifdef use_debug_serial
	SerialDEBUG.println(F("[FILE]:Read file"));  
	#endif 
	size_t size = f.size();
	// Allocate a buffer to store contents of the file.
	std::unique_ptr<char[]> buf(new char[size]);
	f.readBytes(buf.get(), size);
		
	DynamicJsonDocument  jsonBuffer(1024);
	deserializeJson(jsonBuffer,buf.get());
	#ifdef use_debug_serial
	SerialDEBUG.println(F("[FILE]:**jsonBuffer:"));
	serializeJson(jsonBuffer,SerialDEBUG);  
	SerialDEBUG.println(F("[FILE]:#"));  
	#endif 
		
	if (!jsonBuffer.isNull()) {
		//{"Output1":[1,2,500,1000,2],"Output2":[2,3,5000,6000,3]}
		// SerialDEBUG.println("Output 1: ");
		double t ;						
		for(int i=0;i<8;i++){
			#ifdef use_debug_serial
	
			SerialDEBUG.print("Output ");
			SerialDEBUG.println(i+1);
			#endif
			for(int m=0;m<3;m++){												
				t = jsonBuffer[outname[i]][m];
				regs[i*3+m]=(uint16_t)t;
				#ifdef use_debug_serial
	
				SerialDEBUG.print("[");SerialDEBUG.print(m);SerialDEBUG.print("]=");SerialDEBUG.println(t);				
				#endif
			}									
		}			
		
		#ifdef use_debug_serial
	SerialDEBUG.println(F("\n\r[FILE]:End parsed json"));
	#endif 
		f.close();
	} else {
		#ifdef use_debug_serial
	SerialDEBUG.println(F("[FILE]:File incorrect format"));		
		SerialDEBUG.println(F("[FILE]:Load default"));
		#endif 
		f.close();
		load_default(file,regs);		
		#ifdef use_debug_serial
	SerialDEBUG.println(F("\n\r[FILE]:Done"));
	#endif 
	}  

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
//#####################################################
 //write value  into file
//#####################################################
long spiff_writefile(String file, uint16_t *regs) {
	//Open file for write
    File f = SPIFFS.open(file,  "w");
  

  int s = 0;
//if File Exists
  if (f) {
    s = f.size();
	
	DynamicJsonDocument json1(1024);					
	
	for(int i=0;i<8;i++){
		JsonArray outputValues = json1.createNestedArray(outname[i]);
		outputValues.add(regs[i*3+0]);
		outputValues.add(regs[i*3+1]);
		outputValues.add(regs[i*3+2]);		
	}
	//create file		
	#ifdef use_debug_serial
	serializeJson(json1,SerialDEBUG);
	#endif 
	serializeJson(json1,f);
	f.close();		
	#ifdef use_debug_serial
	SerialDEBUG.println(F("\n\r[FILE]:Done"));		
	#endif 
	
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
		regs-> prio= jsonBuffer["prio"];
		regs-> created= jsonBuffer["created"];
		regs-> expired= jsonBuffer["expired"];
		regs-> mode= jsonBuffer["mode"];
		regs-> start= jsonBuffer["start"];
		

		uint16_t u32_index_ =  jsonBuffer["ts"].size();
		
		if(u32_index_>24)u32_index_=24;
		
		for(int i=0; i<u32_index_;i++)
		{
			regs->ts[i]=jsonBuffer["ts"][i];			
		}
		
		u32_index_ =  jsonBuffer["ds"].size();
		if(u32_index_>24)u32_index_=24;
		for(int i=0; i<u32_index_;i++)
		{
			regs->ds[i]=jsonBuffer["ds"][i];			
		}
		regs-> repeat= jsonBuffer["repeat"];
		regs-> days= jsonBuffer["days"];
		
		/*
		strlcpy(regs->url,                  // <- destination
			jsonBuffer["url"] | "https://alta-s3.dev-altamedia.com/lalova/chaomung.txt",  // <- source
			sizeof(regs->url));         // <- destination's capacity
		*/
		regs-> url= jsonBuffer["url"].as<String>();
		regs-> auth= jsonBuffer["auth"].as<String>();
		
		// strlcpy(regs->auth,                  // <- destination
			// jsonBuffer["auth"] | "daiKCong",  // <- source
			// sizeof(regs->auth));         // <- destination's capacity
		
		
		regs-> looop= jsonBuffer["loop"];
		regs-> duration= jsonBuffer["duration"];
		
		regs-> lmd_msg= jsonBuffer["lmd_msg"].as<String>();
		
		// SerialDEBUG.println(F("\n\r[FILE SYS]:End parsed json"));
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

long spiff_readfileoph(String file, mqtt_OPU_GENERIC *regs) {
	
		//If Reading, Check file  
    if (!SPIFFS.exists(file)) {
		#ifdef use_debug_serial
	SerialDEBUG.println(F("[*FILE]: File not exists"));
		SerialDEBUG.println(F("[*FILE]: Need to create new file 310"));
		#endif 
		load_oph_generic_default(file,regs);	
		#ifdef use_debug_serial
	SerialDEBUG.println(F("\n[*FILE]: Done"));		
	#endif 
    }

	//Reading file
    File f = SPIFFS.open(file,  "r");  //r+

  int s = 0;
//if File Exists
  if (f) {
    s = f.size();

	// SerialDEBUG.println(F("[*FILE]: Read file"));  
	size_t size = f.size();
	// Allocate a buffer to store contents of the file.
	std::unique_ptr<char[]> buf(new char[size]);
	f.readBytes(buf.get(), size);
		
	DynamicJsonDocument  jsonBuffer(1024);
	deserializeJson(jsonBuffer,buf.get());
	// SerialDEBUG.print(F("[*FILE]: \t"));
	// serializeJson(jsonBuffer,SerialDEBUG);  	
	// SerialDEBUG.print(F("\n"));		
	if (!jsonBuffer.isNull()) {

		strlcpy(regs->fvers,                  // <- destination
			jsonBuffer["fvers"] | "V1.027112021",  // <- source
			sizeof(regs->fvers));         // <- destination's capacity
		
		regs-> vcode= jsonBuffer["vcode"];
		
		
		for(int i=0; i<20;i++)
		{
			regs->group[i]=jsonBuffer["group"][i];
			if(regs->group[i]==0)break;
		}
		
		regs-> disabled= jsonBuffer["disabled"];
		regs-> mid= jsonBuffer["mid"];
		regs-> txtype= jsonBuffer["txtype"];
		
		strlcpy(regs->ip,                  // <- destination
			jsonBuffer["ip"] | "192.168.1.100",  // <- source
			sizeof(regs->ip));         // <- destination's capacity
			
		strlcpy(regs->phone,                  // <- destination
			jsonBuffer["phone"] | "0909090909",  // <- source
			sizeof(regs->phone));         // <- destination's capacity			
		f.close();
	} else {
		#ifdef use_debug_serial
	SerialDEBUG.println(F("[*FILE]: File incorrect format"));		
		SerialDEBUG.println(F("[*FILE]: Load default"));
		#endif 
		f.close();
		load_oph_generic_default(file,regs);		
		#ifdef use_debug_serial
	SerialDEBUG.println(F("\n\r[*FILE]: Done"));
	#endif 
	}  

    //Return the size of file
    return (s);
	
  } else {
    #ifdef use_debug_serial
	SerialDEBUG.print(F("[*FILE]: Unable to open file "));
    SerialDEBUG.println(file);
	#endif 
  }
  return (1);
}



//#####################################################
 //update value  into exist file
//#####################################################
long spiff_updatefile(String file, mqtt_OPU_GENERIC *regs) {
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
	doc["fvers"]=versionname;
	doc["vcode"]=regs->vcode;
	
	// Create the "group" array
	JsonArray gr = doc.createNestedArray("group");
	for (int pin = 0; pin < 20; pin++) {    
		// Add the value at the end of the array		
		gr.add(regs->group[pin]);
	}
		
	doc["disabled"]=regs->disabled;
	doc["mid"]=regs->mid;
	doc["txtype"]=regs->txtype;
	doc["ip"]=String(regs->ip);
	doc["phone"]=String(regs->phone);
		
	//create file		
	// Generate the minified JSON and send it to the SerialDEBUG port.
	#ifdef use_debug_serial
	serializeJson(doc,SerialDEBUG);
	#endif 
	//create file
	serializeJson(doc,f);
	f.close();		
	#ifdef use_debug_serial
	SerialDEBUG.println(F("\n\r[FILE]:spiff_updatefile Done"));		
	#endif 
	
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
 /*
 #####################################################
 spiff_updatemediafile
 
 //update value  into exist file
 
 ms: 	=1: update message
		=0: update url

#####################################################
*/
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
	doc["prio"]=regs->prio;
	doc["created"]=regs->created;
	doc["expired"]=regs->expired;
	doc["mode"]=regs->mode;
	doc["start"]=regs->start;
	
	// Create the "ts" array
	JsonArray gr = doc.createNestedArray("ts");
	for (int pin = 0; pin < 24; pin++) {    
		// Add the value at the end of the array		
		gr.add(regs->ts[pin]);
	}

	doc["repeat"]=regs->repeat;
	doc["days"]=regs->days;
	if(ms==1)
	{
		doc["url"]="";
	}
	else
	{
		doc["url"]=String(regs->url);	
	}
	doc["auth"]=String(regs->auth);	
	doc["loop"]=regs->looop;
	doc["duration"]=regs->duration;
	
	#ifdef use_debug_serial
	SerialDEBUG.println(F("[FILE]:lmd_msg= "));
    SerialDEBUG.println(regs->lmd_msg);
	SerialDEBUG.println(F("[FILE]:end lmd_msg= "));
	#endif
	
	if(ms==1)
	{
		doc["lmd_msg"]=String(regs->lmd_msg);
	}
	else
	{
		doc["lmd_msg"]="";
	}
	
	
	// Create the "ds" array
	JsonArray dsgr = doc.createNestedArray("ds");
	for (int pin = 0; pin < 24; pin++) {    
		// Add the value at the end of the array		
		dsgr.add(regs->ds[pin]);
	}
	
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

/**
 * @enum getmediaplay
 * @brief
 * Lấy thông tin hiển thị 
 * Tìm ts nhỏ nhất tiếp theo old_ts
 */
 
void getmediaplay(fs::FS &fs, const char * dirname, uint8_t levels, str_media_manager *box,uint32_t ds_offset,size_t *me_,uint32_t old_ts){
    int index_=0;
	ds_offset=0;//tạm thời chưa sử dụng tới
	//khởi tạo giá trị
	box->ts=0;
	box->prio=0;
	box->playing=PLAYER_STOP;
	
	// SerialDEBUG.printf("[*FILE]: Listing directory: %s\n", dirname);
		
	File root = fs.open(dirname);
    if(!root){
        #ifdef use_debug_serial
	SerialDEBUG.println("[*FILE]: Failed to open directory");
	#endif 
        return;
    }
    if(!root.isDirectory()){
        #ifdef use_debug_serial
	SerialDEBUG.println("[*FILE]: Not a directory");
	#endif 
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            // SerialDEBUG.print("[*FILE]:  DIR : ");
            // SerialDEBUG.print (file.name());
            time_t t= file.getLastWrite();
            struct tm * tmstruct = localtime(&t);
            // SerialDEBUG.printf("[*FILE]:  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct->tm_year)+1900,( tmstruct->tm_mon)+1, tmstruct->tm_mday,tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
            //bỏ qua không xử lý folder
        } else {
            #ifdef use_debug_serial
			SerialDEBUG.println("");
			SerialDEBUG.print("[*FILE]: NAME: ");
            SerialDEBUG.print(file.name());
            SerialDEBUG.print("  SIZE: ");            			
			SerialDEBUG.println(file.size());
			#endif 
            *me_=*me_+file.size();
			
			struct tm tmstruct ;		
			tmstruct.tm_year = 0;
			getLocalTime(&tmstruct, 5000);
			time_t  timenow_ = mktime(&tmstruct);
		
			  String s =String(file.name());
			  if (s.endsWith(".lmd")) {
				// SerialDEBUG.println("[*FILE]:\tRead file:");
				//readfile
				mqtt_OPH_MEDIA_HTTP mediatemp;
				spiff_readfilemedia(file.name(),&mediatemp);
				//check mode
				if(mediatemp.mode==0)
				{
					#ifdef use_debug_serial
					SerialDEBUG.println("[*FILE]: \tno play");
					#endif 
				}
				else
				{
					// SerialDEBUG.println("[*FILE]: \tCheck expired");
					if(mediatemp.expired<timenow_)
					{
						#ifdef use_debug_serial
						SerialDEBUG.println("[*FILE]: \texpired-->delete");
						#endif 
						//delete file
						deleteFile(fs,file.name());			
					}
					else
					{
						#ifdef use_debug_serial
						SerialDEBUG.println("[*FILE]: \tCheck start now = " + String(timenow_) + ", ss= " + String(mediatemp.start));						
						#endif
												
						if(mediatemp.start>timenow_)
						{
							#ifdef use_debug_serial
							SerialDEBUG.println("[*FILE]: \ttoo soon");							
							#endif 
						}
						else
						{

							int u;
							uint32_t daybit=mediatemp.days;
							
							// SerialDEBUG.printf("[*FILE]: \tDays = %d", daybit);
							if(mediatemp.repeat==0)
							{
								// SerialDEBUG.println("[*FILE]: \trepeat: theo tuan");
								//kiem tra ngay lap lai
								//lay ngay hien tai
								//bit 0 1 2...6
								//    T2 3... CN
								//u=0 CN
								//u=1 T2...
								u=(tmstruct.tm_wday);
								if(u==0)u=6;
								else u--;
								// SerialDEBUG.printf("[*FILE]: \tbit @%d\n",u);							
							}
							else
							{
								// SerialDEBUG.println("[*FILE]: \trepeat: theo thang");
								u=(tmstruct.tm_mday);
								// SerialDEBUG.printf("[*FILE]: \tbit @%d\n",u);
							}
							
							uint32_t tt=0x0001;
							tt  = tt<<u;
							// SerialDEBUG.printf("[*FILE]: \t tt = %d\n",tt);
							uint32_t dayss=daybit & tt;
							
							if(dayss)
							{
								// SerialDEBUG.println("[*FILE]: \tOn day***");
								//get ts
								uint32_t tsssss=(tmstruct.tm_hour)*3600+(tmstruct.tm_min)*60+(tmstruct.tm_sec);
								// SerialDEBUG.printf("[*FILE]: \t ts = %d second, ",tsssss);				
								// SerialDEBUG.printf("offset = %d second, ",ds_offset);				
								
								tsssss = tsssss + ds_offset;
								#ifdef use_debug_serial
								SerialDEBUG.printf(">>@time = %d second\n",tsssss);				
								#endif 
																
								for(int i=0;i<24;i++)
								{
									if((mediatemp.ts[i]> old_ts && tsssss>mediatemp.ts[i]) && tsssss <= (mediatemp.ts[i]+mediatemp.ds[i]))
									{
										#ifdef use_debug_serial
										SerialDEBUG.printf("[*FILE]: \tIntime\n");
										#endif 
										//đang trong thời gian phát
										//lấy phát liền										
										//so sánh mức độ ưu tiên
										
										if(mediatemp.lmd_msg =="")
										{
											#ifdef use_debug_serial
											SerialDEBUG.printf("[*FILE]: \tEmpty text\n");
											#endif 
										}
										else if(box->playing !=PLAYER_INTIME)
										{
											#ifdef use_debug_serial
											SerialDEBUG.printf("[*FILE]: \tupdate intime at first ts [%d]=%d and mid: %d\n",i,mediatemp.ts[i],mediatemp.mid);
											#endif 
											box->mid=mediatemp.mid;
											box->ts=mediatemp.ts[i];
											box->ds=mediatemp.ds[i];
											box->looop=mediatemp.looop;
											box->index=index_;
											box->mode=mediatemp.mode;
											box->playing=PLAYER_INTIME;
											box->prio=mediatemp.prio;
											box->lmd_msg=mediatemp.lmd_msg;
										}
										else if(mediatemp.prio >= box->prio)
										{
											#ifdef use_debug_serial
											SerialDEBUG.printf("[*FILE]: \tupdate intime > prio greater/equal ts [%d]=%d and mid: %d\n",i,mediatemp.ts[i],mediatemp.mid);
											#endif 
											box->mid=mediatemp.mid;
											box->ts=mediatemp.ts[i];
											box->ds=mediatemp.ds[i];
											box->looop=mediatemp.looop;
											box->index=index_;
											box->mode=mediatemp.mode;
											box->playing=PLAYER_INTIME;
											box->prio=mediatemp.prio;
											box->lmd_msg=mediatemp.lmd_msg;
										}									
										else{
											#ifdef use_debug_serial
											SerialDEBUG.printf("[*FILE]: \t Ignore Intime greater ts [%d]=%d and mid: %d\n",i,mediatemp.ts[i],mediatemp.mid);
											#endif 
										}
																																			
										break;//get next ts										
									}
									else if(tsssss<=mediatemp.ts[i] && box->playing != PLAYER_INTIME && mediatemp.ts[i]> old_ts)//chưa tới thời gian phát
									{										
										//chưa tới thời gian phát
										//lấy mốc thời gian nhỏ nhất để so sánh
										
										if(mediatemp.lmd_msg =="")
										{
											#ifdef use_debug_serial
											SerialDEBUG.printf("[*FILE]: \tEmpty text\n");
											#endif 
										}
										else if(box->ts == 0 || box->ts > mediatemp.ts[i])
										{
											#ifdef use_debug_serial
											SerialDEBUG.printf("[*FILE]: \tupdate ts [%d]=%d and mid: %d\n",i,mediatemp.ts[i],mediatemp.mid);
											#endif 
											box->mid=mediatemp.mid;
											box->ts=mediatemp.ts[i];
											box->ds=mediatemp.ds[i];
											box->looop=mediatemp.looop;
											box->index=index_;
											box->mode=mediatemp.mode;
											box->playing=PLAYER_READY;
											box->prio=mediatemp.prio;
											box->lmd_msg=mediatemp.lmd_msg;
										}										
										else{
											#ifdef use_debug_serial
											SerialDEBUG.printf("[*FILE]: \t Ignore greater ts [%d]=%d and mid: %d\n",i,mediatemp.ts[i],mediatemp.mid);
											#endif 
										}
																																			
										break;//get next ts
									}																	
								}													
							}
							else {
								#ifdef use_debug_serial
								SerialDEBUG.println("[*FILE]: \tOff day");
								#endif 
							}														
						}
					}
				}			  			  			  
			  }
				else{
					//xóa các file không đúng định dạng cho nhẹ máy
					
					// const char *sys_filename="/system10.json";//
					// const char *oph_generic_filename="/sysgnric.json";
					// const char *oph_media_filename="/sysmedia.json";
					
					
					if(s!=sys_filename && s!=oph_generic_filename && s!=oph_media_filename)
					{
						#ifdef use_debug_serial
	SerialDEBUG.println("[*FILE]: incorect file-->delete");
	#endif 
						// SerialDEBUG.println("[*FILE]: -----WARNING---------------------------");
						// SerialDEBUG.println("[*FILE]: debug so remember uncomment this line");
						//delete file
						deleteFile(fs,file.name());
					}
					
				}
				
        }
        file = root.openNextFile();
    }
}
void deleteallfile(fs::FS &fs, const char * dirname, uint8_t levels){
    int index_=0;
	
	#ifdef use_debug_serial
	SerialDEBUG.printf("[*FILE]: Delete All File: %s\n", dirname);
	#endif 
	
	File root = fs.open(dirname);
    if(!root){
        #ifdef use_debug_serial
	SerialDEBUG.println("[*FILE]: Failed to open directory");
	#endif 
        return;
    }
    if(!root.isDirectory()){
        #ifdef use_debug_serial
	SerialDEBUG.println("[*FILE]: Not a directory");
	#endif 
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            #ifdef use_debug_serial
	SerialDEBUG.print("[*FILE]: \tDIR : ");
            SerialDEBUG.print (file.name());
			#endif 
            time_t t= file.getLastWrite();
            struct tm * tmstruct = localtime(&t);
            #ifdef use_debug_serial
	SerialDEBUG.printf("[*FILE]: \tLAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct->tm_year)+1900,( tmstruct->tm_mon)+1, tmstruct->tm_mday,tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
	#endif 
            //bỏ qua không xử lý folder
			// if(levels){
                // getmediafilename(fs, file.name(), levels -1);
            // }
        } else {
            #ifdef use_debug_serial
	SerialDEBUG.print("[*FILE]: NAME: ");
            SerialDEBUG.print(file.name());
            SerialDEBUG.print("  SIZE: ");
            SerialDEBUG.println(file.size());
#endif 
			  String s =String(file.name());
				//delete file
				deleteFile(fs,file.name());						  			  			 
								
        }
        file = root.openNextFile();
    }
}

void deleteallmdeia(fs::FS &fs, const char * dirname, uint8_t levels){
    int index_=0;
	
	#ifdef use_debug_serial
	SerialDEBUG.printf("[*FILE]: Delete All Media File: %s\n", dirname);
	#endif 
	
	File root = fs.open(dirname);
    if(!root){
        #ifdef use_debug_serial
	SerialDEBUG.println("[*FILE]: Failed to open directory");
	#endif 
        return;
    }
    if(!root.isDirectory()){
        #ifdef use_debug_serial
	SerialDEBUG.println("[*FILE]: Not a directory");
	#endif 
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            #ifdef use_debug_serial
	SerialDEBUG.print("[*FILE]: \tDIR : ");
            SerialDEBUG.print (file.name());
			#endif 
            time_t t= file.getLastWrite();
            struct tm * tmstruct = localtime(&t);
            #ifdef use_debug_serial
	SerialDEBUG.printf("[*FILE]: \tLAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct->tm_year)+1900,( tmstruct->tm_mon)+1, tmstruct->tm_mday,tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
	#endif 
            //bỏ qua không xử lý folder
			// if(levels){
                // getmediafilename(fs, file.name(), levels -1);
            // }
        } else {
            #ifdef use_debug_serial
	SerialDEBUG.print("[*FILE]: NAME: ");
            SerialDEBUG.print(file.name());
            SerialDEBUG.print("  SIZE: ");
            SerialDEBUG.println(file.size());
			#endif 

			  String s =String(file.name());
			  if (s.endsWith(".lmd")) {
				#ifdef use_debug_serial
	SerialDEBUG.println("[*FILE]: \tright file-->delete");
	#endif 
				//delete file
				deleteFile(fs,file.name());						  			  			 
			  }									
        }
        file = root.openNextFile();
    }
}
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    #ifdef use_debug_serial
	SerialDEBUG.printf("Listing directory: %s\n", dirname);
	#endif 

    File root = fs.open(dirname);
    if(!root){
        #ifdef use_debug_serial
	SerialDEBUG.println("Failed to open directory");
	#endif 
        return;
    }
    if(!root.isDirectory()){
        #ifdef use_debug_serial
	SerialDEBUG.println("Not a directory");
	#endif 
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            #ifdef use_debug_serial
	SerialDEBUG.print("  DIR : ");
            SerialDEBUG.print (file.name());
			#endif 
            time_t t= file.getLastWrite();
            struct tm * tmstruct = localtime(&t);
            #ifdef use_debug_serial
	SerialDEBUG.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct->tm_year)+1900,( tmstruct->tm_mon)+1, tmstruct->tm_mday,tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
	#endif 
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            #ifdef use_debug_serial
	SerialDEBUG.print("  FILE: ");
            SerialDEBUG.print(file.name());
            SerialDEBUG.print("  SIZE: ");
            SerialDEBUG.print(file.size());
			#endif 
            time_t t= file.getLastWrite();
            struct tm * tmstruct = localtime(&t);
            #ifdef use_debug_serial
	SerialDEBUG.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct->tm_year)+1900,( tmstruct->tm_mon)+1, tmstruct->tm_mday,tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
	#endif 
        }
        file = root.openNextFile();
    }
}
void deleteFile(fs::FS &fs, const char * path){
    #ifdef use_debug_serial
	SerialDEBUG.printf("Deleting file: %s\n", path);
	#endif 
    if(fs.remove(path)){
        
		#ifdef use_debug_serial
	SerialDEBUG.println("File deleted");
	#endif 
    } else {
        #ifdef use_debug_serial
	SerialDEBUG.println("Delete failed");
	#endif 
    }
}

/*
long spiff(String file, int read, String value) {

  if (!SPIFFS.begin()) {
    SerialDEBUG.println("Failed to mount file system");
    return (0);
  }

  FSInfo fs_info;
  SPIFFS.info(fs_info);

  SerialDEBUG.print("Total Bytes: ");
   SerialDEBUG.println(fs_info.totalBytes);

  SerialDEBUG.print("Used Bytes: ");
  SerialDEBUG.println(fs_info.usedBytes);

  if ((fs_info.totalBytes - fs_info.usedBytes) < 1000) {
    SerialDEBUG.println("Memory Full");
    return (0);
  }

//Init the Spiff FS (check if our INIT file exists or not
  File f = SPIFFS.open("init", "r+");

  if (!f) {
    SerialDEBUG.println("Formatting Spiff...");
    delay(200);
    //Format
    SPIFFS.format();

    //Make a Init File
    File f1 = SPIFFS.open("init", "w+");
    f1.println("init");
    f1.close();
   
    SerialDEBUG.println("Done");
    delay(200);
  }
//######################################################

//If Reading Check file
  if (read == 1) {
    if (!SPIFFS.exists(file)) {
      SerialDEBUG.println("File not exists");
      return (0);
    }
  }

//If Reading Open file R or A
  if (read == 1) {
    f = SPIFFS.open(file,  "r+");
  } else {
    f = SPIFFS.open(file,  "a");
  }

  int s = 0;
//if File Exists
  if (f) {
    s = f.size();

    // SerialDEBUG.printf("File Opened , Size=%d\r\n", s);
	//Read
    if (read == 1) {
      sdata = f.readString();
	  

    } else {
      //Write data to file
      f.println(value);
    }
    f.close();
    //Return the size of file
    return (s);
	
  } else {
    SerialDEBUG.print("Unable to open file ");
    SerialDEBUG.println(file);
  }
}
*/
#endif // __USER_DEFINE__
