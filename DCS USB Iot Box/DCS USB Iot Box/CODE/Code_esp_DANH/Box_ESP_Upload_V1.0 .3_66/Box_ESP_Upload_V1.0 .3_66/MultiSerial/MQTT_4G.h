unsigned long cur_time, old_time;
String str_dt;
bool hold = 0;
int len = 0;

#define Serial_ARDUIRNO Serial
#define Serial_SIM Serial1
String cmd;
uint8_t ui8_check_payload=0;
uint8_t ui8_check_topic=0;
uint8_t ui8_check_pub=0;
uint8_t ui8_check_disconnect=0;
uint8_t ui8_check_stopmqtt=0;
uint8_t ui8_check_stopclient=0;
uint8_t ui8_phathien_loisai = 0;
uint8_t ui8_solan_disconnect_4g = 0;
uint8_t ui8_chuoi_disconnect=0;

void sendATCommand_DANHTEST(String command) {
    Serial_SIM.println(command);
    delay(500);
    Serial.print("Sent: ");
    Serial.println(command);
}
String readResponse() {
    String response = "";
    while (Serial_SIM.available()) {
        response += char(Serial_SIM.read());
    }
    Serial.print("Response: ");
    Serial.println(response);
    return response;
}

int sendATCommand(String command, const int timeout) 
{

  int ketqua=0;
  String cmd="";
  String test="";
  String test2="";
  String data_2="";
  String data_3="";
  int i;
  int j;
  int a;
  int b;
  int c;
  int d;
  int e;
  Serial_SIM.println(command);
  long int time = millis();
  while ((time + timeout) > millis()) {
    while (Serial_SIM.available()) 
    {
      char c = Serial_SIM.read();
      Serial.print(c);
      cmd+=c;
    }
  }

  j= cmd.indexOf('O');
  i = cmd.indexOf('K');

  a = cmd.indexOf('+');
  b = cmd.indexOf(',');

  c = cmd.indexOf('K');

  d = cmd.indexOf('+');
  e = cmd.indexOf(':');

  test=cmd.substring(j,i+1);
  test2=cmd.substring(a+1,b+2);
  data_2=cmd.substring(c-1,c+1);
  // data_3=cmd.substring(d,)

  // Serial_ARDUIRNO.println("cmd sau khi check la: ");
  // Serial_ARDUIRNO.println(data_2);
  // Serial_ARDUIRNO.println(test2);
  if(command=="AT+CMQTTPUB=0,1,180,1")
  {
    if(data_2=="OK")
    {
      Serial.println("nhay vao up thanh cong");
      ketqua=1;
    }
  }

  else 
  {
    if(test=="OK")
    {
      ketqua=1;
    }
    else if(test2 == "CMQTTCONNLOST: 0,2")
    {
      // ketqua=0;
      // ui8_check_disconnect_mqtt=1;
      ui8_chuoi_disconnect=1;
      Serial.println("mat ket noi MQTT");
    }
    else
    {
      ketqua=0;
    }
  }

  
  Serial.println();
  return ketqua;
}

String getATCommandResponse() {
  String response = "";
  while (Serial_SIM.available()) {
    char c = Serial_SIM.read();
    response += c;
  }
  return response;
}


// void connectToNetwork() {
//   sendATCommand("AT+CREG?", 2000);
//   sendATCommand("AT+CGATT?", 2000);
//   sendATCommand("AT+CGDCONT=1,\"IP\",\"your_apn\"", 2000);
//   sendATCommand("AT+NETOPEN", 10000);
// }

int wRespon(long waktu) 
{
  int ketqua=0;
  String cmd="";
  String test="";
  String test2="";
  String data_2="";
  String data_3="";
  int i;
  int j;
  int a;
  int b;
  int c;
  int d;
  int e;
  cur_time = millis();
  old_time = cur_time;
  while (cur_time - old_time < waktu ) 
  {
    cur_time = millis();
    while (Serial_SIM.available() > 0) 
    {
      char c = Serial_SIM.read();
      Serial_ARDUIRNO.print(c);
      cmd+=c;
    }
  }
  // Serial_ARDUIRNO.println("cmd nhan duoc la: ");
  // Serial_ARDUIRNO.print(cmd);
  j= cmd.indexOf('O');
  i = cmd.indexOf('K');

  a = cmd.indexOf('+');
  b = cmd.indexOf(',');

  c = cmd.indexOf('K');

  d = cmd.indexOf('+');
  e = cmd.indexOf(':');

  test=cmd.substring(j,i+1);
  test2=cmd.substring(a+1,b+2);
  data_2=cmd.substring(c-1,c+1);
  // data_3=cmd.substring(d,)

  // Serial_ARDUIRNO.println("cmd sau khi check la: ");
  // Serial_ARDUIRNO.println(data_2);
  // Serial_ARDUIRNO.println(test2);

  if(test=="OK" || (data_2=="OK" && test2 =="+CMQTTCONNECT: 0,0") || (data_2=="OK" && test2 =="CMQTTPUB: 0,0"))
  {
    ketqua=1;
  }
  else if(test2 == "CMQTTCONNLOST: 0,2")
  {
    // ketqua=0;
    // ui8_check_disconnect_mqtt=1;
    ui8_chuoi_disconnect=1;
    Serial.println("mat ket noi MQTT");
  }
  else
  {
    ketqua=0;
  }
  return ketqua;
}

void send_at(String _command) 
{
  Serial_SIM.println(_command);
  // wRespon(500);
}

void init_MQTT(char *name,char *myname_device)
{
  char test[100];
  char datastatus[] = "offline";
  char au_topic_pub[100];
  // sprintf(test, "\"client test%s\"", myname_device);
  sprintf(au_topic_pub, "%s/%s/STATUS",name ,myname_device);
  Serial.println(au_topic_pub);
  snprintf(test, 23, "\"CLT_%lld\"", ESP.getEfuseMac());

  // Serial.printf("cliend ID là %c\n", test);
	Serial.println("Connecting To Server...................");
  sendATCommand("AT+CSQ",200);
  sendATCommand("AT+CMQTTSTART",200);
  sendATCommand("AT+CMQTTACCQ=0,"+(String) (test),200);
  // sendATCommand("AT+CMQTTACCQ=0,\"client test5\"", 2000);
  Serial.println("AT+CMQTTACCQ=0,"+(String) (test));
  sendATCommand("AT+CMQTTCFG=\"checkUTF8\",0,0",200);

  // sendATCommand("AT+CMQTTWILLTOPIC=0,"+(String) (strlen(au_topic_pub)),100);
  // Serial.println("AT+CMQTTWILLTOPIC=0,"+(String) (strlen(au_topic_pub)));
  // Serial_SIM.println(au_topic_pub);
  // sendATCommand("AT+CMQTTWILLMSG=0,7,2",100);
  // Serial.println("AT+CMQTTWILLMSG=0,7,2");
  // Serial_SIM.println("offline");
  // delay(100);
}


void res_command(uint32_t waktu, char  *res) 
{
  int kq=0;
  unsigned long cur_time_res, old_time_res;
  String buf_res = "";
  int _i = 0;
  Serial.println();
  Serial.print("tunggu: ");
  Serial.println(waktu);
  Serial.println(res);
  cur_time_res = millis();
  old_time_res = cur_time_res;
  while (cur_time_res - old_time_res < waktu) 
  {
    cur_time_res = millis();
    while (Serial_SIM.available()) 
    {
      _i = Serial_SIM.find(res,strlen(res));
      if (_i == 1) 
      {
        Serial.println("respon ok");
        // return true;
        kq=1;
      }

      else
      {
        kq=0;
      }
    }
    if (_i == 1)break;
  }
  Serial.println("tunggu selesai");
  Serial.println(kq);
  // if(kq==1)
  // {
  //   return kq
  // }
  // return kq;
}

void connected_MQTT(char *data_firstmess,char *nametopic,char *myname_device)
{
  char au_topic_pub_test[100];
  int size_topic;
  int size_data_firstmess;

  char au_topic_pub_topic[100];
  int size_topic_test;
  sprintf(au_topic_pub_topic, "%s/%s/BILL", myname_device);
  size_topic_test = strlen(au_topic_pub_topic);

  sprintf(au_topic_pub_test, "%s/%s/STATUS",nametopic, myname_device);
  size_topic = strlen(au_topic_pub_test);
  Serial.println(size_topic);
  Serial.println(au_topic_pub_test);
  size_data_firstmess = strlen(data_firstmess);
  send_at("AT+CMQTTWILLTOPIC=0,"+(String) (size_topic));
  wRespon(500);
  Serial_SIM.write(au_topic_pub_test);
  wRespon(500);
  send_at("AT+CMQTTWILLMSG=0,"+(String)(size_data_firstmess)+",1");
  wRespon(500);
  Serial_SIM.write(data_firstmess);
  wRespon(500);
  // send_at("AT+CMQTTCONNECT=0,\"tcp://mqtt.altacloud.biz:1883\",60,1,\"altamedia\",\"Altamedia\"");
  // wRespon(1000);  
}


void will_topic_TEST(char *data_firstmess,char *myname_device)
{
  char au_topic_pub_test[100];
  int size_topic;
  int size_data_firstmess;

  char au_topic_pub_topic[100];
  int size_topic_test;
  sprintf(au_topic_pub_topic, "PRINTER/%s/BILL", myname_device);
  size_topic_test = strlen(au_topic_pub_topic);

  sprintf(au_topic_pub_test, "PRINTER/%s/STATUS", myname_device);
  size_topic = strlen(au_topic_pub_test);
  Serial.println(size_topic);
  Serial.println(au_topic_pub_test);
  size_data_firstmess = strlen(data_firstmess);
  send_at("AT+CMQTTTOPIC=0,"+(String) (size_topic));
  wRespon(1000);
  Serial_SIM.write(au_topic_pub_test);
  wRespon(500);
  send_at("AT+CMQTTPAYLOAD=0,"+(String)(size_data_firstmess));
  wRespon(1000);
  Serial_SIM.write(data_firstmess);
  wRespon(500);
  send_at("AT+CMQTTPUB=0,1,60");
  wRespon(3000);

  send_at("AT+CMQTTTOPIC=0,"+(String) (size_topic_test));
  wRespon(3000);
  Serial_SIM.write(au_topic_pub_topic);
  wRespon(1000);
  send_at("AT+CMQTTPAYLOAD=0,6");
  wRespon(3000);
  Serial_SIM.write("hellon");
  wRespon(1000);
  send_at("AT+CMQTTPUB=0,1,60");
  // Serial.println("AT+CMQTTWILLTOPIC=0,"+(String) (size_topic));
  // Serial.println("AT+CMQTTWILLMSG=0,"+(String)(size_data_firstmess)+",1");
}

void will_topic_connectmqtt(char *data_firstmess,char *myname_device)
{
  char au_topic_pub_test[100];
  int size_topic;
  int size_data_firstmess;

  char au_topic_pub_topic[100];
  int size_topic_test;
  sprintf(au_topic_pub_topic, "PRINTER/%s/BILL", myname_device);
  size_topic_test = strlen(au_topic_pub_topic);

  sprintf(au_topic_pub_test, "PRINTER/%s/STATUS", myname_device);
  size_topic = strlen(au_topic_pub_test);
  Serial.println(size_topic);
  Serial.println(au_topic_pub_test);
  size_data_firstmess = strlen(data_firstmess);
  send_at("AT+CMQTTTOPIC=0,"+(String) (size_topic));
  wRespon(3000);
  Serial_SIM.write(au_topic_pub_test);
  wRespon(1000);
  send_at("AT+CMQTTPAYLOAD=0,"+(String)(size_data_firstmess));
  wRespon(3000);
  Serial_SIM.write(data_firstmess);
  wRespon(1000);
  send_at("AT+CMQTTPUB=0,1,60");
}

void send_data_mqtt(String data, int check_buffer,char *myname_device)
{
  uint64_t u = 0;
	uint64_t j = 0;
  String data_chu;
  // for(int i =0; i< check_buffer; i++)
	// {	  	  
	// 	data[u] = i;
	// 	u++;
	// 	j++;
	// 	// if( j >'z') j ='a';	
  //   data_chu+=data[u];  
	// }
	
	// data[u] = '\0';
	// Serial.print("\n buffer pub = ");
	// Serial.println(u);	

  char channel_[100]; 
  int size_channel;
  sprintf(channel_, "PRINTER/%s/BILL", myname_device); 
  size_channel=strlen(channel_);
  send_at("AT+CMQTTTOPIC=0,"+(String) (size_channel));
  wRespon(500);
  Serial_SIM.write(channel_);
  wRespon(500);
  send_at("AT+CMQTTPAYLOAD=0,"+(String)(check_buffer));
  wRespon(500);
  Serial_SIM.println(data);
  wRespon(500);
  send_at("AT+CMQTTPUB=0,1,60");
}

int check_connect_mqtt(char  *res)
{
  int kq=0;
  String input_sim ="";
  while(Serial_SIM.available())
  {
    input_sim = Serial_SIM.readString();
    if(input_sim == res)
    {
      kq=1;
      break;
    }
    else 
    {
      kq=0;
      break;
    }
  }
  // if(res == "+CMQTTCONNECT: 0,0")
  // {
  //   kq=1;
  // }

  // else 
  // {
  //   kq=0;
  // }
  return kq;
}

int gsm_send_serial(String command, int delay)
{
    String buff_resp = "";
    String data = "";
    String data_2="";
    int i_;
    int j_;
    int a_;
    int b_;
    int kq=0;
    Serial.println("Send ->: " + command);
    Serial_SIM.println(command);
    long wtimer = millis();
    while (wtimer + delay > millis())
    {
      while (Serial_SIM.available())
      {
          buff_resp = Serial_SIM.readString();
          Serial.println("tra ve tu sim: ");
          Serial.print(buff_resp);
          // j_= buff_resp.indexOf('+');
          // i_ = buff_resp.indexOf(',');
          j_= buff_resp.indexOf('O');
          i_ = buff_resp.indexOf('K');

          a_ = buff_resp.indexOf('+');
          b_ = buff_resp.indexOf(',');
          // data = buff_resp.substring(j_,i_+2);
          data = buff_resp.substring(j_,i_+1);
          data_2 = buff_resp.substring(a_,b_+2);
          // Serial.print(sub_c);
          // data = sub_c;
          
      }
    }
    Serial.print("da nhan duoc la: ");
    Serial.println(data);
    Serial.println(data_2);

    if((data == "OK" && data_2 == "+CMQTTPUB: 0,0")|| (data == "OK" && data_2 == "+CMQTTCONNECT: 0,0"))
    {
      // bien_kiemtra=1;
      kq=1;
      Serial.println("dung roi"); 
    }
 
    else
    {
      kq=0;
    }
    return kq;
}

int gsm_send_topicpayload(String command, int delay)
{
    String buff_resp = "";
    String data = "";
    int i_;
    int j_;
    int kq=0;
    Serial.println("Send ->: " + command);
    Serial_SIM.println(command);
    wRespon(delay);
    Serial_SIM.println(data);
    long wtimer = millis();
    while (wtimer + delay > millis())
    {
        while (Serial_SIM.available())
        {
            buff_resp = Serial_SIM.readString();
            // Serial.println("tra ve tu sim: ");
            Serial.print(buff_resp);
            j_= buff_resp.indexOf('O');
            i_ = buff_resp.indexOf('K');
            data = buff_resp.substring(j_,i_+1);
            // Serial.print(sub_c);
            // data = sub_c;
            
        }
    }
    Serial.print("da nhan duoc la: ");
    Serial.println(data);

    if(data == "OK")
    {
      // bien_kiemtra=1;
      kq=1;
      Serial.println("dung roi da up load file xong"); 
    }

    else
    {
      kq=0;
    }
    // Serial.println(i_);
    return kq;
}

void disconnect_MQTT()
{
  // char au_topic_pub_test[100];
  // int size_topic;
  // int size_data_finish_mess;

  // sprintf(au_topic_pub_test, "PRINTER/%s/STATUS", myname_device);
  // size_topic = strlen(au_topic_pub_test);
  // Serial.println(size_topic);
  // Serial.println(au_topic_pub_test);
  // size_data_finish_mess = strlen(data_finish_mess);
  // send_at("AT+CMQTTWILLTOPIC=0,"+(String) (size_topic));
  // wRespon(500);
  // Serial_SIM.write(au_topic_pub_test);
  // wRespon(500);
  // send_at("AT+CMQTTWILLMSG=0,"+(String)(size_data_finish_mess)+",1");
  // wRespon(500);
  // Serial_SIM.write(data_finish_mess);
  // wRespon(500);
  // ui8_check_disconnect=0;
  // ui8_check_stopclient=0;
  // ui8_check_stopmqtt=0;
  
  //***BAT DAU PHAN DANH DANG LAM*** */
  // send_at("AT+CMQTTDISC=0,180");
  // ui8_check_disconnect = wRespon(200);
  // delay(100);
  // send_at("AT+CMQTTREL=0");
  // ui8_check_stopclient=wRespon(200);
  // delay(100);
  // send_at("AT+CMQTTSTOP");
  // ui8_check_stopmqtt = wRespon(200);
  // delay(100);
  //***KET THUC PHAN DANH DANG LAM*** */
  // if(ui8_check_disconnect==1 && ui8_check_stopclient==1 && ui8_check_stopmqtt==1)
  // {
  //   // ESP.restart();
  //   ui8_phathien_loisai=0;
  //   ui8_solan_disconnect_4g=0;
  // }

  // Serial_SIM.println("AT+CMQTTDISC=0,180");
  // delay(1000);
  // Serial_SIM.println("AT+CMQTTREL=0");
  // delay(1000);
  // Serial_SIM.println("AT+CMQTTSTOP");
  // delay(1000);



  // sendATCommand("AT+CMQTTDISC=0,180",1000);
  // sendATCommand("AT+CMQTTREL=0",1000);
  // sendATCommand("AT+CMQTTSTOP",1000);

  int result = 0;
  int client_index=0;
    // Bước 1: Ngắt kết nối MQTT
    Serial_SIM.println("AT+CMQTTDISC=" + String(client_index) + ",180"); // 60 là thời gian chờ
    result = sendATCommand("", 1000); // Chờ phản hồi trong 1000ms
    if (result != 1) {
        Serial.println("Lỗi: Không ngắt được kết nối MQTT");
        // return 0; // Trả về 0 nếu thất bại
    }
    Serial.println("Đã ngắt kết nối MQTT thành công");

    // Bước 2: Đóng phiên MQTT
    Serial_SIM.println("AT+CMQTTREL=" + String(client_index));
    result = sendATCommand("", 500); // Chờ phản hồi trong 500ms
    if (result != 1) {
        Serial.println("Lỗi: Không thể đóng phiên MQTT");
        // return 0;
    }
    Serial.println("Đã đóng phiên MQTT thành công");

    // Bước 3: Dừng MQTT hoàn toàn (giải phóng tài nguyên)
    Serial_SIM.println("AT+CMQTTSTOP");
    result = sendATCommand("", 500); // Chờ phản hồi trong 500ms
    if (result != 1) {
        Serial.println("Lỗi: Không thể dừng MQTT");
        // return 0;
    }
    Serial.println("Đã dừng MQTT thành công");

    // return 1; // Trả về 1 nếu tất cả các bước thành công
}


void sub_topic(char *mynamedevice)
{
  int size_channel;
  size_channel = strlen(mynamedevice);
  send_at("AT+CMQTTSUB=0,"+(String) (size_channel)+",0");
  wRespon(200);
  Serial_SIM.write(mynamedevice);
  wRespon(100);
  // send_at("AT+CMQTTSUB=0");
}

// AT+CMQTTSUB=0,9,1

void sub_topic_to_message(char *mynamedevice)
{
  int size_channel;
  size_channel = strlen(mynamedevice);
  send_at("AT+CMQTTSUBTOPIC=0,"+(String) (size_channel)+",1");
  wRespon(200);
  Serial_SIM.write(mynamedevice);
  wRespon(100);
  send_at("AT+CMQTTSUB=0");
  // wRespon(100);
}
int pulic_data_onl(String data, uint64_t check_buffer,char *myname_device)
{
  int size_channel=0;
  int kq_ne=0;
  ui8_check_pub=0;
  ui8_check_payload=0;
  ui8_check_topic=0;
  // int len;
  // len = strlen(data);
  // Serial.println(len);


  /*BAT DAU PHAN DANH DANG LAM*/
  // size_channel = strlen(myname_device);
  // send_at("AT+CMQTTTOPIC=0,"+(String) (size_channel));
  // // Serial_SIM.write(myname_device);
  // delay(30);
  // Serial_SIM.println(myname_device);
  // ui8_check_topic=wRespon(30);
  // if(ui8_check_topic==1 && ui8_check_payload==0)
  // {
  //   ui8_check_topic=0;
  //   Serial.println("nhan duoc ok tu topic");
  //   send_at("AT+CMQTTPAYLOAD=0,"+(String)(check_buffer));
  //   // kq_ne=gsm_send_topicpayload("AT+CMQTTPAYLOAD=0,"+(String)(check_buffer),300);
  //   // wRespon(1000);
  //   delay(200);
  //   Serial_SIM.println(data);
  //   ui8_check_payload=wRespon(30);
  // }

  // if(ui8_check_payload==1)
  // {
  //   Serial.println("nhan duoc ok tu payload");
  //   send_at("AT+CMQTTPUB=0,1,180,1");
  //   kq_ne = wRespon(500);
  //   // ui8_check_pub = wRespon(1000);
  //   // kq_ne=gsm_send_serial("AT+CMQTTPUB=0,1,180,1",30);
  //   // kq_ne = gsm_send_serial("AT+CMQTTPUB=0,1,60",300);
  //   ui8_check_payload=0;
  // }
/*KET THUC PHAN DANH DANG LAM*/

  size_channel = strlen(myname_device);
  // Serial.println(data);
  Serial_SIM.println("AT+CMQTTTOPIC=0,"+(String) (size_channel));
  delay(50);
  ui8_check_topic=sendATCommand(myname_device,30);
  if(ui8_check_topic==1 && ui8_check_payload==0)
  {
    ui8_check_topic=0;
    Serial.println("nhan duoc ok tu topic");
    Serial_SIM.println("AT+CMQTTPAYLOAD=0,"+(String)(check_buffer));
    delay(50);
    ui8_check_payload=sendATCommand(data,30);
  }

  if(ui8_check_payload==1)
  {
    Serial.println("nhan duoc ok tu payload");
    delay(100);
    kq_ne=sendATCommand("AT+CMQTTPUB=0,2,180,1",500);
    ui8_check_payload=0;
  }

  return kq_ne;

    // size_channel = strlen(myname_device);
    // Serial.println("Sending ping to keep alive...");
    // sendATCommand_DANHTEST("AT+CMQTTTOPIC=0,"+(String) (size_channel));
    // delay(100);
    // sendATCommand_DANHTEST(myname_device);
    // delay(100);
    // sendATCommand_DANHTEST("AT+CMQTTPAYLOAD=0,"+(String)(check_buffer));
    // delay(300);
    // sendATCommand_DANHTEST(data);
    // delay(100);
    // sendATCommand_DANHTEST("AT+CMQTTPUB=0,1,180,1"); // Gửi ping giữ kết nối

    // String response = readResponse();
    // return response.indexOf("+CMQTTPUB: 0,0") != -1;  // Kiểm tra xem ping có thành công không
}

int pulic_data(String data, uint64_t check_buffer,char *myname_device)
{
  int size_channel=0;
  int kq_ne=0;
  ui8_check_pub=0;
  ui8_check_payload=0;
  ui8_check_topic=0;
  // int len;
  // len = strlen(data);
  // Serial.println(len);


  /*BAT DAU PHAN DANH DANG LAM*/
  // size_channel = strlen(myname_device);
  // send_at("AT+CMQTTTOPIC=0,"+(String) (size_channel));
  // // Serial_SIM.write(myname_device);
  // delay(30);
  // Serial_SIM.println(myname_device);
  // ui8_check_topic=wRespon(30);
  // if(ui8_check_topic==1 && ui8_check_payload==0)
  // {
  //   ui8_check_topic=0;
  //   Serial.println("nhan duoc ok tu topic");
  //   send_at("AT+CMQTTPAYLOAD=0,"+(String)(check_buffer));
  //   // kq_ne=gsm_send_topicpayload("AT+CMQTTPAYLOAD=0,"+(String)(check_buffer),300);
  //   // wRespon(1000);
  //   delay(200);
  //   Serial_SIM.println(data);
  //   ui8_check_payload=wRespon(30);
  // }

  // if(ui8_check_payload==1)
  // {
  //   Serial.println("nhan duoc ok tu payload");
  //   send_at("AT+CMQTTPUB=0,1,180,1");
  //   kq_ne = wRespon(500);
  //   // ui8_check_pub = wRespon(1000);
  //   // kq_ne=gsm_send_serial("AT+CMQTTPUB=0,1,180,1",30);
  //   // kq_ne = gsm_send_serial("AT+CMQTTPUB=0,1,60",300);
  //   ui8_check_payload=0;
  // }
/*KET THUC PHAN DANH DANG LAM*/

  size_channel = strlen(myname_device);
  // Serial.println(data);
  Serial_SIM.println("AT+CMQTTTOPIC=0,"+(String) (size_channel));
  delay(50);
  ui8_check_topic=sendATCommand(myname_device,30);
  if(ui8_check_topic==1 && ui8_check_payload==0)
  {
    ui8_check_topic=0;
    Serial.println("nhan duoc ok tu topic");
    Serial_SIM.println("AT+CMQTTPAYLOAD=0,"+(String)(check_buffer));
    delay(50);
    ui8_check_payload=sendATCommand(data,30);
  }

  if(ui8_check_payload==1)
  {
    Serial.println("nhan duoc ok tu payload");
    delay(100);
    kq_ne=sendATCommand("AT+CMQTTPUB=0,1,180,1",500);
    ui8_check_payload=0;
  }

  return kq_ne;

    // size_channel = strlen(myname_device);
    // Serial.println("Sending ping to keep alive...");
    // sendATCommand_DANHTEST("AT+CMQTTTOPIC=0,"+(String) (size_channel));
    // delay(100);
    // sendATCommand_DANHTEST(myname_device);
    // delay(100);
    // sendATCommand_DANHTEST("AT+CMQTTPAYLOAD=0,"+(String)(check_buffer));
    // delay(300);
    // sendATCommand_DANHTEST(data);
    // delay(100);
    // sendATCommand_DANHTEST("AT+CMQTTPUB=0,1,180,1"); // Gửi ping giữ kết nối

    // String response = readResponse();
    // return response.indexOf("+CMQTTPUB: 0,0") != -1;  // Kiểm tra xem ping có thành công không
}

void pulic_data_TEST(const char* data, int check_buffer,char *myname_device)
{
  int size_channel;
  int ketqua=0;
  // int len;
  // len = strlen(data);
  // Serial.println(len);
  Serial.println(check_buffer);
  size_channel = strlen(myname_device);
  gsm_send_topicpayload("AT+CMQTTTOPIC=0,"+(String) (size_channel),200);
  // send_at("AT+CMQTTTOPIC=0,"+(String) (size_channel));
  wRespon(200);
  Serial_SIM.println(myname_device);
  Serial.println(myname_device);
  wRespon(300);
  // Serial.write(data,check_buffer);
  gsm_send_topicpayload("AT+CMQTTPAYLOAD=0,"+(String)(check_buffer),200);
  // send_at("AT+CMQTTPAYLOAD=0,"+(String)(check_buffer));
  wRespon(50);
  Serial.println(data);
  Serial_SIM.println("online");
  wRespon(50);
  ketqua=gsm_send_serial("AT+CMQTTPUB=0,2,180,1",300);
  // return ketqua;
  // send_at("AT+CMQTTPUB=0,1,60");
}




void startMQTT() {
  sendATCommand("AT+CMQTTSTART", 10000);
}

void connectToMQTTBroker(char *domain_name, char *username, char *password) {

  sendATCommand("AT+CMQTTACCQ=0,\"client test0\"", 2000);
  sendATCommand("AT+CMQTTCFG=\"checkUTF8\",0,0",2000);
  sendATCommand("AT+CMQTTCONNECT=0,"+(String) (domain_name)+",64800,0,"+(String) (username)+","+(String) (password), 10000);
  sendATCommand("AT+CMQTTSTATUS=0", 2000);
}

void subscribeToTopic(String topic) {
  sendATCommand("AT+CMQTTSUB=0,1,\"" + topic + "\",0", 2000);
}

void publishMessage(String topic, String message) {
  sendATCommand("AT+CMQTTPUB=0,1,\"" + topic + "\",0,0,\"" + message + "\"", 2000);
}

bool isMQTTConnectionLost() {
  sendATCommand("AT+CMQTTSTATUS=0", 2000);
  String response = getATCommandResponse();
  if (response.indexOf("CMQTTCONNLOST") != -1) {
    return true;
  }
  return false;
}

// void check

// // Hàm xử lý phản hồi nhận được từ UART
// void handle_MQTT_callback(char *response) {
//     // Kiểm tra nếu nhận được tin nhắn MQTT
//     if (strstr(response, "+CMQTTRX:")) {
//         printf("MQTT message received: %s\n", response);
//         // Tại đây, bạn có thể xử lý nội dung tin nhắn
//     }
// }

// // Hàm đọc dữ liệu UART và gọi hàm xử lý
// void read_UART_data() {
//     char buffer[256];
//     if (UART_Available()) { // Kiểm tra nếu có dữ liệu từ UART
//         UART_ReadString(buffer); // Đọc dữ liệu vào buffer
//         handle_MQTT_callback(buffer); // Gọi hàm xử lý phản hồi
//     }
// }
// void sendATCommand_DANHTEST(String command) {
//     Serial_SIM.println(command);
//     delay(500);
//     Serial.print("Sent: ");
//     Serial.println(command);
// }



bool isMQTTConnected() {
    sendATCommand_DANHTEST("AT+CMQTTSTATUS=0");
    delay(1000);
    String response = readResponse();
    
    // Kiểm tra xem phản hồi có chứa trạng thái kết nối không
    if (response.indexOf("+CMQTTSTATUS: 0,1") != -1) {
        Serial.println("MQTT connection is active.");
        return true;
    } else {
        Serial.println("MQTT connection is not active.");
        return false;
    }
}



bool connectMQTT(char *myname_device) {
    // Khởi động dịch vụ MQTT

  //    sendATCommand("AT+CSQ",200);
  // sendATCommand("AT+CMQTTSTART",200);
  // sendATCommand("AT+CMQTTACCQ=0,"+(String) (test),200);
  // // sendATCommand("AT+CMQTTACCQ=0,\"client test5\"", 2000);
  // Serial.println("AT+CMQTTACCQ=0,"+(String) (test));
  // sendATCommand("AT+CMQTTCFG=\"checkUTF8\",0,0",200);


    char test[100];
    snprintf(test, 23, "\"CLT_%lld\"", ESP.getEfuseMac());
    sendATCommand_DANHTEST("AT+CSQ");
    delay(200);
    sendATCommand_DANHTEST("AT+CMQTTSTART"); 
    delay(200);
    
    // Đặt Client ID
    sendATCommand_DANHTEST("AT+CMQTTACCQ=0,"+(String) (test));
    delay(200);
    sendATCommand_DANHTEST("AT+CMQTTCFG=\"checkUTF8\",0,0");
    delay(200);
    // Kết nối đến MQTT broker với tên miền, cổng, và thông tin xác thực
    String connectCmd = "AT+CMQTTCONNECT=0,\"tcp://mqtt.altacloud.biz:1883\",180,0,\"altamedia\",\"Altamedia\"";
    sendATCommand_DANHTEST(connectCmd);
    delay(500);

    String response = readResponse();
    return response.indexOf("+CMQTTCONNECT: 0,0") != -1;  // Kiểm tra kết nối
    // Kiểm tra xem kết nối MQTT có thành công không
    // if (isMQTTConnected()) {
    //     Serial.println("Connected to MQTT broker successfully.");
    //     return true;
    // } else {
    //     Serial.println("Failed to connect to MQTT broker.");
    //     return false;
    // }
}

// bool isMQTTConnected() {
//     sendATCommand_DANHTEST("AT+CMQTTSTATUS=0");
//     delay(1000);
//     String response = readResponse();
//     return response.indexOf("+CMQTTSTATUS: 0,1") != -1;  // Kiểm tra kết nối
// }

// void subscribeTopics(char *topic) 
// {
//     // String topics[] = {"home/status", "home/temperature", "home/humidity"};  // Các topic cần đăng ký
//     // for (int i = 0; i < sizeof(topics) / sizeof(topics[0]); i++) {
//     int size_channel;
//     size_channel = strlen(topic);
//     String subscribeCmd = "AT+CMQTTSUB=0,"+(String) (size_channel)+",2";
//     // Serial.println(subscribeCmd);
//     sendATCommand(subscribeCmd,100);
//     delay(100);
//     Serial_SIM.write(topic);
//     // Serial.println(topic);
//     delay(200);
//         // Serial.println("Subscribed to topic: " + topic);
//     // }
// }

void subscribeTopic(const char *topic) 
{
    int clientIndex = 0;  // Chỉ số client, mặc định là 0
    int qos = 2;          // Chọn QoS, có thể là 0, 1 hoặc 2
    int topicLength = strlen(topic);  // Độ dài của topic

    // Tạo chuỗi lệnh subscribe
    String subscribeCmd = "AT+CMQTTSUB=" + String(clientIndex) + "," + String(topicLength) + "," + String(qos);

    // Gửi lệnh AT để đăng ký topic
    sendATCommand(subscribeCmd, 1000);  // Gửi lệnh và chờ phản hồi
    delay(200);  // Delay để đảm bảo module sẵn sàng nhận dữ liệu

    // Gửi tên topic sau khi nhận phản hồi '>'
    Serial_SIM.println(topic);
    delay(200);  // Delay nhỏ để hoàn thành gửi

    // Thông báo đã đăng ký thành công
    Serial.println("Subscribed to topic: " + String(topic));
}


bool sendPing(const char *mynamedevice) {

    char au_topic_pub[100];
    int size_topic;
    sprintf(au_topic_pub, "PRINTER/%s/STATUS", mynamedevice);
    size_topic = strlen(au_topic_pub);
    Serial.println("Sending ping to keep alive...");
    sendATCommand_DANHTEST("AT+CMQTTTOPIC=0,"+(String) (size_topic));
    delay(500);
    sendATCommand_DANHTEST(au_topic_pub);
    delay(500);
    sendATCommand_DANHTEST("AT+CMQTTPAYLOAD=0,5");
    delay(500);
    sendATCommand_DANHTEST("Online");
    delay(500);
    sendATCommand_DANHTEST("AT+CMQTTPUB=0,1,60"); // Gửi ping giữ kết nối

    String response = readResponse();
    return response.indexOf("+CMQTTPUB: 0,0") != -1;  // Kiểm tra xem ping có thành công không
}

void checkForIncomingMessages() {
    if (Serial_SIM.available()) {
        String response = readResponse();
        if (response.indexOf("+CMQTTRX:") != -1) {
            // Phân tích tin nhắn nhận được
            int topicStart = response.indexOf("\"") + 1;
            int topicEnd = response.indexOf("\"", topicStart);
            String topic = response.substring(topicStart, topicEnd);

            int messageStart = response.lastIndexOf("\"") + 1;
            String message = response.substring(messageStart);  

            Serial.print("Message received on topic ");
            Serial.print(topic);
            Serial.print(": ");
            Serial.println(message);
        }
    }
}



// String readResponse() {
//     String response = "";
//     while (Serial_SIM.available()) {
//         response += char(Serial_SIM.read());
//     }
//     Serial.print("Response: ");
//     Serial.println(response);
//     return response;
// }

void connect()
{
  // if (!isMQTTConnected()) 
	// 	{
	// 		Serial.println("Reconnecting to MQTT...");
	// 		connectMQTT();
	// 		// subscribeTopics();  // Đăng ký lại tất cả các topic sau khi kết nối lại
  //   	}
}