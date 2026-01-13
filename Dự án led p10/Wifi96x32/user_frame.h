/**
 * @file 	user_frame.h
 * @version     1.01
 * @date        2021.11.27
 * @author 	Nguyen Thanh Cong
 * @contact     thanhcong402@gmail.com
 * @contribution 
 *
 * @description

 *
 * @license
 *
 */


#ifndef __USER_DEFINE_FRAME__
#define __USER_DEFINE_FRAME__

//"data": "{\"mid\":0,\"prio\":1,\"expired\": 1640856366,\"mode\":1,\"start\": 1638177966,\"ts\":[0,72000,86400],\"repeat\": 0,\"days\": 255,\"url\":\"https://alta-s3.dev-altamedia.com/lalova/chaomung.txt\",\"loop\":20}"
//h2/g/111 - {"id":0,"opcode":74,"data":{"duration":50400,"frequency":21,"swtime":1000,"mid":1743,"prio":4,"mode":2,"expired":1638491071,"start":1638438871,"ts":[],"repeat":0,"days":0,"ds":[],"header":255}}
//Wrong ID: 12111:0

/**
 * @enum MGW_OPCODE_CMD
 * @brief
 * opcode: mã lệnh của gói tin
 */
enum MGW_OPCODE_CMD
{    
    OPH_ACTIVATE			=0x01,	//!< Kích hoạt/ngừng kích hoạt MGW.
	OPH_GROUP_UPDATE		=0x03,	//!< Cập nhật danh sách các Group cho MGW
	OPH_NETWORK_CONFIG		=0x04,	//!< Cập nhật cấu hình mode AUTO/3G/4G
	OPH_MEDIA_HTTP			=0x43,	//!< Tạo bản tin dùng HTTP link để chứa file audio.
	OPH_MEDIA_UPDATE_2		=0x41,	//!< user communication
	OPH_MEDIA_EDIT_2		=0x45,	//!< user communication
	OPH_MEDIA_DELETE		=0x13,	//!< user communication
	OPH_MEDIA_DELETE_ALL	=0x1c,	//!< user communication
	OPH_FM_UPDATE_2			=0x4a,	//!< user communication
	OPH_LIVE_STREAM			=0x18,	//!< user communication
	OPH_HEADER_DATA			=0x16,	//!< user communication
	OPH_HEADER_PLAY			=0x17,	//!< user communication
	OPH_MEDIA_VOLUME		=0x14,	//!< user communication
	OPH_FM_CONFIG			=0x1b,	//!< user communication
	OPH_TIME				=0x31,	//!< user communication
	OPH_REBOOT				=0x32,	//!< Yêu cầu MGW khởi động lại
	OPH_UDF_2				=0x33,	//!< Server dùng gói tin này để yêu cầu MGW tự động kiểm tra và cập nhật phần mềm
	OPU_GENERIC				=0x82,	//!< user communication
	OPU_STATUS				=0x85,	//!< user communication
	OPU_MEDIA2				=0x87,	//!< user communication
	OPU_LOG					=0xb1,	//!< user communication
	OPU_USERLOG				=0xb2,	//!< user communication
	OPU_TIME_GET			=0xc1	//!< user communication	
};


/**
 * @enum MGW_UPDATE_CMD
 * @brief
 * MrCong moi them
 * phiên bản cũ lệnh là 
 * =2: upgrade online for led
 * other: upgrade online for wifi
 */
enum MGW_UPDATE_CMD
{    
    UPDATE_WIFI			=84,	//!< upgrade online for wifi
	UPDATE_LED			=85,	//!< upgrade online for led
	UPDATE_WIFI_OTA		=86,	//!< upgrade OTA/offline for wifi
	UPDATE_LED_OTA		=87	//!< upgrade OTA/offline for led	
};
/**
 * @enum OPU_STATUS_DISABLE
 * @brief
 * Trạng thái kích hoạt hệ thống
 */
enum OPU_STATUS_DISABLE
{    
    SYS_ACTIVE		=0,		//!< kích hoạt hệ thống
	SYS_DEACTIVE	=1		//!< ngừng kích hoạt hệ thống
};
/**
 * @enum NET_CMD
 * @brief
 * Indexes to config output frame positions
 */
enum NET_CMD
{    
    NET_RUNNING_CMD		=24,		//!< user communication
	NET_FUNCTION_NAME	=25,		//!< user communication
	NET_FUNC			=26,//41, //!< neu thay doi gia tri nay, nho thay doi trong file WiFiManager.cpp
	NET_OPENNAME		=27,//42, //!< position name open file
	NET_POLLINGNAME		=28//43, //!< position polling name from config wifi	//!< neu thay doi gia tri nay, nho thay doi trong file WiFiManager.cpp
};

/**
 * @enum WIFI_FC
 * @brief
 * Wifi function codes summary.
 * These are the implement function codes 
 */
enum WIFI_FC
{
    WF_FC_NONE                     	= 0,   /*!< null operator */
    WF_FC_SAVE          		    = 1,	/*!< Save data to file */
    WF_FC_PREVIEW					= 2,	/*!< preview on output */
	WF_FC_APPLY						= 3,	/*!< apply on output */
    WF_FC_LOAD           			= 4,	/*!< Load other file */    
	WF_FC_SAVE_LOAD					=5	/*!< Save and Load other file */    
};



#endif
