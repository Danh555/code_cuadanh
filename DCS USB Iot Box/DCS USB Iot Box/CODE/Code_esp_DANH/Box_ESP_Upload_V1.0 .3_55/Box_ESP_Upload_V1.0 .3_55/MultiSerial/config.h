/**
   @file 	user_frame.h
   @version     1.01
   @date        2021.11.27
   @author 	Nguyen Thanh Cong
   @contact     thanhcong402@gmail.com
   @contribution

   @description


   @license

*/


#ifndef _config_MultiSerial_
#define _config_MultiSerial_
 
const char *name_up_manager = "/up_manage.sys";
const char *name_disk_info = "/disk.sys";

#define max_file_save 130		//hỗ trợ lưu tối đa 130 file


#define DF_mqtt_modem 1
#define DF_UPFILE_USING_MQTT 1
#define DF_USING_EXT_FLASH 1
#define DF_MQTT_UP_4G 1
#define DF_MQTT_MODEM 1
#define code_anhcong 0

/*các biến dùng để debug code*/
#define DF_DEBUG_NOUPLOAD_TO_SERVER 0 /* =1: no upload, =0: normal*/
#define DF_DEBUG_PRINT_EXT_FLASH 0
#define DF_USING_BARCODE 1 /* =0: disbale barcode, =1: normal*/
#define DF_USING_CHECKTIME 1 /* =0: disbale checktime, =1: normal*/

#endif