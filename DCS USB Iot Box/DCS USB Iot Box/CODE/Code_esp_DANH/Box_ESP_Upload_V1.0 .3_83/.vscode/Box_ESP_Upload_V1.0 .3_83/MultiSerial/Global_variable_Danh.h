#define TG_SLEEP_PRINTER 30*60*1000
#define TG_SLEEP_BOX 24*60*60*1000
#define TG_CHECK_SNIFF 20000
#define TG_TIMEOUT_WEBCONFIG 2*60*1000
#define TG_DISCONNECT_MQTT 10000
#define TONG_KICHTHUOCBILL_CHOPHEP 100// mac dinh 1024
#define TG_KICHPIN 15*60*1000
const char* filePath   = "/data.txt";

int res_printer = 0;
int res_barcode = 0;
uint8_t ui8_upload_codeSTM32=0;
uint8_t ui8_ketnoi_github = 0;
// uint8_t ui8_update_esp32 = 0;
uint8_t ui8_ketnoi_github_4G = 0;
uint8_t ui8_ketnoi_github_4G_barcode = 0;
uint8_t ui8_uploadfile_STM32_barcode = 0;
uint8_t ui8_upload_codeSTM32_barcode = 0;
uint8_t ui8_dem_solanketnoi_barcode = 0;
uint8_t ui8_dem_solanketnoi_printer = 0;
uint8_t ui8_update_esp32 = 0;
uint8_t ui8_ketqua_uart = 0;
uint8_t ui8_kiemtra_biensavefile = 0;
bool stringComplete_serial=false;
String index_serial="";
uint8_t ui8_update_mqtt_4g=0;
uint8_t ui8_dem_sl_ketnoi_4g = 0;
uint8_t ui8_dem_sl_ketnoi_4g_o_for=0;
int kt_connect=0;
uint8_t ui8_dem_solan_ketnoimqtt = 0;
uint32_t u32_time_connected_mqtt = 0;
uint8_t ui8_danggui_thongtin_board=0;
uint8_t ui8_sub = 0;
uint32_t u32_timeout_checkinfo_printer = 0;
uint32_t u32_timeout_checkstatus_printer = 0;
uint32_t u32_timeout_checkserinumber_printer = 0;
uint8_t ui8_check_manufacturer = 0;
uint8_t dem_solan_checkmanu = 0;
uint8_t dem_solan_checkstatus_printer = 0;
uint8_t dem_solan_checkserialnumber_printer = 0;
uint8_t ui8_check_serialnumber_printer = 0;
uint8_t ui8_check_product_printer = 0;
char manufacturer[20];
char product[20];
char Serial_number[20];
uint8_t ui8_kt_chunk = 0;
uint8_t ui8_kt_chunk_header=0;
uint8_t ui8_kt_sniff=0;
uint8_t ui8_demsolan_sniff=0;
char name_topic_header[100];
uint8_t ui8_kiemtra_header=0;
uint32_t ui32_timeout_check_set=0;
uint8_t ui8_init_stm32_printer = 0;
uint8_t ui8_init_stm32_barcode = 0;
uint8_t ui8_restart_esp32 = 0;
uint32_t ui32_timeout_restart_esp32 = 0;
uint8_t ui8_dem_solantruycap_tenmayin = 0;
uint8_t ui8_dem_solantruycap_soserimayin = 0;
uint8_t ui8_dem_solantruycap_productname = 0;
String header_tam="";
String header_tam_1="";
String header_tam_2="";
uint8_t ui8_checkketnoi_manufacturer=0;
uint8_t ui8_checkketnoi_product_printer=0;
uint8_t ui8_checkketnoi_serialnumber_printer=0;

uint8_t ui8_guiinfo_manu = 0;
uint8_t ui8_guistatus_manu = 0;
uint8_t ui8_guiseri_manu = 0;

uint8_t ui8_demsolan_ketnoi_mqtt = 0;

uint32_t ui32_timeout_check_barcode = 0;
uint8_t ui8_demsolan_barcode = 0;
uint8_t ui8_truycap_mqtt = 0;

uint8_t ui8_dangconnect_mqtt = 0;
uint8_t ui8_checkbarcode_landau = 0;

uint32_t ui32_timeout_connectmqtt=0;

uint8_t ui8_ketnoithanhcong_mqtt = 0;
uint8_t ui8_dangupdate_printer = 0;
uint8_t ui8_dangupdate_barcode = 0;
uint8_t ui8_dangupdate_esp = 0;

uint8_t ui8_ketnoi_esp_4G=0;
uint8_t ui8_update_esp_4G=0;
uint8_t ui8_haynap_esp=0;

uint32_t ui32_timeout_thoat_config = 0;
uint8_t ui8_dangvao_web = 0;
uint8_t ui8_dangin=0;


uint32_t ui32_timeout_checkpower=0;
uint32_t ui32_timeout_checkprinter=0;
uint32_t ui32_timeout_checksleepbox = 0;
uint8_t ui8_modebusy_printer = 0;

uint32_t ui32_timeout_printsniff = 0;
uint8_t ui8_check_disconnect_mqtt = 0;
uint32_t ui32_timeout_disconnect_mqtt=0;
uint32_t ui8_candisconnectmqtt=0;
uint8_t ui8_solan_pubbill = 0;
uint8_t ui8_connect_mqtt_atcmd=0;
uint8_t ui8_ketnoimqtt_ok=0;
uint8_t ui8_uploadlai=0;
uint8_t ui8_demsolanguisai_info=0;
uint8_t ui8_demsolanguisai_chunk=0;
uint8_t ui8_demsolangui_RESP = 0;
uint8_t ui8_solan_disconnect = 0;
uint32_t ui32_timeout_guilailannua=0;
uint8_t ui8_danggui_chunk=0;
uint8_t ui8_luafw_printer = 0;
uint8_t ui8_upfw_barcode = 0;
uint8_t ui8_upfw_server = 0;
uint8_t ui8_rs_server=0;
uint8_t ui8_rs_printer=0;
uint8_t ui8_rs_barcode=0;
uint8_t ui8_format_disk=0;
uint8_t ui8_check_power=0;
uint8_t ui8_solan_kt=0;
uint8_t ui8_dangupdate_mqtt=0;
uint32_t ui32_timeout_formatdisk = 0;
uint8_t ui8_uploi=0;
uint8_t ui8_reset_box=0;
uint8_t ui8_chophep_reset=0;

uint8_t ui8_canformatdisk=0;

const char* host       = "upload-test.dev-altamedia.com";    // không kèm http://
const uint16_t port    = 80;
const char* serverPath = "/upload.php";

struct info_mqtt
{
	// uint8_t 	status_set=0;
	// // uint8_t 	chunk;
	// uint8_t 	index = 0;
	char name_domain[24];
	char username_mqtt[24];
    char password_mqtt[24];
};

info_mqtt info_of_mqtt;


#define PPP_MODEM_APN "v-internet"
#define PPP_MODEM_PIN NULL  // or NULL

// WaveShare SIM7600 HW Flow Control
#define PPP_MODEM_RST       1
#define PPP_MODEM_RST_LOW   false  //active HIGH
#define PPP_MODEM_RST_DELAY 2000
#define PPP_MODEM_TX        4
#define PPP_MODEM_RX        5
#define PPP_MODEM_RTS       -1
#define PPP_MODEM_CTS       -1
#define PPP_MODEM_FC        ESP_MODEM_FLOW_CONTROL_NONE
#define PPP_MODEM_MODEL     PPP_MODEM_SIM7600