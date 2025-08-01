#pragma once
// #define MASTER
// #define DEBUG
#define DEBUG_EEPROM
/*
* File: GlobalVariables.h
* Date: 30/03/2023
* Description: This is file to declare global variables
*/
#define SOCHAITOIDA 50

#define addr_ExIO_portA 0x20
#define addr_ExIO_portB 0x24
#define addr_ExIO_portC 0x22 //(Motor from 1 to 4)
#define addr_ExIO_portD 0x26 //(Relay and Motor 5)
#define addr_ExIO_portE 0x28

#define timeout_nhaqua 20000
#define timeout_loinhaqua 10000
#define timeout_wait 20000
#define timeout_checkmotor 10000
#define timeout_checknhaqua 5000
// #define DE_PIN 4

#ifdef DEBUG
	// HWCDC Serial;
	#define Serial_Debug Serial
#else
	// #define uart_debug Serial2
	HWCDC Serial_Debug;
	#define Serial Serial
#endif



#ifdef MASTER
	#define RX_PIN_485 18
	#define TX_PIN_485 17		
	namespace PIN_MOTOR
	{
		uint8_t PIN_DIR_MOTOR1=38;
		uint8_t PIN_DIR_MOTOR2=15;
		uint8_t PIN_DIR_MOTOR3=40;
		uint8_t PIN_DIR_MOTOR4=42;
		uint8_t PIN_PWM_MOTOR1=39;
		uint8_t PIN_PWM_MOTOR2=16;
		uint8_t PIN_PWM_MOTOR3=41;
		uint8_t PIN_PWM_MOTOR4=2;
	};

	namespace PIN_CAMBIEN
	{
		uint8_t PIN_S1=48;
		uint8_t PIN_S2=47;
		uint8_t PIN_S3=21;
		uint8_t PIN_S4=14;
		uint8_t PIN_S5=13;
		uint8_t PIN_S6=12;
		uint8_t PIN_S7=11;
		uint8_t PIN_S8=10;
		uint8_t PIN_S9=9;
		uint8_t PIN_S10=3;
	};

	namespace PIN_LED
	{
		uint8_t PIN_W = 4;
		uint8_t PIN_R = 7;
		uint8_t PIN_G = 6;
		uint8_t PIN_B = 5;
	};

#else
	#define RX_PIN_485 48
	#define TX_PIN_485 38	
	namespace PIN_MOTOR
	{
		uint8_t PIN_DIR_MOTOR1=40;
		uint8_t PIN_DIR_MOTOR2=1;
		uint8_t PIN_DIR_MOTOR3=10;
		uint8_t PIN_DIR_MOTOR4=12;
		uint8_t PIN_DIR_MOTOR5=14;
		uint8_t PIN_PWM_MOTOR1=41;
		uint8_t PIN_PWM_MOTOR2=2;
		uint8_t PIN_PWM_MOTOR3=9;
		uint8_t PIN_PWM_MOTOR4=11;
		uint8_t PIN_PWM_MOTOR5=13;
	};

	namespace PIN_CAMBIEN
	{
		uint8_t PIN_S1=15;
		uint8_t PIN_S2=7;
		uint8_t PIN_S3=6;
		uint8_t PIN_S4=5;
		uint8_t PIN_S5=4;
	};

	namespace PIN_ROLE
	{
		uint8_t PIN_ROLE1 = 47;
		uint8_t PIN_ROLE2 = 21;
	};


	enum PortA_Pin{
		PIN_A1 = 0,
		PIN_A2,
		PIN_A3,
		PIN_A4,
		PIN_A5,
		PIN_A6,
		PIN_A7,
		PIN_A8,
		PIN_A9,
		PIN_A10,
		PIN_A11,
		PIN_A12,
		PIN_A13,
		PIN_A14,
		PIN_A15,
		PIN_A16
	};

	enum PortB_Pin{
		PIN_B1 = 0,
		PIN_B2,
		PIN_B3,
		PIN_B4,
		PIN_B5,
		PIN_B6,
		PIN_B7,
		PIN_B8
	};

	enum PortC_Pin{
		PIN_C8 = 0,
		PIN_C7,
		PIN_C6,
		PIN_C5,
		PIN_C4,
		PIN_C3,
		PIN_C2,
		PIN_C1
	};

	enum PortD_Pin{
		PIN_D8 = 0,
		PIN_D7,
		PIN_D6,
		PIN_D5,
		PIN_D4,
		PIN_D3,
		PIN_D2,
		PIN_D1
	};

	enum PortE_Pin{
		PIN_E8 = 0,
		PIN_E7,
		PIN_E6,
		PIN_E5,
		PIN_E4,
		PIN_E3,
		PIN_E2,
		PIN_E1
	};

	// enum VITRI_KHAY{
	// 	KHAY_1 = 0x00,
	// 	KHAY_2,
	// 	KHAY_3,
	// 	KHAY_4,
	// 	KHAY_5
	// };

	// enum VITRI_O{
	// 	O_1 = 0x00,
	// 	O_2,
	// 	O_3,
	// 	O_4,
	// 	O_5,
	// 	O_6,
	// 	O_7,
	// 	O_8
	// };
#endif

namespace TrangThaiMotorDC{
    uint8_t DUNG = 0;
    uint8_t NANGLEN = 1;
    uint8_t HAXUONG = 2;
	uint8_t VAO=1;
	uint8_t RA=2;
	uint8_t DANGNANG=1;
	uint8_t KHONGKET=0;
	uint8_t KET=1;
};

namespace TrangthaiLoi
{
	uint8_t LOI=1;
	uint8_t KHONGLOI=0;
	uint8_t BAN = 2;
};

namespace TrangThaiMotor_nghien_ep_phanloai
{

    const uint8_t KHONGLOI = 0x00;
    const uint8_t LOI = 0x01;
    const uint8_t DANGXULY = 0x02;
};

namespace TrangThaiThungRac
{
	uint8_t KHONGDAY=0;
	uint8_t DAY=1;
};

namespace TrangThaiNhanDien
{
	uint8_t HOANTHANH_NHANDIEN=0;
	uint8_t NHANDIENCHAI=4;
	uint8_t NHANDIENLON=1;
	uint8_t NHANDIENKHAC=2;
	uint8_t NHANDIENSAI=3;
};

namespace TrangThaiRoiVat
{
	uint8_t KHONGNHA=0;
	uint8_t NHACHAI=1;
	uint8_t NHALON=2;
	uint8_t NHAKHAC=3;
};

namespace TrangThaiLed
{
	uint8_t ON=1;
	uint8_t OFF=0;
};

namespace TrangThaiLenhPhanHoi
{
	uint8_t HOANTHANH=0;
	uint8_t DUNG=1;
	uint8_t SAI_FUNCTION=4;
	uint8_t TRUNG_LENH=2;
	uint8_t SAI_CRC=3;
};

namespace TrangThaiVat
{
	uint8_t KHONGCOVAT=0;
	uint8_t COVAT=1;
};

namespace trangthaiRS485
{
const uint8_t DANGGUI = 0x01;
const uint8_t DANGNHAN = 0x02;
}

enum ThungRac{
	THUNGRACCHAI = 1,
	THUNGRACLON,
	THUNGRACKHAC
};

enum ERRO_STATUS
{
    sku_empty = 0, //nhả sản phẩm thành công.
	motor_erro, //lỗi động cơ
    sku_success, //hết sản phẩm ở vị trí này
    sku_over, //không tồn tại sản phẩm ở vị trí này
    df_cmd_erro, //Lỗi cmd nhỏ hơn cmd hiện hành
};



#define DIR_MOTOR1 PIN_MOTOR::PIN_DIR_MOTOR1
#define DIR_MOTOR2 PIN_MOTOR::PIN_DIR_MOTOR2
#define DIR_MOTOR3 PIN_MOTOR::PIN_DIR_MOTOR3
#define DIR_MOTOR4 PIN_MOTOR::PIN_DIR_MOTOR4
#define DIR_MOTOR5 PIN_MOTOR::PIN_DIR_MOTOR5

#define PWM_MOTOR1 PIN_MOTOR::PIN_PWM_MOTOR1
#define PWM_MOTOR2 PIN_MOTOR::PIN_PWM_MOTOR2
#define PWM_MOTOR3 PIN_MOTOR::PIN_PWM_MOTOR3
#define PWM_MOTOR4 PIN_MOTOR::PIN_PWM_MOTOR4
#define PWM_MOTOR5 PIN_MOTOR::PIN_PWM_MOTOR5


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

// //-----------variable for speed of motor-----------------//
// uint16_t ui16_tocdo_cuachai = 100;
// uint16_t ui16_tocdo_nhachai = 25;
// uint16_t ui16_tocdo_xoaychai = 100;
// uint16_t ui16_tocdo_bangtai = 100;
// uint16_t ui16_tocdo_nang_ha = 70;
// uint16_t u16_lastspeed_cua = 0;
// uint16_t u16_lastspeed_bangtai = 0;
// uint16_t u16_lastspeed_xoaychai = 0;
// uint16_t u16_lastspeed_nangha = 0;

//---------------define direction of motor------------------//
#define chieuquay_nhachai 0
#define chieuquay_xoaychai 0

#define chieuquay_cuachai_haxuong 0
#define chieuquay_cuachai_nanglen 1

#define chieuquay_bangtai_vao 0
#define chieuquay_bangtai_ra 1

#define chieu_nang_ha 1


typedef struct 
{
	uint8_t ui8_tatmotor;
	uint8_t ui8_trangthaimotor;
	uint32_t ui32_timeout_dongmo_motor;
	uint32_t ui32_timeout_cho_motor;
	uint16_t ui16_tocdo;
	uint16_t u16_lastspeed;
	uint8_t ui8_kiemtra_trangthailoi_motorDC;
	uint8_t ui8_phanhoitrangthai;
} MOTOR;

typedef struct 
{
	uint8_t ui8_kiemtra_trangthailoi = TrangthaiLoi::KHONGLOI;	
	uint8_t ui8_phanhoi_trangthai=TrangThaiLenhPhanHoi::HOANTHANH;
} MOTOR_TO;

typedef struct
{
	uint8_t ui8_guitrangthaithungracday;
	uint8_t ui8_trangthaithungrac;
	uint8_t ui8_phanhoitrangthai_thungrac;
	uint32_t ui32_timegui_trangthai_thungrac;
	uint16_t ui16_soluong;
	uint8_t ui8_phanhoiphantram_soluong;
	uint32_t ui32_timeout_gui;
} THUNGRAC;

typedef struct 
{
	uint8_t ui8_phanhoi_trangthai_cambien=TrangThaiLenhPhanHoi::HOANTHANH;
} CAMBIEN;



// Khởi tạo giá trị cho từng động cơ
MOTOR motor_cua = {0, TrangThaiMotorDC::DUNG, 0, 10000,100,0,TrangthaiLoi::KHONGLOI,TrangThaiLenhPhanHoi::HOANTHANH};
MOTOR motor_bangtai = {0, TrangThaiMotorDC::DUNG, 0, 1000,100,0,TrangthaiLoi::KHONGLOI,TrangThaiLenhPhanHoi::HOANTHANH};
MOTOR motor_xoaychai = {0, TrangThaiMotorDC::DUNG, 0, 1000,100,0,TrangthaiLoi::KHONGLOI,TrangThaiLenhPhanHoi::HOANTHANH};
MOTOR motor_nangha = {0, TrangThaiMotorDC::DUNG, 0, 1000,70,0,TrangthaiLoi::KHONGLOI,TrangThaiLenhPhanHoi::HOANTHANH};

MOTOR_TO motor_nghien; 
MOTOR_TO motor_ep;
MOTOR_TO motor_phanloai;

THUNGRAC thungrac_chai{0,TrangThaiThungRac::KHONGDAY,0,0,0,0,5000};
THUNGRAC thungrac_lon{0,TrangThaiThungRac::KHONGDAY,0,0,0,0,5000};
THUNGRAC thungrac_khac{0,TrangThaiThungRac::KHONGDAY,0,0,0,0,5000};

CAMBIEN cb_cua;
CAMBIEN cb_kimloai;
CAMBIEN cb_nangha;
CAMBIEN cb_vat;
// uint8_t ui8_tatmotorcua=0;
// uint8_t ui8_trangthaimotorcua = TrangThaiMotorCua::DUNG;
// uint32_t ui32_timemotorcua=0;


String inputString = "";            // a String to hold incoming data
bool stringComplete = false;        // whether the string is complete
uint8_t ui8_startprocess=0;
uint8_t ui8_is_stopuser=0;
uint8_t ui8_trangthaicua=TrangThaiMotorDC::KHONGKET;
uint8_t ui8_kethuc=0;
uint8_t ui8_runmotor_nangchai=0;

uint8_t process_1=0;
uint8_t process_2=0;
uint8_t	process_3=0;
uint8_t ui8_phathien_lon=0;
uint8_t ui8_trangthai_nhavat=TrangThaiRoiVat::KHONGNHA;
uint8_t ui8_trangthaimay=0;
uint8_t ui8_phanhoi_lenh=TrangThaiLenhPhanHoi::HOANTHANH;
uint8_t ui8_trangthai_nhandien=TrangThaiNhanDien::HOANTHANH_NHANDIEN;
uint8_t ui8_hacuadi=0;
uint8_t ui8_trangthaivat=TrangThaiVat::KHONGCOVAT;
uint8_t ui8_chaymotorbangtai=0;
uint8_t ui8_vatrangoai=0;
uint8_t ui8_lalon=0;
uint8_t ui8_is_loop=0;
uint8_t ui8_chophephacua=0;

uint8_t ui8_phanhoi_dungFunction=0;
uint8_t ui8_phanhoi_saiCRC=0;
uint8_t ui8_phanhoi_saiFunction=0;
uint8_t ui8_phanhoi_trunglenh=0;

uint32_t ui32_timeout_kiemtramotornhavat=0;
uint32_t ui32_timeoutnhandien=0;
uint32_t ui32_timemotor_bangtai=0;
uint32_t ui32_timecho_resetCBvat=0;
// uint32_t ui32_timeout_checkrs485=0;
// RS485_Master RS485(Serial1, TX_PIN, RX_PIN);
uint8_t slave1 = 0x09; // Đặt Slave ID
uint8_t dataReceived[30];
uint32_t timeReceive;
uint32_t ui32_timeout_checkrs485[10];
uint32_t ui32_timeout_checkrs485_slave1=0;
uint8_t order_command[2] = {0x00, 0x00};
uint8_t self_order_command_test[2] = {0x00, 0x00};
uint8_t last_order_command[2] = {0x00, 0x00};

String str = "";
String data_lo = "";
String data_hi = "";
String firmware = "RVM3.001";

unsigned long lastSendTime = 0;  // Lưu trữ thời điểm gửi gần nhất
int currentSlave = 0;            // Biến đếm để xác định slave hiện tại

uint8_t ui8_slave[]={
	0x01,
	0x02,
	0x03,
	0x04,
	0x05
};

uint8_t ui16_sochai_trongthungrac=0;
uint8_t ui16_solon_trongthungrac=0;
uint8_t ui16_vat_trongthungrackhac=0;
uint8_t ui8_khongcheck_receive=0;

uint8_t ui8_nhanxoaloi=0;
uint8_t ui8_xoaloi=0;

uint8_t ui8_phanhoi_trangthai_resetthungrac_chai = 0;
uint8_t ui8_phanhoi_trangthai_resetthungrac_lon = 0;
uint8_t ui8_phanhoi_trangthai_resetthungrac_khac=0;

uint8_t ui8_phanhoi_trangthai_thungracchai = 0;
uint8_t ui8_phanhoi_trangthai_thungraclon = 0;
uint8_t ui8_phanhoi_trangthai_thungrackhac = 0;

uint8_t ui8_phanhoi_phantram_thungracchai = 0;
uint8_t ui8_phanhoi_phantram_thungraclon = 0;
uint8_t ui8_phanhoi_phantram_thungrackhac = 0;

uint8_t ui8_receive_respond_from_slave=0;
uint8_t ui8_buffer_send_PC[15];
uint8_t ui8_buffer_receive_PC[20];

uint8_t buffer_send_slave[15];
uint8_t buffer_receive_slave[20];

uint32_t ui32_timebegin_nhanphanhoi = 0;

uint8_t ui8_check_slave = 0;
uint32_t ui32_time_checkslave = 0;

uint8_t ui8_phanhoi_mocuamay=0;
uint8_t ui8_phanhoi_mocuathungrac=0;
uint8_t ui8_trangthaicua_may=0;
uint8_t ui8_trangthaicuathungrac=0;

uint8_t ui8_trangthaithungracchai=0;
uint8_t ui8_trangthaithungraclon=0;
uint8_t ui8_trangthaithungrackhac=0;

uint32_t ui32_timecho_guilenh=0;

// uint16_t ui16_sochai_trongthungrac=0;
// uint16_t ui16_solon_trongthungrac=0;
// uint16_t ui16_sovat_trongthungrac=0;

uint32_t ui32_timeout_nhaqua=0;
uint32_t ui32_timeout_loinhaqua=0;

uint8_t erro_status = sku_empty;
uint8_t status_drop_product = 0;

uint8_t ui8_coquaroi = 0;
uint8_t ui8_khongcoquaroi = 0;


uint32_t ui32_timeout_mocuatren=0;
uint32_t ui32_timeout_mocuaduoi=0;

uint8_t ui8_phanhoi_TrangthaiLoi=0;

uint8_t buffer_send[15];
uint8_t buffer_receive[20];

uint8_t ui8_trangthaiRS485=0;
// uint8_t ui8_check_slave=0;
uint8_t vitri_khay=0;
uint8_t vitri_o=0;

uint32_t ui32_timeout_reset=0;
uint32_t ui32_timeout_resetledthatbai=0;
uint32_t ui8_timeout_ledthanhcong=0;
uint8_t ui8_kiemtratrangthaivat=0;
uint8_t ui8_trangthaimaythatbai=0;
uint8_t ui8_trangthaimaythanhcong=0;
uint8_t ui8_tatledthanhcong=0;
uint8_t ui8_dangchopled=0;
