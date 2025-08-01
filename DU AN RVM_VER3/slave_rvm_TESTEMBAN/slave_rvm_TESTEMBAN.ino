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
#include <Update.h>
// #include "Debug_usb.h"

DONGCO Motor_cua(DIR_MOTOR1,PWM_MOTOR1);
DONGCO Motor_bangtai(DIR_MOTOR2,PWM_MOTOR2);
DONGCO Motor_xoaychai(DIR_MOTOR3,PWM_MOTOR3);
DONGCO Motor_nangha(DIR_MOTOR4,PWM_MOTOR4);


// RS485_Master RS485(Serial1, TX_PIN, RX_PIN);

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


  config();

  Serial_Debug.println("hello");

  for(int i=0; i<sizeof(ui8_slave); i++)
  {
    ui32_timeout_checkrs485[i]=millis()+1000;  
  }
  
  batton = xSemaphoreCreateBinary();
	
	xSemaphoreGive(batton);
	
	xTaskCreatePinnedToCore(
      Task1core, /* Function to implement the task */
      "Task1", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      1,  /* Priority of the task */
      NULL,  /* Task handle. */
      0); /* Core where the task should run */
	
	delay(500);
	xTaskCreatePinnedToCore(
      Task2core, /* Function to implement the task */
      "Task2", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      1,  /* Priority of the task */
      NULL,  /* Task handle. */
      1); /* Core where the task should run */
	  
	
	// vTaskStartScheduler();
	// ui8_is_loop = 1;
	vTaskDelete(NULL);
	delay(500);
}

void loop() {
}

void Task1core(void *parameter)
{
  for(;;)
  {
    xSemaphoreTake(batton, portMAX_DELAY);
    BATLED_RUN();
    if(ui8_controlcuatren==1)
    {
      int tam=dongmocua_control(mocua_tren,cambien_cuatren,ui8_trangthai_cuatren);
      if(tam==1)
      {
        // Serial_Debug.println("cua mo roi khi nhan lenh mo cua tren");
        ui8_resetbien=1;
        ui8_controlcuatren=0;
        // ui8_trangthai_cuatren=TrangThaiCua::MOCUA;
        ui32_timeout_checktrangthai=millis()+5000;
      }

      else if(tam==0)
      {
        // Serial_Debug.println("cua dong roi du da nhan lenh mo cua tren");
        // ui8_trangthai_cuatren=TrangThaiCua::DONGCUA;
        ui8_controlcuatren=0;
        ui8_resetbien=1;
        ui32_timeout_checktrangthai=millis()+5000;
      }
    }

    if(ui8_controlcuaduoi==1)
    {
      int tam=dongmocua_control(mocua_duoi,cambien_cuaduoi,ui8_trangthai_cuaduoi);
      if(tam==1)
      {
        ui8_resetbien=1;
        Serial_Debug.println("cua mo roi sau khi nhan lenh mo cua duoi");
        ui8_controlcuaduoi=0;
        // ui8_trangthai_cuaduoi=TrangThaiCua::MOCUA;
        ui32_timeout_checktrangthai=millis()+5000;
      }

      else if(tam==0)
      {
        Serial_Debug.println("cua dong roi du da nhan lenh mo cua");
        ui8_controlcuaduoi=0;
        // ui8_trangthai_cuaduoi=TrangThaiCua::DONGCUA;
        ui8_resetbien=1;
        ui32_timeout_checktrangthai=millis()+5000;
      }
    }

    if(ui8_dongcuatren==1)
    {
      int tam=dongcua_control(mocua_tren,cambien_cuatren,ui8_trangthai_cuatren);
      if(tam==1)
      {
        ui8_resetbien=1;
        ui32_timeout_checktrangthai=millis()+5000;
        Serial_Debug.printf("cua dong roi sau khi nhan lenh dong cua tren\n");
        // ui8_trangthai_cuatren=TrangThaiCua::DONGCUA;
        ui8_dongcuatren=0;
      }

      if(tam==0)
      {
        ui8_resetbien=1;
        Serial_Debug.printf("cua mo du nhan lenh dong cua\n");
        // ui8_trangthai_cuatren=TrangThaiCua::MOCUA;
        ui32_timeout_checktrangthai=millis()+5000;
        ui8_dongcuatren=0;
      }
    }

    if(ui8_dongcuaduoi==1)
    {
      int tam=dongcua_control(mocua_duoi,cambien_cuaduoi,ui8_trangthai_cuaduoi);
      if(tam==1)
      {
          ui8_resetbien=1;
          Serial_Debug.printf("cua dong roi sau khi nhan lenh dong cua duoi\n");
          // ui8_trangthai_cuaduoi=TrangThaiCua::DONGCUA;
          ui32_timeout_checktrangthai=millis()+5000;
          ui8_dongcuaduoi=0;
      }

      if(tam==0)
      {
        ui8_resetbien=1;
        Serial_Debug.printf("cua mo roi du da nhan lenh dong cua duoi\n");
        // ui8_trangthai_cuaduoi=TrangThaiCua::MOCUA;
        ui32_timeout_checktrangthai=millis()+5000;
        ui8_dongcuaduoi=0;
      }
    }

    kiemtracua();

    // if(ui8_resetbien==1)
    // {
    //   if(ui32_timeout_checktrangthai<=millis())
    //   {
    //     Serial_Debug.println("reset bien trang thai cua");
    //     if(ui8_trangthai_cuaduoi!=TrangThaiCua::BINHTHUONG)
    //     {
    //       ui8_trangthai_cuaduoi=TrangThaiCua::BINHTHUONG;
    //     }
    //     else{
    //       ui8_trangthai_cuatren=TrangThaiCua::BINHTHUONG;
    //     }
        
    //     ui8_resetbien=0;
    //   }
    // }

    // else if(ui8_controlcuaduoi==1)
    // {
    //   dongmocua_control(mocua_duoi,cambien_cuaduoi,ui8_trangthai_cuaduoi);
    // }
    
    xSemaphoreGive(batton);
		vTaskDelay(1);

  }
}

void Task2core(void *paramter)
{ 
  for(;;)
  {
    xSemaphoreTake(batton, portMAX_DELAY);
    rs485();
    xSemaphoreGive(batton);
		vTaskDelay(1);
  }
}

// /*---------------RUN MOTOR SAI MODULE BTS7960------------------*/
// void runMotor(DONGCO &motorController, uint8_t tocdo, uint8_t chieuquay, uint16_t &lastspeed) {
//   register uint16_t u16_setspeed = tocdo * 256 / 110;
//   if (u16_setspeed > 255) u16_setspeed = 255;
//   else if (u16_setspeed < 0) u16_setspeed = 0;
//   if (u16_setspeed > lastspeed) {
//     for (int speed = lastspeed ; speed <= u16_setspeed; speed += 10)
//     {
//       if (chieuquay == 0) {
//         motorController.TurnLeft(speed);
//         delay(10);
//       }
//       else if (chieuquay == 1) {
//         motorController.TurnRight(speed);
//         delay(10);
//       }
//     }
//   }
//   else if (u16_setspeed < lastspeed) {
//     for (int speed = lastspeed ; speed >= u16_setspeed; speed -= 10)
//     {
//       if (chieuquay == 0) {
//         motorController.TurnLeft(speed);
//         delay(10);
//       }
//       else if (chieuquay == 1) {
//         motorController.TurnRight(speed);
//         delay(10);
//       }
//     }
//   }
//   lastspeed = u16_setspeed;
// }

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
          if(input=="kichrole")
          {
            Serial_Debug.printf("kich mo cua\n");
            ui8_yeucau=mocua;
            Serial_Debug.printf("trang thai cua duoi: %d\n", ui8_yeucau);
            ui8_controlcua=1;
            ui8_vitricua=vitricua::CUADUOI;
            ui8_controlcuaduoi=1;
          }

          if(input=="kichrole1")
          {
            Serial_Debug.printf("kich dong cua\n");
            ui8_yeucau=dongcua;
            Serial_Debug.printf("trang thai cua tren: %d\n", ui8_yeucau);
            ui8_controlcua=1;
            ui8_vitricua=vitricua::CUATREN;
            ui8_dongcuatren=1;
          }


          if(input=="kichmocuatren")
          {
            Serial_Debug.printf("kich mo cua\n");
            ui8_yeucau=mocua;
            Serial_Debug.printf("trang thai cua tren: %d\n", ui8_yeucau);
            ui8_controlcua=1;
            ui8_vitricua=vitricua::CUATREN;
            ui8_controlcuatren=1;
          }

          if(input=="reset")
          {
            ESP.restart();
          }

          if(input=="kichdongcuaduoi")
          {
             Serial_Debug.printf("kich mo cua\n");
            ui8_yeucau=dongcua;
            Serial_Debug.printf("trang thai cua duoi: %d\n", ui8_yeucau);
            ui8_controlcua=1;
            ui8_vitricua=vitricua::CUADUOI;
            ui8_dongcuaduoi=1;
          }
        
        }
        break;
        default: break;
        }
  }
}

// /*-----------------DUNG MOTOR DUNG MODULE DONGCO--------------------*/
// void stopMotor(DONGCO &motorController, uint16_t &lastspeed)
// {
//   motorController.Stop();
//   lastspeed = 0;
// }

// /*********************RUN MOTOR CỬA NÂNG LÊN*************************/
// void run_Motor_Cua_Nanglen()
// {
//   runMotor(Motor_cua, motor_cua.ui16_tocdo, chieuquay_cuachai_nanglen, motor_cua.u16_lastspeed);
//   motor_cua.ui8_tatmotor = 1;
//   motor_cua.ui32_timeout_dongmo_motor = millis() + motor_cua.ui32_timeout_cho_motor; 
//   motor_cua.ui8_trangthaimotor = TrangThaiMotorDC::NANGLEN;
// 	Serial_Debug.printf("NANG MOTOR CUA\n");
// }


// /*********************RUN MOTOR CỬA HẠ XUỐNG*************************/
// void run_Motor_Cua_Haxuong()
// {
//   runMotor(Motor_cua, motor_cua.ui16_tocdo, chieuquay_cuachai_haxuong, motor_cua.u16_lastspeed);
//   motor_cua.ui8_tatmotor = 1;
//   motor_cua.ui32_timeout_dongmo_motor = millis() + motor_cua.ui32_timeout_cho_motor; //TẮT MOTOR SAU 10S
//   motor_cua.ui8_trangthaimotor = TrangThaiMotorDC::HAXUONG;
// 	Serial_Debug.printf("HA MOTOR CUA\n");
// }

// /********************STOP MOTOR CỬA**********************/
// void stop_Motor_Cua()
// {
//   stopMotor(Motor_cua, motor_cua.u16_lastspeed);
// 	motor_cua.ui8_tatmotor = 0;
// 	motor_cua.ui8_trangthaimotor = TrangThaiMotorDC::DUNG;
//   Serial_Debug.printf("DUNG MOTOR CUA\n");
// }

// void run_Motor_bangtai(uint8_t chieuquay)
// {
//   runMotor(Motor_bangtai,motor_bangtai.ui16_tocdo,chieuquay,motor_bangtai.u16_lastspeed);
//   if(chieuquay==0)
//   {
//     motor_bangtai.ui8_trangthaimotor = TrangThaiMotorDC::VAO;  
//   }
//   if(chieuquay==1)
//   {
//     motor_bangtai.ui8_trangthaimotor = TrangThaiMotorDC::RA;
//   }
//   motor_bangtai.ui8_tatmotor=1;
//   Serial_Debug.printf("chay motor bang tai\n");
// }

// void stop_Motor_bangtai()
// {
//   stopMotor(Motor_bangtai,motor_bangtai.u16_lastspeed);
//   motor_bangtai.ui8_tatmotor=0;
//   motor_bangtai.ui8_trangthaimotor=TrangThaiMotorDC::DUNG;
//   Serial_Debug.printf("dung motor bang tai\n");
// }

// void run_Motor_nangha()
// {
//   runMotor(Motor_nangha,motor_nangha.ui16_tocdo,chieu_nang_ha,motor_nangha.u16_lastspeed);
// 	motor_nangha.ui8_tatmotor=1;
//   motor_nangha.ui8_trangthaimotor=TrangThaiMotorDC::DANGNANG;
// }

// void stop_Motor_nangha()
// {
// 	stopMotor(Motor_nangha,motor_nangha.u16_lastspeed);
// 	motor_nangha.ui8_trangthaimotor=TrangThaiMotorDC::DUNG;
// 	motor_nangha.ui8_tatmotor=0;
// }

// /*********************RUN MOTOR XOAY CHAI*************************/
// void run_Motor_Xoaychai()
// {
//     runMotor(Motor_xoaychai, motor_xoaychai.ui16_tocdo, chieuquay_xoaychai, motor_xoaychai.u16_lastspeed);
// }

// void stop_Motor_xoaychai()
// {
// 	stopMotor(Motor_xoaychai, motor_xoaychai.u16_lastspeed);
// 	motor_xoaychai.ui8_tatmotor = 0;
// }

// void KIEMTRATATDONGCO()
// {
// 	if(motor_cua.ui8_tatmotor==1)
// 	{
// 		if(motor_cua.ui32_timeout_dongmo_motor <= millis()){
// 			stop_Motor_Cua();
// 			Serial_Debug.println("DA TAT DONG CO CUA");
			
// 		}
// 	}
// }

// /**********************KIỂM TRA CỬA KẸT*********************/
// void kiemtracuaket()
// {
//     int tam1 = doccambien(cambien_cua1,20);
//     int tam2 = doccambien(cambien_cua2,20);
//     int tam3 = doccambien(cambien_cua3,20);
// 	  int tam4 = doccambien(cambien_cua4,20);
//     register int tong = tam1 + tam2 + tam3 + tam4;
//     if(tong>0 && ui8_trangthaicua == TrangThaiMotorDC::KHONGKET)
//     { //NẾU CỬA ĐANG BỊ KẸT
// 			ui8_trangthaicua = TrangThaiMotorDC::KET;
// 			Serial_Debug.printf("CUA KET\n");   
// 			//STOP MOTOR CỬA VÀ HẠ MOTOR XUỐNG
// 			stop_Motor_Cua();
// 			run_Motor_Cua_Nanglen();
//     }
	
//     else if(tong == 0 && ui8_trangthaicua == TrangThaiMotorDC::KET){ //NẾU KHÔNG KẸT 
		
// 		Serial_Debug.printf("CUA KHONG KET\n");
// 		ui8_trangthaicua = TrangThaiMotorDC::KHONGKET;

// 		//NẾU ĐÃ KẾT THÚC QUÁ TRÌNH THÌ ĐÓNG CỬA
// 		// if(ui8_startprocess == 0) {
// 		if(ui8_kethuc == 1) {
//       Serial_Debug.printf("CUA DANG XUONG\n");
// 			stop_Motor_Cua();
// 			run_Motor_Cua_Haxuong();
// 		}
//     }
// }

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

int kich_mocua(uint8_t vitri, uint8_t cambien)
{
  static uint8_t chophepnhavat = 0;
	static uint8_t dachaymotor = 0;
	static uint8_t dungdongco = 0; 
	static uint8_t khacvitrihome = 0;

  if(dachaymotor==0)
  {
    digitalWrite(vitri,1);
    ui32_timeout_mocua=millis()+3000;
    Serial_Debug.print("da kich mo cua");
    dachaymotor=1;

    int tam=doccambien(cambien,100);

    if(tam==0)
    {
      khacvitrihome = 1;
    }
  }

  if(ui32_timeout_mocua<=millis())
  {
    int tam=doccambien(cambien,100);
    if(tam==0)
    {
      Serial_Debug.printf("da mo cua thanh cong");
      chophepnhavat = 0;
				dachaymotor = 0;
				dungdongco = 0; 
				khacvitrihome = 0;
        if(vitri==mocua_tren)
        {
          ui8_trangthai_cuatren=TrangThaiCua::MOCUA;
        }
        
        else
        {
          ui8_trangthai_cuaduoi=TrangThaiCua::MOCUA;
        }
      return 1;
    }

    else 
    {
      Serial_Debug.printf("cua van dang dong");
      chophepnhavat = 0;
				dachaymotor = 0;
				dungdongco = 0; 
				khacvitrihome = 0;
         if(vitri==mocua_tren)
        {
          ui8_trangthai_cuatren=TrangThaiCua::DONGCUA;
        }
        
        else
        {
          ui8_trangthai_cuaduoi=TrangThaiCua::DONGCUA;
        }

        // digitalWrite(vitri,0);
      return 0;
    }
  }
  return -1;
}

int dong_mocua(uint8_t vitri, uint8_t cambien)
{
  static uint8_t chophepnhavat = 0;
	static uint8_t dachaymotor = 0;
	static uint8_t dungdongco = 0; 
	static uint8_t khacvitrihome = 0;

  if(dachaymotor==0)
  {
    digitalWrite(vitri,1);
    ui32_timeout_mocua=millis()+3000;
    Serial_Debug.print("da kich mo cua");
    dachaymotor=1;

    int tam=doccambien(cambien,100);

    if(tam==1)
    {
      khacvitrihome = 1;
    }
  }

  if(ui32_timeout_mocua<=millis())
  {
    int tam=doccambien(cambien,100);
    if(tam==1)
    {
      Serial_Debug.printf("da dong cua thanh cong");
      chophepnhavat = 0;
				dachaymotor = 0;
				dungdongco = 0; 
				khacvitrihome = 0;

        if(vitri==mocua_tren)
        {
          ui8_trangthai_cuatren=TrangThaiCua::DONGCUA;
        }
        
        else
        {
          ui8_trangthai_cuaduoi=TrangThaiCua::DONGCUA;
        }
      return 1;
    }

    else 
    {
      Serial_Debug.printf("cua van dang mo");
      chophepnhavat = 0;
				dachaymotor = 0;
				dungdongco = 0; 
				khacvitrihome = 0;
         if(vitri==mocua_tren)
        {
          ui8_trangthai_cuatren=TrangThaiCua::MOCUA;
        }
        
        else
        {
          ui8_trangthai_cuaduoi=TrangThaiCua::MOCUA;
        }
        digitalWrite(vitri,0);
      return 0;
    }
  }
  return -1;
}


// void kich_mocua(uint8_t vitri, uint8_t cambien)
// {
//   int khoacua_1 = 0;
// 	int khoacua_2 = 0;
// 	unsigned long u32wait;
// 	unsigned long u32checmocua;
// 	unsigned long u32current;

//   digitalWrite(vitri,1);
//   u32wait = millis()+20000;
// 	u32checmocua = millis()+10000;
// 	u32current=millis();
// 	// ui32_timeout_nhaqua = millis()+1000;
// 	// ui32_timeout_loinhaqua = millis()+1000;  

//   while(u32current<u32wait)
//   {
//       u32current = millis();
//       khoacua_1=doccambien(cambien,100);
//       // khoacua_2=doccambien(cambien_cuaduoi,100);

//       if(khoacua_1==0)
//       {
//         // if(khoacua_1==1)
//         // {
//         //   Serial_Debug.printf("cua tren dang mo\n");  
//         //   ui8_trangthai_cuatren=TrangThaiCua::MOCUA;
//         // }

//         // if(khoacua_2==1)
//         // {
//         //   Serial_Debug.printf("cua duoi dang mo\n");
//         //   ui8_trangthai_cuaduoi=TrangThaiCua::MOCUA;
//         // }
//         // ui8_trangthai_cuatren=TrangThaiCua::MOCUA;
//         if(vitri==mocua_tren)
//         {
//           ui8_trangthai_cuatren=TrangThaiCua::MOCUA;
//         }
        
//         else
//         {
//           ui8_trangthai_cuaduoi=TrangThaiCua::MOCUA;
//         }
//         Serial_Debug.printf("cua dang mo\n");
//         digitalWrite(vitri,0);
//         u32checmocua = millis()+5000;
//         // ui32_timeout_nhaqua = millis()+1000;
//         break;
//       }

//       if(u32current>u32checmocua)
//       {
//         // if(khoacua_1==0)
//         // {
//         //   Serial_Debug.printf("cua tren van dang dong\n");  
//         //   ui8_trangthai_cuatren=TrangThaiCua::DONGCUA;
//         // }

//         // if(khoacua_2==0)
//         // {
//         //   Serial_Debug.printf("cua duoi van dang dong\n");  
//         //   ui8_trangthai_cuaduoi=TrangThaiCua::DONGCUA;
//         // }
//         // ui8_trangthai_cuatren=TrangThaiCua::DONGCUA;
//          if(vitri==mocua_tren)
//         {
//           ui8_trangthai_cuatren=TrangThaiCua::DONGCUA;
//         }
        
//         else
//         {
//           ui8_trangthai_cuaduoi=TrangThaiCua::DONGCUA;
//         }
//         digitalWrite(vitri,0);
//         Serial_Debug.printf("cua van dang dong\n");
//         break;
//       }
//   }
// }

// void kich_dongcua(uint8_t vitri, uint8_t cambien)
// {
//   int khoacua_1 = 0;
// 	int khoacua_2 = 0;
// 	unsigned long u32wait;
// 	unsigned long u32checdongcua;
// 	unsigned long u32current;

//   digitalWrite(vitri,1);
//   u32wait = millis()+20000;
// 	u32checdongcua = millis()+10000;
// 	u32current=millis();

//   while(u32current<u32wait)
//   {
//       u32current = millis();
//       khoacua_1=doccambien(cambien,100);

//       if(khoacua_1==1)
//       {
//         // Serial_Debug.printf("cua dang dong\n");  
//         if(vitri==mocua_tren)
//         {
//           ui8_trangthai_cuatren=TrangThaiCua::DONGCUA;
//         }
        
//         else
//         {
//           ui8_trangthai_cuaduoi=TrangThaiCua::DONGCUA;
//         }
//         Serial_Debug.printf("cua da dong\n");
//         digitalWrite(vitri,0);
//         u32checdongcua = millis()+5000;
//         // ui32_timeout_nhaqua = millis()+1000;
//         break;
//       }

//       if(u32current>u32checdongcua)
//       {
//         // Serial_Debug.printf("cua tren van dang mo\n");  
//         if(vitri==mocua_tren)
//         {
//           ui8_trangthai_cuatren=TrangThaiCua::MOCUA;
//         }
        
//         else
//         {
//           ui8_trangthai_cuaduoi=TrangThaiCua::MOCUA;
//         }
//         digitalWrite(vitri,0);
//         Serial_Debug.printf("cua van dang mo\n");
//         break;
//       }
//   }
// }

void processRS485()
{
    if (RS485.incorrectCRC == true)
    {
        Serial_Debug.println("Incorrect CRC");
        RS485.incorrectCRC = false;
        return;
    }
    if (RS485.isReceived == true)
    {
//        Serial_Debug.print("\nReceive: ");
        // ui8_chophanhoi=0;
        // for (uint8_t i = 0; i <= buffer_receive_slave[0]; i++)
        // {
        //   Serial_Debug.print(buffer_receive_slave[i], HEX);
        //   Serial_Debug.print(" ");
        // }
        RS485.isReceived = false;
        memcpy(order_command, (buffer_receive + 2), 2);
        if (order_command[0] == last_order_command[0] && order_command[1] == last_order_command[1])
        {
            // return;
        }
        memcpy(last_order_command, order_command, 2);

        uint8_t size = buffer_receive[0] - 4;
        uint8_t function = buffer_receive[1];
        uint8_t bytecount = buffer_receive[4];

        uint8_t data[buffer_receive[0] - 4];
        memcpy(data, buffer_receive + 5, buffer_receive[0] - 4);

        switch (function)
        {

        case codeMaster::check_error:
        {
            if (buffer_receive[0] - 4 != 1 || // Check size
                buffer_receive[5] != 0x00)
            {
                Serial_Debug.println(" Incorrect command check error");
                break;
            }
            else
            {
                ui8_phanhoi_TrangthaiLoi = 1;
                break;
            }
        }

        case codeMaster::DONGMOCUA:
        {
          if (buffer_receive[0] - 4 != 2 || // Check size
                (buffer_receive[5] != vitricua::CUATREN &&
                buffer_receive[5] != vitricua::CUADUOI))
            {
                Serial_Debug.println(" Incorrect command check error");
                break;
            }
          
          else
          {
            ui8_checkmocua=1;
            if(buffer_receive[5]==vitricua::CUATREN)
            {
              Serial_Debug.printf("lenh cua tren\n");
              ui8_yeucau=buffer_receive[6];
              Serial_Debug.printf("trang thai cua tren: %d\n", ui8_yeucau);
              if(ui8_yeucau==mocua)
              {
                ui8_controlcuatren=1;
              }

              else
              {
                ui8_dongcuatren=1;
              }
              ui8_controlcua=1;
              ui8_vitricua=vitricua::CUATREN;
              // ui8_controlcuatren=1;
              break;
            }
            else if(buffer_receive[5]==vitricua::CUADUOI)
            {
              Serial_Debug.printf("\nlenh cua duoi\n");
              ui8_yeucau=buffer_receive[6];
              Serial_Debug.printf("trang thai cua duoi: %d\n", ui8_yeucau);
              if(ui8_yeucau==mocua)
              {
                ui8_controlcuaduoi=1;
              }

              else
              {
                ui8_dongcuaduoi=1;
              }
              ui8_controlcua=1;
              ui8_vitricua=vitricua::CUADUOI;
              
              break;
            }
            // xulyyeucau_mocua();
            
          }
        }

        case codeMaster::check_firmware:
        {
            if (size != 1 || // Check size
                data[0] != 0x00)
            {
                Serial_Debug.println("Incorrect command get firmware");
                break;
            }
            else
            {
                Serial_Debug.println("correct command get firmware");
                ui8_phanhoi_firmware = true;
                break;
            }
        }

        // case codeMaster::DONGCUA:
        // {
        //   if (buffer_receive[0] - 4 != 1 || // Check size
        //         (buffer_receive[5] != vitricua::CUATREN &&
        //         buffer_receive[5] != vitricua::CUADUOI))
        //     {
        //         Serial_Debug.println(" Incorrect command check error");
        //         break;
        //     }
          
        //   else
        //   {
        //     ui8_checkdongcua=1;
        //     if(buffer_receive[5]==vitricua::CUATREN)
        //     {
        //       Serial_Debug.printf("dong cua tren\n");
        //       ui8_dongcuatren=buffer_receive[5];
        //     }
        //     else if(buffer_receive[5]==vitricua::CUADUOI)
        //     {
        //       Serial_Debug.printf("dong cua duoi\n");
        //       ui8_dongcuaduoi=buffer_receive[5];
        //     }
        //     // xulyyeucau_dongcua();
        //     break;
        //   }
        // }


        default:
            Serial_Debug.println(" Incorrect command all");
            break;
        }
    }
}


void rs485()
{
    if (ui8_update_firmware == 1)
    {
      receiveFirmware();
    }

    else
    {
      if (ui8_phanhoi_TrangthaiLoi == 1 || ui8_phanhoi_firmware == 1)
            ui8_trangthaiRS485 = trangthaiRS485::DANGGUI;
        else
            ui8_trangthaiRS485 = trangthaiRS485::DANGNHAN; 

    if (ui8_trangthaiRS485 == trangthaiRS485::DANGNHAN)
        {
            // Serial_Debug.println("\ndang nhan");
            RS485.receive(buffer_receive);
            if (RS485.isReceived)
            {
                // Serial_Debug.print("\nReceive: ");
                // for (uint8_t i = 0; i <= buffer_receive[0]; i++)
                // {
                //     Serial_Debug.print(buffer_receive[i], HEX);
                //     Serial_Debug.print(" ");
                // }
            }

            processRS485();
        }

        // Xử lý khi ở trạng thái gửi dữ liệu đến master
        else if (ui8_trangthaiRS485 == trangthaiRS485::DANGGUI)
        {
            // Serial_Debug.println("\ndang gui");
            if (ui8_phanhoi_TrangthaiLoi == 1)
            {
                phanhoi_trangthailoi();
                ui8_phanhoi_TrangthaiLoi = 0;
            }

            if (ui8_phanhoi_firmware == 1)
            {
                phanhoi_firmware();
                ui8_phanhoi_firmware = 0;
                Serial_Debug.println("phanhoi firmware");
            }
        }
    }
   
}

int dongmocua_control(uint8_t vitri,uint8_t cambien,uint8_t trangthai)
{
  static int doctrangthaicua=0;
  
  // uint8_t vitri;
  // uint8_t cambien;
  // uint8_t trangthai;
  // int tam=doccambien(cambien_cuatren,100);
  // int tam1=doccambien(cambien_cuaduoi,100);
  // if(ui8_vitricua==vitricua::CUATREN)
  // {
  //   vitri=0;
  //   cambien=CB[0];
  //   trangthai=ui8_trangthai_cuatren;
  //   ui8_vitricua=0;
  // }

  // if(ui8_vitricua==vitricua::CUADUOI)
  // {
  //   vitri=1;
  //   cambien=CB[1];
  //    trangthai=ui8_trangthai_cuaduoi;
  //   ui8_vitricua=0;
  // }


      if(ui8_controlcua==1)
      {
        // Serial_Debug.println("kiem tra cua may");
        int tam=doccambien(cambien,100);
        if(tam==1 && ui8_yeucau==mocua)
        {
          digitalWrite(vitri,1);
          Serial_Debug.printf("phat hien cua dang dong\n");
          ui32_timeout_mocua=millis()+60000;
          doctrangthaicua=1;
          ui8_controlcua=0;
        }

        if(tam==0 && ui8_yeucau==mocua)
        {
          Serial_Debug.printf("cua mo san roi\n");
          ui8_controlcua=0;
          ui8_yeucau=binhthuong;
          // ui8_controlcuatren=0;
          // ui8_controlcuaduoi=0;
          return 1;
          
        }
      }

      if(doctrangthaicua==1)
      {
        // Serial_Debug.printf("kiem tra xem cua mo chua\n");
        int tam=doccambien(cambien,100); 
        if(tam==0)
        {
          // trangthai=TrangThaiCua::MOCUA;
          ui32_timeout_kiemtraloi=millis()+5000;
          Serial_Debug.printf("cua mo roi\n");
          digitalWrite(vitri,0);
          doctrangthaicua=0;
          ui8_yeucau=binhthuong;
          return 1;
        }
        if(ui32_timeout_mocua<=millis())
        {
          // if(vitri==0)
          // {
          //   ui8_trangthai_cuatren=TrangThaiCua::DONGCUA;
              // }

              // else
              // {
              //   ui8_trangthai_cuaduoi=TrangThaiCua::DONGCUA;
              // }
              // trangthai=TrangThaiCua::DONGCUA;
              Serial_Debug.printf("cua van dang dong\n");
              digitalWrite(vitri,0);
              doctrangthaicua=0;
              ui8_yeucau=binhthuong;
              // ui8_controlcuatren=0;
              // ui8_controlcuaduoi=0;
              return 0;
        }
        
      }
    
  return -1;
}

int dongcua_control(uint8_t vitri,uint8_t cambien,uint8_t trangthai)
{
  static int dongcuane=0;
      // Serial_Debug.printf("dong cua ne\n");
      if(ui8_controlcua==1)
      {
        int tam=doccambien(cambien,100); 
        if(tam==0 && ui8_yeucau==dongcua)
        {
          digitalWrite(vitri,1);
          Serial_Debug.printf("phat hien cua dang mo\n");
          ui32_timeout_dongcua=millis()+60000;
          dongcuane=1;
          ui8_controlcua=0;
        }

        if(tam==1 && ui8_yeucau==dongcua)
        {
          Serial_Debug.printf("cua dong san roi\n");
          ui8_controlcua=0;
          ui8_yeucau=binhthuong;
          // ui8_controlcuatren=0;
          // ui8_controlcuaduoi=0;
          return 1;
        }
      }

      if(dongcuane==1)
      {
        // Serial_Debug.printf("kiem tra xem cua dong chua\n");
        int tam = doccambien(cambien,100);
        if(tam==1)
        {
          // if(vitri==0)
          // {
          //   ui8_trangthai_cuatren=TrangThaiCua::DONGCUA;
          // }

          // else
          // {
            // trangthai=TrangThaiCua::DONGCUA;
          // }
          Serial_Debug.printf("cua dong roi\n");
          digitalWrite(vitri,0);
          dongcuane=0;
          ui8_yeucau=binhthuong;
          // ui8_controlcuatren=0;
          // ui8_controlcuaduoi=0;
          return 1;
        }
        if(ui32_timeout_dongcua<=millis())
        {
          // if(vitri==0)
          // {
          //   ui8_trangthai_cuatren=TrangThaiCua::MOCUA;
          // }

          // else
          // {
          //   ui8_trangthai_cuaduoi=TrangThaiCua::MOCUA;
          // }
          // trangthai=TrangThaiCua::MOCUA;
          ui32_timeout_kiemtraloi=millis()+5000;
          Serial_Debug.printf("cua van dang mo\n");
          digitalWrite(vitri,0);
          dongcuane=0;
          ui8_yeucau=binhthuong;
          return 0;
        }    
      }

  return -1;
    
}

void kiemtracua()
{
  int tam=doccambien(cambien_cuatren,20);
  int tam1=doccambien(cambien_cuaduoi,20);

  if(tam==0 && ui8_trangthai_cuatren==TrangThaiCua::DONGCUA)
  {
    Serial_Debug.printf("cua tren mo\n");
    ui8_trangthai_cuatren=TrangThaiCua::MOCUA;
  }

  else if(tam==1 && ui8_trangthai_cuatren==TrangThaiCua::MOCUA)
  {
    Serial_Debug.printf("cua tren dong\n");
    ui8_trangthai_cuatren=TrangThaiCua::DONGCUA;
  }


  if(tam1==0 && ui8_trangthai_cuaduoi==TrangThaiCua::DONGCUA)
  {
    Serial_Debug.printf("cua duoi mo\n");
    ui8_trangthai_cuaduoi=TrangThaiCua::MOCUA;
  }

  else if(tam1==1 && ui8_trangthai_cuaduoi==TrangThaiCua::MOCUA)
  {
    Serial_Debug.printf("cua duoi dong\n");
    ui8_trangthai_cuaduoi=TrangThaiCua::DONGCUA;
  }
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


// Nhận firmware qua UART và cập nhật
void receiveFirmware()
{

    uint8_t buffer[BUFFER_SIZE + 1024]; // DATA_SIZE + HEADER_SIZE
    size_t totalReceived = 0;
    bool updateStarted = false;
    uint32_t expectedSize = 0;
    bool firmwareInvalid = false;
    bool firmwareWrongSize = false;
    bool timeoutReceive = false;

    bool receivedEOP = false;
    bool receivedEOF = false;
    uint8_t counter = 0;

    /* Nhận thông tin tổng số byte */
    Serial1.readBytes((uint8_t *)&expectedSize, sizeof(expectedSize));
    Serial_Debug.printf("Expected firmware size: %u bytes\n", expectedSize);

    // Bắt đầu cập nhật
    if (!updateStarted)
    {
        if (expectedSize == 0)
        {
            Serial_Debug.println(" No firmware to update.");
            ui8_update_firmware = false;
            return;
        }
        if (!Update.begin(expectedSize))
        {
            Serial_Debug.println("Not enough space for firmware update.");
            ui8_update_firmware = false;
            return;
        }
        updateStarted = true;
        Serial_Debug.println("Firmware update started...");
    }
    uint32_t timer0 = millis();
    uint32_t timer1 = millis();

    while (true)
    {
        /* Kiểm tra thời gian không có dữ liệu để break while */
        if (millis() - timer1 > 5000)
        {
            timeoutReceive = true;
            break;
        }
        /* Nếu có dữ liệu */
        if (Serial1.available())
        {
            timer1 = millis();
            int len = Serial1.readBytes(buffer, BUFFER_SIZE);

            /* Kiểm tra checksum */
            uint8_t checksum = calculateChecksum(buffer + 3, uint16_t(len - HEADER_SIZE));

            Serial_Debug.printf("Checksum: %02X\n", checksum);
            if (buffer[0] == 'B' && buffer[1] == 'O' && buffer[2] == 'P')
            {
                Serial_Debug.println("Received Begin Page Maker ");
            }
            if (buffer[len - 3] == 'E' && buffer[len - 2] == 'O' && buffer[len - 1] == 'P')
            {
                Serial_Debug.println("Received End Page Maker ");
            }

            /*Kiểm tra Begin và End Maker*/
            if (buffer[0] == 'B' && buffer[1] == 'O' && buffer[2] == 'P' &&
                buffer[len - 3] == 'E' && buffer[len - 2] == 'O' && buffer[len - 1] == 'P')
            {
                receivedEOP = true;
            }
            else if (buffer[0] == 'B' && buffer[1] == 'O' && buffer[2] == 'P' &&
                     buffer[len - 3] == 'E' && buffer[len - 2] == 'O' && buffer[len - 1] == 'F')
            {
                receivedEOF = true;
            }

            /*Nếu nhận được dữ liệu hợp lệ*/
            if ((receivedEOP || receivedEOF) &&
                checksum == buffer[len - 4])
            {
                receivedEOP = false; /* Reset EOP để nhận trong while tiếp theo , EOF thì không không reset để ngắt while*/
                counter = 0;
                /*Ghi dữ liệu vào Update */
                size_t written = Update.write(buffer + 3, len - HEADER_SIZE);

                /*Kiểm tra ghi dữ liệu thành công*/
                if (written == len - HEADER_SIZE) /*Nếu thành công thì gửi phản hồi về*/
                {
                    // digitalWrite(RS485_DE_PIN, HIGH);
                    uint8_t code[3] = {'R', 'O', 'K'};
                    Serial1.write(code, sizeof(code));
                    Serial1.flush();
                    // digitalWrite(RS485_DE_PIN, LOW);
                    Serial_Debug.println("Sent respond OK");
                }
                else /* Nếu không thành công thì không phản hồi*/
                {
                    Serial_Debug.println("Error writing firmware data!");
                    break;
                    firmwareInvalid = true;
                }
            }
            else if (counter <= 3) /* Nếu dữ liệu không hợp lệ*/
            {
                Serial_Debug.printf("Received error %d bytes \n", len - HEADER_SIZE);
                counter++;
                // digitalWrite(RS485_DE_PIN, HIGH);
                uint8_t code[3] = {'E', 'R', 'R'};
                Serial1.write(code, sizeof(code));
                Serial1.flush();
                // digitalWrite(RS485_DE_PIN, LOW);
                Serial_Debug.println("Request resend -------------------------------------------------------------------------------------------------------------");
                continue;
            }
            else if (counter > 3)
            {
                firmwareInvalid = true;
                break;
            }

            totalReceived += len - HEADER_SIZE;
            Serial_Debug.printf("Received %d bytes|| Total received: %d \n", len - HEADER_SIZE, totalReceived);

            /*Kiểm tra khi nhận tất cả dữ liệu thì break while*/
            if (receivedEOF)
            {
                /* Nếu không nhận được đúng size như ban đầu gửi thì báo lỗi*/
                if (totalReceived != expectedSize)
                    firmwareWrongSize = true;

                Serial_Debug.println("Received End File Maker ");
                break;
            }

            Serial_Debug.printf("Time: %d ms\n", millis() - timer1);
        }
    }

    Serial_Debug.printf("Total time: %d ms\n", millis() - timer0);

    /*Kiểm tra các lỗi để không tiến hành cập nhật*/
    if (timeoutReceive || firmwareInvalid || firmwareWrongSize)
    {
        Serial_Debug.println("Firmware update failed.");
        Serial_Debug.printf("timeoutReceive: %d\n", timeoutReceive);
        Serial_Debug.printf("firmwareInvalid: %d\n", firmwareInvalid);
        Serial_Debug.printf("firmwareWrongSize: %d\n", firmwareWrongSize);
        Update.abort();
    }
    /*Tiến hành cập nhật và khởi động lại*/
    else if (updateStarted)
    {
        Serial_Debug.println("Firmware transfer completed.");
        Update.end();
        Serial_Debug.println("Firmware update completed! Restarting...");
        ESP.restart();
        Serial_Debug.println("Error");
    }

    ui8_update_firmware = false;
}

uint8_t calculateChecksum(const uint8_t *data, uint16_t length)
{
    if (data == NULL || length == 0) // Kiểm tra con trỏ NULL hoặc độ dài bằng 0
    {
        return 0; // Trả về 0 để tránh lỗi
    }

    uint8_t checksum = 0;
    for (int i = 0; i < length; i++)
    {
        checksum += data[i]; // Tổng tất cả các byte
    }
    return checksum; // Trả về checksum (mod 256 tự động do kiểu uint8_t)
}


