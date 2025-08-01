#pragma once
/*
* File: GlobalVariables.h
* Date: 30/03/2023
* Description: This is file to declare global variables
*/

/*
	- TỐI ƯU QUAY NGHỊCH TRONG KIẾM TRA MÁY NGIỀN (XONG) --- CẦN TỐI ƯU THÊM 
	- THÊM QUAY NGHICH KHI SỐ CHAI > 10 (XONG)
	- KHÔNG CÓ LON RỚT XUỐNG VẪN ÉP 1 LẦN (XONG)
	- TIME OUT SỐ LẦN ÉP (XONG)
	- ĐẾM SỐ CHAI VA LON TRONG THÙNG RÁC ĐỂ TÍNH RA % THÙNG ĐÃ SỬ DỤNG GỬI LÊN PC KHI PC YÊU CẦU HOẶC TỚI NGƯỠNG NÀO ĐÓ (XONG)
	- HIỂN THỊ LED 
	- BUTTON XOÁ LỖI
*/

#include <WebServer.h>
#include <WiFi.h>
#include <Update.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <DNSServer.h>

const char index_html[] = R"rawliteral( 
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    <!-- html { -->
     <!-- font-family: Arial; -->
     <!-- display: inline-block; -->
     <!-- margin: 0px auto; -->
     <!-- /* text-align: center; */ -->
    <!-- } -->
    h2 { font-size: 2.5rem; text-align: center;}
	h3 { font-size: 2.0rem; text-align: center;}
    p { font-size: 1.5rem; text-align: center;}
    .units { font-size: 1.0rem; vertical-align:text-top;}
    .dht-labels{
      font-size: 1.6rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
    .button {
        background-color: #f6f6f6;
        font-size: 1.5rem;
        vertical-align: middle;
		margin-left: 30px;
    }
    input[type = number], select{
      width: 10%;
      padding: 9px 9px;
      margin: 8px 0;
      display: inline-block;
      border: 1px solid #ccc;
      border-radius: 4px;
      box-sizing: border-box;
      vertical-align: middle;
    }
	label {
		<!-- position: absol -->
		display: inline-block;
		top: 0;
		left: 8px;
		color: #00d151;
		line-height: 41px;
		margin:20px
	}

  </style>
</head>
<body>
  <h2>WEB CONTROL</h2>
  <!-- <p> -->
    <!-- <span class="dht-labels">Temperature:</span>  -->
    <!-- <span id="temperature">%TEMPERATURE%</span> -->
    <!-- <label id="temperature">0.00</label> -->
    <!-- <span> / </span> -->
    <!-- <label id="Threshold_temparature">0.00</label> -->
    <!-- <sup class="units">&deg;C</sup> -->
      <!-- <p> -->
        <!-- <span class="dht-labels">Threshold_Temperature</span> -->
        <!-- <input  type="number" name="inputTemp" id="inputTemp"> -->
        <!-- <button class="button" onclick="Send_data(1)">SEND</button> -->
      <!-- </p> -->
  <!-- </p> -->
  <!-- <p> -->
    <!-- <span class="dht-labels">Humidity:</span> -->
    <!-- <label id="humidity">0.00</label> -->
    <!-- <span> / </span> -->
    <!-- <label id="threshold_humi">0.00</label> -->
    <!-- <sup class="units">&percnt;</sup> -->
      <!-- <p> -->
        <!-- <span class="dht-labels">Threshold_humidity</span> -->
        <!-- <input  type="number" name="inputHumi" id="inputHumi"> -->
        <!-- <button class="button" onclick="Send_data(2)">SEND</button> -->
      <!-- </p> -->
  <!-- </p> -->
  <h3>STATE</h3>
  <p>
	<span class="dht-labels">THÙNG RÁC CHAI:</span>
    <label id="thungracchai">BÌNH THƯỜNG</label>
	<span class="dht-labels">THÙNG RÁC LON:</span>
    <label id="thungraclon">BÌNH THƯỜNG</label>
  </p>
  
  <p>
	<span class="dht-labels">MOTOR NHẢ:</span>
    <label id="motornha">BÌNH THƯỜNG</label>
	<span class="dht-labels">MOTOR ÉP:</span>
    <label id="motorep">BÌNH THƯỜNG</label>
  </p>
  
  <p>
	<span class="dht-labels">MOTOR NGHIỀN:</span>
    <label id="motornghien">BÌNH THƯỜNG</label>
  </p>
    <p>
    <button class="button" onclick="deleteButton(1)">RST_M_NHẢ</button>
    <button class="button" onclick="deleteButton(2)">RST_M_ÉP</button>
    <button class="button" onclick="deleteButton(3)">RST_M_NGHIỀN</button>
  </p>
  <h3>CONTROL</h3>
    <p>
    <button class="button" onclick="startButton()">START</button>
    <button class="button" onclick="stopButton(0)">STOP</button>
    <button class="button" onclick="NDButton(1)">ND CHAI</button>
    <button class="button" onclick="NDButton(2)">ND LON</button>
  </p>
  <p>
	<span class="dht-labels">MOTOR XOAY CHAI:</span>
    <button class="button" onclick="runButton(1)">CHẠY</button>
	<button class="button" onclick="stopButton(1)">DỪNG</button>
  </p>
  <p>
	<span class="dht-labels">MOTOR NHẢ:</span>
    <button class="button" onclick="runButton(2)">NHẢ CHAI</button>
    <button class="button" onclick="runButton(3)">NHẢ LON</button>
	<button class="button" onclick="stopButton(2)">DỪNG</button>
  </p>
  <p>
	<span class="dht-labels">MOTOR BĂNG TẢI:</span>
    <button class="button" onclick="runButton(4)">CHẠY</button>
	<button class="button" onclick="stopButton(3)">DỪNG</button>
  </p>
  <p>
	<span class="dht-labels">MOTOR ÉP:</span>
    <button class="button" onclick="runButton(5)">CHẠY</button>
	<button class="button" onclick="stopButton(4)">DỪNG</button>
  </p>
  <p>
	<span class="dht-labels">MOTOR NGHIỀN:</span>
    <button class="button" onclick="runButton(6)">THUẬN</button>
    <button class="button" onclick="runButton(7)">NGHỊCH</button>
	<button class="button" onclick="stopButton(5)">DỪNG</button>
  </p>
  <p>
  <br></br>
    <button class="button" onclick="logoutButton()">LOGOUT</button>
    <button class="button" onclick="OTAButton()">OTA</button>
  </p>
</body>

<script>
function deleteButton(data){
    switch(data){
        case 1:
            <!-- var data1 = document.getElementById("inputTemp").value; -->
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/rst_m_nha",true);
            xhttp.send();
            break;
        case 2:
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/rst_m_ep",true);
            xhttp.send();
            break;
		case 3:
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/rst_m_nghien",true);
            xhttp.send();
            break;
    }
}

function startButton(){

	var xhttp = new XMLHttpRequest();
	xhttp.open("GET", "/start",true);
	xhttp.send();
	break;
	
}

function NDButton(data){

	switch(data){
        case 1:
            <!-- var data1 = document.getElementById("inputTemp").value; -->
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/nd_chai",true);
            xhttp.send();
            break;
        case 2:
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/nd_lon",true);
            xhttp.send();
            break;
    }
	
}

function runButton(data){
    switch(data){
        case 1:
            <!-- var data1 = document.getElementById("inputTemp").value; -->
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/chay_xoay",true);
            xhttp.send();
            break;
        case 2:
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/nha_chai",true);
            xhttp.send();
            break;
		case 3:
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/nha_lon",true);
            xhttp.send();
            break;
		case 4:
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/chay_bt",true);
            xhttp.send();
            break;
		case 5:
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/chay_ep",true);
            xhttp.send();
            break;
		case 6:
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/chay_thuan",true);
            xhttp.send();
            break;
		case 7:
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/chay_nghich",true);
            xhttp.send();
            break;
    }
}

function stopButton(data){
    switch(data){
		case 0:
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/stop",true);
            xhttp.send();
            break;
        case 1:
            <!-- var data1 = document.getElementById("inputTemp").value; -->
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/stop_xoay",true);
            xhttp.send();
            break;
        case 2:
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/stop_nha",true);
            xhttp.send();
            break;
		case 3:
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/stop_bt",true);
            xhttp.send();
            break;
		case 4:
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/stop_ep",true);
            xhttp.send();
            break;
		case 5:
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/stop_nghien",true);
            xhttp.send();
            break;
    }
}
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("thungracchai").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/thungracchai", true);
  xhttp.send();
}, 1000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("thungraclon").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/thungraclon", true);
  xhttp.send();
}, 1000 ) ;
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("motornha").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/motornha", true);
  xhttp.send();
}, 1000) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("motorep").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/motorep", true);
  xhttp.send();
}, 1000) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("motornghien").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/motornghien", true);
  xhttp.send();
}, 1000) ;

function logoutButton(){
  var xhttp = new XMLHttpRequest();
  xhttp.open("GET", "/logout", true);
  xhttp.send();
  setTimeout(function(){window.open("/login", "_self");}, 1000);
}
function OTAButton(){
  var xhttp = new XMLHttpRequest();
  setTimeout(function(){window.open("/OTA", "_self");}, 1000);
}
</script>
</html>
)rawliteral";

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
					

void setupAP(){
	
	WiFi.disconnect();
	WiFi.mode(WIFI_OFF);
	delay(100);
	
	WiFi.mode(WIFI_AP);
	
    WiFi.softAP("WIFI_AP");//,"123456789");
	IPAddress IP = WiFi.softAPIP();
	IP_Server = String (IP[0]);
	IP_Server = IP_Server + ".";
	IP_Server = IP_Server + String (IP[1]);
	IP_Server = IP_Server + ".";
	IP_Server = IP_Server + String (IP[2]);
	IP_Server = IP_Server + ".";
	IP_Server = IP_Server + String (IP[3]);;
	Wifi_SSID = "WIFI_AP";
	Serial.print("IP OTA:"); Serial.println(IP);
//    IPAddress apIP(192, 168, 4, 1);
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP()); //Set captive portal
    // server.begin();
    ui8_SoftAP = 1;
	
 
	// ui8_mode_config = 1;
	// time_config = millis();
}

void setupServer(){
	
	// server.on("/", HTTP_GET, [](){
		// server.send(200, "text/html", handleRoot);
	// });
	// MDNS.begin(host_web);
    
	server.on("/", handleRoot);
	server.on("/login", handleLogin);
	
	server.on("/rst_m_nha", HTTP_GET, [](){
		Serial.println("RESET MOTOR NHA");
	});
	
	server.on("/rst_m_ep", HTTP_GET, [](){
		Serial.println("RESET MOTOR EP");
	});
	
	server.on("/rst_m_nghien", HTTP_GET, [](){
		Serial.println("RESET MOTOR NGHIEN");
	});
	
	server.on("/start", HTTP_GET, [](){
		Serial.println("RESET MOTOR NGHIEN");
	});
	
	server.on("/nd_chai", HTTP_GET, [](){
		Serial.println("NHAN DIEN CHAI");
	});
	
	server.on("/nd_lon", HTTP_GET, [](){
		Serial.println("NHAN DIEN LON");
	});
	
	server.on("/chay_xoay", HTTP_GET, [](){
		Serial.println("CHAY XOAY CHAI");
	});
	
	server.on("/nha_chai", HTTP_GET, [](){
		Serial.println("CHAY NHA CHAI");
	});
	
	server.on("/nha_lon", HTTP_GET, [](){
		Serial.println("CHAY NHA LON");
	});
	
	server.on("/chay_bt", HTTP_GET, [](){
		Serial.println("CHAY BANG TAI");
	});
	
	server.on("/chay_ep", HTTP_GET, [](){
		Serial.println("CHAY MOTOR EP");
	});
	
	server.on("/chay_thuan", HTTP_GET, [](){
		Serial.println("CHAY MOTOR NGHIEN THUAN");
	});
	
	server.on("/chay_nghich", HTTP_GET, [](){
		Serial.println("CHAY MOTOR NGHIEN NGHICH");
	});
	
	server.on("/stop", HTTP_GET, [](){
		Serial.println("STOP");
	});
	
	server.on("/stop_xoay", HTTP_GET, [](){
		Serial.println("STOP XOAY CHAI");
	});
	
	server.on("/stop_nha", HTTP_GET, [](){
		Serial.println("STOP NHA CHAI");
	});
	
	server.on("/stop_bt", HTTP_GET, [](){
		Serial.println("STOP BANG TAI");
	});
	
	server.on("/stop_ep", HTTP_GET, [](){
		Serial.println("STOP MOTOR EP");
	});
	
	server.on("/stop_nghien", HTTP_GET, [](){
		Serial.println("STOP MOTOR NGHIEN");
	});
	
	server.on("/logout", HTTP_GET, [](){
		is_authenticated = false;
		server.send(200, "text/plain", "OK");
		Serial.println("LOGOUT");
	});
	
	server.on("/thungracchai", HTTP_GET, [](){
		server.send(200, "text/html", String("DAY").c_str());
	});
	
	server.on("/thungraclon", HTTP_GET, [](){
		server.send(200, "text/html", String("DAY").c_str());
	});
	
	server.on("/motornha", HTTP_GET, [](){
		server.send(200, "text/html", String("LOI").c_str());
	});
	
	server.on("/motorep", HTTP_GET, [](){
		server.send(200, "text/html", String("LOI").c_str());
	});
	
	server.on("/motornghien", HTTP_GET, [](){
		server.send(200, "text/html", String("LOI").c_str());
	});
	
	// server.on("/get", HTTP_GET, [](){motornghien
		// if(server.hasArg(PARAM_INPUT_1)){
			// threshold_temp = server.arg(PARAM_INPUT_1).toFloat();
			// EEPROM.put(0, threshold_temp);
		// }
		// if(server.hasArg(PARAM_INPUT_2)){
			// threshold_humi = server.arg(PARAM_INPUT_2).toFloat();
			// EEPROM.put(sizeof(threshold_temp), threshold_humi);
		// }
		// server.send(200, "text/plain", "OK");
		// EEPROM.commit();
	// });
	setup_serverOTA();
    server.onNotFound([]() {
   // if(!handleFileRead(server.uri())){
      // const char *metaRefreshStr = "<head><meta http-equiv=\"refresh\" content=\"0; url=http://192.168.4.1/login\" /></head><body><p>redirecting...</p></body>";
      // server.send(200, "text/html", metaRefreshStr);
      server.sendHeader("Location", "/");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
      server.send(301);
   // }
    });
	
	server.begin();
	// MDNS.addService("http", "tcp", 80);

    // Serial.printf("Ready! Open http://%s.local in your browser\n", host_web);
}

void setup_serverOTA(){
	
	
	server.on("/OTA", handleOTA);
	
	server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
    }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      // Serial.setDebugOutput(true);
      // WiFiUDP::stopAll();
      Serial.printf("Update: %s\n", upload.filename.c_str());
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if (!Update.begin(maxSketchSpace)) { //start with max available size
      Update.printError(Serial);
    //        myserial.println("Update Failed");
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
    //        myserial.println("Update Failed");
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
    //        myserial.println("Update Success");
      Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
      Update.printError(Serial);
      }
      Serial.setDebugOutput(false);
    }
    // yield();
    });
    // server.begin(); 
	
}

void handleLogin() {
  String msg;
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")) {
    if (server.arg("USERNAME") == "admin" &&  server.arg("PASSWORD") == "admin") {
      server.sendHeader("Location", "/");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
      server.send(301);
	  is_authenticated = true;
      Serial.println("Log in Successful");
      return;
    }
    msg = "Wrong username/password! try again.";
    Serial.println("Log in Failed");
  }
  
  String content = "<html><body><form action='/login' method='POST'><p>LOGIN PAGE</p><br>";
  content += "User:<input type='text' name='USERNAME' placeholder='user name'><br>";
  content += "Password:<input type='password' name='PASSWORD' placeholder='password'><br>";
  content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";
  server.send(200, "text/html", content);
}

void handleRoot() {
  Serial.println("Enter handleRoot");
  String header;
  if (!is_authenticated) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }
  server.send(200, "text/html", index_html);
}

void handleOTA() {
  Serial.println("Enter handleRoot");
  String header;
  if (!is_authenticated) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }
  server.sendHeader("Connection", "close");
  server.send(200, "text/html", update_ota);
}
