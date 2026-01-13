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

#include "WiFiManager.h"

WiFiManagerParameter::WiFiManagerParameter(const char *custom) {
  _id = NULL;
  _placeholder = NULL;
  _length = 0;
  _value = NULL;

  _customHTML = custom;
}

WiFiManagerParameter::WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length) {
  init(id, placeholder, defaultValue, length, "");
}

WiFiManagerParameter::WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom) {
  init(id, placeholder, defaultValue, length, custom);
}

void WiFiManagerParameter::init(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom) {
  _id = id;
  _placeholder = placeholder;
  _length = length;
  _value = new char[length + 1];
  for (int i = 0; i < length; i++) {
    _value[i] = 0;
  }
  if (defaultValue != NULL) {
    strncpy(_value, defaultValue, length);
  }

  _customHTML = custom;
}

const char* WiFiManagerParameter::getValue() {
  return _value;
}
const char* WiFiManagerParameter::getID() {
  return _id;
}
const char* WiFiManagerParameter::getPlaceholder() {
  return _placeholder;
}
int WiFiManagerParameter::getValueLength() {
  return _length;
}
const char* WiFiManagerParameter::getCustomHTML() {
  return _customHTML;
}

WiFiManager::WiFiManager() {
}

void WiFiManager::addParameter(WiFiManagerParameter *p) {
  if(_paramsCount + 1 > WIFI_MANAGER_MAX_PARAMS)
  {
    //Max parameters exceeded!
	DEBUG_WM("WIFI_MANAGER_MAX_PARAMS exceeded, increase number (in WiFiManager.h) before adding more parameters!");
	DEBUG_WM("Skipping parameter with ID:");
	DEBUG_WM(p->getID());
	return;
  }
  _params[_paramsCount] = p;
  _paramsCount++;
  DEBUG_WM("Adding parameter");
  DEBUG_WM(p->getID());
}

void WiFiManager::setupConfigPortal() {
  dnsServer.reset(new DNSServer());
#ifdef ESP8266
  server.reset(new ESP8266WebServer(80));
#else
  server.reset(new WebServer(80));
#endif

  DEBUG_WM(F(""));
  _configPortalStart = millis();

  DEBUG_WM(F("Configuring access point... "));
  DEBUG_WM(_apName);
  if (_apPassword != NULL) {
    if (strlen(_apPassword) < 8 || strlen(_apPassword) > 63) {
      // fail passphrase to short or long!
      DEBUG_WM(F("Invalid AccessPoint password. Ignoring"));
      _apPassword = NULL;
    }
    DEBUG_WM(_apPassword);
  }

  //optional soft ip config
  if (_ap_static_ip) {
    DEBUG_WM(F("Custom AP IP/GW/Subnet"));
    WiFi.softAPConfig(_ap_static_ip, _ap_static_gw, _ap_static_sn);
  }

  if (_apPassword != NULL) {
    WiFi.softAP(_apName, _apPassword);//password option
  } else {
    WiFi.softAP(_apName);
  }

  delay(500); // Without delay I've seen the IP address blank
  DEBUG_WM(F("AP IP address: "));
  DEBUG_WM(WiFi.softAPIP());

  /* Setup the DNS server redirecting all the domains to the apIP */
  dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer->start(DNS_PORT, "*", WiFi.softAPIP());

  /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */
  server->on("/", std::bind(&WiFiManager::handleRoot, this));
  //chức năng update firmware
  server->on("/updateFirm", std::bind(&WiFiManager::handleGetpasscode, this));
  server->on("/rootupdateFirm", std::bind(&WiFiManager::handleRoot_dapdatefirmware, this));
  server->on("/updateServerF", std::bind(&WiFiManager::handleUpdateServerF, this));
  server->on("/update", HTTP_POST,std::bind(&WiFiManager::handleUpdateServerresult,this), std::bind(&WiFiManager::handleUpdateServernow,this));    
  server->on("/updateLedF", std::bind(&WiFiManager::handleUpdateLedF, this));
  server->on("/action_page", std::bind(&WiFiManager::handlereadtest, this));
  
  // server->on("/wifi", std::bind(&WiFiManager::handleWifi, this, true));
  server->on("/0wifi", std::bind(&WiFiManager::handleWifi, this, false));
  server->on("/wifisave", std::bind(&WiFiManager::handleWifiSave, this));
  server->on("/configled", std::bind(&WiFiManager::handleLedSet, this));
  server->on("/e", std::bind(&WiFiManager::handleExit, this));
  // server->on("/i", std::bind(&WiFiManager::handleInfo, this));
  // server->on("/r", std::bind(&WiFiManager::handleReset, this));
  //server->on("/generate_204", std::bind(&WiFiManager::handle204, this));  //Android/Chrome OS captive portal check.
  server->on("/fwlink", std::bind(&WiFiManager::handleRoot, this));  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
  server->onNotFound (std::bind(&WiFiManager::handleNotFound, this));
  server->begin(); // Web server start
  DEBUG_WM(F("HTTP server started"));

}

boolean WiFiManager::autoConnect() {
#if defined(ESP8266)
	String ssid = "LMD" + String(ESP_getChipId());
#else
	char wifiname_id[23];
	snprintf(wifiname_id, 23, "LMD%lld", ESP.getEfuseMac());
	String ssid=String(wifiname_id);
#endif
  return autoConnect(ssid.c_str(), NULL);
}

boolean WiFiManager::autoConnect(char const *apName, char const *apPassword) {
  DEBUG_WM(F(""));
  DEBUG_WM(F("AutoConnect"));

  // read eeprom for ssid and pass
  //String ssid = getSSID();
  //String pass = getPassword();
	
	// Firstcheck mode congif wifi
	if(_configwifi==1){
		DEBUG_WM(F("Enter config portal"));
		return startConfigPortal(apName, apPassword);
	}
	else if(_configwifi==2){
		DEBUG_WM(F("Enter reset factory portal"));
		return startConfigPortalF(apName, apPassword);
	}
	else{
		  DEBUG_WM(F("Enter connect wifi"));
		  
		  WiFi.mode(WIFI_STA);
		  if (connectWifi(apName, apPassword) == WL_CONNECTED){
			DEBUG_WM(F("IP Address:"));
			DEBUG_WM(WiFi.localIP());
			//connected
			return true;
		  }
		  /*
		  if (connectWifi("", "") == WL_CONNECTED){
			DEBUG_WM(F("IP Address:"));
			DEBUG_WM(WiFi.localIP());
			//connected
			return true;
		  }
		  */
		DEBUG_WM(F("MrCong return no connect"));
		return false;
	}
  // attempt to connect; should it fail, fall back to AP
}

//-----------MrCong add here-----------------------------------------------------
void WiFiManager::setsavecommand(uint16_t *regs)
{
	au16regs =regs; 		
}
void WiFiManager::setsaveledcommand(uint16_t *regs)
{
	au16regsled =regs; 		
}


boolean WiFiManager::startConfigPortalF() {
#if defined(ESP8266)
	String ssid = "LMD" + String(ESP_getChipId());
#else
	char wifiname_id[23];
	snprintf(wifiname_id, 23, "LMD%lld", ESP.getEfuseMac());
	String ssid=String(wifiname_id);
#endif
  return startConfigPortalF(ssid.c_str(), NULL);
}

boolean  WiFiManager::startConfigPortalF(char const *apName, char const *apPassword) {
  //setup AP
  WiFi.mode(WIFI_AP_STA);
  DEBUG_WM("SET UP RESET FACTORY AP STA");

  _apName = apName;
  _apPassword = apPassword;

  //notify we entered AP mode
  if ( _apcallback != NULL) {
    _apcallback(this);
  }

  connectF = false;
  setupConfigPortalF();

  while(1){

    // check if timeout

	if(configPortalHasTimeout()) 
	{
		//need reset
		DEBUG_WM(F("Exit cause timeout"));
		// delay(2000);
		// ESP.restart();
		// delay(2000);
		break;
		
	}

    //DNS
    dnsServer->processNextRequest();
    //HTTP
    server->handleClient();

    if (connectF==1) {		
		DEBUG_WM(F("Button Exit pressed"));
		*au16regs=0;
		_savecallback();		
		connectF = false;
		break;
    } 
	else if (connectF==2) {		
		DEBUG_WM(F("Update Led Controller"));
		*au16regs=2;
		_savecallback();		
		connectF = false;		
    }
	else if (connectF==3 && bancodongykhong==1) {
		//nút save được nhấn				
		DEBUG_WM(F("Button 3 pressed and OK"));
		*au16regs=3;
		bancodongykhong=0;
		_savecallback();
		// delay(5000);
		// handleRootF();
		connectF = false;
    }

    yield();
  }

  server.reset();
  dnsServer.reset();  
  delay(200);

  return  WiFi.status() == WL_CONNECTED;
}

/**
setupConfigPortalF()

old name : setupConfigPortal

*/

void WiFiManager::setupConfigPortalF() {
  dnsServer.reset(new DNSServer());
#ifdef ESP8266
  server.reset(new ESP8266WebServer(80));
#else
  server.reset(new WebServer(80));
#endif

  DEBUG_WM(F(""));
  _configPortalStart = millis();

  DEBUG_WM(F("Configuring access point... "));
  DEBUG_WM(_apName);
  if (_apPassword != NULL) {
    if (strlen(_apPassword) < 8 || strlen(_apPassword) > 63) {
      // fail passphrase to short or long!
      DEBUG_WM(F("Invalid AccessPoint password. Ignoring"));
      _apPassword = NULL;
    }
    DEBUG_WM(_apPassword);
  }

  //optional soft ip config
  if (_ap_static_ip) {
    DEBUG_WM(F("Custom AP IP/GW/Subnet"));
    WiFi.softAPConfig(_ap_static_ip, _ap_static_gw, _ap_static_sn);
  }

  if (_apPassword != NULL) {
    WiFi.softAP(_apName, _apPassword);//password option
  } else {
    WiFi.softAP(_apName);
  }

  delay(500); // Without delay I've seen the IP address blank
  DEBUG_WM(F("AP IP address: "));
  DEBUG_WM(WiFi.softAPIP());

  /* Setup the DNS server redirecting all the domains to the apIP */
  dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer->start(DNS_PORT, "*", WiFi.softAPIP());

  /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */
  server->on("/", std::bind(&WiFiManager::handleRootF, this));
  // server->on("/wifiF", std::bind(&WiFiManager::handleResetFactoryF, this, true));
  // server->on("/0wifi", std::bind(&WiFiManager::handleWifi, this, false));
  
	//chức năng update firmware
  server->on("/updateFirm", std::bind(&WiFiManager::handleGetpasscode, this));
  server->on("/rootupdateFirm", std::bind(&WiFiManager::handleRoot_dapdatefirmware, this));
  server->on("/updateServerF", std::bind(&WiFiManager::handleUpdateServerF, this));
  server->on("/update", HTTP_POST,std::bind(&WiFiManager::handleUpdateServerresult,this), std::bind(&WiFiManager::handleUpdateServernow,this));    
  server->on("/updateLedF", std::bind(&WiFiManager::handleUpdateLedF, this));
  
  //nút exit
  server->on("/eF", std::bind(&WiFiManager::handleExit, this));
  server->on("/iF", std::bind(&WiFiManager::handleInfoF, this));
  server->on("/rF", std::bind(&WiFiManager::handleResetF, this));
    server->on("/yF", std::bind(&WiFiManager::handleYesF, this));
  server->on("/nF", std::bind(&WiFiManager::handleNoF, this));
  //server->on("/generate_204", std::bind(&WiFiManager::handle204, this));  //Android/Chrome OS captive portal check.
  server->on("/fwlink", std::bind(&WiFiManager::handleRootF, this));  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
  server->onNotFound (std::bind(&WiFiManager::handleNotFoundF, this));
  server->begin(); // Web server start
  DEBUG_WM(F("HTTP server started"));

}

/** Handle root or redirect to captive portal */
void WiFiManager::handleRootF() {
  DEBUG_WM(F("Handle root"));
  if (captivePortal()) { // If caprive portal redirect instead of displaying the page.
    return;
  }

  String page = FPSTR(HTTP_HEAD_CONG);
  page.replace("{v}", "Options");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);
  page += "<h1>";
  page += _apName;
  page += "</h1>";
  page += F("<h3>SystemManager</h3>");
  page += FPSTR(HTTP_PORTAL_OPTIONSF);
  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);
}


/** Handle root or redirect to captive portal */
void WiFiManager::handleRoot_dapdatefirmware() {
  DEBUG_WM(F("handleRoot_dapdatefirmware"));
  if (captivePortal()) { // If caprive portal redirect instead of displaying the page.
    return;
  }

  String page = FPSTR(HTTP_HEAD_CONG);
  page.replace("{v}", "Update Firm");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);
  page += "<h1>";
  page += _apName;
  page += "</h1>";
  page += F("<h3>Update Firmware</h3>");
  page += FPSTR(HTTP_PORTAL_UPDATEF);
  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);
  _configPortalStart = millis();
}

/** Handle the WLAN save form and redirect to WLAN config page again */

void WiFiManager::handleUpdateServerF() {
  DEBUG_WM(F("handleUpdateServerF"));
// String page="";
  
  String page = FPSTR(HTTP_HEAD_CONG);
  page.replace("{v}", "Update Server Controller");
  // page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);
  page += F("<h3>Update Firmware to Server Controller</h3>");
  
  page += F("<br/>Note: You are about to update the program for the server controller<br/>");
  page += F("Please choose the correct file format as:<br/>");
  page += F("<h3>Wifi*.bin</h3><br/>"); 
	
page += F("<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><br/><br/><br/><input type='submit' value='Update'></form>");
  
  // page += FPSTR(HTTP_HEAD_END);
  // page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);

_configPortalStart = millis();
}

/** Handle the WLAN save form and redirect to WLAN config page again */

void WiFiManager::handleUpdateServerresult() {
	DEBUG_WM(F("handleUpdateServerresult"));

	server->sendHeader("Connection", "close");
	server->send(200, "text/plain", (Update.hasError()) ? "FAIL la" : "OK la");
	ESP.restart();

}

void WiFiManager::handleUpdateServernow() {
  // DEBUG_WM(F("handleUpdateServernow"));

    HTTPUpload& upload = server->upload();
	
	if (upload.status == UPLOAD_FILE_START) {
	  // Serial.setDebugOutput(true);
	  // WiFiUDP::stopAll();
	  DEBUG_WM("Update: " + String(upload.filename));
	  uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
	  if (!Update.begin(maxSketchSpace)) { //start with max available size
		Update.printError(Serial);
	  }
	} 
	else if (upload.status == UPLOAD_FILE_WRITE) {
	  if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
		Update.printError(Serial);
		DEBUG_WM("Update: " + String(upload.currentSize));		
	  }
	  DEBUG_WM("c/t" + String(upload.currentSize) + "\t" + String(upload.totalSize));
	} 
	else if (upload.status == UPLOAD_FILE_END) {
	  if (Update.end(true)) { //true to set the size to the current progress		
		DEBUG_WM("Update Success: " + String(upload.totalSize));
		DEBUG_WM("Rebooting...");
	  } else {
		 DEBUG_WM(F("Update Error"));
		Update.printError(Serial);
	  }
	  // Serial.setDebugOutput(false);
	}
}
/*
void WiFiManager::handleUpdateServerF() {
  DEBUG_WM(F("handleUpdateServerF"));

  String page = FPSTR(HTTP_HEAD_CONG);
  page.replace("{v}", "Update Server Controller");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += F("Update server by Manual<br/>");
  page += F("Huong dan update o day");   
  page += FPSTR(HTTP_HEAD_END);
  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);

  DEBUG_WM(F("Sent wifi save page"));
  // delay(2000);

  connectF=1;

  // connect = true; //signal ready to connect/reset
}

*/

/** Handle the WLAN save form and redirect to WLAN config page again */
void WiFiManager::handleGetpasscode() {
  DEBUG_WM(F("handleGetpasscode"));
  
  String page = FPSTR(HTTP_HEAD_CONG);
  page.replace("{v}", "Log in");  
  page += FPSTR(getpasscode_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);
  
  page += "<h1>";
  page += _apName;
  page += "</h1>";
  page += F("<h3>Login</h3>");
  
  page += FPSTR(getpasscodeupdate);
  
  page += FPSTR(HTTP_END);
  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);
_configPortalStart = millis();
}
/*
void WiFiManager::handleGetpasscode() {
  DEBUG_WM(F("handleGetpasscode"));
  
  String page="";
  page +=getpasscodeupdate;

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);

}
*/

/** Handle the WLAN save form and redirect to WLAN config page again */
void WiFiManager::handleUpdateLedF() {
  DEBUG_WM(F("handleUpdateLedF"));

    String page = FPSTR(HTTP_HEAD_CONG);
  page.replace("{v}", "Update Led controller");
  // page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);
  page += F("<h3>Update Firmware to Led Matrix Display</h3>");
  
  page += F("<br/>Command was sent to led controller<br/>");
  page += F("Please open your network, connect to wifi OTALxxxx<br/>");
    
  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);
  connectF=2;
  _configPortalStart = millis();
}

void WiFiManager::handleYesF(){
	DEBUG_WM(F("Yes confirm"));
	
   String page = FPSTR(HTTP_HEAD_CONG);
  page.replace("{v}", "Yes Conrfirm");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += F("The command has been executed<br/>");  
  page += F("Click back to continues<br/>");
  page += FPSTR(HTTP_HEAD_END);
  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);

	bancodongykhong=1;	
}
void WiFiManager::handleNoF(){
	DEBUG_WM(F("No confirm"));
	
   String page = FPSTR(HTTP_HEAD_CONG);
  page.replace("{v}", "No Conrfirm");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += F("The command has been cancelled<br/>");      
  page += F("Click back to continues<br/>");
  page += FPSTR(HTTP_HEAD_END);
  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);
  
	bancodongykhong=2;	
}
void WiFiManager::handleExit(){
	DEBUG_WM(F("Exit confirm"));
	connectF=1;

	String page = FPSTR(HTTP_HEAD_CONG);
	page.replace("{v}", "Exit");
	page += FPSTR(HTTP_SCRIPT);
	page += FPSTR(HTTP_STYLE);
	page += _customHeadElement;
	page += FPSTR(HTTP_HEAD_END);
	page += "<h1>";
	page += _apName;
	page += "</h1>";
	page += F("<h3>Exit System Manager</h3>");

	page += FPSTR(HTTP_END);
	
  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);

}


/** Handle the info page */
void WiFiManager::handleInfoF() {
  DEBUG_WM(F("Info"));
	
	char wifiname_id[20];
	snprintf(wifiname_id, 20, "%lld", ESP.getEfuseMac());
		
		
  String page = FPSTR(HTTP_HEAD_CONG);
  page.replace("{v}", "Info");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);
  page += F("<dl>");
  
  page += F("<dt>Chip ID</dt><dd>");
  // page += ESP_getChipId();
  page += wifiname_id;
  page += F("</dd>");
  
  
  page += F("<dt>Flash Chip ID</dt><dd>");
#if defined(ESP8266)
  page += ESP.getFlashChipId();
#else
  // TODO
  page += F("TODO");
#endif
  page += F("</dd>");
  
  
  page += F("<dt>IDE Flash Size</dt><dd>");
  page += ESP.getFlashChipSize();
  page += F(" bytes</dd>");
  
  /*
  page += F("<dt>Real Flash Size</dt><dd>");
#if defined(ESP8266)
  page += ESP.getFlashChipRealSize();
#else
  // TODO
  page += F("TODO");
#endif
  page += F(" bytes</dd>");
  */
  // page += F("<dt>Soft AP IP</dt><dd>");
  // page += WiFi.softAPIP().toString();
  // page += F("</dd>");
  
  page += F("<dt>Soft AP MAC</dt><dd>");
  page += WiFi.softAPmacAddress();
  page += F("</dd>");
  page += F("<dt>Station MAC</dt><dd>");
  page += WiFi.macAddress();
  page += F("</dd>");
  
  
  page += F("</dl>");
  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);

  DEBUG_WM(F("Sent info page"));
_configPortalStart = millis();
}

/** Handle the reset page */
void WiFiManager::handleResetF() {
  DEBUG_WM(F("Reset factory"));

  String page = FPSTR(HTTP_HEAD_CONG);
  page.replace("{v}", "Reset factory");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);
  page += "<h1>";
  page += _apName;
  page += "</h1>";
  page += F("<h3>Reset factory</h3>");    
  page += F("<br/>WARNING!!!!!<br/>");
  page += F("If you Click YES, You will format all parameters to default values<br/><br/>");
  
  page += FPSTR(HTTP_FORM_YES_NO_END);
  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);

  connectF=3;
  _configPortalStart = millis();
}


void WiFiManager::handleNotFoundF() {
  if (captivePortal()) { // If captive portal redirect instead of displaying the error page.
    return;
  }
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server->uri();
  message += "\nMethod: ";
  message += ( server->method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server->args();
  message += "\n";

  for ( uint8_t i = 0; i < server->args(); i++ ) {
    message += " " + server->argName ( i ) + ": " + server->arg ( i ) + "\n";
  }
  server->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server->sendHeader("Pragma", "no-cache");
  server->sendHeader("Expires", "-1");
  server->sendHeader("Content-Length", String(message.length()));
  server->send ( 404, "text/plain", message );
}
//-----------End MrCong add here-----------------------------------------------------

boolean WiFiManager::configPortalHasTimeout(){
#if defined(ESP8266)
    if(_configPortalTimeout == 0 || wifi_softap_get_station_num() > 0){
#else
    if(_configPortalTimeout == 0){  // TODO
#endif
      _configPortalStart = millis(); // kludge, bump configportal start time to skew timeouts
      return false;
    }
    return (millis() > _configPortalStart + _configPortalTimeout);
}

boolean WiFiManager::startConfigPortal() {
#if defined(ESP8266)
	String ssid = "LMD" + String(ESP_getChipId());
#else
	char wifiname_id[23];
	snprintf(wifiname_id, 23, "LMD%lld", ESP.getEfuseMac());
	String ssid=String(wifiname_id);
#endif

  return startConfigPortal(ssid.c_str(), NULL);
}

boolean  WiFiManager::startConfigPortal(char const *apName, char const *apPassword) {
  //setup AP
  WiFi.mode(WIFI_AP_STA);
  DEBUG_WM("SET AP STA");

  _apName = apName;
  _apPassword = apPassword;

  //notify we entered AP mode
  if ( _apcallback != NULL) {
    _apcallback(this);
  }

  connect = false;
  connectF=0;
  setupConfigPortal();

  while(1){

    // check if timeout
    if(configPortalHasTimeout()) 
	{
		//need reset tại sao cần
		DEBUG_WM(F("Exit cause timeout"));
		// DEBUG_WM(F("Reset cause timeout"));
		// delay(2000);
		// ESP.restart();
		// delay(2000);
		break;
		
	}

    //DNS
    dnsServer->processNextRequest();
    //HTTP
    server->handleClient();

    if (connectF==1) {		
		DEBUG_WM(F("Button Exit pressed"));		
		connect = false;
		break;
    } 
	if (connect) {
      connect = false;
      delay(2000);
      DEBUG_WM(F("Connecting to new AP"));

      // using user-provided  _ssid, _pass in place of system-stored ssid and pass
      if (connectWifi(_ssid, _pass) != WL_CONNECTED) {
        DEBUG_WM(F("Failed to connect."));
      } else {
        //connected
        WiFi.mode(WIFI_STA);
        //notify that configuration has changed and any optional parameters should be saved
        if ( _savecallback != NULL) {
          //todo: check if any custom parameters actually exist, and check if they really changed maybe
          _savecallback();
        }
        break;
      }

      if (_shouldBreakAfterConfig) {
        //flag set to exit after config after trying to connect
        //notify that configuration has changed and any optional parameters should be saved
        if ( _savecallback != NULL) {
          //todo: check if any custom parameters actually exist, and check if they really changed maybe
          _savecallback();
        }
        break;
      }
    }
	else if (connectF==2) {		
		DEBUG_WM(F("Update Led Controller"));
		*au16regs=2;
		_savecallback();		
		connectF = false;		
    }
	else if (connectF==3) {		
		DEBUG_WM(F("Config Led Controller"));		
		_savecallback();		
		connectF = false;		
    }
    yield();
  }

  server.reset();
  dnsServer.reset();

  return  WiFi.status() == WL_CONNECTED;
}


int WiFiManager::connectWifi(String ssid, String pass) {
  DEBUG_WM(F("Connecting as wifi client..."));

  // check if we've got static_ip settings, if we do, use those.
  if (_sta_static_ip) {
    DEBUG_WM(F("Custom STA IP/GW/Subnet"));
    WiFi.config(_sta_static_ip, _sta_static_gw, _sta_static_sn);
    DEBUG_WM(WiFi.localIP());
  }
  //fix for auto connect racing issue
  if (WiFi.status() == WL_CONNECTED) {
    DEBUG_WM("Already connected. Bailing out.");
    return WL_CONNECTED;
  }
  //check if we have ssid and pass and force those, if not, try with last saved values
  if (ssid != "") {
    WiFi.begin(ssid.c_str(), pass.c_str());
  } else {
    if (WiFi.SSID()) {
      DEBUG_WM("Using last saved values, should be faster");
#if defined(ESP8266)
      //trying to fix connection in progress hanging
      ETS_UART_INTR_DISABLE();
      wifi_station_disconnect();
      ETS_UART_INTR_ENABLE();
#else
      esp_wifi_disconnect();
#endif

      WiFi.begin();
    } else {
      DEBUG_WM("No saved credentials");
    }
  }

  int connRes = waitForConnectResult();
  DEBUG_WM ("Connection result: ");
  DEBUG_WM ( connRes );
  //not connected, WPS enabled, no pass - first attempt
  if (_tryWPS && connRes != WL_CONNECTED && pass == "") {
    startWPS();
    //should be connected at the end of WPS
    connRes = waitForConnectResult();
  }
  return connRes;
}

uint8_t WiFiManager::waitForConnectResult() {
  if (_connectTimeout == 0) {
    return WiFi.waitForConnectResult();
  } else {
    DEBUG_WM (F("Waiting for connection result with time out"));
    unsigned long start = millis();
    boolean keepConnecting = true;
    uint8_t status;
    while (keepConnecting) {
      status = WiFi.status();
      if (millis() > start + _connectTimeout) {
        keepConnecting = false;
        DEBUG_WM (F("Connection timed out"));
      }
      if (status == WL_CONNECTED || status == WL_CONNECT_FAILED) {
        keepConnecting = false;
      }
      delay(100);
    }
    return status;
  }
}

void WiFiManager::startWPS() {
#if defined(ESP8266)
  DEBUG_WM("START WPS");
  WiFi.beginWPSConfig();
  DEBUG_WM("END WPS");
#else
  // TODO
  DEBUG_WM("ESP32 WPS TODO");
#endif
}

  String WiFiManager::getSSID() {
  if (_ssid == "") {
    DEBUG_WM(F("Reading SSID"));
    _ssid = WiFi.SSID();
    DEBUG_WM(F("SSID: "));
    DEBUG_WM(_ssid);
  }
  return _ssid;
  }

  String WiFiManager::getPassword() {
  if (_pass == "") {
    DEBUG_WM(F("Reading Password"));
    _pass = WiFi.psk();
    DEBUG_WM("Password: " + _pass);
    //DEBUG_WM(_pass);
  }
  return _pass;
  }

String WiFiManager::getConfigPortalSSID() {
  return _apName;
}

void WiFiManager::resetSettings() {
  DEBUG_WM(F("settings invalidated"));
  DEBUG_WM(F("THIS MAY CAUSE AP NOT TO START UP PROPERLY. YOU NEED TO COMMENT IT OUT AFTER ERASING THE DATA."));
  // TODO On ESP32 this does not erase the SSID and password. See
  // https://github.com/espressif/arduino-esp32/issues/400
  // For now, use "make erase_flash".
  WiFi.disconnect(true);
  //delay(200);
}
void WiFiManager::setTimeout(unsigned long seconds) {
  setConfigPortalTimeout(seconds);
}

void WiFiManager::setConfigPortalTimeout(unsigned long seconds) {
  _configPortalTimeout = seconds * 1000;
}

void WiFiManager::setConnectTimeout(unsigned long seconds) {
  _connectTimeout = seconds * 1000;
}

void WiFiManager::setConfigwifi(int value) {
  _configwifi = value;
}

void WiFiManager::setDebugOutput(boolean debug) {
  _debug = debug;
}

void WiFiManager::setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn) {
  _ap_static_ip = ip;
  _ap_static_gw = gw;
  _ap_static_sn = sn;
}

void WiFiManager::setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn) {
  _sta_static_ip = ip;
  _sta_static_gw = gw;
  _sta_static_sn = sn;
}

void WiFiManager::setMinimumSignalQuality(int quality) {
  _minimumQuality = quality;
}

void WiFiManager::setBreakAfterConfig(boolean shouldBreak) {
  _shouldBreakAfterConfig = shouldBreak;
}

/** Handle root or redirect to captive portal */
void WiFiManager::handleRoot() {
  DEBUG_WM(F("Handle root"));
  if (captivePortal()) { // If caprive portal redirect instead of displaying the page.
    return;
  }

  String page = FPSTR(HTTP_HEAD_CONG);
  page.replace("{v}", "Options");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);
  page += "<h1>";
  page += _apName;
  page += "</h1>";
  page += F("<h3>SystemManager</h3>");
  page += FPSTR(HTTP_PORTAL_OPTIONS);
  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);

}

/** Led config page handler */
void WiFiManager::handleLedSet() {

  String page = FPSTR(HTTP_HEAD_CONG);
  page.replace("{v}", "Config ESP");
  // page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);

  char parLength[2];
  // add the extra parameters to the form

  page += FPSTR(Leddropdown);

  // page += FPSTR(HTTP_FORM_END);
  // page += FPSTR(HTTP_SCAN_LINK);

  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);


  DEBUG_WM(F("Sent config page"));
}
void WiFiManager::handlereadtest() {
  DEBUG_WM(F("handlereadtest"));

  //parameters
    //read parameter

	String value;    
	value = server->arg("cars").c_str();
	if(value=="p10in")*au16regsled=1;
	else *au16regsled=2;
		
	value = server->arg("Width").c_str();    
	DEBUG_WM(value);
	*(au16regsled+1)=value.toInt();
	
	value = server->arg("Heigh").c_str();    
	DEBUG_WM(value);
	*(au16regsled+2)=value.toInt();
	
   String page = FPSTR(HTTP_HEAD_CONG);
  page.replace("{v}", "Completed");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += F("The command has been executed<br/>");  
  page += F("Click back to continues<br/>");
  page += FPSTR(HTTP_HEAD_END);
  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);

  DEBUG_WM(F("Sent save Led parameters"));

  // connect = true; //signal ready to connect/reset
  connectF=3;
  _configPortalStart = millis();
}
/** Wifi config page handler */
void WiFiManager::handleWifi(boolean scan) {

  String page = FPSTR(HTTP_HEAD_CONG);
  page.replace("{v}", "Config ESP");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);

  if (scan) {
    int n = WiFi.scanNetworks();
    DEBUG_WM(F("Scan done"));
    if (n == 0) {
      DEBUG_WM(F("No networks found"));
      page += F("No networks found. Refresh to scan again.");
    } else {

      //sort networks
      int indices[n];
      for (int i = 0; i < n; i++) {
        indices[i] = i;
      }

      // RSSI SORT

      // old sort
      for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
          if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) {
            std::swap(indices[i], indices[j]);
          }
        }
      }

      /*std::sort(indices, indices + n, [](const int & a, const int & b) -> bool
        {
        return WiFi.RSSI(a) > WiFi.RSSI(b);
        });*/

      // remove duplicates ( must be RSSI sorted )
      if (_removeDuplicateAPs) {
        String cssid;
        for (int i = 0; i < n; i++) {
          if (indices[i] == -1) continue;
          cssid = WiFi.SSID(indices[i]);
          for (int j = i + 1; j < n; j++) {
            if (cssid == WiFi.SSID(indices[j])) {
              DEBUG_WM("DUP AP: " + WiFi.SSID(indices[j]));
              indices[j] = -1; // set dup aps to index -1
            }
          }
        }
      }

      //display networks in page
      for (int i = 0; i < n; i++) {
        if (indices[i] == -1) continue; // skip dups
        DEBUG_WM(WiFi.SSID(indices[i]));
        DEBUG_WM(WiFi.RSSI(indices[i]));
        int quality = getRSSIasQuality(WiFi.RSSI(indices[i]));

        if (_minimumQuality == -1 || _minimumQuality < quality) {
          String item = FPSTR(HTTP_ITEM);
          String rssiQ;
          rssiQ += quality;
          item.replace("{v}", WiFi.SSID(indices[i]));
          item.replace("{r}", rssiQ);
#if defined(ESP8266)
          if (WiFi.encryptionType(indices[i]) != ENC_TYPE_NONE) {
#else
          if (WiFi.encryptionType(indices[i]) != WIFI_AUTH_OPEN) {
#endif
            item.replace("{i}", "l");
          } else {
            item.replace("{i}", "");
          }
          //DEBUG_WM(item);
          page += item;
          delay(0);
        } else {
          DEBUG_WM(F("Skipping due to quality"));
        }

      }
      page += "<br/>";
    }
  }

  page += FPSTR(HTTP_FORM_START);
  char parLength[2];
  // add the extra parameters to the form
  for (int i = 0; i < _paramsCount; i++) {
    if (_params[i] == NULL) {
      break;
    }

    String pitem = FPSTR(HTTP_FORM_PARAM);
    if (_params[i]->getID() != NULL) {
      pitem.replace("{i}", _params[i]->getID());
      pitem.replace("{n}", _params[i]->getID());
      pitem.replace("{p}", _params[i]->getPlaceholder());
      snprintf(parLength, 2, "%d", _params[i]->getValueLength());
      pitem.replace("{l}", parLength);
      pitem.replace("{v}", _params[i]->getValue());
      pitem.replace("{c}", _params[i]->getCustomHTML());
    } else {
      pitem = _params[i]->getCustomHTML();
    }

    page += pitem;
  }
  if (_params[0] != NULL) {
    page += "<br/>";
  }

  if (_sta_static_ip) {

    String item = FPSTR(HTTP_FORM_PARAM);
    item.replace("{i}", "ip");
    item.replace("{n}", "ip");
    item.replace("{p}", "Static IP");
    item.replace("{l}", "15");
    item.replace("{v}", _sta_static_ip.toString());

    page += item;

    item = FPSTR(HTTP_FORM_PARAM);
    item.replace("{i}", "gw");
    item.replace("{n}", "gw");
    item.replace("{p}", "Static Gateway");
    item.replace("{l}", "15");
    item.replace("{v}", _sta_static_gw.toString());

    page += item;

    item = FPSTR(HTTP_FORM_PARAM);
    item.replace("{i}", "sn");
    item.replace("{n}", "sn");
    item.replace("{p}", "Subnet");
    item.replace("{l}", "15");
    item.replace("{v}", _sta_static_sn.toString());

    page += item;

    page += "<br/>";
  }

  page += FPSTR(HTTP_FORM_END);
  page += FPSTR(HTTP_SCAN_LINK);

  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);


  DEBUG_WM(F("Sent config page"));
}

/** Handle the WLAN save form and redirect to WLAN config page again */
void WiFiManager::handleWifiSave() {
  DEBUG_WM(F("WiFi save"));

  //SAVE/connect here
  _ssid = server->arg("s").c_str();
  _pass = server->arg("p").c_str();

  //parameters
  for (int i = 0; i < _paramsCount; i++) {
    if (_params[i] == NULL) {
      break;
    }
    //read parameter
    String value = server->arg(_params[i]->getID()).c_str();
    //store it in array
    value.toCharArray(_params[i]->_value, _params[i]->_length);
    DEBUG_WM(F("Parameter"));
    DEBUG_WM(_params[i]->getID());
    DEBUG_WM(value);
  }

  if (server->arg("ip") != "") {
    DEBUG_WM(F("static ip"));
    DEBUG_WM(server->arg("ip"));
    //_sta_static_ip.fromString(server->arg("ip"));
    String ip = server->arg("ip");
    optionalIPFromString(&_sta_static_ip, ip.c_str());
  }
  if (server->arg("gw") != "") {
    DEBUG_WM(F("static gateway"));
    DEBUG_WM(server->arg("gw"));
    String gw = server->arg("gw");
    optionalIPFromString(&_sta_static_gw, gw.c_str());
  }
  if (server->arg("sn") != "") {
    DEBUG_WM(F("static netmask"));
    DEBUG_WM(server->arg("sn"));
    String sn = server->arg("sn");
    optionalIPFromString(&_sta_static_sn, sn.c_str());
  }

  String page = FPSTR(HTTP_HEAD_CONG);
  page.replace("{v}", "Credentials Saved");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);
  page += FPSTR(HTTP_SAVED);
  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);

  DEBUG_WM(F("Sent wifi save page"));

  connect = true; //signal ready to connect/reset
}

/** Handle the info page */
void WiFiManager::handleInfo() {
  DEBUG_WM(F("Info"));
char wifiname_id[20];
	snprintf(wifiname_id, 20, "%lld", ESP.getEfuseMac());

  String page = FPSTR(HTTP_HEAD_CONG);
  page.replace("{v}", "Info");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);
  page += F("<dl>");
  page += F("<dt>ID</dt><dd>");
  // page += ESP_getChipId();
  page += wifiname_id;
  
  page += F("</dd>");
  page += F("<dt>Flash Chip ID</dt><dd>");
#if defined(ESP8266)
  page += ESP.getFlashChipId();
#else
  // TODO
  page += F("TODO");
#endif
  
  page += F("</dd>");
  page += F("<dt>IDE Flash Size</dt><dd>");
  page += ESP.getFlashChipSize();
  page += F(" bytes</dd>");
  /*
  page += F("<dt>Real Flash Size</dt><dd>");
#if defined(ESP8266)
  page += ESP.getFlashChipRealSize();
#else
  // TODO
  page += F("TODO");
#endif
  page += F(" bytes</dd>");
  page += F("<dt>Soft AP IP</dt><dd>");
  page += WiFi.softAPIP().toString();
  page += F("</dd>");
 */ 
  
  page += F("<dt>Soft AP MAC</dt><dd>");
  page += WiFi.softAPmacAddress();
  page += F("</dd>");
  page += F("<dt>Station MAC</dt><dd>");
  page += WiFi.macAddress();
  page += F("</dd>");
  page += F("</dl>");
  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);

  DEBUG_WM(F("Sent info page"));
  _configPortalStart = millis();
}

/** Handle the reset page */
void WiFiManager::handleReset() {
  DEBUG_WM(F("Reset"));

  String page = FPSTR(HTTP_HEAD_CONG);
  page.replace("{v}", "Info");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);
  page += F("Module will reset in a few seconds.");
  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);

  DEBUG_WM(F("Sent reset page"));
  delay(5000);
#if defined(ESP8266)
  ESP.reset();
#else
  ESP.restart();
#endif
  delay(2000);
}

void WiFiManager::handleNotFound() {
  if (captivePortal()) { // If captive portal redirect instead of displaying the error page.
    return;
  }
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server->uri();
  message += "\nMethod: ";
  message += ( server->method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server->args();
  message += "\n";

  for ( uint8_t i = 0; i < server->args(); i++ ) {
    message += " " + server->argName ( i ) + ": " + server->arg ( i ) + "\n";
  }
  server->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server->sendHeader("Pragma", "no-cache");
  server->sendHeader("Expires", "-1");
  server->sendHeader("Content-Length", String(message.length()));
  server->send ( 404, "text/plain", message );
}


/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
boolean WiFiManager::captivePortal() {
  if (!isIp(server->hostHeader()) ) {
    DEBUG_WM(F("Request redirected to captive portal"));
    server->sendHeader("Location", String("http://") + toStringIp(server->client().localIP()), true);
    server->send ( 302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server->client().stop(); // Stop is needed because we sent no content length
    return true;
  }
  return false;
}

//start up config portal callback
void WiFiManager::setAPCallback( void (*func)(WiFiManager* myWiFiManager) ) {
  _apcallback = func;
}

//start up save config callback
void WiFiManager::setSaveConfigCallback( void (*func)(void) ) {
  _savecallback = func;
}

//sets a custom element to add to head, like a new style tag
void WiFiManager::setCustomHeadElement(const char* element) {
  _customHeadElement = element;
}

//if this is true, remove duplicated Access Points - defaut true
void WiFiManager::setRemoveDuplicateAPs(boolean removeDuplicates) {
  _removeDuplicateAPs = removeDuplicates;
}



template <typename Generic>
void WiFiManager::DEBUG_WM(Generic text) {
  if (_debug) {
    Serial.print("*WM: ");
    Serial.println(text);
  }
}

int WiFiManager::getRSSIasQuality(int RSSI) {
  int quality = 0;

  if (RSSI <= -100) {
    quality = 0;
  } else if (RSSI >= -50) {
    quality = 100;
  } else {
    quality = 2 * (RSSI + 100);
  }
  return quality;
}

/** Is this an IP? */
boolean WiFiManager::isIp(String str) {
  for (int i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}

/** IP to String? */
String WiFiManager::toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}
