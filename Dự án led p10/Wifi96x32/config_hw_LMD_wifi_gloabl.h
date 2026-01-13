/**
Chú ý setup wifi:
Bảng 1 chuyển đi Hà Nội: (HW V0)
	dùng modem4G IT đưa, 
	wifi=	{"LED 1","Altaledmbf$"};
	ID =	88171961787146789
	server=	mbs.mobifone.vn
	use_manhinhDemoHaNoi

Bảng 2 chuyển đi Đồng Nai:(HW V1) 
	dùng modem4G IT đưa, 
	wifi=	{"LED 1","Altaledmbf$"};
	ID =	60171961787146666
	server=	mbs.mobifone.vn
	use_manhinh2DongNai

Bảng 3 chuyển đi Cần Thơ: (HW V1)
	dùng modem4G IT đưa, 
	wifi=	{"LED 1","Altaledmbf@"};
	ID =	2022001149383402
	server=	mbs.mobifone.vn
	use_main_wifi

Bảng 4 chuyển đi Cần Thơ ngày 9/3/2022: (HW V1)
	dùng modem4G mới mua, 
	wifi=	{"LED 1","Altaledmbf@"};
	ID =	2022001149383403
	server=	mbs.mobifone.vn
	use_main_wifi
	
*/
  
#ifndef __CONFIG_HARDWARE__

#define __CONFIG_HARDWARE__



// #define debug_mode //dùng debug đọc serial, khi chạy chính thức thì comment dòng này lại


//-----------chọn phần cứng sử dụng--------------------------------

#define use_main_wifi //Board điều khiển V1, dùng chức năng wifi==áp dụng cho 20 bảng led sau này
// #define use_main_wifi_v2 //Board điều khiển V2, dùng chức năng wifi==áp dụng cho 43 bảng led sau này

// #define use_manhinhDemoHaNoi	//Board điều khiển V0, khung sắt, không ốp Mica, bảng demo đầu tiên mang ra Hà Nội	


// #define use_manhinh2DongNai //Board điều khiển V2, khung nhôm, có ốp Mica, bảng thứ 2
// #define use_debugRD//Board điều khiển V2, dùng debug, test code


//-----------chọn server--------------------------------
// #define df_server "ttcs.thuathienhue.gov.vn"
#define df_server "mbs.mobifone.vn"
// #define df_server "vernemq.altacloud.biz"


#if defined (use_manhinhDemoHaNoi)
	#define hwv0	// hwv0: bảng demo ngoài Hà Nội
	#define hardwareversionname "hwv0 HaNoi"
	//link update firmware
	
	//link cũ update 
	// #define URL_fw_Version "https://raw.githubusercontent.com/DaikCong/Firmware-LMD-Mobiphone/main/bin_version.txt"
	// #define URL_fw_Bin "https://raw.githubusercontent.com/DaikCong/Firmware-LMD-Mobiphone/main/Wifi96x32.ino.esp32.bin"
	
	//cập nhật sang link mới
	#define URL_fw_Version "https://raw.githubusercontent.com/DaikCong/FirmwareLMD20board/main/bin_version.txt"
	#define URL_fw_Bin "https://raw.githubusercontent.com/DaikCong/FirmwareLMD20board/main/Wifi96x32.hwv0.bin"
	
	
	#define df_wifi_name "LED 1"
	#define df_wifi_pass "Altaledmbf$"
	// #define df_device_code "88171961787146789"
	// #define df_filename "/system16.json"//cũ đang là 16
	
	#define df_filename "/system.json"

#elif defined (use_manhinh2DongNai)
	#define hwv1	//hw1: phần cứng phiên bản 1, làm mạch in, chỉ có 4G
	#define hardwareversionname "hwv1 DongNai"
	//link update firmware
	#define URL_fw_Version "https://raw.githubusercontent.com/DaikCong/Firmware-LMD-DongNai/main/bin_version.txt"	
	#define URL_fw_Bin "https://raw.githubusercontent.com/DaikCong/Firmware-LMD-DongNai/main/Wifi96x32.ino.bin"
	
	#define df_wifi_name "LED 1"
	#define df_wifi_pass "Altaledmbf$"
	// #define df_device_code "60171961787146666"
	#define df_filename "/system12.	"

#elif	defined (use_debugRD)
	#define hwv1	//hw1: phần cứng phiên bản 1, làm mạch in, chỉ có 4G
	#define hardwareversionname "hwv1 developed"
	//link update firmware
		#define URL_fw_Version "https://raw.githubusercontent.com/DaikCong/Firmware-LMD-DebugRD/main/bin_version.txt"
	#define URL_fw_Bin "https://raw.githubusercontent.com/DaikCong/Firmware-LMD-DebugRD/main/WifiLedcommunicate.bin"
	
	#define df_wifi_name "LED 1"
	#define df_wifi_pass "Altaledmbf$"
	#define df_filename "/system17.json"

#elif	defined (use_main_wifi)
	#define hwv1	//hw1: phần cứng phiên bản 1, làm mạch in, chỉ có 4G
	#define hardwareversionname "hwv1 Main"
	
	//link update firmware
	
	// #define URL_fw_Version "https://raw.githubusercontent.com/DaikCong/Mobifone_Thongtindientu/HardwareV1/bin_version.txt"
	// #define URL_fw_Bin "https://raw.githubusercontent.com/DaikCong/Mobifone_Thongtindientu/HardwareV1/Wifi96x32.ino.bin"

	#define URL_fw_Version "https://raw.githubusercontent.com/DaikCong/FirmwareLMD20board/main/bin_version.txt"
	#define URL_fw_Bin "https://raw.githubusercontent.com/DaikCong/FirmwareLMD20board/main/Wifi96x32.hwv1.bin"

	#define df_wifi_name "LED 1"
	#define df_wifi_pass "Altaledmbf1"
	#define df_filename "/system1.json"
	
#elif	defined (use_main_wifi_v2)
	#define hwv2
	#define hardwareversionname "hwv2 Main"
	
	//link update firmware
	
	// #define URL_fw_Version "https://raw.githubusercontent.com/DaikCong/Mobifone_Thongtindientu/HardwareV1/bin_version.txt"
	// #define URL_fw_Bin "https://raw.githubusercontent.com/DaikCong/Mobifone_Thongtindientu/HardwareV1/Wifi96x32.ino.bin"

	#define URL_fw_Version "https://raw.githubusercontent.com/DaikCong/FirmwareLMD20board/main/bin_versionv2.txt"
	#define URL_fw_Bin "https://raw.githubusercontent.com/DaikCong/FirmwareLMD20board/main/Wifi96x32.hwv2.bin"

	#define df_wifi_name "LED 1"
	#define df_wifi_pass "Altaledmbf1"
	#define df_filename "/system1.json"
	
#else
#erro "Please select one of use_code"
#endif

/*
#define df_device_code     String("2022001" + String(((uint32_t)ESP.getEfuseMac())))
#define df_id_mqtt     String(((uint32_t)ESP.getEfuseMac()))
#define df_user_mqtt     String("user" + String(((uint32_t)ESP.getEfuseMac())))
#define df_pass_mqtt     String("pass" + String(((uint32_t)ESP.getEfuseMac())))
*/

#define df_data_mode 0
#define df_port	"1883"

// #define df_user_wifi     String("LMD_" + String(((uint32_t)ESP.getEfuseMac())))
#define df_pass_wifi "66669999"


#define df_user_up_server     String("LMD_SEVER_" + String(((uint32_t)ESP.getEfuseMac())))//dùng update server


#define df_pass_firm "12ab34cd"
#define df_time_send_opu (1*60*1000)

#define df_time_disconnect 150000//150S=150*1000ms=150000ms// thời gian mất kết nối: board sẽ tự động vào OTA
//150S=150*1000ms

/*
Qui định đặt tên phiên bản

x.y.z

x: x: phiên bản hardware
	=0: hàn tay, bảng demo ngoài Hà Nội
	=1: Phiên bản 1	
	=2: Phiên bản 2	
y: phiên bản phần mềm
	=0: bảng led demo đầu tiên
	=1: bảng led Đồng Nai
	=2: bảng led Cần Thơ..
	=3: 5/20 bảng led sử dụng led outdoor.-->các bản sau này chuyển thành 4 
	=4: /43 bảng led sử dụng led outdoor.
z: lần build thứ mấy

*/

#if defined (hwv0)
	#define versionname "0.3.5"
#elif defined (hwv1)
	#define versionname "1.4.3"
#elif defined (hwv2)
	#define versionname "2.4.3"
#else
	#error "Please define one of use_ in config_hw_local.h"
#endif



#endif