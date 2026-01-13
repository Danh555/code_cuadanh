  
#ifndef __user_define_matrix__

#define __user_define_matrix__

/**
 * @enum LMD_MODE
 * @brief
 * kiểu chạy chữ
 */
enum LMD_MODE
{    
    LMD_UNKNOW			=0,//!< Chưa định nghĩa
	LMD_MOVER			=1,
	LMD_MOVEL			=2,
	LMD_MOVEUP		=3,
	LMD_MOVEDOWN		=4,
	LMD_STATIC		=5
};

/**
 * @enum LMD_SPEED
 * @brief
 * tốc độ chạy
 */
enum LMD_SPEED
{    
    LMD_SPEED_FAST			=1,
	LMD_SPEED_SLOW			=2,
	LMD_SPEED_NORMAL		=3
	
};

enum system_exe
{
  SYS_NORMAL = 0,
  SYS_ERR0,//wifi không kết nối được
  SYS_ERR_SERVER,
  SYS_WAIT_RESET,
  SYS_DISCONNECT
};

enum scanmode_led_display
{
  
  LEDMODE_INDOOR = 1 , 
  LEDMODE_OUTDOOR = 2
  
};

const char *sys_filename="/sysconfig.json";

//default values
const char ssid[] = "LED 1";
// const char pass[] = "Altaledmbf$";
const char pass[] = "Altaledmbf@";

#define df_wifi_name "LED 1"
#define df_wifi_pass "Altaledmbf$"
#define df_wifi_scanmode 0
#define df_led_kind 2
#define df_led_width 64
#define df_led_heigh 32


#define e_address_wifi 4
#define df_max_color_support 30

#define time_enable_ota 600000//60s 10m*60*1000

#endif