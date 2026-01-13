/*
 Copyright (C) 2011 J. Coliz <thanhcong402@gmail.com>
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */
 
/**
 * @file Wifi96x32/Globalvaiables.h
 *
 */

/**
Tin để test
{
  "id": 12113,
  "opcode": 67,
  "data":{"mid":3,"mode": 1,"ts":[43500],"ds":[60],"duration":40,"created":1638779409,"size":0,"codec":1,"format":1,"cs":0,"url":"https://alta-s3.dev-altamedia.com/lalova/chaomung.txt","auth":"","loop":0,"prio":0,"expired":1639303200,"start":1638829040,"repeat":0,"days":255,"header":255}
}

*/


#ifndef __GLOABL_VARIABLES__
#define __GLOABL_VARIABLES__

unsigned long lastMillis = 0;
int counter_index=0;
uint8_t need_reset_chip=0;
uint8_t ui8_bathuong=0;
uint8_t flash_led=0; 
 enum system_exe
{
  SYS_NORMAL = 0,
  SYS_ERR0,//wifi không kết nối được
  SYS_ERR_SERVER,
  SYS_WAIT_RESET
};

unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long tempus=0;
unsigned long u32_timedelay=0;
unsigned long timegetdata=0;
uint32_t ui32timeoutrx=0;
const int led_sign=2;
int status_led;
int system_status=SYS_ERR0;
// constants won't change:
const long interval = 1000;           // interval at which to blink (milliseconds)
unsigned int counter=0;
uint16_t au16outputdata[30];
uint8_t read_playlist=1;//cho phep doc playlist
uint8_t phatngaychotoi=0;
uint8_t need_reset_playlist=0;
uint8_t empty_playlist;
//-----------------------button----------------------------------------------


#if defined (hwv0)
	const int encoder0Click=0;
	const int encoder1Click=25;
	const int encoder2Click=32;
	const int encoder3Click=33;
	const int pinReset=15;
	const int pinPower=13;
#elif defined (hwv1)
	const int encoder0Click=0;
	const int encoder1Click=25;
	const int encoder2Click=32;
	const int encoder3Click=33;
	const int pinReset=15;
	const int pinPower=13;
#elif defined (hwv2)
	const int encoder0Click=0;
	const int encoder1Click=35;
	const int encoder2Click=32;
	const int encoder3Click=33;
	const int pinReset=15;
	const int pinPower=13;
#else
	#error "Please define one of use_ in config_hw_local.h"
#endif






unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

struct button_info_t
{
	int pin_sw;
	int button_;
	int buttonStateEncoder;             // the current reading from the input pin
	int lastButtonState;   // the previous reading from the input pin
	int keypressed;	
	// the following variables are unsigned longs because the time, measured in
	// milliseconds, will quickly become a bigger number than can be stored in an int.
	unsigned long lastDebounceTime;  // the last time the output pin was toggled	
};

button_info_t button0;
button_info_t button1;
button_info_t button2;
button_info_t button3;

uint8_t count_press=0;
uint32_t sw_timeout=0;


struct mqtt_net_info_t
{
    char wifi_name[20];
	char wifi_pass[20];
	char port[5];
    char sever_mqtt[50];
	char client_id[20];
	char username[24];
	char pwdMqtt[24];//"pass"+device_code
	//uint32_t device_code;
	// uint64_t device_code;	
	char device_code[20];	
	uint8_t mode_data_is_wifi;
}; 

mqtt_net_info_t mqtt;

// char pass_update_firm[10];
// char pass_update_led[10];
// char versiondisplay[20];
/**
 * @enum MGW_PROR
 * @brief
 * độ ưu tiên của bản tin
 */
enum MGW_PRIOR
{
    MGW_PRIOR_NORMAL               	= 0,   /*!< null operator */
    MGW_PRIOR_HIGH          		    = 1,	/*!< Save data to file */
    MGW_PRIOR_EMERGENCY					= 2	/*!< preview on output */	
};

/**
 * @enum mqtt_OPH_MEDIA_HTTP
 * @brief
 * Chứa thông tin của bản tin
 * được lưu trữ trong flash
 */
struct mqtt_OPH_MEDIA_HTTP
{
    uint32_t mid;	//id của bản tin
	uint8_t prio;
	uint32_t created;//UTC Unix time
	uint32_t expired;//UTC Unix time
	uint8_t mode;	//cách thức phát bản tin =0: không phát, =1: phát theo lịch, =2: phát ngay lập tức
	uint32_t start;//thời điểm (UTC Unix time) phát bản tin
	uint32_t ts[24];//các thời điểm phát bản tin trong ngày 0-86399s	
	uint8_t repeat; //0: lặp theo tuần, =1: lặp theo tháng
	uint32_t days;//bit-mask các ngày lặp lại bit0: ngày 1...bit30: ngày 31
	String url;
	// char url[150];
	String auth;
	// char auth[100];//thông tin xác thực để download file
	uint16_t looop;//số lần lặp lại khi phát
	uint16_t duration;//thời lượng phát của bản tin (hết file) : cho trường hợp mode=2
	uint32_t ds[24];//Thời lượng phát bản tin, cho trường hợp mode =1
	// uint32_t ds[24];==ts	
	// char lmd_msg[500];//nội dung bản tin sau khi đã load về. (nội dung hiển thị lên led matrix
	String lmd_msg;
};

mqtt_OPH_MEDIA_HTTP oph_media_buffer;//buffer de xu ly du lieu trong ham loop

mqtt_OPH_MEDIA_HTTP oph_media_temp;//buffer de xu ly tin nhan trong ham messageReceived

/**
 * @enum mqtt_OPU_GENERIC
 * @brief
 * Chứa thông tin của hệ thống
 * được lưu trữ trong flash
 */
struct mqtt_OPU_GENERIC
{
	char fvers[20];//Firmware version hiện tại của chip điều khiển khối giao tiếp mqtt	
	uint32_t vcode;	//mã xác thực thiết bị, là số nguyên 32-bit không dấu, được sinh random mỗi khi khởi động.
	uint32_t group[20];	//danh sách các GROUP mà MGW đang chờ bản tin.
	char disabled;	//=0: kích hoạt, =1: ngừng kích hoạt
	uint32_t mid;	//ID của bản tin mới nhất mà MGW đang lưu.
	// char volume;
	// char fmvolume;
	// char fmauto;	
	char txtype;	//=0: Ethernet, =1: 3G, =2; Wifi
	char ip[20];	//địa chỉ IP mạng LAN của MGW.
	char phone[15];	//số điện thoại hiện tại.
	// char camera[10];
	// char sensor[10];
	// char alarm[10];
};
 
mqtt_OPU_GENERIC opu_generic_system;

String inputString = "";
char fverl[20];//Firmware version hiện tại của chip điều khiển hiển thị LED.

/**
 * @enum mqtt_OPU_STATUS
 * @brief
 * Chứa thông tin của hệ thống
 * được lưu trữ trong flash
 * cập nhật lên server 1phut/lần/ hoặc khi có thay đổi conn, spkerr
 */
struct mqtt_OPU_STATUS
{
	uint16_t conn;//kiểu kết nối hiện tại. 0: ethernet, 1: wifi, 2: 2G, 3:3g, 4:4g, 5:5g, 255 không rõ
	uint16_t csqm;//mức tín hiệu Mobile Network, từ 0 – 100%.
	uint16_t csqw;//mức tín hiệu Wifi, từ 0 – 100%.
	uint16_t spkerr;// trạng thái loa: =0; bình thường, =1:lỗi
	uint16_t spksta;//dòng tiêu thụ hiện tại của loa, x10, mA. VD: spksta = 1234 → 123.4mA.
	uint16_t temp;//nhiệt độ bên trong hộp MGW, x10, degree celsius. VD: temp = 345 → 34.5℃.
	uint16_t fmsta;// trạng thái tín hiệu FM thu được.0: no signal, =1: good signal
	uint16_t sdfree;//kích thước dung lượng còn trống, đơn vị: kbyte (1000 bytes).
	uint16_t sdtotal;//: kích thước dung lượng tổng, đơn vị: kbyte (1000 bytes).	
};
mqtt_OPU_STATUS opu_status_system;

/**
 * @enum mqtt_OPU_STATUS
 * @brief
 * Chứa thông tin của hệ thống
 * được lưu trữ trong flash
 * cập nhật lên server 1phut/lần/ hoặc khi có thay đổi conn, spkerr
 */
struct str_media_manager
{
	uint32_t mid;//kiểu kết nối hiện tại. 0: ethernet, 1: wifi, 2: 2G, 3:3g, 4:4g, 5:5g, 255 không rõ
	uint32_t ts;//mức tín hiệu Mobile Network, từ 0 – 100%.
	uint32_t ds;//mức tín hiệu Mobile Network, từ 0 – 100%.
	uint8_t mode;
	uint8_t prio;
	uint16_t looop;//mức tín hiệu Wifi, từ 0 – 100%.
	uint16_t playing;//mức tín hiệu Wifi, từ 0 – 100%.	
	uint8_t	index;
	String lmd_msg;
};

#define store_index_max 50
#define dfnofile 0xfffe
uint32_t store_name[store_index_max];
uint32_t downdloadingthisfile=dfnofile;
uint8_t	index_storing=0; 
uint8_t	index_process=0; 

#define max_file_process 10
str_media_manager media_manager[max_file_process];

 enum system_playing
{
  PLAYER_READY = 0Xffff,
  PLAYER_STOP = 0Xfffe,
	PLAYER_INTIME  =0Xfffd,
	PLAYER_PLAYING = 0Xfffb
};

int havedata=0;

uint32_t u32timecheck=0;
uint32_t u32chk=0;// thoi gian ping qua bang led
#define df_time_ack 30000
int index_send=10;
int index_lmd=0;
int counterget=0;
int check_firmware=0;
int u8_needreub_channle=0;
/**
 * @enum LED_SCREEN_CMD
 * @brief
 */
enum LED_SCREEN_CMD
{    
    LMD_NONE =0,
	LMD_STOP,
	LMD_PLAY
};

uint8_t lmd_repeat =10;
uint8_t lmd_command =0;
uint32_t u32_lmd_resend;


uint8_t need_getinfo=1;//lần đầu vào là đọc liền.
uint32_t u32_get_info =0;

#define df_time_upgrade_minute 10//10 minutes
int addr_configwifi=0;


char mqtt_server[40];
char mqtt_port[6] = "1883";
char u8_devicecode[33] = "YOUR_DEVICE_TOKEN";
bool shouldSaveConfig = false;


uint32_t lastReconnectAttempt = 0;



enum system_time_status
{
	TIME_NORMAL = 0,
	TIME_ERRO
};
enum system_gettime_status
{
	TIME_START = 0,
	TIME_WAIT,
	TIME_FINISH,
	TIME_NONE
};

enum system_time_control
{
	BYNONE = 0,
	BYMQTT,	
	BYSERVER,
	BYSIM4G,
	BYRTC	
};
//----------------------------------------------------------


struct str_time_manager
{
	uint8_t		status;
	uint8_t		mode=BYNONE;
	// uint8_t		counter;
	uint32_t	u32check_system_time=0;
	uint32_t	u32checkrespone=0;
	int32_t 	timeset=0;
	uint8_t 	process	=0;
};
str_time_manager info_time_system;

uint8_t ui8_ds1307_need_update =0;

uint16_t ui16command;
uint16_t ui16ledcommand[4];//chứa width,heigh, kind of led

#endif // __GLOABL_VARIABLES__
