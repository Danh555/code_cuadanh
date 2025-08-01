/**************************************************************
   WiFiManager is a library for the ESP8266/Arduino platform
   (https://github.com/esp8266/Arduino) to enable easy
   configuration and reconfiguration of WiFi credentials using a Captive Portal
   inspired by:
   http://www.esp8266.com/viewtopic.php?f=29&t=2520
   https://github.com/chriscook8/esp-arduino-apboot
   https://github.com/esp8266/Arduino/tree/master/libraries/DNSServer/examples/CaptivePortalAdvanced
   Built by AlexT https://github.com/tzapu
   Licensed under MIT license
 **************************************************************/

#ifndef SettingServer_h
#define SettingServer_h

const char HTTP_HEAD_CONG[] PROGMEM            = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";
const char HTTP_STYLE[] PROGMEM           = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;}</style>";
const char HTTP_SCRIPT[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const char HTTP_HEAD_END[] PROGMEM        = "</head><body text=#ffffff bgcolor=##000000><div style='text-align:left;display:inline-block;min-width:260px;'>";
// const char HTTP_PORTAL_OPTIONS[] PROGMEM  = "<form action=\"/wifi\" method=\"get\"><button>Configure WiFi</button></form><br/><form action=\"/0wifi\" method=\"get\"><button>Configure WiFi (No Scan)</button></form><br/>";
const char HTTP_PORTAL_OPTIONS[] PROGMEM  = "<form action=\"/wifi\" method=\"get\"><button>Configure WiFi</button></form>";

// const char HTTP_HOTLINE[] PROGMEM             = "<br/><br/><br/><h3>Hotline: Ms Trang 0989 050 705</h3>";
const char HTTP_HOTLINE[] PROGMEM             = "<br/><br/><br/><h3>Hotline: 0915.947.117</h3>";

//<form action=\"/i\" method=\"get\"><button>Info</button></form><br/><form action=\"/r\" method=\"post\"><button>Reset</button></form>";
const char HTTP_ITEM[] PROGMEM            = "<div><a href='#p' onclick='c(this)'>{v}</a>&nbsp;<span class='q {i}'>{r}%</span></div>";
// const char HTTP_FORM_START[] PROGMEM      = "<form method='get' action='wifisave'><input id='s' name='s' length=32 placeholder='SSID'><br/><input id='p' name='p' length=64 type='password' placeholder='password'><br/>";
const char HTTP_FORM_START[] PROGMEM      = "<form method='get' action='wifisave'><br/>";
const char HTTP_FORM_PARAM[] PROGMEM      = "<br/><input id='{i}' name='{n}' length={l} placeholder='{p}' value='{v}' {c}>";
const char HTTP_FORM_END[] PROGMEM        = "<br/><button type='submit'>save</button></form>";
const char HTTP_SCAN_LINK[] PROGMEM       = "<br/><div class=\"c\"><a href=\"/wifi\">Scan</a></div>";
const char HTTP_HOME_LINK[] PROGMEM       = "<br/><div class=\"c\"><a href=\"/Login\">Back</a></div>";
const char HTTP_SAVED[] PROGMEM           = "<div>Credentials Saved<br />Trying to connect Weread to network.<br />If it fails reconnect to AP to try again</div>";
const char HTTP_END[] PROGMEM             = "</div></body></html>";
const char HTTP_NW_MODE_SELECTION[] PROGMEM      = "<br/><label for=\"modef1\">-Network Mode :</label><select name=\"modef1\" id=\"modef11\"><option value=\"1\">Wifi</option><option value=\"0\">Ethernet</option><option value=\"2\">4G</option><option value=\"3\">None</option></select>";
// const char HTTP_NW_MODE_SELECTION[] PROGMEM      = "<br/><label for=\"modef1\">-Network Mode :</label><select name=\"modef1\" id=\"modef11\"><option value=\"1\">Wifi</option><option value=\"0\">Wifi</option><option value=\"2\">Wifi</option><option value=\"3\">Wifi</option></select>";

struct str_system_info
{    
	char wifi_name[20];
	char wifi_pass[20];
	char apn[20];
	char use_network[2];
	char use_ethernet[2];
	char use_wifi[2];
	char use_modem[2];
  char save_file_config[20];
  char USERNAME[20];
  char PASSWORD[20];
  char log_debug[20];
  char name_domain[40];
	char username_mqtt[40];
  char password_mqtt[40];
  char header_ID[40];
};

str_system_info systeminfo;

#endif
