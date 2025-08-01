String topic_mqtt = "";
String payload_mqtt = "";
uint8_t ui8_comess=0;
int dem=0;
bool mqttConnected = false;
unsigned long lastPing = 0;
const unsigned long keepAliveInterval = 60000; // Thời gian giữ kết nối, 60 giây

unsigned long lastActionTime = 0; // Thời gian cho các hoạt động không dùng delay

#define Serial_ARDUIRNO Serial
#define Serial_SIM Serial1
// void setup() {
//   Serial.begin(115200);         // Serial cho debug
//   Serial_SIM.begin(9600, SERIAL_8N1, SIM_RX, SIM_TX);  // Serial cho module SIM

//   // Bắt đầu các kết nối
//   connectToNetwork();
//   startMQTTService();
//   initializeMQTTWithWill("home/status", "offline");
//   connectToMQTTBrokerAndSendOnline();
//   subscribeToTopic("home/status");

//   lastPing = millis();  // Ghi nhận thời điểm kết nối
// }

// void loop() {
//   // Kiểm tra và reconnect nếu mất kết nối MQTT
//   if (!mqttConnected) {
//     reconnectMQTT();
//   }

//   // Kiểm tra trạng thái mạng mỗi 10 giây
//   static unsigned long lastNetworkCheck = 0;
//   if (millis() - lastNetworkCheck > 10000) {
//     lastNetworkCheck = millis();
//     checkNetworkStatus();
//   }

//   // Kiểm tra thời gian giữ kết nối và gửi dữ liệu nếu cần
//   if (mqttConnected && (millis() - lastPing > keepAliveInterval)) {
//     if (!publishToTopic("home/status", "ping")) {
//       mqttConnected = false; // Cập nhật lại trạng thái nếu không thể gửi
//     } else {
//       lastPing = millis();
//     }
//   }

//   // Xử lý phản hồi từ module mà không dùng delay
//   handleSerialResponse();
// }

// void connectToNetwork() {
//   sendATCommand_new("AT+CGATT=1", "OK", 2000);
//   sendATCommand_new("AT+CGDCONT=1,\"IP\",\"your_apn\"", "OK", 2000);  // Thay "your_apn" bằng APN của nhà mạng
//   sendATCommand_new("AT+NETOPEN", "+NETOPEN: 0", 5000);
// }
bool sendATCommand_new(String command, String expected_response, unsigned int timeout) 
{
  Serial_SIM.println(command);
  long int time = millis();
  while ((millis() - time) < timeout) {
    if (Serial_SIM.find(expected_response.c_str())) 
    {
      Serial.println("Command executed: " + command);
      return true;
    }
  }
  Serial.println("Timeout waiting for response for command: " + command);
  return false;
}

void waitForResponse(unsigned long waitTime) {
  unsigned long startTime = millis();
  while (millis() - startTime < waitTime) {
    // Không làm gì, chờ đợi để không chặn luồng chính
  }
}

void subscribeToTopic(const char *name_domain, const char *username, const char *pass, const char *name_topic, const char *myname_device) {
  // int topicLength = strlen(topic);

  char au_topic_pub[100];
  char au_topic_onl[100];
  char au_topic_query[100];
  char au_topic_set[100];
  char au_topic_sniff[100];
  char au_topic_printer_fw[100];
  char au_topic_barcode_fw[100];
  char au_topic_server_fw[100];
  char au_topic_resetserver[100];
  char au_topic_resetprinter[100];
  char au_topic_resetbarcode[100];

  sprintf(au_topic_query, "%s/%s/QUERY",name_topic, myname_device);
  sprintf(au_topic_set, "%s/%s/SET",name_topic, myname_device);
  sprintf(au_topic_sniff, "%s/%s/SNIFF",name_topic, myname_device);
  sprintf(au_topic_printer_fw, "%s/%s/PRINTER_FW",name_topic, myname_device); //cấu hình update loại máy IN
  sprintf(au_topic_barcode_fw, "%s/%s/BARCODE_FW",name_topic, myname_device);
  sprintf(au_topic_server_fw, "%s/%s/SERVER_FW",name_topic, myname_device);
  sprintf(au_topic_resetserver, "%s/%s/RS_SERVER",name_topic, myname_device);
  sprintf(au_topic_resetprinter, "%s/%s/RS_PRINTER",name_topic, myname_device);
  sprintf(au_topic_resetbarcode,"%s/%s/RS_BARCODE",name_topic, myname_device);


   // Danh sách các topic dưới dạng mảng
    const char *topics[] = {
        au_topic_query,
        au_topic_set,
        au_topic_sniff,
        au_topic_printer_fw,
        au_topic_barcode_fw,
        au_topic_server_fw,
        au_topic_resetserver,
        au_topic_resetprinter,
        au_topic_resetbarcode
    };

    int numTopics = sizeof(topics) / sizeof(topics[0]); // Số lượng topic

    // Vòng lặp để đăng ký từng topic
    for (int i = 0; i < numTopics; i++) {
        int topicLength = strlen(topics[i]);

        // Lệnh AT để đăng ký topic
        String subscribeCmd = "AT+CMQTTSUB=0," + String(topicLength) + ",2"; // QoS là 2
        sendATCommand_new(subscribeCmd.c_str(), ">", 2000); // Gửi lệnh đăng ký, chờ ký tự ">"
        Serial_SIM.print(topics[i]);                       // Gửi tên topic
        waitForResponse(200);                              // Chờ phản hồi

        // In thông tin đăng ký thành công
        Serial.println("Subscribed to topic: " + String(topics[i]));
    }


  // String subscribeCmd = "AT+CMQTTSUB=0," + String(topicLength) + ",2";  // QoS là 1

  // sendATCommand_new(subscribeCmd.c_str(), ">", 2000);  // Gửi lệnh đăng ký, chờ ký tự ">"
  // Serial_SIM.print(topic);                         // Gửi tên topic
  // waitForResponse(200);
  // Serial.println("Subscribed to topic: " + String(topic));
}

// bool publishToTopic(const char* topic, const char* message) 
// {
//   int topicLength = strlen(topic);
//   int messageLength = strlen(message);
//   int size_channel=0;
  
//   String topic_publish = "AT+CMQTTTOPIC=0,31";
//   sendATCommand_new(topic_publish.c_str(), ">", 2000);  // Gửi lệnh publish, chờ ký tự ">"
//   Serial_SIM.println(topic);                        // Gửi tên topic
//   Serial.println(topic);
//   waitForResponse(500);

//   String payload_publish = "AT+CMQTTPAYLOAD=0," + String(messageLength);
//   sendATCommand_new(payload_publish.c_str(), ">", 2000);  // Gửi lệnh publish, chờ ký tự ">"
//   Serial_SIM.println(message);                        // Gửi tên topic
//   Serial.println(message);
//   waitForResponse(500);

//   String publishCmd = "AT+CMQTTPUB=0,2,180,1";  // QoS là 1, không giữ lại (retain)  
//   sendATCommand_new(publishCmd.c_str(), "OK", 2000);  // Gửi lệnh publish, chờ ký tự ">"
//   // Serial_SIM.print(message);                        // Gửi tên topic
//   waitForResponse(200);

//   Serial.println("Published message: " + String(message) + " to topic: " + String(topic));
//   return true;
// }

bool publish_data(String data, uint64_t check_buffer,char *myname_device) 
{
    // int topicLength = strlen(topic);
    // int messageLength = strlen(message);

    int size_channel=0;
    int kq_ne=0;

    size_channel = strlen(myname_device);

    // Lệnh AT thiết lập topic
     waitForResponse(50);
    String topicCmd = "AT+CMQTTTOPIC=0," + String(size_channel);
    if (!sendATCommand_new(topicCmd.c_str(), ">", 2000)) {
        Serial.println("Failed to set MQTT topic.");
        return false;
    }
     waitForResponse(20);
    Serial_SIM.print(myname_device);  // Gửi tên topic
     waitForResponse(20);
    // if (!waitForResponse("OK", 2000)) {
    //     Serial.println("Error in topic setup.");
    //     return false;
    // }

    // Lệnh AT thiết lập payload
    String payloadCmd = "AT+CMQTTPAYLOAD=0," + String(check_buffer);
    if (!sendATCommand_new(payloadCmd.c_str(), ">", 2000)) {
        Serial.println("Failed to set MQTT payload.");
        return false;
    }
     waitForResponse(20);
    Serial_SIM.println(data);  // Gửi payload
    // if (!waitForResponse("OK", 2000)) {
    //     Serial.println("Error in payload setup.");
    //     return false;
    // }

    // Lệnh AT publish
     waitForResponse(100);
      String publishCmd="";
    //  if(message=="online")
    //  {
    //   publishCmd = "AT+CMQTTPUB=0,2,180,1";  // QoS = 1, không giữ lại
    //  }
    // else
    // {
      publishCmd = "AT+CMQTTPUB=0,1,180,1";
    // }
    if (!sendATCommand_new(publishCmd.c_str(), "OK", 2000)) {
        Serial.println("Failed to publish message.");
        return false;
    }

    // Log thông tin thành công
    // Serial.println("Published message: \"" + String(message) + "\" to topic: \"" + String(topic) + "\"");
    return true;
}

bool publishToTopic(const char* topic, const char* message) 
{
    int topicLength = strlen(topic);
    int messageLength = strlen(message);

    // Lệnh AT thiết lập topic
     waitForResponse(500);
    String topicCmd = "AT+CMQTTTOPIC=0," + String(topicLength);
    if (!sendATCommand_new(topicCmd.c_str(), ">", 2000)) {
        Serial.println("Failed to set MQTT topic.");
        return false;
    }
     waitForResponse(50);
    Serial_SIM.print(topic);  // Gửi tên topic
     waitForResponse(50);
    // if (!waitForResponse("OK", 2000)) {
    //     Serial.println("Error in topic setup.");
    //     return false;
    // }

    // Lệnh AT thiết lập payload
    String payloadCmd = "AT+CMQTTPAYLOAD=0," + String(messageLength);
    if (!sendATCommand_new(payloadCmd.c_str(), ">", 2000)) {
        Serial.println("Failed to set MQTT payload.");
        return false;
    }
     waitForResponse(50);
    Serial_SIM.print(message);  // Gửi payload
    // if (!waitForResponse("OK", 2000)) {
    //     Serial.println("Error in payload setup.");
    //     return false;
    // }

    // Lệnh AT publish
     waitForResponse(100);
      String publishCmd="";
     if(message=="online")
     {
      publishCmd = "AT+CMQTTPUB=0,2,180,1";  // QoS = 1, không giữ lại
     }
    else
    {
      publishCmd = "AT+CMQTTPUB=0,1,180,1";
    }
    if (!sendATCommand_new(publishCmd.c_str(), "OK", 2000)) {
        Serial.println("Failed to publish message.");
        return false;
    }

    // Log thông tin thành công
    Serial.println("Published message: \"" + String(message) + "\" to topic: \"" + String(topic) + "\"");
    return true;
}


void startMQTTService() {
  sendATCommand_new("AT+CMQTTSTART", "OK", 2000);  // Bắt đầu dịch vụ MQTT
}

void checkNetworkStatus() {
  // Kiểm tra trạng thái mạng
  if (!sendATCommand_new("AT+CGATT?", "+CGATT: 1", 2000)) {
    Serial.println("Network disconnected. Attempting to reconnect...");
    mqttConnected = false;
    // connectToNetwork();
  }
}

void initializeMQTTWithWill(const char* name_topic, const char* myname_device, const char* willMessage) {
  char test[100];
  char topic[100];
  sprintf(topic, "%s/%s/STATUS",name_topic, myname_device);
  snprintf(test, 23, "\"CLT_%lld\"", ESP.getEfuseMac());
  sendATCommand_new("AT+CMQTTACCQ=0,"+(String) (test), "OK", 2000);  // Khởi tạo client MQTT với client_id
  waitForResponse(50);

  sendATCommand_new("AT+CMQTTCFG=\"checkUTF8\",0,0", "OK", 2000);
  waitForResponse(50);

  // Thiết lập will topic
  int willTopicLength = strlen(topic);
  String willTopicCmd = "AT+CMQTTWILLTOPIC=0," + String(willTopicLength);
  sendATCommand_new(willTopicCmd.c_str(), ">", 2000);  // Chờ dấu ">" để gửi will topic
  Serial_SIM.print(topic);                     // Gửi tên topic
  waitForResponse(50);

  // Thiết lập will message
  int willMessageLength = strlen(willMessage);
  String willMessageCmd = "AT+CMQTTWILLMSG=0," + String(willMessageLength) + ",2";  // QoS = 1 cho will message
  sendATCommand_new(willMessageCmd.c_str(), ">", 2000);  // Chờ dấu ">" để gửi will message
  Serial_SIM.print(willMessage);                     // Gửi nội dung will message
  waitForResponse(50);

  Serial.println("Initialized MQTT with will message: " + String(willMessage) + " on topic: " + String(topic));
}

void connectToMQTTBrokerAndSendOnline(const char *name_domain, const char *username_mqtt, const char *pass, const char *name_topic, const char *myname_device) {

  char domain_name[100];
	char username[100];
	char password[100];
  char au_topic_pub[100];
	// init_MQTT();
	sprintf(domain_name,"\"tcp://%s:1883\"",name_domain);
	sprintf(username,"\"%s\"",username_mqtt);
	sprintf(password,"\"%s\"",pass);

  sprintf(au_topic_pub, "%s/%s/STATUS",name_topic, myname_device);

  if (sendATCommand_new("AT+CMQTTCONNECT=0,"+(String) (domain_name)+",180,0,"+(String) (username)+","+(String) (password), "OK", 5000)) 
  {
    mqttConnected = true;
    int kq=publishToTopic(au_topic_pub, "online");
    if(kq==0)
    {
      publishToTopic(au_topic_pub, "online");
      dem++;
      if(dem==5)
      {
        return;
      }
    }

    if(kq==1)
    {
      dem=0;
    }

    // pulic_data_onl("online",6,"PARC/ITB_128274385044540/STATUS");
    lastPing = millis();
  } 
  else 
  {
    Serial.println("Failed to connect to MQTT broker.");
    mqttConnected = false;
  }
}


void reconnectMQTT(const char *name_domain, const char *username, const char *pass, const char *name_topic, const char *myname_device) {
  Serial.println("Attempting to reconnect to MQTT...");
  connectToMQTTBrokerAndSendOnline(name_domain, username, pass, name_topic, myname_device);
  if (mqttConnected) {
    subscribeToTopic(name_domain, username, pass, name_topic, myname_device);
    Serial.println("Reconnected and subscribed to topic.");
  }
}


// void handleSerialResponse() {
//   if (Serial_SIM.available()) {
//     String response = Serial_SIM.readString();
//     Serial.println("Received from SIM: " + response);

//     // Kiểm tra nếu mất kết nối từ broker
//     if (response.indexOf("+CMQTTDISC:") != -1) {
//       mqttConnected = false;
//       Serial.println("MQTT disconnected by broker.");
//     }

//      // Kiểm tra nếu có dữ liệu MQTT nhận được
//         if (response.indexOf("+CMQTTRXSTART:") != -1) {
//             // Phân tích phản hồi `+CMQTTRXSTART`
//             int clientIndex, topicLen, payloadLen;

//             sscanf(response.c_str(), "+CMQTTRXSTART: %d,%d,%d", &clientIndex, &topicLen, &payloadLen);

//             // Đọc tiếp dòng `+CMQTTRXTOPIC`
//             if (Serial_SIM.find("+CMQTTRXTOPIC:")) {
//                 Serial_SIM.read(); // Bỏ qua ký tự đầu tiên (cách ly ":")
//                 String topic = Serial_SIM.readStringUntil('\n').trim(); // Đọc và bỏ khoảng trắng

//                 // Đọc tiếp dòng `+CMQTTRXPAYLOAD`
//                 if (Serial_SIM.find("+CMQTTRXPAYLOAD:")) {
//                     Serial_SIM.read(); // Bỏ qua ký tự đầu tiên
//                     String payload = Serial_SIM.readStringUntil('\n').trim(); // Đọc payload

//                     // Xử lý dữ liệu nhận được
//                     Serial.println("MQTT Message Received:");
//                     Serial.println("Topic: " + topic);
//                     Serial.println("Payload: " + payload);

//                     // Bạn có thể xử lý thêm ở đây, ví dụ:
//                     // processMQTTMessage(topic, payload);
//                 }
//             }
//         }
//   }
// }

void handleSerialResponse() {
    if (Serial_SIM.available()) {
        // Đọc toàn bộ phản hồi từ SIM
        String response = Serial_SIM.readString();
        Serial.println("Received from SIM: " + response);

        // Kiểm tra nếu có dữ liệu MQTT nhận được
        if (response.indexOf("+CMQTTRXSTART:") != -1) {

            // ui8_comess=1;
            topic_mqtt = "";
            payload_mqtt = "";

            // Tách topic
            if (response.indexOf("+CMQTTRXTOPIC:") != -1) {
                int topicStartIndex = response.indexOf("+CMQTTRXTOPIC:") + 14; // Bỏ qua "+CMQTTRXTOPIC: "
                int topicLineStart = response.indexOf('\n', topicStartIndex) + 1; // Bắt đầu chuỗi topic
                int topicLineEnd = response.indexOf('\n', topicLineStart);       // Kết thúc chuỗi topic
                topic_mqtt = response.substring(topicLineStart, topicLineEnd);
                topic_mqtt.trim(); // Bỏ khoảng trắng
            }

            // Tách payload
            if (response.indexOf("+CMQTTRXPAYLOAD:") != -1) {
                int payloadStartIndex = response.indexOf("+CMQTTRXPAYLOAD:") + 16; // Bỏ qua "+CMQTTRXPAYLOAD: "
                int payloadLineStart = response.indexOf('\n', payloadStartIndex) + 1; // Bắt đầu chuỗi payload
                int payloadLineEnd = response.indexOf('\n', payloadLineStart);        // Kết thúc chuỗi payload
                payload_mqtt = response.substring(payloadLineStart, payloadLineEnd);
                payload_mqtt.trim(); // Bỏ khoảng trắng
            }

            // In kết quả
            Serial.println("MQTT Message Received:");
            Serial.println("Topic: " + topic_mqtt);
            Serial.println("Payload: " + payload_mqtt);
            ui8_comess=1;
            // Gọi hàm xử lý nếu cần
            // processMQTTMessage(topic, payload);
        }
    }
}



void setup_mqtt_atcmd(const char *name_domain, const char *username, const char *pass, const char *name_topic, const char *myname_device)
{
  // connectToNetwork();
  startMQTTService();
  initializeMQTTWithWill(name_topic,myname_device, "offline");
  connectToMQTTBrokerAndSendOnline(name_domain, username, pass, name_topic, myname_device);
  subscribeToTopic(name_domain, username, pass, name_topic, myname_device);

  lastPing = millis();  // Ghi nhận thời điểm kết nối 
}

void reconnect(const char *name_domain, const char *username, const char *pass, const char *name_topic, const char *myname_device)
{
   if (!mqttConnected) {
    reconnectMQTT(name_domain, username, pass, name_topic, myname_device);
  }

  // Kiểm tra trạng thái mạng mỗi 10 giây
  static unsigned long lastNetworkCheck = 0;
  if (millis() - lastNetworkCheck > 20000) 
  {
    lastNetworkCheck = millis();
    checkNetworkStatus();
  }

  // // Kiểm tra thời gian giữ kết nối và gửi dữ liệu nếu cần
  // if (mqttConnected && (millis() - lastPing > keepAliveInterval)) {
  //   if (!publishToTopic("home/status", "ping")) {
  //     mqttConnected = false; // Cập nhật lại trạng thái nếu không thể gửi
  //   } else {
  //     lastPing = millis();
  //   }
  // }

  // Xử lý phản hồi từ module mà không dùng delay
  handleSerialResponse();
}

// void processIncomingMQTTMessages() {
//     if (Serial_SIM.available()) {
//         String response = Serial_SIM.readStringUntil('\n');

//         // Kiểm tra xem có phản hồi tin nhắn MQTT không
//         if (response.startsWith("+CMQTTRXSTART:")) {
//             int clientIndex, topicLen, payloadLen;

//             // Tách thông tin từ phản hồi
//             sscanf(response.c_str(), "+CMQTTRXSTART: %d,%d,%d", &clientIndex, &topicLen, &payloadLen);

//             // Đọc topic
//             String topic = Serial_SIM.readStringUntil('\n');
//             // Đọc payload
//             String payload = Serial_SIM.readStringUntil('\n');

//             // Log thông tin nhận được
//             Serial.println("Received MQTT Message:");
//             Serial.println("Topic: " + topic);
//             Serial.println("Payload: " + payload);
//         }
//     }
// }