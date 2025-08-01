#pragma once
/*
* File: GlobalVariables.h
* Date: 30/03/2023
* Description: This is file to declare global variables
*/
#define CODEDANH
// #define DEBUG_EEPROM
// #define DEBUG
#define addr_ExIO_portA 0x20
#define addr_ExIO_portB 0x24
#define addr_ExIO_Motor 0x22 //(Motor from 1 to 4)
#define addr_ExIO_OutputRL 0x26 //(Relay and Motor 5)
#define addr_Output_PWM 0x40

#define EN_M1 13
#define EN_M2 14
#define EN_M3 27
#define EN_M4 26
#define EN_M5 25

#define RX_PIN 32
#define TX_PIN 33
#define DE_PIN 4


#define timeout_hopep 2000
#define timeout_nhachai 3000
#define timeout_nhandien 20000
#define timeout_reset_cbvat 1000
#define timeout_chorot_lon 1000
#define timeout_motorcua 10000
#define timeout_chayxongquatrinh_4 2000
#define timeout_motorbangtai 5000
#define timeout_motornangha 1000
#define timeout_epchai 3000

// #define SOCHAITOIDA 600

// #define DEBUG

#ifdef DEBUG
	#define uart_debug Serial
	#define uart_PC Serial2
#else
	#define uart_debug Serial2
	#define uart_PC Serial
#endif

#define SOCHAITOIDA 50
#define PHANTRAM_THUNGCHAI_MUC25 ((uint8_t)(SOCHAITOIDA * 25 / 100))
#define PHANTRAM_THUNGCHAI_MUC50 ((uint8_t)(SOCHAITOIDA * 50 / 100))
#define PHANTRAM_THUNGCHAI_MUC75 ((uint8_t)(SOCHAITOIDA * 75 / 100))

#define SOLONTOIDA 50
#define PHANTRAM_THUNGLON_MUC25 ((uint8_t)(SOLONTOIDA * 25 / 100))
#define PHANTRAM_THUNGLON_MUC50 ((uint8_t)(SOLONTOIDA * 50 / 100))
#define PHANTRAM_THUNGLON_MUC75 ((uint8_t)(SOLONTOIDA * 75 / 100))


#define SOLANEPTOIDA 10
#define THOIGIANCHOGUILENH 200 //(MS) 

hw_timer_t *timer1 = nullptr;

enum PortA_Pin{
	PIN_A8 = 8,
	PIN_A7,
	PIN_A6,
	PIN_A5,
	PIN_A4,
	PIN_A3,
	PIN_A2,
	PIN_A1
};

enum PortB_Pin{
	PIN_B8 = 8,
	PIN_B7,
	PIN_B6,
	PIN_B5,
	PIN_B4,
	PIN_B3,
	PIN_B2,
	PIN_B1
};

enum PortMotor_Pin{
	PWMR_M1 = 0,
	PWML_M1,
	PWMR_M2,
	PWML_M2,
	PWMR_M3,
	PWML_M3,
	PWMR_M4,
	PWML_M4
};

enum PortRelay_Pin{
	RL1 = 0,
	RL2,
	RL3,
	RL4,
	RL5,
	PWMR_M5,
	PWML_M5,
};

enum PortPWM_Pin{
	PWM1 = 0,
	PWM2,
	PWM3,
	PWM4,
	PWM5,
	PWM6,
	PWM7,
	PWM8
};


namespace check_trangthai_slave
{

    const uint8_t KHONGLOI = 0x00;
    const uint8_t LOI = 0x01;
    const uint8_t DANGXULY = 0x02;
};


/*-------------------------------------khai bao adafruit-------------------------------------*/
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

Adafruit_PCF8574 pcf_Motor;
Adafruit_PCF8574 pcf_PortA;
Adafruit_PCF8574 pcf_PortB;
Adafruit_PCF8574 pcf_PortRelay;


/*****************************WIFI****************************/

const char* ssid     = "WIFI_BINH";
const char* password = "1751050056";

/*****************************SERVER OTA********************************/
const char *update_ota = "<form method='POST'\
					action='/update' enctype='multipart/form-data'>\
					<input type='file' name='update'><input type='submit' value='Update'></form>\
					<p>\
					<button class=\"button\" onclick=\"logoutButton()\">LOGOUT</button>\
					<button class=\"button\" onclick=\"HomeButton()\">HOME</button>\
                    <p>*May not function work in captive portal, Open in browser http://192.168.4.1</p>\
					</p><script>\
					function logoutButton(){\
					  var xhttp = new XMLHttpRequest();\
					  xhttp.open(\"GET\", \"/logout\", true);\
					  xhttp.send();\
					  setTimeout(function(){window.open(\"/login\", \"_self\");}, 1000);\
					}\
					function HomeButton(){\
					  var xhttp = new XMLHttpRequest();\
					  setTimeout(function(){window.open(\"/\", \"_self\");}, 1000);\
					}</script>";
					

//-----------Serial------------//
uint8_t ui8_Byteindex = 0;
uint8_t ui8_data_send[15];
uint8_t ui8_data_rec[100];
uint8_t ui8_status = 0x00;
uint8_t ui8_function = 0x00;
uint8_t ui8_slaveID = 0x03;
bool ui2_BTcomplete = false;
bool ui2_BTreceive = false;
bool ui2_start = false;
uint16_t ui16_order_command, ui16_last_order_command = 0xFFFF;
uint16_t ui16_self_order_command = 0x0001;
uint32_t ui32_timechar = 0;

//---------------Variable for doccambien function----------------//
#define socambien 20
int lastButtonState[socambien];
int lastCBState[socambien]; 
unsigned long lastDebounceTime[socambien] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

//-----------variable for speed of motor-----------------//
uint16_t ui16_tocdo_cuachai = 100;
uint16_t ui16_tocdo_nhachai = 25;
uint16_t ui16_tocdo_xoaychai = 100;
uint16_t ui16_tocdo_bangtai = 100;
uint16_t ui16_tocdo_nang_ha = 70;

//---------------define direction of motor------------------//
#define chieuquay_nhachai 0
#define chieuquay_xoaychai 0

#define chieuquay_cuachai_haxuong 0
#define chieuquay_cuachai_nanglen 1

#define chieuquay_bangtai_vao 0
#define chieuquay_bangtai_ra 1

#define chieu_nang_ha 1

//----------------Global Variable-----------------//

enum TrangThaiMay{
	KHONGLOI = 0,
	LOI,
	BAN
};

enum TrangThaiMotorCua{
    DUNG = 0,
    NANGLEN,
    HAXUONG
};

// enum TrangThaiMotorNhaChai{
    // DUNGNHACHAI = 2,
    // CHAYNHACHAI
// };

// enum TrangThaiMotorEpLon{
    // DUNGEPLON = 2,
    // CHAYEPLON
// };
enum TrangThaiMotorEpNghien{
	DUNGHOATDONG = 2,
	CHAYMOTOR,
	CHAYTHUAN,
	CHAYNGHICH
};

enum TrangThaiCua{
    KHONGKET = 0,
    KET
};

enum TrangThaiNhanDien{
    NHANDIENCHAI = 1,
	NHANDIENLON,
    NHANDIENSAI,
	NHANDIENKHAC
};

enum TrangThaiNhaVat{
    NHACHAI = 1,
	NHALON,
    KHONGNHA
};

enum TrangThaiThungRac{
	KHONGDAY = 0,
	DAY
};

enum TrangThaiVat{
	KHONGCOVAT = 0,
	COVAT
};

enum ThungRac{
	THUNGRACCHAI = 1,
	THUNGRACLON,
	THUNGRACKHAC
};

enum TrangThaiBangTai{
	DUNGBANGTAI = 0,
	VAO,
	RA
}; 


#ifndef CODEDANH
	uint8_t ui8_reconnectWifi = 0;

	uint8_t ui8_is_loop = 0;
	uint8_t ui8_is_stopuser = 0;

	uint8_t ui8_tatmotorcua = 0;
	uint8_t ui8_tatmotornhachai = 0;
	uint8_t ui8_tatmotorxoaychai = 0;
	uint8_t ui8_tatdongcobangtai = 0;
	uint8_t ui8_tatmotornghienchai = 0;

	uint8_t ui8_trangthaimay = 0;
	uint8_t ui8_trangthaicua = KHONGKET;
	uint8_t ui8_trangthaimotorcua = DUNG;
	uint8_t ui8_trangthaimotornhavat = DUNGHOATDONG;
	uint8_t ui8_trangthaimotoreplon = DUNGHOATDONG;
	uint8_t ui8_trangthaimotornghienchai = DUNGHOATDONG;
	uint8_t ui8_trangthaimotorbangtai = DUNGHOATDONG;
	uint8_t ui8_trangthaicuamay = 0;

	uint8_t ui8_kiemtramotornhavat = KHONGLOI;
	uint8_t ui8_kiemtramotoreplon = KHONGLOI;
	uint8_t ui8_kiemtramotornghienchai = KHONGLOI;
	uint8_t ui8_kiemtramotorbangtai = KHONGLOI;

	uint8_t ui8_trangthaithungraclon = KHONGDAY;
	uint8_t ui8_trangthaithungracchai = KHONGDAY;

	uint8_t ui8_guitrangthai_thungracchai = 0;
	uint8_t ui8_guitrangthai_thungraclon = 0;
	uint8_t ui8_guitrangthai_motornghien = 0;

	uint8_t ui8_trangthaivat = KHONGCOVAT;
	uint8_t ui8_trangthai_cambienepvat = KHONGCOVAT;
	uint8_t ui8_trangthainhandien = 0;
	uint8_t ui8_trangthai_longep = 0;
	uint8_t ui8_trangthai_nhavat = 0;

	uint8_t ui8_chaymotorbangtai = 0;
	uint8_t ui8_chaymotoreplon = 0;
	uint8_t ui8_runmotor_nhachai = 0;

	uint8_t ui8_startprocess = 0;
	uint8_t ui8_colonmoi = 0;
	uint8_t ui8_cochaimoi = 0;
	uint8_t ui8_solanep = 0;
	uint8_t ui8_sochairoixuong = 0;

	uint8_t ui8_chaymotornghienchai = 0;
	volatile uint8_t ui8_state_encoder = 0;
	uint8_t ui8_last_state_encoder = 1;


	uint8_t ui8_process1 = 0;
	uint8_t ui8_process2 = 0;
	uint8_t ui8_process3 = 0;

	uint8_t ui8_webcontrol = 0;
	uint8_t ui8_solannhayled_OTA = 0;

	uint8_t ui8_phanhoi_dungFunction = 0;
	uint8_t ui8_phanhoi_saiCRC = 0;
	uint8_t ui8_phanhoi_saiFunction = 0;
	uint8_t ui8_phanhoi_trunglenh = 0;
	uint8_t ui8_phanhoi_phantram_thungracchai = 0;
	uint8_t ui8_phanhoi_phantram_thungraclon = 0;
	uint8_t ui8_phanhoi_trangthai_rotlon = 0;
	uint8_t ui8_phanhoi_trangthai_rotchai = 0;
	uint8_t ui8_phanhoi_trangthai_covat = 0;
	uint8_t ui8_phanhoi_trangthai_thungracchai = 0;
	uint8_t ui8_phanhoi_trangthai_thungraclon = 0;
	uint8_t ui8_phanhoi_trangthai_motorep = 0;
	uint8_t ui8_phanhoi_trangthai_motornha = 0;
	uint8_t ui8_phanhoi_trangthai_motornghien = 0;

	uint8_t ui8_phanhoi_trangthai_resetthungrac_chai = 0;
	uint8_t ui8_phanhoi_trangthai_resetthungrac_lon = 0;

	uint8_t ui8_reset_chai=0;
	uint8_t ui8_reset_lon=0;

	uint8_t ui8_phanhoi_trangthai_vehome_motornhavat = 0;
	uint8_t ui8_trangthaivehom_nhavat = 0;
	uint8_t ui8_kiemtralayvat = 0;

	uint8_t dulieu_lo;
	uint8_t dulieu_hi;

	uint8_t ui8_guitrangthaithungraclonday = 0;
	uint8_t ui8_guitrangthaithungracchaiday = 0;

	uint8_t ui8_phanhoi_nhanlenhxoaychai = 0;
	uint8_t ui8_xoayrulonho = 0;
	uint8_t ui8_phanhoi_mocuamay = 0;
	uint8_t ui8_trangthaicua_may = 0;
	uint8_t ui8_tatbangtaingay = 0;

	uint8_t ui8_hacuadi = 0;
	uint8_t ui8_nangcuadi = 0;
	uint8_t ui8_chophephacua = 0;
	uint8_t ui8_ketthucquatrinh = 0;

	uint8_t ui8_runmotor_ruloto = 0;

	uint8_t ui8_runmotor_bangtai = 0;


	uint8_t ui8_runmotor_rulotochai = 0;
	uint8_t ui8_runmotor_rulotolon = 0;

	uint8_t ui8_runmotor_bangtai_chai = 0;
	uint8_t ui8_runmotor_bangtai_lon = 0;
	uint8_t ui8_xongquayruloto = 0;
	uint8_t ui8_tatmotornhavat = 0;
	uint8_t ui8_nhanduoc_lenh = 0;

	uint8_t ui8_phanhoi_ketbangtai = 0;

	uint8_t ui8_dangchaymotorchai = 0;
	uint8_t ui8_dangchaymotorlon = 0;
	uint8_t hoanthanh_kiemtra_nhavat = 0;


	uint16_t u16_lastspeed_xoaychai = 0;
	uint16_t u16_lastspeed_nhavat = 0;
	uint16_t u16_lastspeed_cua = 0;
	uint16_t u16_lastspeed_bangtai = 0;
	uint16_t ui16_sochai = 0;
	uint16_t ui16_sochai_trongthungrac = 0;
	uint16_t ui16_solon_trongthungrac = 0;
	uint16_t ui16_thoigian_maynghien_chaythuan = 60000;
	uint16_t ui16_thoigian_maynghien_chaynghich = 10000;

	uint32_t ui32_timemotorcua = 0;
	uint32_t ui32_timemotor_nhachai = 0;
	uint32_t ui32_timegui_trangthai_thungrac = 0;
	uint32_t ui32_timeoutnhandien = 0;
	uint32_t ui32_timecho_resetCBvat = 0;
	uint32_t ui32_timecho_resetCBbangtai = 0;
	uint32_t ui32_timeout_longep = 0;
	uint32_t ui32_timeout_chaynhavat = 0;
	uint32_t ui32_timeout_chaynghienchai = 0;
	uint32_t ui32_timeout_chaybangtai = 0;
	uint32_t ui32_timeout_kiemtramotornhavat = 0;
	uint32_t ui32_timeloi_motornghien = 0;
	uint32_t ui32_timecheck_encoder = 0 ;
	uint32_t ui32_timecho_guilenh = 0 ;
	uint32_t ui32_timeout_phathienvat = 0;

	uint32_t ui32_timeout_nhanlenh_resetchai = 0;
	uint32_t ui32_timeout_nhanlenh_resetlon = 0;

	uint32_t ui32_timeout_xoayruloto = 0;
	uint32_t ui32_timeout_bangtai = 0;

#else
	uint8_t state_sensor_drop = 0;
	uint8_t ui8_tatmotorcua = 0;
	uint8_t ui8_tatmotornhachai = 0;
	uint8_t ui8_tatmotorxoaychai = 0;
	uint8_t ui8_tatmotorbangtai = 0;
	uint8_t ui8_trangthaimay = 0;
	uint8_t ui8_trangthaicua = KHONGKET;
	uint8_t ui8_trangthaimotorcua = DUNG;
	uint8_t ui8_trangthaithungrac = KHONGDAY;
	uint8_t ui8_trangthaivat = KHONGCOVAT;
	uint8_t ui8_trangthai_cambienepvat = KHONGCOVAT;
	uint8_t ui8_trangthaimotorbangtai = DUNG;
	uint8_t ui8_trangthaimotornghienchai = DUNGHOATDONG;
	uint8_t ui8_kiemtramotornghienchai = KHONGLOI;
	uint8_t ui8_trangthai_nhandien = 0;
	uint8_t ui8_trangthai_motornhachai = 0;
	uint8_t ui8_trangthai_motorepchai = 0;
	uint8_t ui8_startprocess = 0;
	uint8_t ui8_loidongco_nangha = 0;
	uint8_t process_1 = 0;
	uint8_t process_2 = 0;
	uint8_t process_3 = 0;
	uint8_t process_4 = 0;
	uint8_t ui8_loidongco_nhachai = 0;
	uint8_t ui8_loidongco_epchai = 0;
	uint8_t ui8_loidongco_nghienchai = 0;
	uint8_t ui8_chophep_quatrinh3_chay = 0;
	uint8_t ui8_tatmotornangha = 0;
	uint8_t ui8_chaymotornghienchai = 0;

	uint8_t ui8_state_encoder = 0;
	uint8_t ui8_last_state_encoder = 0;
	uint8_t ui8_chophep_nghien = 0;
	uint8_t ui8_tatmotornghienchai = 0;
	uint8_t ui8_phathien_lon=0;
	uint8_t ui8_xoayrulonho = 0;
	// uint8_t covat_roi = 0;

	uint8_t ui8_kethuc = 0;
	uint8_t ui8_hacuadi = 0;
	uint8_t ui8_nangcua = 0;
	uint8_t ui8_phanhoilenh_hacua = 0;
	uint8_t ui8_phanhoilenh_nangcua = 0;
	uint8_t ui8_chophephacua = 0;
	uint8_t ui8_phanhoilenh_dongcua = 0;
	uint8_t ui8_phanhoi_trangthai_motornangha = 0;

	uint8_t ui8_phanhoi_mocuamay = 0;
	uint8_t ui8_trangthaicua_may = 0;
	uint8_t ui8_phanhoi_mocuathungrac = 0;
	uint8_t ui8_trangthaicuathungrac=0;
	uint8_t ui8_is_stopuser=0;
	uint8_t ui8_trangthaithungraclon = KHONGDAY;
	uint8_t ui8_trangthaithungracchai = KHONGDAY;
	uint8_t ui8_trangthaithungrackhac = KHONGDAY;
	uint8_t ui8_guitrangthaithungraclonday = 0;
	uint8_t ui8_guitrangthaithungracchaiday = 0;
	uint8_t ui8_guitrangthaithungrackhacday = 0;

	uint8_t ui8_kiemtramotornangha = KHONGLOI;
	uint8_t ui8_kiemtramotoreplon = KHONGLOI;
	// uint8_t ui8_kiemtramotornghienchai = KHONGLOI;
	uint8_t ui8_kiemtramotorbangtai = KHONGLOI;
	uint8_t ui8_kiemtramotorphanloai = KHONGLOI;

	uint8_t ui8_phanhoi_dungFunction = 0;
	uint8_t ui8_phanhoi_saiCRC = 0;
	uint8_t ui8_phanhoi_saiFunction = 0;
	uint8_t ui8_phanhoi_trunglenh = 0;
	uint8_t ui8_phanhoi_phantram_thungracchai = 0;
	uint8_t ui8_phanhoi_phantram_thungraclon = 0;
	uint8_t ui8_phanhoi_phantram_thungrackhac = 0;
	uint8_t ui8_phanhoi_trangthai_rotlon = 0;
	uint8_t ui8_phanhoi_trangthai_rotchai = 0;
	uint8_t ui8_phanhoi_trangthai_covat = 0;
	uint8_t ui8_phanhoi_trangthai_thungracchai = 0;
	uint8_t ui8_phanhoi_trangthai_thungraclon = 0;
	uint8_t ui8_phanhoi_trangthai_thungrackhac = 0;
	uint8_t ui8_phanhoi_trangthai_motorep = 0;
	uint8_t ui8_phanhoi_trangthai_motornha = 0;
	uint8_t ui8_phanhoi_trangthai_motornghien = 0;
	uint8_t ui8_phanhoi_trangthai_motorphanloai=0;
	uint8_t ui8_trangthai_motor_nangha=0;

	uint8_t ui8_phanhoi_trangthai_resetthungrac_chai = 0;
	uint8_t ui8_phanhoi_trangthai_resetthungrac_lon = 0;
	uint8_t ui8_phanhoi_trangthai_resetthungrac_khac=0;
	uint8_t ui8_vatrangoai=0;
	uint8_t ui8_khongcheck_receive=0;
	uint8_t ui8_chophanhoi=0;
	uint8_t ui8_check_guislave=0;
	uint8_t ui8_runmotor_nangchai=0;
	uint8_t ui8_phanhoi_trangthai_kimloai=0;
	uint8_t ui8_nhanxoaloi=0;
	uint8_t ui8_lalon=0;
	
	uint16_t ui16_sochai = 0;
	uint16_t ui16_sochai_trongthungrac = 0;
	uint16_t ui16_solon_trongthungrac = 0;
	uint16_t ui16_vat_trongthungrackhac=0;
	uint16_t u16_lastspeed_xoaychai = 0;
	uint16_t u16_lastspeed_nhachai = 0;
	uint16_t u16_lastspeed_cua = 0;
	uint16_t u16_lastspeed_bangtai = 0;
	uint16_t u16_lastspeed_nang_ha = 0;
	uint16_t ui16_sochai_hientai = 0;
	// uint16_t ui16_sochai = 0;

	uint32_t ui32_timemotorcua = 0;
	uint32_t ui32_timemotor_bangtai = 0;
	uint32_t ui32_timegui_trangthai_thungrac = 0;
	uint32_t ui32_timechaydongco_eplon = 0;
	uint32_t ui32_timeoutnhandien = 0;
	uint32_t ui32_timecho_resetCBvat = 0;
	uint32_t ui32_timeout_dongco_nhalon = 0;
	uint32_t ui32_timeout_dongco_eplon = 0;
	uint32_t ui32_timeout_rotchai = 0;
	uint32_t ui32_timeout_kiemtradongcoep = 0;
	uint32_t ui32_timeout_end = 0;
	uint32_t ui32_timemotorbangtai = 0;
	uint32_t ui32_timemotornangha = 0;
	uint32_t ui32_timeloi_motornghien = 0;
	uint32_t ui32_timeout_chaynghienchai = 0;
	uint32_t ui32_timemotorchayra=0;
	uint32_t ui32_timeout_tatmaynghien = 0;
	uint32_t ui32_timecho_guilenh_485=0;
	uint16_t ui16_thoigian_maynghien_chaynghich = 0;
	uint16_t ui16_thoigian_maynghien_chaythuan = 0;

	uint32_t ui32_timeout_hacuaxuong = 0;
	uint32_t ui32_timeout_ketthuc = 0;	
	uint32_t ui32_timeout_nangbangtai=0;
	uint32_t ui32_timeout_kiemtramotornhavat = 0;
	// uint32_t ui32_timeout_nhanlenh_resetchai = 0;
	// uint32_t ui32_timeout_nhanlenh_resetlon = 0;

	int chomotor_eplonxong = 0;
	int chomotor_nhachaichayxong = 0;
	int chophep_nhalon = 0;
	int dem_solan_ep = 0;
	int cochaimoi = 0;


	int chaymotorbangtai = 0;
	int kiemtramotoreplon = 0;
	// int cholayvat = 0;
	int chaymortoreplon = 0;
	int chochay_motor_eplon = 0;
	int end_user = 0;
	int dongcua_lai = 0;
	int dongxong_cua = 0;
	int chophep_dongcua=0;
	int motorbangtaira = 0;
	int chaythuandi = 0;
	int chaymaynghiendi = 0;


	String inputString = "";            // a String to hold incoming data
	bool stringComplete = false;        // whether the string is complete

#endif

String str = "";
String data_lo = "";
String data_hi = "";
String firmware = "RVM3.001";


RS485_Master RS485(Serial1, TX_PIN, RX_PIN, DE_PIN);

uint8_t slave1 = 0x09; // Đặt Slave ID

uint8_t dataReceived[30];
uint32_t timeReceive;


uint32_t ui32_timeout_checkrs485 = 0;


uint8_t order_command[2] = {0x00, 0x00};
uint8_t self_order_command_test[2] = {0x00, 0x00};
uint8_t last_order_command[2] = {0x00, 0x00};
// uint8_t ui8_buffer_send_slave[100];
// uint8_t ui8_buffer_receive_slave[100];


// // Set your Static IP address
// IPAddress local_IP(192, 168, 137, 10);
// // Set your Gateway IP address
// IPAddress gateway(192, 168, 137, 1);

// IPAddress subnet(255, 255, 255, 0);
// IPAddress primaryDNS(8, 8, 8, 8);   //optional
// IPAddress secondaryDNS(8, 8, 4, 4); //optional

// bool is_authenticated = false;
// WebServer server(80);

// const byte DNS_PORT = 53; //FOR CAPTIVE PORTAL
// DNSServer dnsServer;

// String Wifi_SSID;
// String IP_Server;
// SemaphoreHandle_t batton = NULL;