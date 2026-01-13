
struct INFO_MQTT
{
  char sessionId[50];
  char code[30];
};

INFO_MQTT info_mqtt;

struct INFO_WIFI
{
  char* ssid     = "AM-RD2.4";
  char* pass = "AmRD@2025";
};

INFO_WIFI info_wifi;

struct INFO_HTTPS
{
  char* host       = "qms-api.dev.altasoftware.vn";    // không kèm http://
  uint16_t port    = 443;
  char* serverPath = "/upload.php";
  char* filePath   = "/data.txt";
  int uploadCounter = 50;
};

INFO_HTTPS info_https;