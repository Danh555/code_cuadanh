/*VERSION MOI them phan update fw cho slave*/

#include "include/GlobalVariables.h"
#include "include/SensorMotorDefine.h" //MOTOR VA SENSOR DUOC DEFINE TRONG NAY. THAY DOI DEFINE COCACOLA HOAC HEINEKEN TUONG UNG CHO MAY
#include "lib/DONGCO.h"
#include "lib/CRC16.h"
#include <EEPROM.h>
#include "SerialComunicationv2.h"
#include <Wire.h> 
#include "RS485_PROTOCOL.h"
#include "include/definefunction.h"
#include "lib/PROTOCOL.h"
#include <SPIFFS.h>
#include "esp_task_wdt.h"
// #include "Debug_usb.h"

DONGCO Motor_cua(DIR_MOTOR1,PWM_MOTOR1);
DONGCO Motor_bangtai(DIR_MOTOR2,PWM_MOTOR2);
DONGCO Motor_xoaychai(DIR_MOTOR3,PWM_MOTOR3);
DONGCO Motor_nangha(DIR_MOTOR4,PWM_MOTOR4);


// RS485_Master RS485(Serial1, TX_PIN, RX_PIN);
// SemaphoreHandle_t spiffsMutex; // Khai báo Semaphore bảo vệ SPIFFS
SemaphoreHandle_t batton = NULL;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(50);
  Serial_Debug.onEvent(usbEventCallback);
  Serial_Debug.begin();
  delay(50);
  RS485.begin(115200);
  delay(50);
  EEPROM.begin(4096);
  config();

  Serial_Debug.println("hello");

  if (!SPIFFS.begin(true))
  {
    Serial_Debug.println("Failed to mount SPIFFS");
    return;
  }

  #ifdef DEBUG_EEPROM
		EEPROM.put(0, motor_nangha.ui8_kiemtra_trangthailoi_motorDC);
		EEPROM.put(1, motor_nghien.ui8_kiemtra_trangthailoi);
		EEPROM.put(2, motor_ep.ui8_kiemtra_trangthailoi);
		EEPROM.put(3, motor_phanloai.ui8_kiemtra_trangthailoi);
		EEPROM.put(10, ui16_sochai_trongthungrac);
		EEPROM.put(12, ui16_solon_trongthungrac);
		EEPROM.put(14, ui16_vat_trongthungrackhac);
    EEPROM.put(16, ui8_trangthaithungracchai);
    EEPROM.put(17, ui8_trangthaithungraclon);
    EEPROM.put(18, ui8_trangthaithungrackhac);

		// EEPROM.put(14, ui8_trangthaithungracchai);
		// EEPROM.put(15, ui8_trangthaithungraclon);
		EEPROM.commit();
	#else
		EEPROM.get(0, motor_nangha.ui8_kiemtra_trangthailoi_motorDC);
		EEPROM.get(1, motor_nghien.ui8_kiemtra_trangthailoi);
		EEPROM.get(2, motor_ep.ui8_kiemtra_trangthailoi);
		EEPROM.get(3, motor_phanloai.ui8_kiemtra_trangthailoi);
		EEPROM.get(10, ui16_sochai_trongthungrac);
		EEPROM.get(12, ui16_solon_trongthungrac);
		EEPROM.get(14, ui16_vat_trongthungrackhac);
    EEPROM.get(16, ui8_trangthaithungracchai);
    EEPROM.get(17, ui8_trangthaithungraclon);
    EEPROM.get(18, ui8_trangthaithungrackhac);
		// EEPROM.get(14, ui8_trangthaithungracchai);
		// EEPROM.get(15, ui8_trangthaithungraclon);
	#endif
  esp_task_wdt_deinit();
  ui8_khongcheck_receive=0;
  ui8_danggui_fw=1;
  Serial_Debug.printf("\ntrangthai motor nghien: %d\n",motor_nghien.ui8_kiemtra_trangthailoi);
  Serial_Debug.printf("\ntrangthai bien gui fw: %d\n",ui8_danggui_fw);
  // SETTRANGTHAIMAY();
  // ui8_sendfw_slave=0x01;
  // ui8_khongcheck_receive=1;
  timeout=millis()+10000;
  // ui32_timeout_guifw=millis()+10000;
  ui32_timeout_guiphanhoi=millis()+20000;
  ui32_timeout_hetguifw=millis()+5000;
  ui8_upfw_slave1=1;
  // if(ui8_trangthaimay>0)
  // {
    
  // }
  ui8_trangthai_nhavat = TrangThaiRoiVat::NHACHAI;
  // ui8_trangthai_motor_nangha = 1;
  // caidatgiatribandau();
  // ui32_timeout_guilenhfirmware = millis();
  send_firmware();

  for(int i=0; i<sizeof(ui8_slave); i++)
  {
    ui32_timeout_checkrs485[i]=millis()+1000;  
  }

  batton = xSemaphoreCreateBinary();
	
	xSemaphoreGive(batton);

  // spiffsMutex = xSemaphoreCreateBinary();  // Khởi tạo semaphore
  // xSemaphoreGive(spiffsMutex); // Cho phép sử dụng SPIFFS ngay từ đầu
	
	xTaskCreatePinnedToCore(
      Task1core, /* Function to implement the task */
      "Task1", /* Name of the task */
      30000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      1,  /* Priority of the task */
      NULL,  /* Task handle. */
      0); /* Core where the task should run */
	
	delay(500);
	xTaskCreatePinnedToCore(
      Task2core, /* Function to implement the task */
      "Task2", /* Name of the task */
      30000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      1,  /* Priority of the task */
      NULL,  /* Task handle. */
      1); /* Core where the task should run */
	  
	
	// vTaskStartScheduler();
	// ui8_is_loop = 1;
	vTaskDelete(NULL);
	delay(500);
  timeout=millis()+2000;
}

void loop() {
}

void Task1core(void *parameter)
{
  for(;;)
  {
    xSemaphoreTake(batton, portMAX_DELAY);
    guilenhlenPC();
		readSerial();
    DOCNUTNHAN();
    // check_slave();
    // check_slave1(0x01);
    send_oder_to_slave();
    receive_respond_from_slave();
    check_loi();
    send_fw_toslave();

    if(millis()>timeout)
    {
      ui8_danggui_fw=1;
      quet_tenfile(SPIFFS,"/", 0);
      // timeout=millis()+2000;
    }

    if(ui2_update_firmware==true)
    {
      update_firmware_slave_RS485(ui8_fw_can_up);
      // Serial_Debug.print("file update: ");
      // Serial_Debug.println(filename_update);
      sendFirmware(filename_update);
    }

    if(ui32_timeout_guiphanhoi<=millis())
    {
      ui8_danggui_fw=0;
    }

    xSemaphoreGive(batton);
		vTaskDelay(1);

  }
}

void Task2core(void *paramter)
{ 
  for(;;)
  {
    xSemaphoreTake(batton, portMAX_DELAY);
    kiemtracuaket();
		KIEMTRATATDONGCO();
    kiemtrakimloai();
    kt_bonghien();

    if(ui8_rs_master==1 && ui8_reset_all==0)
    {
      ESP.restart();
      ui8_rs_master=0;
    }


    if(ui8_is_stopuser == 1)
		{
      //moi them vao
      // static int check_trangthai=0;
			// if(check_trangthai==0 && ui8_trangthaimay==0 && ui8_xoaloi!=1)
			// {
			// 	Serial_Debug.println("ko co loi tien hanh xoay nguoc");
			// 	ui8_ketthucnghien=1;
			// 	ui32_timeout_quaynguocnghien=millis()+10000;
			// 	check_trangthai=1;
			// }

			// if(check_trangthai==0 && ui8_trangthaimay>0)
			// {
			// 	Serial_Debug.println("co loi tien hanh dung luon bo nghien");
      //   ui8_control_BoNghien=0x00;
			// 	// control_BoNghien(0x00);
			// 	// ui32_timecho_guilenh_485=millis()+500;
			// 	check_trangthai=1;
			// }
      //moi them vao
			caidatgiatribandau();
      if(motor_ep.ui8_kiemtra_trangthailoi != TrangthaiLoi::LOI)
      {
        Serial_Debug.println("xoa nguoc bo nghien");
        ui8_ketthucnghien=1;
				ui32_timeout_quaynguocnghien=millis()+10000;
      }
		}

    if(ui8_startprocess == 0)
		{
			kiemtrathungrac();
			kiemtracuamay();
		}

    if(ui8_hacuadi == 1 ) 
		{
			ui8_chophephacua=1;
			if(ui8_trangthaicua != TrangThaiMotorDC::KET)
			{
				stop_Motor_Cua();
				run_Motor_Cua_Haxuong();  
			}
				ui8_hacuadi = 0;
		}

    if(ui8_trangthaimay>0)
		{
			BATLED_LOI();
			ui8_kethuc = 1;
		}

    else 
    {
      BATLED_RUN();
      if(ui8_startprocess==1)
			{
				// TATLED();
				if(process_1==0)
				{
					quatrinh_1();
				}
				else if(process_2==0)
				{
					quatrinh_2();
				}

				else if(process_3==0)
				{
					quatrinh_3();
				}
			}

			else if(ui8_startprocess == 0 && motor_cua.ui8_trangthaimotor == TrangThaiMotorDC::DUNG) BATLED_CHAOMOI();
			else if(motor_cua.ui8_trangthaimotor == TrangThaiMotorDC::HAXUONG) BATLED_CANHBAO();
    }
    xSemaphoreGive(batton);
		vTaskDelay(1);
  }
}

/*---------------RUN MOTOR SAI MODULE BTS7960------------------*/
void runMotor(DONGCO &motorController, uint8_t tocdo, uint8_t chieuquay, uint16_t &lastspeed) {
  register uint16_t u16_setspeed = tocdo * 256 / 110;
  if (u16_setspeed > 255) u16_setspeed = 255;
  else if (u16_setspeed < 0) u16_setspeed = 0;
  if (u16_setspeed > lastspeed) {
    for (int speed = lastspeed ; speed <= u16_setspeed; speed += 10)
    {
      if (chieuquay == 0) {
        motorController.TurnLeft(speed);
        delay(10);
      }
      else if (chieuquay == 1) {
        motorController.TurnRight(speed);
        delay(10);
      }
    }
  }
  else if (u16_setspeed < lastspeed) {
    for (int speed = lastspeed ; speed >= u16_setspeed; speed -= 10)
    {
      if (chieuquay == 0) {
        motorController.TurnLeft(speed);
        delay(10);
      }
      else if (chieuquay == 1) {
        motorController.TurnRight(speed);
        delay(10);
      }
    }
  }
  lastspeed = u16_setspeed;
}

static void usbEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  if (event_base == ARDUINO_HW_CDC_EVENTS) {
    switch (event_id) {
      case ARDUINO_HW_CDC_RX_EVENT:
        //Serial0.println("\nCDC EVENT:: ARDUINO_HW_CDC_RX_EVENT");
        // sends all bytes read from USB Hardware Serial to UART0
        while (Serial_Debug.available()) 
        {
          // Serial_Debug.write(Serial_Debug.read());
          String input = Serial_Debug.readString();
          if(input=="start")
          {
            START_USER();
              ui8_kethuc=0;
            ui8_hacuadi = 0;
            // control_BoNghien(0x01,ui8_slave[1]);
            ui8_control_BoNghien=0x01;
            ui8_xoaloi=0;
          }

          if(input=="tatled")
          {
            Serial_Debug.printf("tat led rgb\n");
            TATLED();
          }

          if(input=="led green")
          {
            Serial_Debug.printf("led green");
            LEDGREEN();
          }
          if(input=="reset")
          {
            ESP.restart();
          }

          if(input=="stop")
          {
            STOP_USER();
            ui8_kethuc=1;
          }

          if(input=="xoaloi")
          {
            xoaloi();
            xoathungrac();
          }

          // if(input=="nhaqua")
          // {
          //   Serial_Debug.println("NHAN LENH NHA QUA");
          //   ui16_last_order_command = ui16_order_command;
          //   // ui8_status = 0x00;
          //   // ui8_sobyte = uc8_data[4];
          //   // ui8_phanhoi_dungFunction = 1;
          //   // int so_khay = 0;
          //   // int so_o = 0;
          //   // // so_khay = uc8_data[5];
          //   // // so_o = uc8_data[6];
          //   ui8_vitrikhay_sp = uint8_t(so_khay);
          //   ui8_vitri_sp = uint8_t(so_o);
          //   control_mayphatqua(ui8_vitrikhay_sp, ui8_vitri_sp, ui8_slave[4]);
          // }


          if(input=="ndchai")
          {
             if(process_2==1)
             {
                ui8_trangthai_nhandien=TrangThaiNhanDien::NHANDIENCHAI;
                Serial_Debug.printf("NHAN DIEN CHAI\n");
                ui8_trangthai_nhavat=TrangThaiRoiVat::NHACHAI;
                ui8_control_BoPhanLoai=0x01;
             }
            
            // control_BoPhanLoai(0x01,ui8_slave[1]);
          }
          
          if(input=="ndlon")
          {
            if(process_2==1)
            {
              ui8_trangthai_nhandien=TrangThaiNhanDien::NHANDIENLON;
              ui8_trangthai_nhavat=TrangThaiRoiVat::NHALON;
              ui8_lalon=1;
              ui8_control_BoPhanLoai=0x02;
              // control_BoPhanLoai(0x02,ui8_slave[1]);
              Serial_Debug.printf("NHAN DIEN LON\n");
            }
            
          }

          if(input=="ndkhac")
          {
            if(process_2==1)
            {
              ui8_trangthai_nhandien=TrangThaiNhanDien::NHANDIENKHAC;
              ui8_trangthai_nhavat=TrangThaiRoiVat::NHAKHAC;
              ui8_control_BoPhanLoai=0x03;
              // control_BoPhanLoai(0x03,ui8_slave[1]);
              Serial_Debug.printf("NHAN DIEN KHAC\n");
            }
            
          }

          if(input=="ndsai")
          {
            if(process_2==1)
            {
              ui8_trangthai_nhandien=TrangThaiNhanDien::NHANDIENSAI;
              ui8_trangthai_nhavat=TrangThaiRoiVat::NHACHAI;
              Serial_Debug.printf("NHAN DIEN SAI\n");
            }
          }

          if(input=="runnangha")
          {
            run_Motor_nangha();
          }

          if(input=="stopnangha")
          {
            stop_Motor_nangha();
          }

          if(input=="runxoaychai")
          {
            run_Motor_Xoaychai();
          }

          if(input=="stopxoaychai")
          {
            stop_Motor_xoaychai();
          }

          if(input=="runbangtaivao")
          {
            run_Motor_bangtai(chieuquay_bangtai_vao);
          }

          if(input=="runbangtaira")
          {
            run_Motor_bangtai(chieuquay_bangtai_ra);
          }

          if(input=="stopbangtai")
          {
            stop_Motor_bangtai();
          }

          if(input=="mocuatren")
          {
            // mocua_trenduoi(0x01,ui8_slave[3]);
            Serial_Debug.printf("mo cua tren\n");
            ui8_mocua=0x01;
            ui8_trangthaicua_cuamay=0x02;
          }

          if(input=="dongcuatren")
          {
            // dongcua_trenduoi(0x01,ui8_slave[3]);
            Serial_Debug.printf("dong cua tren\n");
            ui8_mocua=0x01;
            ui8_trangthaicua_cuamay=0x01;
          }

          if(input=="mocuaduoi")
          {
            // mocua_trenduoi(0x02,ui8_slave[3]);
            Serial_Debug.printf("mo cua duoi\n"); 
          //  ui8_mocuaduoi = 0x02;
            ui8_mocua=0x02;
            ui8_trangthaicua_cuamay=0x02;
          } 

          if(input=="dongcuaduoi")
          {
            // dongcua_trenduoi(0x02,ui8_slave[3]);
            Serial_Debug.printf("dong cua duoi\n");
            ui8_mocua=0x02;
            ui8_trangthaicua_cuamay=0x01;
          }      

          if(input=="nhahang")
          {
            Serial_Debug.printf("nha qua test\n");
            // control_mayphatqua(0x02, 0x02, ui8_slave[4]);
            ui8_control_hang=0x01;
            ui8_control_cot=0x01;
            
          }  

          if(input=="resetall")
          {
            Serial_Debug.printf("reset tat ca\n");
            ui8_reset_all=0x05;
          }

          if(input=="guifw")
          {
            Serial_Debug.printf("gui fw slave 1");
            ui32_timeout_guiphanhoi=millis()+20000;
            ui8_sendfw_slave=0x01;
            ui8_danggui_fw=1;
            ui8_upfw_slave1=1;
          }

          if(input=="sendfw")
          {

          }
        }
    break;
    default: break;
    }
  }
}

/*-----------------DUNG MOTOR DUNG MODULE DONGCO--------------------*/
void stopMotor(DONGCO &motorController, uint16_t &lastspeed)
{
  motorController.Stop();
  lastspeed = 0;
}

/*********************RUN MOTOR CỬA NÂNG LÊN*************************/
void run_Motor_Cua_Nanglen()
{
  runMotor(Motor_cua, motor_cua.ui16_tocdo, chieuquay_cuachai_nanglen, motor_cua.u16_lastspeed);
  motor_cua.ui8_tatmotor = 1;
  motor_cua.ui32_timeout_dongmo_motor = millis() + motor_cua.ui32_timeout_cho_motor; 
  motor_cua.ui8_trangthaimotor = TrangThaiMotorDC::NANGLEN;
	Serial_Debug.printf("NANG MOTOR CUA\n");
}


/*********************RUN MOTOR CỬA HẠ XUỐNG*************************/
void run_Motor_Cua_Haxuong()
{
  runMotor(Motor_cua, motor_cua.ui16_tocdo, chieuquay_cuachai_haxuong, motor_cua.u16_lastspeed);
  motor_cua.ui8_tatmotor = 1;
  motor_cua.ui32_timeout_dongmo_motor = millis() + motor_cua.ui32_timeout_cho_motor; //TẮT MOTOR SAU 10S
  motor_cua.ui8_trangthaimotor = TrangThaiMotorDC::HAXUONG;
	Serial_Debug.printf("HA MOTOR CUA\n");
}

/********************STOP MOTOR CỬA**********************/
void stop_Motor_Cua()
{
  stopMotor(Motor_cua, motor_cua.u16_lastspeed);
	motor_cua.ui8_tatmotor = 0;
	motor_cua.ui8_trangthaimotor = TrangThaiMotorDC::DUNG;
  Serial_Debug.printf("DUNG MOTOR CUA\n");
}

void run_Motor_bangtai(uint8_t chieuquay)
{
  runMotor(Motor_bangtai,motor_bangtai.ui16_tocdo,chieuquay,motor_bangtai.u16_lastspeed);
  if(chieuquay==0)
  {
    motor_bangtai.ui8_trangthaimotor = TrangThaiMotorDC::VAO;  
  }
  if(chieuquay==1)
  {
    motor_bangtai.ui8_trangthaimotor = TrangThaiMotorDC::RA;
  }
  motor_bangtai.ui8_tatmotor=1;
  Serial_Debug.printf("chay motor bang tai\n");
}

void stop_Motor_bangtai()
{
  stopMotor(Motor_bangtai,motor_bangtai.u16_lastspeed);
  motor_bangtai.ui8_tatmotor=0;
  motor_bangtai.ui8_trangthaimotor=TrangThaiMotorDC::DUNG;
  Serial_Debug.printf("dung motor bang tai\n");
}

void run_Motor_nangha()
{
  runMotor(Motor_nangha,motor_nangha.ui16_tocdo,chieu_nang_ha,motor_nangha.u16_lastspeed);
	motor_nangha.ui8_tatmotor=1;
  motor_nangha.ui8_trangthaimotor=TrangThaiMotorDC::DANGNANG;
}

void stop_Motor_nangha()
{
	stopMotor(Motor_nangha,motor_nangha.u16_lastspeed);
	motor_nangha.ui8_trangthaimotor=TrangThaiMotorDC::DUNG;
	motor_nangha.ui8_tatmotor=0;
}

/*********************RUN MOTOR XOAY CHAI*************************/
void run_Motor_Xoaychai()
{
    runMotor(Motor_xoaychai, motor_xoaychai.ui16_tocdo, chieuquay_xoaychai, motor_xoaychai.u16_lastspeed);
}

void stop_Motor_xoaychai()
{
	stopMotor(Motor_xoaychai, motor_xoaychai.u16_lastspeed);
	motor_xoaychai.ui8_tatmotor = 0;
}

void KIEMTRATATDONGCO()
{
	if(motor_cua.ui8_tatmotor==1)
	{
		if(motor_cua.ui32_timeout_dongmo_motor <= millis()){
			stop_Motor_Cua();
			Serial_Debug.println("DA TAT DONG CO CUA");
			
		}
	}
}

/**********************KIỂM TRA CỬA KẸT*********************/
void kiemtracuaket()
{
    int tam1 = doccambien(cambien_cua1,20);
    int tam2 = doccambien(cambien_cua2,20);
    int tam3 = doccambien(cambien_cua3,20);
	  int tam4 = doccambien(cambien_cua4,20);
    register int tong = tam1 + tam2 + tam3 + tam4;
    if(tong>0 && ui8_trangthaicua == TrangThaiMotorDC::KHONGKET)
    { //NẾU CỬA ĐANG BỊ KẸT
			ui8_trangthaicua = TrangThaiMotorDC::KET;
			Serial_Debug.printf("CUA KET\n");   
			//STOP MOTOR CỬA VÀ HẠ MOTOR XUỐNG
			stop_Motor_Cua();
			run_Motor_Cua_Nanglen();
    }
	
    else if(tong == 0 && ui8_trangthaicua == TrangThaiMotorDC::KET){ //NẾU KHÔNG KẸT 
      Serial_Debug.printf("CUA KHONG KET\n");
      ui8_trangthaicua = TrangThaiMotorDC::KHONGKET;

      //NẾU ĐÃ KẾT THÚC QUÁ TRÌNH THÌ ĐÓNG CỬA
      // if(ui8_startprocess == 0) {
      if(ui8_kethuc == 1|| ui8_startprocess==0) {
        Serial_Debug.printf("CUA DANG XUONG\n");
        stop_Motor_Cua();
        run_Motor_Cua_Haxuong();
      }
    }
}

int doccambien(int pin_cambien,uint32_t timeout)
{
  int index;
  for(int i=0;i<=sizeof(CB);i++)
  {
    if(CB[i] == pin_cambien)
    {
      // Serial_Debug.printf("gia tri pin cambien la: %d\n",i);
      index=i;
      break;
    }
  }

  int reading = digitalRead(pin_cambien);
  if (reading != lastButtonState[index]) {		
		lastDebounceTime[index] = millis();	
  }

  if ((millis() - lastDebounceTime[index]) > timeout) {
	if(reading == LOW){
        return 1;
    }	
  }

  lastButtonState[index] = reading;
  return 0;
}


void kiemtrakimloai()
{
	if(process_1==0) return;
	int phathien_lon1=doccambien(cambien_kimloai1,20);
	int phathien_lon2=doccambien(cambien_kimloai2,20);
	register int tong = phathien_lon1+phathien_lon2;
	if(tong > 0 && ui8_phathien_lon==0)
	{
		ui8_phathien_lon = 1;
		Serial_Debug.printf("phat hien kim loai\n");
    cb_kimloai.ui8_phanhoi_trangthai_cambien=1;
		// ui8_phanhoi_trangthai_kimloai=1;
	}

	else if(tong == 0 && ui8_phathien_lon==1)
	{
		ui8_phathien_lon=0;
		Serial_Debug.printf("het phat hien kim loai\n");
	}
}


int chaynangbangtai()
{
	static uint8_t chophepnhavat = 0;
	static uint8_t dachaymotor = 0;
	static uint8_t dungdongco = 0; 
	static uint8_t khacvitrihome = 0;
	// ui8_kiemtramotorphanloai=BAN;
	if(motor_nangha.ui8_trangthaimotor != TrangThaiMotorDC::DANGNANG && dachaymotor==0)
	{
		run_Motor_nangha();
		motor_nangha.ui32_timeout_dongmo_motor = millis();
		dachaymotor = 1;

		int tam = doccambien(cambien_nangha,20);
		if(tam == 0) 
		{
			khacvitrihome = 1;
		}
		Serial_Debug.printf("DA KHOI DONG MOTOR NANG HA\n");

	}

	if(motor_nangha.ui32_timeout_dongmo_motor<=millis())
	{
		int tam = doccambien(cambien_nangha, 20);
		if(tam == 1)
		{
			if(motor_nangha.ui32_timeout_dongmo_motor + 2200 >= millis() && motor_nangha.ui32_timeout_dongmo_motor + 2000 <= millis() && khacvitrihome == 0)
			{
				chophepnhavat = 0;
				dachaymotor = 0;
				dungdongco = 0; 
				khacvitrihome = 0;
				// ui8_trangthai_nhavat=TrangThaiRoiVat::KHONGNHA;
				Serial_Debug.printf("DONG CO NHA VAT KHONG CHAY\n");
				return 0;
			}
			else if(khacvitrihome == 1 || (khacvitrihome == 0 && (motor_nangha.ui32_timeout_dongmo_motor + 2200 <= millis())))
			{
				Serial_Debug.printf("da ve home nang ha\n");
				stop_Motor_nangha();
				// dataReceived[5]=0x02;
				// dataReceived[6]=0x02;
				// dataReceived[7]=0x02;
				// ui8_kiemtramotorphanloai=BAN;
				// ui8_kiemtramotoreplon=BAN;
				ui8_trangthai_nhavat=TrangThaiRoiVat::KHONGNHA;
				dachaymotor = 0;
				chophepnhavat = 0;
				dungdongco = 0;
				khacvitrihome = 0;
				return 1;
			}
		}

		else if(chophepnhavat==0)
		{
			if(motor_nangha.ui32_timeout_dongmo_motor+2200<=millis())
			{
				chophepnhavat=1;
				ui32_timeout_kiemtramotornhavat = millis();
				Serial_Debug.println("CHO PHEP NHA VAT XUONG");
			}
		}

		else if(chophepnhavat==1)
		{
			if(motor_nangha.ui8_trangthaimotor!=TrangThaiMotorDC::DANGNANG)
			{
				run_Motor_nangha();
				Serial_Debug.printf("DA KHOI DONG LAI MOTOR NANG HA\n");
			}

			if(ui32_timeout_kiemtramotornhavat+5000<millis())
			{
				//LOI
				chophepnhavat = 0;
				dachaymotor = 0;
				dungdongco = 0; 
				khacvitrihome = 0;
				// hoanthanh_kiemtra_nhavat = 0;
				Serial_Debug.printf("DONG CO NHA VAT KHONG VE HOME\n");
				// kiemtracuaket();
				// ui8_trangthai_nhavat=TrangThaiRoiVat::KHONGNHA;
				return 0;
			}
			
		}
	}
	return -1;
}	


void LEDRED() {

	BATLED(0,255,255);
}

void LEDGREEN() {

	BATLED(255,0,255);
}

void LEDBLUE() {

	BATLED(255,255,0);
}

void TATLED(){

	BATLED(255,255,255);
}

void LEDYELLOW(){
	
	BATLED(0,80,255);
}

void LEDPURPLE(){
	
	BATLED(0,255,0);
}
void LEDORANGE(){
	
	BATLED(0,210,255);
}

void LEDWHITE(){
	BATLED(255,255,255);
}

void LEDLOIBANGTAI(){
	BATLED(0,0,0);
}

void BATTAT_LED_NHANDIEN(int state){
    analogWrite(led_white,state);
}

void BATLED_RUN()
{
  int tam = digitalRead(ledon_board1);
  static uint32_t thoigianled = 0;
  static const uint16_t thoigianled_binhthuong = 1000;
  if(millis() - thoigianled >= thoigianled_binhthuong){
    digitalWrite(ledon_board1, !tam);
    thoigianled = millis();
  }
}

void BATLED_CHAOMOI()
{	
	static uint32_t ui32_timeled_chaomoi = 0;
  static uint8_t ui8_ledstate_chaomoi = 0;
	XULYLED(ui32_timeled_chaomoi, 1000, ui8_ledstate_chaomoi, LEDGREEN);
}

void BATLED_CANHBAO()
{
	static uint32_t ui32_timeled_canhbao = 0;
    static uint8_t ui8_ledstate_canhbao = 0;
	XULYLED(ui32_timeled_canhbao, 200, ui8_ledstate_canhbao, LEDYELLOW);
	
}

void BATLED_LOI()
{
    static uint32_t ui32_timeled = 0;
    static uint8_t ui8_ledstate = 0;
	if(motor_nghien.ui8_kiemtra_trangthailoi == TrangthaiLoi::LOI){
		XULYLED(ui32_timeled, 1000, ui8_ledstate, LEDRED);
	}
	else if(motor_phanloai.ui8_kiemtra_trangthailoi == TrangthaiLoi::LOI){
		XULYLED(ui32_timeled, 500, ui8_ledstate, LEDLOIBANGTAI);
	}
	else if(motor_ep.ui8_kiemtra_trangthailoi == TrangthaiLoi::LOI){
		XULYLED(ui32_timeled, 500, ui8_ledstate, LEDORANGE);
	}
	else if(motor_nangha.ui8_kiemtra_trangthailoi_motorDC == TrangthaiLoi::LOI){
		XULYLED(ui32_timeled, 500, ui8_ledstate, LEDPURPLE);
	}
  else if((ui8_trangthaithungracchai == TrangThaiThungRac::DAY || ui8_trangthaithungraclon == TrangThaiThungRac::DAY || ui8_trangthaithungrackhac==TrangThaiThungRac::DAY))
  {
	  XULYLED(ui32_timeled, 500, ui8_ledstate, LEDBLUE);
  }
}

void readSerial() {
  if (Serial.available()) 
  {
    ui32_timechar = millis();
    while (Serial.available()) {
      unsigned char inChar = (unsigned char)Serial.read();
      if (!ui2_start) {
        ui2_BTreceive = true;
        ui2_start = true;
        ui8_Byteindex = 0;
      }
      if (ui2_start && (millis() - ui32_timechar) < 10) {
        ui8_data_rec[ui8_Byteindex] = inChar;
        ui8_Byteindex++;
        ui32_timechar = millis();
      }
    }
  }
  if (ui2_BTreceive && ((millis() - ui32_timechar) > 20)) 
  {
    ui2_BTreceive = false;
    ui2_BTcomplete = true;
  }
  if (ui2_BTcomplete == true) {
    if (ui8_Byteindex != 0 && ui8_data_rec[0] != ui8_slaveID) 
    {

      ui8_status = 0x02;
      ui8_data_rec[0] = 0x02;
      ui8_Byteindex = 0;
      ui2_BTcomplete = false;
      ui2_start = false;
    }
    else if (ui8_Byteindex != 0 && ui8_data_rec[0] == ui8_slaveID) 
    {

      process();
      ui8_Byteindex = 0;
      ui2_BTcomplete = false;
      ui2_start = false;
    }
  }
}

void process()
{
   uint8_t uc8_data[ui8_Byteindex - 2], uc8_checkcrc[2];
	uint8_t uc8_crchigh, uc8_crclow;
	uint16_t ui16_crc = 0;
	uint16_t ui16_du_lieu_data_nhan;
	for (int i = 0; i < ui8_Byteindex - 2; i++) {
		uc8_data[i] = ui8_data_rec[i];
	}
	uc8_checkcrc[0] = ui8_data_rec[ui8_Byteindex - 2];
	uc8_checkcrc[1] = ui8_data_rec[ui8_Byteindex - 1];
	ui16_crc = calcCRC16(ui8_Byteindex - 2, uc8_data);
	uc8_crclow = ui16_crc >> 8;
	uc8_crchigh = ui16_crc & 0x00FF;
	if (uc8_crchigh != uc8_checkcrc[1] || uc8_crclow != uc8_checkcrc[0]){
		ui8_function = 0x00;
		ui8_status = 0x01;
		ui8_phanhoi_saiCRC = 1;
		return;
	}
    ui16_crc = 0;
    ui16_order_command = uc8_data[2];
    ui16_order_command = (ui16_order_command << 8) | uc8_data[3];
    ui16_du_lieu_data_nhan = uc8_data[5];
    ui16_du_lieu_data_nhan = (ui16_du_lieu_data_nhan << 8);
    ui16_du_lieu_data_nhan = ui16_du_lieu_data_nhan | uc8_data[6];
    if (ui16_order_command == ui16_last_order_command) 
    {
    	ui8_phanhoi_trunglenh=1;
    	return;
    }
	switch (uc8_data[1])
	{
    case 0x04:
    {
      ui16_last_order_command = ui16_order_command;
      if (ui16_du_lieu_data_nhan == 0x0000) 
      {
        START_USER();
        ui8_kethuc=0;
				ui8_hacuadi = 0;
        ui8_xoaloi=0;
        // control_BoNghien(0x01,ui8_slave[1]);
        ui8_control_BoNghien=0x01;
        ui8_status = 0x00;
        ui8_phanhoi_dungFunction=1;
      }

      else if(ui16_du_lieu_data_nhan == 0x00FF)
      {
        STOP_USER();
        ui8_kethuc=1;
        ui8_status = 0x00;
        ui8_phanhoi_dungFunction=1;
        
      }

      else 
      {
        ui8_status = 0x02;
				ui8_phanhoi_saiFunction = 1;
        // ui8_phanhoi_saiFunction=1;
      }
      break;
    }

    case 0x05:
    {
      Serial_Debug.println("NHAN LENH NHA QUA");
			ui16_last_order_command = ui16_order_command;
			ui8_status = 0x00;
			// ui8_sobyte = uc8_data[4];
			ui8_phanhoi_dungFunction = 1;
			int so_khay = 0;
			int so_o = 0;
			so_khay = uc8_data[5];
			so_o = uc8_data[6];
			ui8_vitrikhay_sp = uint8_t(so_khay);
			ui8_vitri_sp = uint8_t(so_o);
      ui8_control_hang=ui8_vitrikhay_sp;
      ui8_control_cot=ui8_vitri_sp;
      // control_mayphatqua(ui8_vitrikhay_sp, ui8_vitri_sp, ui8_slave[4]);
      break;
    }

    case 0x07:
    {
      ui16_last_order_command = ui16_order_command;
			if(ui8_startprocess==0) return;

      if (ui16_du_lieu_data_nhan == 0x0000 && process_2==1) 
      {
        ui8_trangthai_nhandien=TrangThaiNhanDien::NHANDIENCHAI;
        Serial_Debug.printf("NHAN DIEN CHAI\n");
        ui8_trangthai_nhavat=TrangThaiRoiVat::NHACHAI;
        ui8_control_BoPhanLoai=0x01;
        // control_BoPhanLoai(0x01,ui8_slave[1]);
        ui8_status = 0x00;
        ui8_phanhoi_dungFunction=1;
      }

      else if (ui16_du_lieu_data_nhan == 0x0001 && process_2==1) 
      {
        ui8_trangthai_nhandien=TrangThaiNhanDien::NHANDIENSAI;
        ui8_trangthai_nhavat=TrangThaiRoiVat::NHACHAI;
        ui8_status = 0x00;
        Serial_Debug.printf("NHAN DIEN SAI\n");
        ui8_phanhoi_dungFunction=1;
      }

      else if (ui16_du_lieu_data_nhan == 0x0002 && process_2==1) 
      {
        ui8_trangthai_nhandien=TrangThaiNhanDien::NHANDIENLON;
        ui8_trangthai_nhavat=TrangThaiRoiVat::NHALON;
        ui8_lalon=1;  
        ui8_status = 0x00;
        ui8_control_BoPhanLoai=0x02;
        // control_BoPhanLoai(0x02,ui8_slave[1]);
        Serial_Debug.printf("NHAN DIEN LON\n");
        ui8_phanhoi_dungFunction=1;
      }

      else if(ui16_du_lieu_data_nhan == 0x0003 && process_2==1)
			{
        ui8_trangthai_nhandien=TrangThaiNhanDien::NHANDIENKHAC;
        ui8_trangthai_nhavat=TrangThaiRoiVat::NHAKHAC;
        ui8_status = 0x00;
        ui8_control_BoPhanLoai=0x03;
        // control_BoPhanLoai(0x03,ui8_slave[1]);
        Serial_Debug.printf("NHAN DIEN KHAC\n");
        ui8_phanhoi_dungFunction=1;
      }      

      else 
      {
        ui8_status = 0x02;
				ui8_phanhoi_saiFunction = 1;
      }
      break;
    }

    case 0x09:
    {
      ui16_last_order_command = ui16_order_command;
      if(ui16_du_lieu_data_nhan == 0x0001)
      {
        xoaloi();
        ui8_status = 0x00;
				ui8_phanhoi_dungFunction = 1;
      }

      else 
      {
        ui8_status = 0x02;
				ui8_phanhoi_saiFunction = 1;
      }
      break;
    }

    case 0x10:
		{
           
		  ui16_last_order_command = 0x0000;
		  ui8_function = 0x10;
		  ui8_status = 0x00;
      ui8_phanhoi_dungFunction=1;
      break;
		}

    case 0x21:
    {
      ui16_last_order_command = ui16_order_command;
			if(ui16_du_lieu_data_nhan == 0x0001) // reset thung rac chai
			{
        ui8_erase_error_slave=0x01;
				ui16_sochai_trongthungrac = 0;
        ui8_trangthai_thungracchai=0;
				ui8_trangthaithungracchai = TrangThaiThungRac::KHONGDAY;
        EEPROM.put(16, ui8_trangthaithungracchai);
        EEPROM.commit();
				SETTRANGTHAIMAY();
        
				EEPROM.put(10, ui16_sochai_trongthungrac);
				EEPROM.commit();
				Serial_Debug.print("so chai trong thung rac sau khi reset: ");
				Serial_Debug.println(ui16_sochai_trongthungrac);
				ui8_phanhoi_trangthai_resetthungrac_chai = 1;
				// ui8_phanhoi_trangthai_thungracchai = 1;
        thungrac_chai.ui8_phanhoitrangthai_thungrac=1;
				ui8_phanhoi_phantram_thungracchai = 1;
				// ui8_reset_chai = 1;
				// ui32_timeout_nhanlenh_resetchai = millis();
			}

			else if(ui16_du_lieu_data_nhan == 0x0002) // reset thung rac lon
			{
        ui8_erase_error_slave=0x01;
				ui16_solon_trongthungrac = 0;
        ui8_trangthai_thungraclon=0;
				ui8_trangthaithungraclon = TrangThaiThungRac::KHONGDAY;
        EEPROM.put(17, ui8_trangthaithungraclon);
        EEPROM.commit();
				SETTRANGTHAIMAY();
				EEPROM.put(12, ui16_solon_trongthungrac);
				EEPROM.commit();
				ui8_phanhoi_trangthai_resetthungrac_lon = 1;
				// ui8_phanhoi_trangthai_thungraclon = 1;
        thungrac_lon.ui8_phanhoitrangthai_thungrac=1;
				ui8_phanhoi_phantram_thungraclon = 1;
				// ui8_reset_lon = 1;
				// ui32_timeout_nhanlenh_resetlon = millis();
			}

			else if(ui16_du_lieu_data_nhan == 0x0003) // reset thung rac lon
			{
        ui8_erase_error_slave=0x02;
				ui16_vat_trongthungrackhac = 0;
        ui8_trangthai_thungrackhac=0;
				ui8_trangthaithungrackhac = TrangThaiThungRac::KHONGDAY;
        EEPROM.put(18, ui8_trangthaithungrackhac);
        EEPROM.commit();
				SETTRANGTHAIMAY();
				EEPROM.put(14, ui16_vat_trongthungrackhac);
				EEPROM.commit();
				ui8_phanhoi_trangthai_resetthungrac_khac = 1;
				// ui8_phanhoi_trangthai_thungrackhac = 1;
        thungrac_khac.ui8_phanhoitrangthai_thungrac=1;
				ui8_phanhoi_phantram_thungrackhac = 1;
				// ui8_reset_lon = 1;
				// ui32_timeout_nhanlenh_resetlon = millis();
			}

			else
			{
				ui8_status = 0x02;
				ui8_phanhoi_saiFunction = 1;
			}

			break;

    }
    case 0xFB:
		{
			ui16_last_order_command = ui16_order_command;
      ui8_phanhoi_dungFunction=1;
			send_firmware();	
			break;
		}

    case 0x80:
    {
      ui16_last_order_command = ui16_order_command;
			if(ui16_du_lieu_data_nhan == 0x0101 || ui16_du_lieu_data_nhan == 0x0102)
			{
				run_Motor_Xoaychai();
				ui8_status = 0x00;
				ui8_phanhoi_dungFunction = 1;
			}

			else if(ui16_du_lieu_data_nhan == 0x0100)
			{
				stop_Motor_xoaychai();
				ui8_status = 0x00;
				ui8_phanhoi_dungFunction = 1;
			}

			else if(ui16_du_lieu_data_nhan == 0x0601)
			{
				ui8_kethuc=1;
				ui8_hacuadi = 1;
				// ui32_timeout_hacuaxuong = millis();
        Serial_Debug.printf("lenh dong cua bo qua\n");
				ui8_status = 0x00;
				ui8_phanhoi_dungFunction = 1;
			}

			else if(ui16_du_lieu_data_nhan == 0x0602)
			{
				ui8_kethuc=0;
				// ui8_nangcua = 1;
				// ui8_hacuadi = 0;
        Serial_Debug.printf("lenh mo cua bo qua\n");
				if(motor_cua.ui8_trangthaimotor != TrangThaiMotorDC::NANGLEN && ui8_trangthaicua == TrangThaiMotorDC::KHONGKET)
				{
          Serial_Debug.printf("nhay vao cua nang len\n");
          stop_Motor_Cua();
          run_Motor_Cua_Nanglen();
				}
				ui8_chophephacua=0;
				ui8_status = 0x00;
				ui8_phanhoi_dungFunction = 1;
			}			

			else if(ui16_du_lieu_data_nhan == 0x0600)
			{
				stop_Motor_Cua();
				ui8_status = 0x00;
				ui8_phanhoi_dungFunction = 1;
			}

      else if(ui16_du_lieu_data_nhan==0x0901)
      {
          // mocua_trenduoi(0x01,ui8_slave[3]);
          ui8_mocua=0x01;
          ui8_trangthaicua_cuamay=0x02;
          ui8_saukhinhanlenh_mo=1;
          ui8_saukhinhanlenh_dong=0;
          ui32_timeout_nhalenhmocua=millis()+60000;
          ui8_status = 0x00;
				  ui8_phanhoi_dungFunction = 1;
      }

      else if(ui16_du_lieu_data_nhan==0x0900)
      {
          // dongcua_trenduoi(0x01,ui8_slave[3]);
          ui8_mocua=0x01;
          ui8_trangthaicua_cuamay=0x01;
          ui8_saukhinhanlenh_dong=1;
          ui8_saukhinhanlenh_mo=0;
          ui32_timeout_nhalenhdongcua=millis()+60000;
          ui8_status = 0x00;
				  ui8_phanhoi_dungFunction = 1;
      }

      else if(ui16_du_lieu_data_nhan==0x1001)
      {
          ui8_mocua=0x02;
          ui8_trangthaicua_cuamay=0x02;
          ui8_saukhinhanlenh_moduoi=1;
          ui8_saukhinhanlenh_dongduoi=0;
          ui32_timeout_nhalenhmocuaduoi=millis()+60000;
          ui8_status = 0x00;
				  ui8_phanhoi_dungFunction = 1; 
      }

      else if(ui16_du_lieu_data_nhan==0x1000)
      {
          ui8_mocua=0x02;
          ui8_trangthaicua_cuamay=0x01;
          ui8_status = 0x00;
          ui8_saukhinhanlenh_moduoi=0;
          ui8_saukhinhanlenh_dongduoi=1;
          ui32_timeout_nhalenhdongcuaduoi=millis()+60000;
				  ui8_phanhoi_dungFunction = 1;
      }

      else 
      {
        ui8_status = 0x02;
				ui8_phanhoi_saiFunction = 1;
      }
      break;
    }

    default:
    {
      Serial.write(0x01);
      Serial.write(0x02);
      Serial.write(0x03);
      Serial.write(0x04);
    }
  }
}

void quatrinh_1()
{
  if((motor_bangtai.ui8_tatmotor==0) && (motor_bangtai.ui8_trangthaimotor != TrangThaiMotorDC::VAO) || (motor_bangtai.ui8_trangthaimotor == TrangThaiMotorDC::VAO))
  {
    Serial_Debug.printf("chay motor vao\n");
    stop_Motor_bangtai();
    run_Motor_bangtai(chieuquay_bangtai_vao);
    process_1=1;
  }
}

void quatrinh_2()
{
  int phathien_chai = doccambien(cambien_vat1,20);
  int tam = doccambien(cambien_vat2,20);

  if(tam==1)
  {
    Serial_Debug.printf("co vat\n");
    ui8_trangthaivat=TrangThaiVat::COVAT;
    stop_Motor_bangtai();
    run_Motor_nangha();
		delay(500);
		stop_Motor_nangha();
    run_Motor_Xoaychai();
    cb_vat.ui8_phanhoi_trangthai_cambien=TrangThaiLenhPhanHoi::DUNG;
    ui32_timeoutnhandien = millis() + 30000;
		process_2 = 1;
  }
}

void quatrinh_3()
{
  static int cholayvat = 0;
	static int check_ban=0;
  static int nhandiensai=0;
  static int nhandienlon=0;
  static int nhandienkhac=0;

  if(((ui8_trangthai_nhandien==TrangThaiNhanDien::NHANDIENCHAI) || (ui8_trangthai_nhandien==TrangThaiNhanDien::NHANDIENLON) || (ui8_trangthai_nhandien==TrangThaiNhanDien::NHANDIENKHAC)) && check_ban==0)
  {
    Serial_Debug.printf("xu ly nhan dien\n");
    if(ui8_trangthai_nhandien == TrangThaiNhanDien::NHANDIENCHAI)
		{
			ui16_sochai_trongthungrac++;
			EEPROM.put(10, ui16_sochai_trongthungrac);
			EEPROM.commit();
		}

		else if(ui8_trangthai_nhandien == TrangThaiNhanDien::NHANDIENLON)
		{
      nhandienlon=1;
        
			ui16_solon_trongthungrac++;
			EEPROM.put(12, ui16_solon_trongthungrac);
			EEPROM.commit();	
		}

		else if(ui8_trangthai_nhandien==TrangThaiNhanDien::NHANDIENKHAC)
		{
      nhandienkhac=1;
			ui16_vat_trongthungrackhac++;
			EEPROM.put(14, ui16_vat_trongthungrackhac);
			EEPROM.commit();	
		}
    stop_Motor_xoaychai();
		ui8_trangthai_nhandien=TrangThaiNhanDien::HOANTHANH_NHANDIEN;
		ui8_runmotor_nangchai=1;
		Serial_Debug.printf("CHUAN BI CHAY MOTOR NANG HA\n");
  }

  else if((ui8_trangthai_nhandien == TrangThaiNhanDien::NHANDIENSAI) || ((ui32_timeoutnhandien <= millis()) && ui8_chaymotorbangtai==0 && cholayvat == 0))
	{
		Serial_Debug.printf("Nhan duoc lenh nhan dien sai\n");
		stop_Motor_xoaychai();
		
		// run_Motor_Nangha();
		// delay(1000);
		// stop_Motor_nangha();
		nangbangtai();
		ui8_trangthai_nhandien=TrangThaiNhanDien::HOANTHANH_NHANDIEN;
		
		// ui8_chophep_nghien = 1;
		// ui32_timecho_resetCBvat = millis() + timeout_reset_cbvat;
		ui32_timemotor_bangtai = millis() + 15000;
		run_Motor_bangtai(chieuquay_bangtai_ra);
		cholayvat=1;
    // nhandiensai=1;
	}

  if(ui8_runmotor_nangchai==1)
  {
    int tam=chaynangbangtai();
    if(tam==1)
    { 
      if((motor_phanloai.ui8_kiemtra_trangthailoi==TrangthaiLoi::BAN || motor_ep.ui8_kiemtra_trangthailoi==TrangthaiLoi::BAN) && (nhandienlon==1 || nhandienkhac==1))
      {
        
        Serial_Debug.printf("he thong dang ban\n");
        ui8_runmotor_nangchai=0;
        stop_Motor_bangtai();
        nhandienkhac=0;
        nhandienlon=0;
        check_ban=1;
      }

    else 
    {
      if(ui8_lalon==1)
      {
          ui8_control_BoEp=0x02;
          Serial_Debug.printf("nha vat xun bo ep NE\n");
          if(ui8_phathien_lon==1)
          {
            ui8_phathien_lon=0;
          }
          ui8_lalon=0;
      }
      Serial_Debug.printf("nha vat xun\n");
      run_Motor_bangtai(chieuquay_bangtai_vao);	
      ui8_chaymotorbangtai = 1;
      ui32_timecho_resetCBvat = millis() + 1000;
      ui8_runmotor_nangchai=0;
    }

  }

    else if(tam==0)
    {
      Serial_Debug.printf("loi nang ha/n");
			process_1 = 0;
			process_2 = 0;
			process_3 = 0;
			cholayvat = 0;
      nhandiensai=0;
      motor_bangtai.ui8_tatmotor=0;
			stop_Motor_nangha();
			ui8_trangthaivat = TrangThaiVat::KHONGCOVAT;
			ui8_chaymotorbangtai=0;
			check_ban=0;
			ui8_runmotor_nangchai=0;
      motor_nangha.ui8_kiemtra_trangthailoi_motorDC=TrangthaiLoi::LOI;
			// ui8_loidongco_nangha=LOI;
			EEPROM.put(0, motor_nangha.ui8_kiemtra_trangthailoi_motorDC);
			EEPROM.commit();
			SETTRANGTHAIMAY();
			motor_nangha.ui8_phanhoitrangthai = TrangThaiLenhPhanHoi::DUNG;
			ui8_is_stopuser = 1;
      return;
    }
  }

  if(ui8_chaymotorbangtai == 1)
  {
    int tam = doccambien(cambien_vat2,20);
    if(tam==0)
    {
      if(ui32_timecho_resetCBvat <= millis())
      {
        process_1 = 0;
        process_2 = 0;
        process_3 = 0;
        cholayvat = 0;
        motor_bangtai.ui8_tatmotor=0;
        ui8_trangthaivat = TrangThaiVat::KHONGCOVAT;
        // ui8_chophep_nghien = 1;
        // ui8_tatmotorbangtai = 0;
        ui8_chaymotorbangtai=0;
        Serial_Debug.printf("VAT DA ROI SAU KHI NHANDIEN\n");	
        check_ban=0;
      }
    }

    else if(tam==1)
		{
			ui32_timecho_resetCBvat = millis() + 1000;
		}

  }

  if(cholayvat==1)
  {
    int tam=doccambien(cambien_vat1,200);
    int tam2 = doccambien(cambien_vat2,20);
    if(tam==1)
    {
      ui32_timemotor_bangtai=millis()+800;
      stop_Motor_bangtai();
      ui8_vatrangoai=1;
    }

    if(ui8_vatrangoai==1)
    {
      {
         if(ui32_timemotor_bangtai <= millis())
         {
          int tam=doccambien(cambien_vat1,20);
          if(tam==0)
          {
            process_1 = 0;
            process_2 = 0;
            process_3 = 0;
            cholayvat = 0;
            motor_bangtai.ui8_tatmotor=0;
            ui8_trangthaivat = TrangThaiVat::KHONGCOVAT;
            // ui8_chophep_nghien = 1;
            // ui8_tatmotorbangtai = 0;
            ui8_chaymotorbangtai=0;
            Serial_Debug.printf("LAY VAT RA\n");  
            check_ban=0;
            ui8_vatrangoai=0;
          }
            
         }
        
      }

      // else if(ui32_timemotor_bangtai <= millis())
      // {
      //   process_1 = 0;
      //   process_2 = 0;
      //   process_3 = 0;
      //   cholayvat = 0;
      //   motor_bangtai.ui8_tatmotor=0;
      //   ui8_trangthaivat = TrangThaiVat::KHONGCOVAT;
      //   // ui8_chophep_nghien = 1;
      //   // ui8_tatmotorbangtai = 0;
      //   ui8_chaymotorbangtai=0;
      //   Serial_Debug.printf("LAY VAT RA KHONG THAAY VAT CHAM CAM BIEN OUT\n");	
      //   check_ban=0;
      //   ui8_vatrangoai=0;
      // }
    }

    if(tam2==0)
    {
      if(ui32_timemotor_bangtai <= millis())
      {
        // ui32_timemotor_bangtai = millis() + 10000;
      
          process_1 = 0;
          process_2 = 0;
          process_3 = 0;
          cholayvat = 0;
          motor_bangtai.ui8_tatmotor=0;
          ui8_trangthaivat = TrangThaiVat::KHONGCOVAT;
          // ui8_chophep_nghien = 1;
          // ui8_tatmotorbangtai = 0;
          ui8_chaymotorbangtai=0;
          Serial_Debug.printf("LAY VAT RA VI LAU QUA\n");	
          check_ban=0;
          ui8_vatrangoai=0;
        
      }  
    }

    else if(tam2==1)
    {
        ui32_timemotor_bangtai=millis()+5000;
    }
    // else if(ui32_timemotor_bangtai <= millis())
    // {
    //   // ui32_timemotor_bangtai = millis() + 10000;
    
    //     process_1 = 0;
    //     process_2 = 0;
    //     process_3 = 0;
    //     cholayvat = 0;
    //     motor_bangtai.ui8_tatmotor=0;
    //     ui8_trangthaivat = TrangThaiVat::KHONGCOVAT;
    //     // ui8_chophep_nghien = 1;
    //     // ui8_tatmotorbangtai = 0;
    //     ui8_chaymotorbangtai=0;
    //     Serial_Debug.printf("LAY VAT RA\n");	
    //     check_ban=0;
    //     ui8_vatrangoai=0;
      
    // }

  }

  if(check_ban==1)
  {
    ui32_timeoutnhandien=millis()+30000;
    if(motor_phanloai.ui8_kiemtra_trangthailoi==TrangthaiLoi::KHONGLOI && motor_ep.ui8_kiemtra_trangthailoi==TrangthaiLoi::KHONGLOI)
    {
      if(ui8_lalon==1)
			{
        ui8_control_BoEp=0x02;
				// control_BoEp(0x02,ui8_slave[2]);
				// ui32_timeout_checkrs485=millis()+200;
				Serial_Debug.printf("nha vat xun bo ep\n");
				if(ui8_phathien_lon==1)
				{
					ui8_phathien_lon=0;
				}
				ui8_lalon=0;
			}
      run_Motor_bangtai(chieuquay_bangtai_vao);	
      ui8_chaymotorbangtai = 1;
      ui32_timecho_resetCBvat = millis() + 1000;
      ui8_runmotor_nangchai=0;
      check_ban=0;
    }
  }
}

void caidatgiatribandau()
{
  static uint8_t hoanthanh_kiemtra_nanghavat = 0;
  static uint8_t check_nangha=0;
  static uint8_t check_tatnghien=0;
  ui8_dangcaidat=1;
	Serial_Debug.printf("Vao che do cai dat ban dau\n");
	stop_Motor_xoaychai();
	stop_Motor_bangtai();
  if(motor_nangha.ui8_kiemtra_trangthailoi_motorDC!=TrangthaiLoi::LOI && check_nangha==0)
  {
    if(ui8_trangthai_nhavat==TrangThaiRoiVat::KHONGNHA)
    {
      ui8_trangthai_nhavat=TrangThaiRoiVat::NHACHAI;
      check_nangha=1;
    }
  }

  if(hoanthanh_kiemtra_nanghavat==0 && ui8_trangthai_nhavat != TrangThaiRoiVat::KHONGNHA && motor_nangha.ui8_kiemtra_trangthailoi_motorDC!=TrangthaiLoi::LOI)
	{
		Serial_Debug.printf("kiem tra motor nang ha\n");
		KIEMTRAMOTOR(motor_nangha.ui8_kiemtra_trangthailoi_motorDC, chaynangbangtai,motor_nangha.ui8_phanhoitrangthai, stop_Motor_nangha,hoanthanh_kiemtra_nanghavat, cambien_nangha);
		// if(ui8_loidongco_nangha == LOI){
		// 		EEPROM.put(0, ui8_loidongco_nangha);
		// 		EEPROM.commit();
		// 	}	
	}

  else if(ui8_trangthai_nhavat==TrangThaiRoiVat::KHONGNHA || motor_nangha.ui8_kiemtra_trangthailoi_motorDC==TrangthaiLoi::LOI)
  {
    hoanthanh_kiemtra_nanghavat=1;
  }

  if(ui8_nhanxoaloi==1)
	{
		Serial_Debug.printf("xoa loi roi ne\n");
		ui8_khongcheck_receive=0;
		ui8_nhanxoaloi=0;
	}

  int tam1 = doccambien(cambien_cua1, 20);
  int tam2 = doccambien(cambien_cua2, 20);
  int tam3 = doccambien(cambien_cua3, 20);
  int tam4 = doccambien(cambien_cua4, 20);
  register int tong = tam1 + tam2 + tam3 + tam4;
  if(motor_cua.ui8_trangthaimotor != TrangThaiMotorDC::HAXUONG && tong==0)
  {
    stop_Motor_Cua();
    run_Motor_Cua_Haxuong(); 
  }

  ui8_startprocess=0;

  // if(ui8_xoaloi==0 && check_tatnghien==0)
	// {
	// 	Serial_Debug.printf("yeu cau tat bo nghien\n");
  //   ui8_control_BoNghien=0x00;
  //   check_tatnghien=1;
  //   // ui8_xoaloi=1;
  //   // control_BoNghien(0x00,ui8_slave[1]);
	// 	// control_BoNghien(0x00);
	// 	// ui32_timecho_guilenh_485=millis()+500;
	// }

  if(hoanthanh_kiemtra_nanghavat==1)
	{
		BATTAT_LED_NHANDIEN(255);
		// TATLED();
		ui8_trangthaivat = TrangThaiVat::KHONGCOVAT;
		ui8_trangthai_nhavat=TrangThaiRoiVat::KHONGNHA;
		motor_nangha.ui8_trangthaimotor=TrangThaiMotorDC::DUNG;
		process_1 = 0;
		process_2 = 0;
		process_3 = 0;
		Serial_Debug.printf("DA XONG CAI DAT GIA TRI BAN DAU\n");
    check_nangha=0;
    ui8_dangcaidat=0;
    check_tatnghien=0;
    ui8_vatrangoai=0;
		hoanthanh_kiemtra_nanghavat=0;
    ui8_is_stopuser=0;
	}
}

void kt_bonghien()
{
	if(ui8_ketthucnghien==1)
	{
		if(ui32_timeout_quaynguocnghien<=millis())
		{
			Serial_Debug.println("bat dau chay nghich nghien");
      ui8_control_BoNghien=0x02;
			ui32_timeout_dungnghien=millis()+2000;
			ui8_ketthucnghien=0;
			ui8_dungbonghien=1;
		}
	}

	if(ui8_dungbonghien==1)
	{
		if(ui32_timeout_dungnghien<=millis())
		{
			Serial_Debug.println("yeu cau tat bo nghien");
      ui8_control_BoNghien=0x00;
			ui8_dungbonghien=0;
		}
	}
}

void guilenhlenPC()
{
  if(ui32_timecho_guilenh +  200 <= millis()){
    PHANHOI(ui8_phanhoi_dungFunction, 0x00, ui8_data_rec[1], respondtoPC(ui8_function, ui8_status));
    PHANHOI(ui8_phanhoi_saiCRC, 0x01, ui8_data_rec[1], send_data_khinhansailenh(ui8_function,ui8_status));
    PHANHOI(ui8_phanhoi_saiFunction, 0x02, ui8_data_rec[1], send_data_khinhansailenh(ui8_function,ui8_status));
    PHANHOI(ui8_phanhoi_trunglenh, 0x03, ui8_data_rec[1], send_data_khinhansailenh(ui8_function,ui8_status));

    PHANHOI(ui8_phanhoi_trangthainhaqua,0x00,0x06,send_state_to_PC(ui8_function));
    PHANHOI(cb_vat.ui8_phanhoi_trangthai_cambien,0x00,0x11,send_state_to_PC(ui8_function));
    PHANHOI(motor_ep.ui8_phanhoi_trangthai,0x00,0x14,send_state_to_PC(ui8_function));
    PHANHOI(motor_nangha.ui8_phanhoitrangthai,0x00,0x15,send_state_to_PC(ui8_function));
    PHANHOI(motor_nghien.ui8_phanhoi_trangthai,0x00,0x16,send_state_to_PC(ui8_function));
    PHANHOI(motor_phanloai.ui8_phanhoi_trangthai,0x00,0x17,send_state_to_PC(ui8_function));

    PHANHOI(thungrac_chai.ui8_phanhoitrangthai_thungrac, 0x00, 0x12, send_state_to_PC(ui8_function));
    PHANHOI(thungrac_lon.ui8_phanhoitrangthai_thungrac, 0x00, 0x12, send_state_to_PC(ui8_function));
    PHANHOI(thungrac_khac.ui8_phanhoitrangthai_thungrac, 0x00, 0x12, send_state_to_PC(ui8_function));

    PHANHOI(cb_kimloai.ui8_phanhoi_trangthai_cambien, 0x00, 0x08, send_state_to_PC(ui8_function));

    PHANHOI(ui8_phanhoi_phantram_thungracchai, 0x00, 0x20, send_cnt_to_PC((uint8_t)THUNGRACCHAI));
    PHANHOI(ui8_phanhoi_phantram_thungraclon, 0x00, 0x20, send_cnt_to_PC((uint8_t)THUNGRACLON));
    PHANHOI(ui8_phanhoi_phantram_thungrackhac, 0x00, 0x20, send_cnt_to_PC((uint8_t)THUNGRACKHAC));
    PHANHOI(ui8_phanhoi_trangthai_resetthungrac_chai,0x00,0x21,send_state_to_PC(ui8_function));
    PHANHOI(ui8_phanhoi_trangthai_resetthungrac_lon,0x00,0x21,send_state_to_PC(ui8_function));	
    PHANHOI(ui8_phanhoi_trangthai_resetthungrac_khac,0x00,0x21,send_state_to_PC(ui8_function));

    PHANHOI(ui8_phanhoi_mocuamay, 0x00, 0xFA, send_state_to_PC(ui8_function));
    PHANHOI(ui8_phanhoi_mocuathungrac,0x00,0xFA,send_state_to_PC(ui8_function));
  }
}


void check_loi()
{
	if(ui8_khongcheck_receive==1) return;
  // if()
	// if(motor_phanloai.ui8_kiemtra_trangthailoi==TrangthaiLoi::LOI || motor_ep.ui8_kiemtra_trangthailoi==TrangthaiLoi::LOI || motor_nghien.ui8_kiemtra_trangthailoi==TrangthaiLoi::LOI||ui8_checkloi_nghien==)
	// {
		if(motor_phanloai.ui8_kiemtra_trangthailoi==TrangthaiLoi::LOI)
		{
			EEPROM.put(3, motor_phanloai.ui8_kiemtra_trangthailoi);
			EEPROM.commit();
			motor_phanloai.ui8_phanhoi_trangthai=TrangThaiLenhPhanHoi::DUNG;
      SETTRANGTHAIMAY();
      ui8_is_stopuser=1;
      ui8_khongcheck_receive=1;
		}

		else if(motor_nghien.ui8_kiemtra_trangthailoi==TrangthaiLoi::LOI)
		{
      Serial_Debug.print("\nloi motor nghien luu eeprom\n");
      // motor_nghien.ui8_kiemtra_trangthailoi=TrangthaiLoi::LOI;
			EEPROM.put(1, motor_nghien.ui8_kiemtra_trangthailoi);
			EEPROM.commit();
			motor_nghien.ui8_phanhoi_trangthai=TrangThaiLenhPhanHoi::DUNG;
      SETTRANGTHAIMAY();
      ui8_is_stopuser=1;
      ui8_khongcheck_receive=1;
		}

		else if(motor_ep.ui8_kiemtra_trangthailoi==TrangthaiLoi::LOI)
		{
			EEPROM.put(2, motor_ep.ui8_kiemtra_trangthailoi);
			EEPROM.commit();
			motor_ep.ui8_phanhoi_trangthai=TrangThaiLenhPhanHoi::DUNG;
      SETTRANGTHAIMAY();
      ui8_is_stopuser=1;
      ui8_khongcheck_receive=1;
		}
		
		// ui8_khongcheck_receive=1;
		// SETTRANGTHAIMAY();
	// }
}

void xoaloi()
{
  ui8_lalon=0;
  ui8_nhanxoaloi=1;
  ui8_xoaloi=1;
  ui8_xoaloiroi=1;
  ui8_checkloi_nghien=0;
  // ui8_trangthai_motor_nangha = 1;
  Serial_Debug.printf("xoa loi\n");
  ui8_erase_error_slave=0x05;
  // erase_error_slave_RS485();
  // ui32_timeout_checkrs485=millis()+200;
  ui8_khongcheck_receive=0;
  ui8_trangthai_nhavat = TrangThaiRoiVat::NHACHAI;
  // ui8_kiemtramotoreplon=KHONGLOI;
  // Serial_Debug.println(ui8_khongcheck_receive);
  // dataReceived[5]=0x00;
  // dataReceived[6]=0x00;
  // dataReceived[7]=0x00;
  buffer_receive_slave[5]=0x00;
  buffer_receive_slave[6]=0x00;
  buffer_receive_slave[7]=0x00;

  motor_ep.ui8_kiemtra_trangthailoi=TrangthaiLoi::KHONGLOI;
  motor_ep.ui8_phanhoi_trangthai=TrangThaiLenhPhanHoi::DUNG;
  EEPROM.put(2, motor_ep.ui8_kiemtra_trangthailoi);
  EEPROM.commit();

  motor_nghien.ui8_kiemtra_trangthailoi=TrangthaiLoi::KHONGLOI;
  motor_nghien.ui8_phanhoi_trangthai=TrangThaiLenhPhanHoi::DUNG;
  EEPROM.put(1, motor_nghien.ui8_kiemtra_trangthailoi);
  EEPROM.commit();

  motor_nangha.ui8_kiemtra_trangthailoi_motorDC=TrangthaiLoi::KHONGLOI;
  motor_nangha.ui8_phanhoitrangthai=TrangThaiLenhPhanHoi::DUNG;
  EEPROM.put(0, motor_nangha.ui8_kiemtra_trangthailoi_motorDC);
  EEPROM.commit();

  motor_phanloai.ui8_kiemtra_trangthailoi=TrangthaiLoi::KHONGLOI;
  motor_phanloai.ui8_phanhoi_trangthai=TrangThaiLenhPhanHoi::DUNG;
  EEPROM.put(3, motor_phanloai.ui8_kiemtra_trangthailoi);
  EEPROM.commit();

  SETTRANGTHAIMAY();
  // ui8_is_stopuser = 1;
  ui8_chophephacua=0;
  ui8_kethuc=0;
}

void xoathungrac()
{
  ui8_erase_error_slave=0x02;
  ui16_sochai_trongthungrac = 0;
  ui8_trangthai_thungracchai=0;
  ui8_trangthaithungracchai = TrangThaiThungRac::KHONGDAY;
  // SETTRANGTHAIMAY();
  EEPROM.put(16, ui8_trangthai_thungracchai);
  EEPROM.commit();
  EEPROM.put(10, ui16_sochai_trongthungrac);
  EEPROM.commit();
  Serial_Debug.print("so chai trong thung rac sau khi reset: ");
  Serial_Debug.println(ui16_sochai_trongthungrac);
  ui8_phanhoi_trangthai_resetthungrac_chai = 1;
  // ui8_phanhoi_trangthai_thungracchai = 1;
  thungrac_chai.ui8_phanhoitrangthai_thungrac=1;
  ui8_phanhoi_phantram_thungracchai = 1;

  ui16_solon_trongthungrac = 0;
  ui8_trangthai_thungraclon=0;
  ui8_trangthaithungraclon = TrangThaiThungRac::KHONGDAY;
  EEPROM.put(17, ui8_trangthaithungraclon);
  EEPROM.commit();
  // SETTRANGTHAIMAY();
  EEPROM.put(12, ui16_solon_trongthungrac);
  EEPROM.commit();
  ui8_phanhoi_trangthai_resetthungrac_lon = 1;
  // ui8_phanhoi_trangthai_thungraclon = 1;
  thungrac_lon.ui8_phanhoitrangthai_thungrac=1;
  ui8_phanhoi_phantram_thungraclon = 1;

  ui16_vat_trongthungrackhac = 0;
  ui8_trangthai_thungrackhac=0;
  ui8_trangthaithungrackhac = TrangThaiThungRac::KHONGDAY;
  EEPROM.put(18, ui8_trangthaithungrackhac);
  EEPROM.commit(); 
  EEPROM.put(14, ui16_vat_trongthungrackhac);
  EEPROM.commit();
  ui8_phanhoi_trangthai_resetthungrac_khac = 1;
  // ui8_phanhoi_trangthai_thungrackhac = 1;
  thungrac_khac.ui8_phanhoitrangthai_thungrac=1;
  ui8_phanhoi_phantram_thungrackhac = 1;
  SETTRANGTHAIMAY();
}

void DOCNUTNHAN(){
	static int doclandau = 0;
	static uint32_t timedocnutnhan = 0;
	int tam = digitalRead(nutbam);
	if(tam == HIGH){
		if(doclandau == 0){
			timedocnutnhan = millis();
			doclandau = 1;
		}
		
	// 	if(timedocnutnhan + 3000 <= millis() && ui8_webcontrol == 0){
				
	// 			ui8_webcontrol = 1;
	// 			ui8_solannhayled_OTA = 3;
	// 			ConnectWifi();
	// 			setupServer();
	// 			Serial_Debug.println("WEB CONTROL ON");
	// 	}
		
	}
	if(tam == LOW)
	{
		if(doclandau == 1)
		{	
			if(timedocnutnhan + 3000 > millis())
			{
				xoathungrac();
				xoaloi();
			}
		}
		doclandau = 0;
	}
}


void send_oder_to_slave()
{
    if(ui8_dangcaidat==1) return;
    // if(ui8_khongcheck_receive==1) return;
    
    if (ui8_receive_respond_from_slave == 0 && // Không đang trong trạng thái nhận phản hồi từ slave nào
        (millis() - RS485.timeSend > 50))      // Giãn cách truyền tránh bị gộp lệnh
    {
        // USBSerial.println("test");
        if(ui8_reset_all !=0x00)
        {
          Serial_Debug.println("reset het ");
          reset_slave(ui8_reset_all);
          ui8_receive_respond_from_slave=ui8_reset_all;
          // ui8_rs_master=1;
          ui8_reset_all--;
          return;
        }

        if ((ui8_check_slave != 0x00) && (ui8_check_slave != 0xFF)  && (ui8_danggui_fw==0)) // Check trạng thái slave
        {
            // Serial_Debug.println("bat dau check trang thai");
            check_error_slave_RS485(ui8_check_slave);
            ui8_receive_respond_from_slave = ui8_check_slave;
            ui32_timebegin_nhanphanhoi = millis();
            return;
        }

        if (ui8_erase_error_slave != 0x00) // Xóa toàn bộ lỗi
        {
            erase_error_slave_RS485(ui8_erase_error_slave);
            ui8_erase_error_slave--;
            return;
        }

        if (ui8_control_BoPhanLoai != 0x00)
        {
            control_BoPhanLoai(ui8_control_BoPhanLoai,ui8_slave[1]);
            ui8_control_BoPhanLoai = 0x00;
            return;
        }

        if (ui8_control_BoEp != 0x00)
        {
            Serial_Debug.printf("dk bo ep lon\n"); 
            control_BoEp(ui8_control_BoEp,ui8_slave[2]);
            ui8_control_BoEp = 0x00;
            return;
        }

        if (ui8_control_BoNghien != 0xFF)
        {
            control_BoNghien(ui8_control_BoNghien,ui8_slave[1]);
            ui8_control_BoNghien = 0xFF;
            return;
        }

        if(ui8_control_hang!=0x00 && ui8_control_cot!=0x00)
        {
          Serial_Debug.printf("nha qua ne\n");
          control_mayphatqua(ui8_control_hang,ui8_control_cot,ui8_slave[4]);
          ui8_control_hang=0x00;
          ui8_control_cot=0x00;
          return;
        }

        if(ui8_mocua!=0x00 && ui8_trangthaicua_cuamay!=0x00)
        {
          dongmocua_trenduoi(ui8_mocua,ui8_trangthaicua_cuamay,ui8_slave[3]);
          ui8_mocua=0x00;
          ui8_trangthaicua_cuamay=0x00;
          return;
        }

        if(ui8_sendfw_slave != 0x00)
        {
          check_firmware_slave_RS485(ui8_sendfw_slave);
          ui8_receive_respond_from_slave=ui8_sendfw_slave;
          // ui8_sendfw_slave--;
          // ui8_khongcheck_receive=1;
          ui8_sendfw_slave=0x00;
          ui32_timebegin_nhanphanhoi = millis();
          // ui8_receive_respond_from_slave=0x01;
          return;
        }
    }
}
void receive_respond_from_slave()
{
    // if(ui8_khongcheck_receive==1) return;
    if (ui8_receive_respond_from_slave == ui8_slave[1])
    {
      // Serial_Debug.println("nhay vao slave 1");
        RS485.receive(buffer_receive_slave, ui8_slave[1]);
        if (millis() - ui32_timebegin_nhanphanhoi > 50 || // Sau 50 ms không nhận được respond thì bỏ qua
            RS485.isReceived)                             // Hoặc đã nhận được dữ liệu
        {
            ui8_check_slave = ui8_slave[2];
            ui8_receive_respond_from_slave = 0x00;
        }
        processRS485(ui8_slave[1]);
    }

    if (ui8_receive_respond_from_slave == ui8_slave[2])
    {
        // Serial_Debug.println("nhay vao slave 2");
        RS485.receive(buffer_receive_slave, ui8_slave[2]);
        if (millis() - ui32_timebegin_nhanphanhoi > 50 || // Sau 50 ms không nhận được respond thì bỏ qua
            RS485.isReceived)                             // Hoặc đã nhận được dữ liệu
        {
            ui8_check_slave = ui8_slave[3];
            ui8_receive_respond_from_slave = 0x00;
        }
        processRS485(ui8_slave[2]);
    }

    if (ui8_receive_respond_from_slave == ui8_slave[3])
    {
        RS485.receive(buffer_receive_slave, ui8_slave[3]);
        if (millis() - ui32_timebegin_nhanphanhoi > 50 || // Sau 100 ms không nhận được respond thì bỏ qua
            RS485.isReceived)                             // Hoặc đã nhận được dữ liệu
        {
            ui8_check_slave = ui8_slave[4];
            ui8_receive_respond_from_slave = 0x00;
        }
        processRS485(ui8_slave[3]);
    }

    if (ui8_receive_respond_from_slave == ui8_slave[4])
    {
        RS485.receive(buffer_receive_slave, ui8_slave[4]);
        if (millis() - ui32_timebegin_nhanphanhoi > 50 || // Sau 100 ms không nhận được respond thì bỏ qua
            RS485.isReceived)                             // Hoặc đã nhận được dữ liệu
        {
            ui8_check_slave = ui8_slave[5];
            ui8_receive_respond_from_slave = 0x00;
        }
        processRS485(ui8_slave[4]);
    }

    if (ui8_receive_respond_from_slave == ui8_slave[5])
    {
        RS485.receive(buffer_receive_slave, ui8_slave[5]);
        if (millis() - ui32_timebegin_nhanphanhoi > 50 || // Sau 100 ms không nhận được respond thì bỏ qua
            RS485.isReceived)                             // Hoặc đã nhận được dữ liệu
        {
            ui8_check_slave = 0xff;
            ui8_receive_respond_from_slave = 0x00;
        }
        processRS485(ui8_slave[5]);
    }

    if ((ui8_check_slave == 0xff || ui8_check_slave == 0x00) &&
        (millis() - ui32_time_checkslave > 1000))
    {
        ui8_check_slave = ui8_slave[1];
        ui32_time_checkslave = millis();
        if(ui8_xoaloiroi==1)
        {
          ui8_is_stopuser=1;
          ui8_xoaloiroi=0;
        }
    }
}


void processRS485(uint8_t slaveID)
{
  // Serial_Debug.println("slave ID");
  // Serial_Debug.print(slaveID);
  // Serial_Debug.println(RS485.isReceived);
  if (RS485.incorrectCRC == true)
  {
    Serial_Debug.println("Incorrect CRC");
    RS485.incorrectCRC = false;
    return;
  }
  if (RS485.isReceived == true)
  {
    Serial_Debug.print("\nReceive: ");
		// ui8_chophanhoi=0;
		for (uint8_t i = 0; i <= buffer_receive_slave[0]; i++)
		{
			Serial_Debug.print(buffer_receive_slave[i], HEX);
			Serial_Debug.print(" ");
		}
    RS485.isReceived = false;
    memcpy(order_command, (buffer_receive_slave + 2), 2);
    if (order_command[0] == last_order_command[0] && order_command[1] == last_order_command[1])
    {
      // return;
    }
    memcpy(last_order_command, order_command, 2);

    uint8_t size = buffer_receive_slave[0] - 4;
    uint8_t function = buffer_receive_slave[1];
    uint8_t bytecount = buffer_receive_slave[4];

    uint8_t data[buffer_receive_slave[0] - 4];
    memcpy(data, buffer_receive_slave + 5, buffer_receive_slave[0] - 4);

    if (slaveID == ui8_slave[1])
    {
      Serial_Debug.printf("slave 1 dang gui\n");
      
      switch (function)
      {
        case codeSlaveRS485::check_error:
        {
            // Serial_Debug.printf("data thu 6: %d\n",buffer_receive_slave[6]);
            if(buffer_receive_slave[5]==TrangThaiMotor_nghien_ep_phanloai::KHONGLOI)
            {
              if(motor_phanloai.ui8_kiemtra_trangthailoi==TrangthaiLoi::LOI) return;
                motor_phanloai.ui8_kiemtra_trangthailoi=TrangthaiLoi::KHONGLOI;
                Serial_Debug.printf("dong phan loai ok\n");
                break;  
            }

            if(buffer_receive_slave[5]==TrangThaiMotor_nghien_ep_phanloai::LOI)
            {
                motor_phanloai.ui8_kiemtra_trangthailoi=TrangthaiLoi::LOI;
                // Serial_Debug.println("loi bo phan loai ");
                break;
            }

            if(buffer_receive_slave[5]==TrangThaiMotor_nghien_ep_phanloai::DANGXULY)
            {
                motor_phanloai.ui8_kiemtra_trangthailoi=TrangthaiLoi::BAN;
                // Serial_Debug.printf("dong phan loai dang ban\n");
                break;
            }        


            if(buffer_receive_slave[6]==TrangThaiMotor_nghien_ep_phanloai::KHONGLOI)
            {
              if(motor_nghien.ui8_kiemtra_trangthailoi!=TrangthaiLoi::KHONGLOI)
              {
                motor_nghien.ui8_kiemtra_trangthailoi=TrangthaiLoi::LOI;
                EEPROM.put(1, motor_nghien.ui8_kiemtra_trangthailoi);
		          	EEPROM.commit();
                return;
              } 
              // if(ui8_checkloi_nghien==0 && motor_nghien.ui8_kiemtra_trangthailoi==TrangthaiLoi::KHONGLOI)
              // {
              //   motor_nghien.ui8_kiemtra_trangthailoi=TrangthaiLoi::KHONGLOI;
              //   Serial_Debug.printf("dong nghien ok\n");
              // }
              else
              {
                motor_nghien.ui8_kiemtra_trangthailoi=TrangthaiLoi::KHONGLOI;
                // EEPROM.put(1, motor_nghien.ui8_kiemtra_trangthailoi);
		          	// EEPROM.commit();
                // Serial_Debug.printf("dong co nghien ok\n");
              }
                break;
            }

            if(buffer_receive_slave[6]==TrangThaiMotor_nghien_ep_phanloai::LOI)
            {
                motor_nghien.ui8_kiemtra_trangthailoi=TrangthaiLoi::LOI;
                // ui8_checkloi_nghien=1;
                // Serial_Debug.printf("LOI MOTOR NGHIEN\n");
                break;
            }

            if(buffer_receive_slave[6]==TrangThaiMotor_nghien_ep_phanloai::DANGXULY)
            {
                motor_nghien.ui8_kiemtra_trangthailoi=TrangthaiLoi::BAN;
                break;
            }  


            if(buffer_receive_slave[7]==TrangThaiMotor_nghien_ep_phanloai::KHONGLOI) // rac binh thuong
            {
              if(ui16_sochai_trongthungrac==SOCHAITOIDA) return;
              ui8_trangthai_thungracchai=0;
                // motor_nghien.ui8_kiemtra_trangthailoi=TrangthaiLoi::KHONGLOI;
                // if(ui8_trangthaithungracchai==TrangThaiThungRac::DAY)
                // {
                //   ui8_trangthaithungracchai=TrangThaiThungRac::KHONGDAY;
                
                // thungrac_chai.ui8_phanhoitrangthai_thungrac=1;

                // }
                                break;
            }

            if(buffer_receive_slave[7]==TrangThaiMotor_nghien_ep_phanloai::LOI )// rac day
            {
                ui8_trangthai_thungracchai=1;
                Serial_Debug.printf("rac day\n");
                // motor_nghien.ui8_kiemtra_trangthailoi=TrangthaiLoi::LOI;
                // if(ui8_trangthaithungracchai==TrangThaiThungRac::KHONGDAY)
                // {
                //   ui8_trangthaithungracchai=TrangThaiThungRac::DAY;
                //   SETTRANGTHAIMAY();
                //   thungrac_chai.ui8_phanhoitrangthai_thungrac=1;
                // }
                break;
            }

            if(buffer_receive_slave[8]==TrangThaiMotor_nghien_ep_phanloai::KHONGLOI) // rac binh thuong
            {
              if(ui16_solon_trongthungrac==SOCHAITOIDA) return;
                ui8_trangthai_thungraclon=0;
                // motor_nghien.ui8_kiemtra_trangthailoi=TrangthaiLoi::KHONGLOI;
                // if(ui8_trangthaithungrackhac==TrangThaiThungRac::DAY)
                // {
                //   ui8_trangthaithungrackhac=TrangThaiThungRac::KHONGDAY;
                // thungrac_khac.ui8_phanhoitrangthai_thungrac=1;
                // }
                
                break;
            }

            if(buffer_receive_slave[8]==TrangThaiMotor_nghien_ep_phanloai::LOI)// rac day
            {
              ui8_trangthai_thungraclon=1;
                // motor_nghien.ui8_kiemtra_trangthailoi=TrangthaiLoi::LOI;
                // if(ui8_trangthaithungraclon==TrangThaiThungRac::KHONGDAY)
                // {
                //   ui8_trangthaithungraclon=TrangThaiThungRac::DAY;
                // SETTRANGTHAIMAY();
                // thungrac_lon.ui8_phanhoitrangthai_thungrac=1;
                // }
                
                break;
            }
        }

        case codeSlaveRS485::check_fw:
        {
          Serial_Debug.println("nhayvaocheck_fw_slave1");
          for(int i=0;i<4;i++)
          {
            buffer_version_slave1[i]=buffer_receive_slave[i+5];
          }

          ver_slave1=xuly_version_slave(buffer_version_slave1);
          Serial_Debug.println("phien ban slave 1");
          // Serial_Debug.print(ver_slave1);

          ver_slave1.toCharArray(v_slave1,sizeof(v_slave1));
          Serial_Debug.println(v_slave1);
          // delay(1000);
          // ui32_timeout_guifw=millis()+5000;
          ui32_timeout_guifw=millis();
          ui8_upfw_slave2=1;
          // ui8_khongcheck_receive=0;
          break;
        }

      default:
        // Serial_Debug.println(" Incorrect command all");
        break;
      }
    }
    else if (slaveID == ui8_slave[2])
    {
      Serial_Debug.printf("slave 2 dang gui\n");
      switch (function)
      {
        case codeSlaveRS485::check_error:
        {

            if(buffer_receive_slave[5]==TrangThaiMotor_nghien_ep_phanloai::KHONGLOI)
            {
              if(motor_ep.ui8_kiemtra_trangthailoi==TrangthaiLoi::LOI) return;
                motor_ep.ui8_kiemtra_trangthailoi=TrangthaiLoi::KHONGLOI;
                // Serial_Debug.printf("dong ep ko loi\n");
               break;
            }

            if(buffer_receive_slave[5]==TrangThaiMotor_nghien_ep_phanloai::LOI)
            {
                motor_ep.ui8_kiemtra_trangthailoi=TrangthaiLoi::LOI;
                // Serial_Debug.printf("dong ep dang loi\n");
               break;
            }

            if(buffer_receive_slave[5]==TrangThaiMotor_nghien_ep_phanloai::DANGXULY)
            {
                motor_ep.ui8_kiemtra_trangthailoi=TrangthaiLoi::BAN;
                // Serial_Debug.printf("dong ep dang ban\n");
               break;
            }  


            if(buffer_receive_slave[6]==TrangThaiMotor_nghien_ep_phanloai::KHONGLOI ) // rac binh thuong
            {
              if(ui16_vat_trongthungrackhac==SOCHAITOIDA) return;
              ui8_trangthai_thungrackhac=0;
                // motor_nghien.ui8_kiemtra_trangthailoi=TrangthaiLoi::KHONGLOI;
                // ui8_trangthaithungraclon=TrangThaiThungRac::KHONGDAY;
                // if(ui8_trangthaithungraclon==TrangThaiThungRac::DAY)
                // {
                  // ui8_trangthaithungraclon=TrangThaiThungRac::KHONGDAY;
                  // thungrac_lon.ui8_phanhoitrangthai_thungrac=1;
                // }
                
               break;
            }

            if(buffer_receive_slave[6]==TrangThaiMotor_nghien_ep_phanloai::LOI)// rac day
            {
              ui8_trangthai_thungrackhac=1;
                // motor_nghien.ui8_kiemtra_trangthailoi=TrangthaiLoi::LOI;
                // if(ui8_trangthaithungrackhac==TrangThaiThungRac::KHONGDAY)
                // {
                //   ui8_trangthaithungrackhac=TrangThaiThungRac::DAY;
                // SETTRANGTHAIMAY();
                // thungrac_khac.ui8_phanhoitrangthai_thungrac=1;
                // }
                
               break;
            }              
        }

        case codeSlaveRS485::check_fw:
        {
         
          Serial_Debug.println("nhayvaocheck_fw_slave2");
          for(int i=0;i<4;i++)
          {
            buffer_version_slave2[i]=buffer_receive_slave[i+5];
          }

          ver_slave2=xuly_version_slave(buffer_version_slave2);
          Serial_Debug.println("phien ban slave 2");
          // Serial_Debug.print(ver_slave2);
          ver_slave2.toCharArray(v_slave2,sizeof(v_slave2));
          Serial_Debug.print(v_slave2);
          ui32_timeout_guifw=millis();
           ui8_upfw_slave3=1;
          break;
        }

        default:
//        Serial_Debug.println(" Incorrect command all");
        break;
      }
    }
    else if (slaveID == ui8_slave[3])
    {
      Serial_Debug.printf("slave 3 dang gui\n");
      switch (function)
      {
        case codeSlaveRS485::check_error:
        {
          if(buffer_receive_slave[5] == 0x02)
          {
            ui8_trangthai_cuatren=MOCUA;
            // Serial_Debug.printf("mo cua tren\n");
            // break;
          }
          if(buffer_receive_slave[5]==0x01)
          {
            ui8_trangthai_cuatren=DONGCUA;
            // Serial_Debug.printf("dong cua tren\n");
            // break;
          }

          if(buffer_receive_slave[5]==0x00)
          {
            ui8_trangthai_cuatren=BINHTHUONG;
            // break;
          }

          else if(buffer_receive_slave[6]==0x01)
          {
            ui8_trangthai_cuaduoi=DONGCUA;
            // Serial_Debug.printf("dong cua duoi\n");
            // break;
          }

          else if(buffer_receive_slave[6]==0x00)
          {
            ui8_trangthai_cuaduoi=BINHTHUONG;
            // break;
          }

          else if(buffer_receive_slave[6]==0x02)
          {
            ui8_trangthai_cuaduoi=MOCUA;
            // Serial_Debug.printf("mo cua duoi\n");
            // break;
          }

        }

        case codeSlaveRS485::check_fw:
        {  
          for(int i=0;i<4;i++)
          {
            buffer_version_slave3[i]=buffer_receive_slave[i+5];
          }

          ver_slave3=xuly_version_slave(buffer_version_slave3);
          Serial_Debug.println("phien ban slave 3: ");
          // Serial_Debug.print(ver_slave3);
          ver_slave3.toCharArray(v_slave3,sizeof(v_slave3));
          Serial_Debug.print(v_slave3);
          ui32_timeout_guifw=millis();
          ui8_upfw_slave4=1;
          break;
        }
      default:
        // Serial_Debug.println(" Incorrect command all");
        break;
      }
    }

    else if(slaveID == ui8_slave[4])
    {
      Serial_Debug.printf("slave 4 dang gui\n");
      switch (function)
      {
        case codeSlaveRS485::check_error:
        {
          if(buffer_receive_slave[5]==0x02)
          {
            // Serial_Debug.printf("nha qua thanh cong\n");
            ui8_trangthai_nhaqua=THANHCONG;
            ui8_phanhoi_trangthainhaqua=1;
            break;
          }

          if(buffer_receive_slave[5]==0x01)
          {
            // Serial_Debug.printf("nha qua that bai\n");
            ui8_trangthai_nhaqua=THATBAI;
            ui8_phanhoi_trangthainhaqua=1;
            break;
          }

          if(buffer_receive_slave[5]==0x00)
          {
            // Serial_Debug.printf("nha qua RESET\n");
            ui8_trangthai_nhaqua=BINHTHUONG;
            // ui8_phanhoi_trangthainhaqua=1;
            break;
          }
        }

        case codeSlaveRS485::check_fw:
        {
          // ui8_upfw_slave4=1;
          for(int i=0;i<4;i++)
          {
            buffer_version_slave4[i]=buffer_receive_slave[i+5];
          }
          break;

          ver_slave4=xuly_version_slave(buffer_version_slave4);
          Serial_Debug.println("phien ban slave 4");
          // Serial_Debug.print(ver_slave4);
          ver_slave4.toCharArray(v_slave4,sizeof(v_slave4));
          Serial_Debug.print(v_slave4);
        }

      default:
        Serial_Debug.println(" Incorrect command all");
        break;
      }
    }
  }
}

String xuly_version_slave(uint8_t *data)
{
    uint8_t value1 = data[0];
    uint8_t value2 = data[1];

    // Gộp hai byte cuối thành uint16_t
    uint16_t combinedValue = (data[2] << 8) | data[3];

    // In kết quả ra Serial Monitor
    Serial_Debug.print("Value1 (uint8_t): ");
    Serial_Debug.println(value1);

    Serial_Debug.print("Value2 (uint8_t): ");
    Serial_Debug.println(value2);

    Serial_Debug.print("Combined Value (uint16_t): ");
    Serial_Debug.println(combinedValue);

    String result = String(value1) + "_" + String(value2) + "_" + String(combinedValue);

    // In ra Serial Monitor
    Serial_Debug.println(result);
    return result;
}


void kiemtrathungrac()
{
  if(ui8_startprocess == 1) 
	{
		return;
	}	

  if(ui8_trangthai_thungracchai==1 && ui8_trangthaithungracchai==TrangThaiThungRac::KHONGDAY)
  {
    ui8_trangthaithungracchai=TrangThaiThungRac::DAY;
    EEPROM.put(16, ui8_trangthaithungracchai);
    EEPROM.commit();

    ui8_is_stopuser = 1;
    ui16_sochai_trongthungrac = SOCHAITOIDA;
    EEPROM.put(10, ui16_sochai_trongthungrac);
    EEPROM.commit();
    SETTRANGTHAIMAY();
    thungrac_chai.ui8_phanhoitrangthai_thungrac=1;
    ui8_phanhoi_phantram_thungracchai = 1;
  }

  else if(ui8_trangthai_thungraclon==1 && ui8_trangthaithungraclon==TrangThaiThungRac::KHONGDAY)
  {
    ui8_trangthaithungraclon=TrangThaiThungRac::DAY;
    EEPROM.put(17, ui8_trangthaithungraclon);
    EEPROM.commit();
    ui8_is_stopuser = 1;
    ui16_solon_trongthungrac = SOCHAITOIDA;
    EEPROM.put(12, ui16_solon_trongthungrac);
    EEPROM.commit();
    SETTRANGTHAIMAY();
    thungrac_lon.ui8_phanhoitrangthai_thungrac=1;
    ui8_phanhoi_phantram_thungraclon = 1;
  }

  else if(ui8_trangthai_thungrackhac==1 && ui8_trangthaithungrackhac==TrangThaiThungRac::KHONGDAY)
  {
    ui8_trangthaithungrackhac=TrangThaiThungRac::DAY;
    EEPROM.put(18, ui8_trangthaithungrackhac);
    EEPROM.commit();
    ui8_is_stopuser = 1;
    ui16_vat_trongthungrackhac = SOCHAITOIDA;
    EEPROM.put(14, ui16_vat_trongthungrackhac);
    EEPROM.commit();
    SETTRANGTHAIMAY();
    thungrac_khac.ui8_phanhoitrangthai_thungrac=1;
    ui8_phanhoi_phantram_thungrackhac = 1;
  }
}

void kiemtracuamay()
{
  if(ui8_startprocess == 1)
	{
		return;
	}

  if(ui8_trangthai_cuatren==MOCUA && ui8_trangthaicua_may==0)
  {
    Serial_Debug.printf("cua tren mo\n");
    ui8_trangthaicua_may = 1;
		ui8_phanhoi_mocuamay = 1;
  }

  else if(ui8_trangthai_cuatren==DONGCUA && ui8_trangthaicua_may==1)
  {
    Serial_Debug.printf("cua tren dong\n");
    ui8_trangthaicua_may = 0;
		ui8_phanhoi_mocuamay = 1;
  }

  // else if(ui8_trangthai_cuatren==MOCUA)
  // {
  //     if(ui8_saukhinhanlenh_dong==1)
  //     {
  //       if(ui32_timeout_nhalenhdongcua<=millis())
  //       {
  //         ui8_trangthai_cuatren=DONGCUA;
  //         ui8_trangthaicua_may = 0;
	// 	      ui8_phanhoi_mocuamay = 1; 
  //         ui8_saukhinhanlenh_dong=0;
  //       }
  //     }
  // }

  // else if(ui8_trangthai_cuatren==DONGCUA)
  // {
  //     if(ui8_saukhinhanlenh_mo==1)
  //     {
  //       if(ui32_timeout_nhalenhmocua<=millis())
  //       {
  //         ui8_trangthaicua_may = 1;
	// 	      ui8_phanhoi_mocuamay = 1; 
  //         ui8_saukhinhanlenh_mo=0;
  //       }
  //     }
  // }

  if(ui8_trangthai_cuaduoi==MOCUA && ui8_trangthaicuathungrac==0)
  {
    Serial_Debug.printf("cua duoi mo\n");
    ui8_trangthaicuathungrac = 1;
		ui8_phanhoi_mocuathungrac = 1;
  }

  else if(ui8_trangthai_cuaduoi==DONGCUA && ui8_trangthaicuathungrac==1)
  {
    Serial_Debug.printf("cua duoi dong\n");
    ui8_trangthaicuathungrac = 0;
		ui8_phanhoi_mocuathungrac = 1;
  }

  // else if(ui8_trangthai_cuaduoi==MOCUA)
  // {
  //     if(ui8_saukhinhanlenh_dongduoi==1)
  //     {
  //       if(ui32_timeout_nhalenhdongcuaduoi<=millis())
  //       {
  //         ui8_trangthaicuathungrac = 0;
	// 	      ui8_phanhoi_mocuathungrac = 1; 
  //         ui8_saukhinhanlenh_dongduoi=0;
  //       }
  //     }
  // }

  // else if(ui8_trangthai_cuatren==DONGCUA)
  // {
  //     if(ui8_saukhinhanlenh_moduoi==1)
  //     {
  //       if(ui32_timeout_nhalenhmocuaduoi<=millis())
  //       {
          
  //         ui8_trangthaicuathungrac = 1;
	// 	      ui8_phanhoi_mocuathungrac = 1; 
  //         ui8_saukhinhanlenh_moduoi=0;
  //       }
  //     }
  // }
}

void nangbangtai()
{
	// int doc_cambiennang_bangtai = doccambien_portA(cambien_nangha,6);
	int doc_cambiennang_bangtai = digitalRead(cambien_nangha);
	if(doc_cambiennang_bangtai == 1)
	{
		int thuchien_nangha = 1;
		// ui8_check_guislave=1;
		run_Motor_nangha();
		Serial_Debug.println("dang ve cam bien nang ha");
		unsigned long thoigiantam_nangha = millis();
		while(thuchien_nangha==1)
		{	
			doc_cambiennang_bangtai = digitalRead(cambien_nangha);
			if(doc_cambiennang_bangtai == 0)
			{
				// ui8_check_guislave=0;
				stop_Motor_nangha();
				// thuchien_nangha=0;
				// ui8_kiemtramotorphanloai=BAN;
				Serial_Debug.println("cam bien nang ha da ve home");
				return;
			}
			
			if(millis() - thoigiantam_nangha >= (1000*5))
			{
				Serial_Debug.println("loi dong co nang ha");
				// thuchien_nangha=0;
				stop_Motor_nangha();
				// ui8_trangthaimay=1;
				motor_nangha.ui8_kiemtra_trangthailoi_motorDC = TrangthaiLoi::LOI;
				EEPROM.put(0, motor_nangha.ui8_kiemtra_trangthailoi_motorDC);
				EEPROM.commit();
				motor_nangha.ui8_phanhoitrangthai=TrangThaiLenhPhanHoi::DUNG;

			}
		}
	}
}

void getFileNameWithoutExtension(const char *filename, char *output, size_t outputSize) 
{
  const char *dot = strrchr(filename, '.'); // Tìm dấu chấm cuối cùng
  if (dot) {
      size_t len = dot - filename; // Độ dài phần tên không có đuôi
      if (len < outputSize) {
          strncpy(output, filename, len);
          output[len] = '\0'; // Đảm bảo chuỗi kết thúc đúng
      }
  } else {
      strncpy(output, filename, outputSize); // Trường hợp không có dấu chấm
  }
}


void getFileName(const char *filename, char *output, size_t outputSize) 
{
  const char *underscore = strrchr(filename, '_'); // Tìm dấu _ cuối cùng
    if (underscore) {
        size_t len = underscore - filename; // Độ dài phần trước dấu _
        if (len < outputSize) {
            strncpy(output, filename, len);
            output[len] = '\0'; // Đảm bảo chuỗi kết thúc đúng
        }
    } else {
        strncpy(output, filename, outputSize); // Trường hợp không có dấu _
    }
}

void getSuffix(const char *filename, char *output, size_t outputSize) {
  const char *underscore = strrchr(filename, '_'); // Tìm dấu _ cuối cùng
  if (underscore) {
      strncpy(output, underscore + 1, outputSize - 1); // Sao chép phần sau dấu _
      output[outputSize - 1] = '\0'; // Đảm bảo chuỗi kết thúc đúng
  } else {
      strncpy(output, filename, outputSize); // Trường hợp không có dấu _
  }
}

void quet_tenfile(fs::FS &fs, const char * dirname, uint8_t levels)
{
  if(timeout>millis())
  {
    return;
  }
  int index=0;

  // if (xSemaphoreTake(batton, portMAX_DELAY)) {  // Chiếm quyền truy cập SPIFFS
  
  File root = fs.open(dirname);

  if(!root)
  {
    Serial_Debug.println("[*FILE]: Failed to open directory"); 
    return;
  }

  if(!root.isDirectory())
  {
    // #ifndef use_debug_serial
    Serial_Debug.println("[*FILE]: Not a directory");
// #endif 
    return;
  }

  File file = root.openNextFile();
  while(file)
  {
    if(file.isDirectory()){
      // #ifndef use_debug_serial
        Serial_Debug.print("[*FILE]: \tDIR : ");
              Serial_Debug.print (file.name());
        // #endif 

              //bỏ qua không xử lý folder
        // if(levels){
                  // getmediafilename(fs, file.name(), levels -1);
              // }
          } 
          else 
          {
              // #ifndef use_debug_serial
          Serial_Debug.print("[*FILE]: NAME: ");
                Serial_Debug.print(file.name());
                Serial_Debug.print("  SIZE: ");
                Serial_Debug.println(file.size());
          // #endif 

            const char *s =(file.name());
            snprintf(filename_update, sizeof(filename_update), "/%s", s); // Ghép dấu '/' vào trước chuỗi
            // sprintf(filename_update,"\"/%s\"",s);
            // Serial_Debug.println(filename_update);
            // Serial_Debug.print("file update: ");
            // Serial_Debug.println(filename_update);
            getFileNameWithoutExtension(s,nameOnly,sizeof(nameOnly));
           
            Serial_Debug.print("Tên file không có đuôi: ");
            Serial_Debug.println(nameOnly); 
            getFileName(nameOnly,nameOnly_finish,sizeof(nameOnly_finish));

            Serial_Debug.print("Tên version cua slave 1 hien tai: ");
            Serial_Debug.println(v_slave1); 

            // for(int i=0; i<6; i++)
            // {
            //   nameOnly_uncut[i] = nameOnly[i+8];
            // }

            getSuffix(nameOnly,nameOnly_uncut,sizeof(nameOnly_uncut));
            Serial_Debug.print("Tên file sau khi cat ra: ");
            Serial_Debug.println(nameOnly_uncut);

            if(String(nameOnly_uncut)=="slave1")
            {
              if(strcmp(nameOnly_finish,v_slave1)!=0)
              {
                Serial_Debug.println("[*FILE]: \tright file-->keep file for slave 1");
                Serial_Debug.println("tien hanh update slave 1");
                ui2_update_firmware=true;
                // for(int i=0;i<sizeof(nameOnly_finish);i++)
                // {
                //   v_slave1[i]=nameOnly_finish[i];
                // }
                ui8_fw_can_up=ui8_slave[1];
                // Serial_Debug.print("Tên version cua slave 1 sau update: ");
                // Serial_Debug.println(v_slave1); 
                timeout=millis()+3*60*1000;
              }
              else if(strcmp(nameOnly_finish,v_slave1)==0)
              {
                Serial_Debug.println("khong can update");
                deleteFile(fs,file.name());
                ui8_danggui_fw=0;
                timeout=millis()+3*60*1000;
              }
              else
              {
                ui8_danggui_fw=0;
                Serial_Debug.println("khong co file luu");
                timeout=millis()+3*60*1000;
              }
            }
            // if(nameOnly[2]==)
            
            if(String(nameOnly_uncut)=="slave4")
            {
              if(strcmp(nameOnly_finish,v_slave4)!=0)
              {
                Serial_Debug.println("[*FILE]: \tright file-->keep file for slave 4");
                Serial_Debug.println("tien hanh update slave 1");
                ui2_update_firmware=true;
                ui8_fw_can_up=ui8_slave[4];
                timeout=millis()+3*60*1000;
              }
              else if(strcmp(nameOnly_finish,v_slave4)==0)
              {
                ui8_danggui_fw=0;
                Serial_Debug.println("khong can update");
                deleteFile(fs,file.name());
                timeout=millis()+3*60*1000;
              }
              else
              {
                ui8_danggui_fw=0;
                Serial_Debug.println("khong co file luu");
                timeout=millis()+3*60*1000;
              }
            }

            if(String(nameOnly_uncut)=="slave2")
            {
              if(strcmp(nameOnly_finish,v_slave2)!=0)
              {
                Serial_Debug.println("[*FILE]: \tright file-->keep file for slave 1");
                Serial_Debug.println("tien hanh update slave 2");
                ui2_update_firmware=true;
                ui8_fw_can_up=ui8_slave[2];
                timeout=millis()+3*60*1000;
              }
              else if(strcmp(nameOnly_finish,v_slave2)==0)
              {
                ui8_danggui_fw=0;
                Serial_Debug.println("khong can update");
                deleteFile(fs,file.name());
                timeout=millis()+3*60*1000;
              }
              else
              {
                ui8_danggui_fw=0;
                Serial_Debug.println("khong co file luu");
                timeout=millis()+3*60*1000;
              }
            }

            if(String(nameOnly_uncut)=="slave3")
            {
              if(strcmp(nameOnly_finish,v_slave3)!=0)
              {
                Serial_Debug.println("[*FILE]: \tright file-->keep file for slave 3");
                Serial_Debug.println("tien hanh update slave 1");
                ui2_update_firmware=true;
                ui8_fw_can_up=ui8_slave[3];
                timeout=millis()+3*60*1000;
              }
              else if(strcmp(nameOnly_finish,v_slave3)==0)
              {
                ui8_danggui_fw=0;
                Serial_Debug.println("khong can update");
                deleteFile(fs,file.name());
                timeout=millis()+3*60*1000;
              }
              else
              {
                ui8_danggui_fw=0;
                Serial_Debug.println("khong co file luu");
                timeout=millis()+3*60*1000;
              }
            }
          }
          file = root.openNextFile();
  }
// }
}


void deleteFile(fs::FS &fs, String path)
{
  Serial.printf("Deleting file: %s\r\n", path);

if (!path.startsWith("/")) path = "/" + path;


  if(fs.remove(path)){
      Serial_Debug.println("- file deleted");
  }
else 
{
      Serial_Debug.println("- delete failed");
}
}

uint8_t calculateChecksum(const uint8_t *data, size_t length)
{
  uint8_t checksum = 0;
  for (size_t i = 0; i < length; i++)
  {
    checksum += data[i]; // Tổng tất cả các byte
  }
  return checksum; // Trả về checksum (mod 256 tự động do kiểu uint8_t)
}

void sendFirmware(const char *filePath)
{

  uint8_t buffer[BUFFER_SIZE];
  size_t bytesRead;
  uint32_t totalSend = 0;

  if((ui8_upfw_slave1 + ui8_upfw_slave2 + ui8_upfw_slave3 + ui8_upfw_slave4)!=0 && (ui32_timeout_hetguifw > millis())) return;
  // digitalWrite(RS485_DE_PIN, HIGH);
  // delay(100);

  /* Open firmware file */
  File file = SPIFFS.open(filePath,"r");
  if (!file)
  {
    Serial_Debug.println("Failed to open firmware file");
    return;
  }

  Serial_Debug.println("Sending firmware...");

  /* Gửi tổng số byte */
  uint32_t fileSize = file.size();
  Serial1.write((uint8_t *)&fileSize, sizeof(fileSize));
  Serial_Debug.printf("Firmware size: %u bytes\n", fileSize);
  delay(500);

  /* Gửi từng gói dữ liệu + checksum đến khi hết file */
  while ((bytesRead = file.read(buffer, sizeof(buffer))) > 0)
  {
    uint8_t checksum = calculateChecksum(buffer, bytesRead);

    /* Gửi begin marker */
    Serial1.write(BEGIN_PAGE_MARKER, sizeof(BEGIN_PAGE_MARKER));
    Serial1.write(buffer, bytesRead);
    Serial1.write(checksum);

    /* Gửi end marker */
    if (bytesRead == sizeof(buffer))
      Serial1.write(END_PAGE_MARKER, sizeof(END_PAGE_MARKER));
    else
      Serial1.write(END_FILE_MARKER, sizeof(END_FILE_MARKER));

    Serial1.flush();

    totalSend += bytesRead;
    Serial_Debug.printf("Sent %d bytes || Total sent: %d bytes\n", bytesRead, totalSend);
    Serial_Debug.printf("Checksum: 0x%02X\n", checksum);

    /* Chờ phản hồi nhận OK*/
    // digitalWrite(RS485_DE_PIN, LOW);
    uint8_t receiveOK = false;
    uint32_t timeWaitRespond = millis();
    uint8_t counter = 0;
    uint8_t code[100];
    while (millis() - timeWaitRespond < 5000)
    {
      if (Serial1.available())
      {
        int len = Serial1.readBytes(code, 3);
        if (len != 3)
          continue;
        if (code[0] == 'R' && code[1] == 'O' && code[2] == 'K')
        {
          receiveOK = true;
          Serial_Debug.println("Receive OK");
          break;
        }
        /* Nhận phản hồi lỗi, gửi lại dữ liệu */
        if (code[0] == 'E' && code[1] == 'R' && code[2] == 'R' && counter <= 3)
        {
          counter++;
          delay(100);
          Serial_Debug.println("Resend");
          // digitalWrite(RS485_DE_PIN, HIGH);
          /* Gửi begin marker */
          Serial1.write(BEGIN_PAGE_MARKER, sizeof(BEGIN_PAGE_MARKER));
          Serial1.write(buffer, bytesRead);
          Serial1.write(checksum);

          /* Gửi end marker */
          if (bytesRead == sizeof(buffer))
            Serial1.write(END_PAGE_MARKER, sizeof(END_PAGE_MARKER));
          else
            Serial1.write(END_FILE_MARKER, sizeof(END_FILE_MARKER));
          Serial1.flush();
          // digitalWrite(RS485_DE_PIN, LOW);
          timeWaitRespond = millis();
          continue;
        }
      }
    }
    // digitalWrite(RS485_DE_PIN, HIGH);
    /* Kiểm tra có nhận được phản hồi không*/
    if (!receiveOK)
      ui2_sendOK = false;
    // delay(1000);
  }

  Serial_Debug.println("Firmware sent successfully!");
  if(ui8_fw_can_up==ui8_slave[1])
  {
    for(int i=0;i<sizeof(nameOnly_finish);i++)
    {
      v_slave1[i]=nameOnly_finish[i];
    }
  }

  else if(ui8_fw_can_up==ui8_slave[2])
  {
    for(int i=0;i<sizeof(nameOnly_finish);i++)
    {
      v_slave2[i]=nameOnly_finish[i];
    }
  }

  else if(ui8_fw_can_up==ui8_slave[3])
  {
    for(int i=0;i<sizeof(nameOnly_finish);i++)
    {
      v_slave3[i]=nameOnly_finish[i];
    }
  }

  else if(ui8_fw_can_up==ui8_slave[4])
  {
    for(int i=0;i<sizeof(nameOnly_finish);i++)
    {
      v_slave4[i]=nameOnly_finish[i];
    }
  }
  ui2_update_firmware = false; /* Reset biến trạng thái cập nhật firmware */
  Serial_Debug.printf("Send OK: %d\n", ui2_sendOK);
  // ui8_reset_all=0x01;
  // digitalWrite(RS485_DE_PIN, LOW);
  file.close();
}

void send_fw_toslave()
{
  // if(ui32_timeout_guifw >= millis())
  // {
  //   return;
  // }
  // Serial_Debug.println("gui fw cho slave");
  if(ui8_upfw_slave1==1)
  {
    Serial_Debug.println("gui lenh fw cho slave 1");
    ui8_sendfw_slave=0x01;
    // ui32_timeout_guiphanhoi=millis()+20000;
    ui32_timeout_guifw=millis();
    ui8_upfw_slave1=0;
  }

  else if(ui8_upfw_slave2==1 && (millis() - ui32_timeout_guifw >= 1000))
  {
    Serial_Debug.println("gui lenh fw cho slave 2");
    ui8_sendfw_slave=0x02;
    // ui32_timeout_guiphanhoi=millis()+2000;
    // ui32_timeout_guifw=millis()+5000;
    ui32_timeout_guifw=millis();
    ui8_upfw_slave2=0;
  }

  else if(ui8_upfw_slave3==1 && (millis() - ui32_timeout_guifw >= 1000))
  {
    Serial_Debug.println("gui lenh fw cho slave 3");
    ui8_sendfw_slave=0x03;
    // ui32_timeout_guiphanhoi=millis()+2000;
    ui32_timeout_guifw=millis();
    ui8_upfw_slave3=0;
  }

  else if(ui8_upfw_slave4==1 && (millis() - ui32_timeout_guifw >= 1000))
  {
    Serial_Debug.println("gui lenh fw cho slave 4");
    ui8_sendfw_slave=0x04;
    // ui32_timeout_guiphanhoi=millis()+2000;
    ui32_timeout_guifw=millis();
    ui8_upfw_slave4=0;
  }
  // else if(millis() - ui32_timeout_guifw >= 5000)
  // {
  //   Serial_Debug.println("hong con gi out nhe");
    
  // }
}



