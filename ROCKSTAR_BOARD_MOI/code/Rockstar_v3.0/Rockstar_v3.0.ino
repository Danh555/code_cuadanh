#include "LIB/CRC16.h"
#include "LIB/GlobalVariable.h"
#include <EEPROM.h>
#include "LIB/MOTOR_THAN_ALTA.h"
#include "LIB/SensorVariable.h"
#include <Wire.h>
#include <Adafruit_PCF8575.h>
#include "LIB/PROTOCOL.h"

// #define DEBUG_EEPROM

#define PHANHOI(biendieukien, status, function, guilenh)\
  if(biendieukien != 0)\
  {\
    ui8_status = status;\
    ui8_function = function;\
    guilenh;\
    biendieukien = 0;\
    return;\
  }

MOTOR Motor_tamche(M2_PWM,M2_DIR);
MOTOR Motor_thangmay(M1_PWM,M1_DIR);
MOTOR Motor_khoatu(K_PWM,K_DIR);

Adafruit_PCF8575 MOTOR_COT;
Adafruit_PCF8575 MOTOR_HANG;


SemaphoreHandle_t batton = NULL;

/*----------------giao tiep PC-------------------*/
void readSerial();
void process();

/*-----------------DEBUG SERIAL--------------------*/
void uart_debugserial();

/*-----------------HAM CHAY MOTOR-------------------*/
void runMotor(MOTOR &motorController, uint8_t tocdo, uint8_t chieuquay, uint16_t &lastspeed);
void stopMotor(MOTOR &motorController, uint16_t &lastspeed);
void Motor_thangmay_len();
void Motor_thangmay_xuong(uint8_t ui8_tocdo);
void Motor_thangmay_dung();
void Motor_tamche_vao();
void Motor_tamche_ra();
void Motor_tamche_dung();
void Motor_khoacua_hocqua();
void Motor_mocua_hocqua();
void Motor_dungkich_hocqua();

/*------------------CAC HAM CON LAI-------------------*/
void reset_khaychuasanpham();
void nhaqua();
void drop_sku_process();
void guilenhlenPC();
void led_pixel();
void ledbaodo();
void tatled();

void setup() {
  // put your setup code here, to run once:
  #ifdef DEBUG
	uart_debug.begin(9600);
	delay(50);
	uart_PC.begin(9600,SERIAL_8N1,41,42);
	// uart_PC.begin(115200);
	delay(50);
  #else
    uart_debug.begin(9600,SERIAL_8N1,41,42);
    delay(50);
    uart_PC.begin(9600);
    delay(50);
  #endif	

  Wire.begin(9,8);
  EEPROM.begin(4096);
  delay(50);

  setup_cambien();
  setup_motor();

  if(!MOTOR_COT.begin(addr_ExIO_Motor_COL,&Wire))
  { 
      uart_debug.println("KHONG BAT DAU VOI MODULE MOTOR COT");
  }
  delay(50);

  if(!MOTOR_HANG.begin(addr_ExIO_Motor_ROL,&Wire))
  { 
      uart_debug.println("KHONG BAT DAU VOI MODULE MOTOR HANG");
  }

  MOTOR_COT.digitalWriteWord(0xffff);
  MOTOR_HANG.digitalWriteWord(0xffff);

  delay(50);
  

  // for(int i=0; i<16; i++)
  // {
  //     // MOTOR_COT.pinMode(i,OUTPUT);
  //     // MOTOR_HANG.pinMode(i,OUTPUT);
  //     MOTOR_COT.digitalWrite(i,HIGH);
  //     MOTOR_HANG.digitalWrite(i,HIGH);
  // }
  tatled();
  
  #ifdef DEBUG_EEPROM
		EEPROM.put(0, ui8_trangthai_thangmay);
		EEPROM.commit();
	#else
		EEPROM.get(0,ui8_trangthai_thangmay);

    if(ui8_trangthai_thangmay!=1)
    {
      reset_khaychuasanpham();
    }
  #endif

  uart_debug.println("*************************VENDING MARCHINE ALTA MEDIA************************************");
  uart_debug.println("-------------------------LENH DEBUG------------------------------------------------------");
  uart_debug.println("LENH NHA: HxxCxx vi du nha qua hang 1 cot 1 thi nhap lenh-> H01C01");
  uart_debug.println("LENH NHAP SO NGUOI VAO: Nxx");
  uart_debug.println("LENH XOA LOI: xoaloi");
  uart_debug.println("LENH DIEU KHIEN THANG MAY: THANG LEN->motor thang len; THANG XUONG->motor thang xuong; THANG DUNG->motor thang dung");
  uart_debug.println("LENH DIEU KHIEN TAM CHE: MO TAM CHE->tam che mo; DONG TAM CHE->tam che dong; DUNG TAM CHE->tam che dung");
  uart_debug.println("LENH DIEU KHIEN KHOA CUA HOC QUA: KHOA CUA HOC QUA->khoa hoc qua; MO CUA HOC QUA->mo hoc qua; DUNG KICH CUA HOC QUA->dung kich hoc qua");
  uart_debug.println("KIEM TRA DONG CO HANG COT(LUU Y CHI SAI KHI KHONG CAM TAI VAO):bat all");
  uart_debug.println("TAT DONG CO HANG COT KHI DANG TEST: tat_motor");
  uart_debug.println("-----------------------------------------------------------------------------------------");

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
  // put your main code here, to run repeatedly:
}

void readSerial() 
{
	if (uart_PC.available()) 
	{
		ui32_timechar = millis();
		while (uart_PC.available()) 
		{
			unsigned char inChar = (unsigned char)uart_PC.read();
			if (!ui2_start) 
			{
				ui2_BTreceive = true;
				ui2_start = true;
				ui8_Byteindex = 0;
			}
			if (ui2_start && (millis() - ui32_timechar) < 10) 
			{
				ui8_data_rec[ui8_Byteindex] = inChar;
				ui8_Byteindex++;
				ui32_timechar = millis();
			}
		}
	}
	if (ui2_BTreceive && ((millis() - ui32_timechar) > 50)) 
	{
		ui2_BTreceive = false;
		ui2_BTcomplete = true;
	}
	if (ui2_BTcomplete == true) 
	{
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
    	ui8_phanhoi_trunglenh = 1;
    	return;
    }
	switch (uc8_data[1])
	{

		case 0x07:
		{
			ui16_last_order_command = ui16_order_command;
			ui8_songuoi_tuphanmem=uc8_data[6];
      uart_debug.print("so nguoi quet QR la: ");
      uart_debug.println(ui8_songuoi_tuphanmem);
      ui8_phanhoi_dungFunction = 1;
     	break;
		}

    case 0x19:
    {
      if(ui8_trangthai_thangmay==1) 
      {
        uart_debug.println("THANG MAY DANG LOI");  
        ui8_phanhoi_huthangmay=1;
        ui8_trangthai_thangmay=1;
        ui8_phanhoi_dungFunction = 1;
        return;
      }
      uart_debug.println("NHAN LENH NHA QUA");
      ui16_last_order_command = ui16_order_command;
      ui8_status = 0x00;
			// ui8_sobyte = uc8_data[4];
			ui8_phanhoi_dungFunction = 1;
			uint8_t row_temp=0;
      uint8_t col_temp=0;
			row_temp = uc8_data[5];
			col_temp = uc8_data[6];
      row_=row_temp;
      column_=col_temp;
      ui8_check_data=1;
      // ui32_tiemout_rskhay=millis()+30000;
      // ui32_timecho_guilenh = millis();
      break;
    }

    case 0x09:
		{
           
		  ui16_last_order_command = ui16_order_command;
      ui8_status = 0x00;
      uart_debug.println("XOA LOI MAY");
      ui8_trangthai_thangmay=0;
      // ui8_trangthai_dongconhaqua=0;
      ui8_phanhoi_huthangmay=1;
      ui8_phanhoi_dungFunction = 1;
      EEPROM.put(0,ui8_trangthai_thangmay);
      EEPROM.commit();
      delay(1000);
      reset_khaychuasanpham();
      // ui32_timecho_guilenh = millis();
      break;
		}


    case 0x10:
		{
           
		  ui16_last_order_command = 0x0000;
		  ui8_function = 0x10;
		  ui8_status = 0x00;
      ui8_phanhoi_dungFunction=1;
      // ui32_timecho_guilenh = millis();
      break;
		}

    case 0x80:
    {
      ui16_last_order_command = ui16_order_command;
      // ui32_timecho_guilenh = millis();
      if(ui16_du_lieu_data_nhan == 0x0101)
      {
        Motor_thangmay_len();
        ui8_status = 0x00;
				ui8_phanhoi_dungFunction = 1;
      }

      else if(ui16_du_lieu_data_nhan==0x0102)
      {
        Motor_thangmay_xuong(30);
        ui8_status = 0x00;
				ui8_phanhoi_dungFunction = 1;
      }

      else if(ui16_du_lieu_data_nhan==0x0100)
      {
        Motor_thangmay_dung();
        ui8_status = 0x00;
				ui8_phanhoi_dungFunction = 1;
      }

      else if(ui16_du_lieu_data_nhan == 0x0201)
      {
        Motor_tamche_ra();
        ui8_status = 0x00;
				ui8_phanhoi_dungFunction = 1;
      }

      else if(ui16_du_lieu_data_nhan==0x0202)
      {
        Motor_tamche_vao();
        ui8_status = 0x00;
				ui8_phanhoi_dungFunction = 1;
      }

      else if(ui16_du_lieu_data_nhan==0x0200)
      {
        Motor_tamche_dung();
        ui8_status = 0x00;
				ui8_phanhoi_dungFunction = 1;
      }

      else if(ui16_du_lieu_data_nhan == 0x0301)
      {
        Motor_khoacua_hocqua();
        ui8_status = 0x00;
				ui8_phanhoi_dungFunction = 1;
      }

      else if(ui16_du_lieu_data_nhan==0x0302)
      {
        Motor_mocua_hocqua();
        ui8_status = 0x00;
				ui8_phanhoi_dungFunction = 1;
      }

      else if(ui16_du_lieu_data_nhan==0x0300)
      {
        Motor_dungkich_hocqua();
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


    default:
    {
      uart_PC.write(0x01);
      uart_PC.write(0x02);
      uart_PC.write(0x03);
      uart_PC.write(0x04);
      break;
    }

  }
}

void nhaqua()
{
  if(ui8_check_data==0) return;

  // row_=row_temp;      
  // column_=col_temp;
  if(check_col_row())
  {
    start_drop_sku=true;
    ui32_timeout_rsqua=millis()+100;
    // ui32_timeout_khoacua=millis()+100;
    ui8_solangoilenh++;
    uart_debug.print("Start drop\n\r");
    // row_=0;
    // column_=0;
    ui8_check_data=0;
  }

  else
      {
        uart_debug.print("khong ton tai san pham nay\n\r");
        erro_status=sku_over;
        // flash_led();
        // respone2PC();
      }
}



void docserial() 
{
    while (uart_debug.available()) 
    {
          // get the new byte:
          char inChar = (char)uart_debug.read();
      if (inChar == '\n') 
      {
        stringComplete = true;
      }
      else
      {
        inputString += inChar;
      }
    }
}

void uart_debugserial()
{
	docserial();
	if (stringComplete) 
	{
		serialProcess();

		inputString = "";
		stringComplete = false;
	}
}

void serialProcess()
{
  uart_debug.println("test bang debug");
  uart_debug.println(inputString);

  if(inputString=="xoaloi")
  {
    uart_debug.println("XOA LOI MAY");
    ui8_trangthai_thangmay=0;
    // ui8_trangthai_dongconhaqua=0;
    ui8_phanhoi_huthangmay=1;
    // ui8_phanhoi_dungFunction = 1;
    EEPROM.put(0,ui8_trangthai_thangmay);
    EEPROM.commit();
    delay(1000);
    reset_khaychuasanpham();
  }

  if(inputString=="tat_motor")
  {
    uart_debug.println("tat motor nha qua");
    disable_row_col();
  }

  if(inputString == "bat all")
  {
    for(int i=0; i<16; i++)
  {
      // MOTOR_COT.pinMode(i,OUTPUT);
      // MOTOR_HANG.pinMode(i,OUTPUT);
      MOTOR_COT.digitalWrite(i,LOW);
      MOTOR_HANG.digitalWrite(i,LOW);
  }
  }

  if(inputString=="motor thang len")
  {
    uart_debug.println("thang len");
    Motor_thangmay_len();
  }

  if(inputString=="motor thang dung")
  {
    uart_debug.println("thang dung");
    Motor_thangmay_dung();
  }

  if(inputString=="motor thang xuong")
  {
    uart_debug.println("thang xuong");
    // Motor_thangmay_xuong();
    reset_khaychuasanpham();
  }

  if(inputString=="tam che dong")
  {
    uart_debug.println("dong tam che");
    // Motor_thangmay_len();
    Motor_tamche_ra();
  }

  if(inputString=="tam che mo")
  {
    uart_debug.println("mo tam che");
    // Motor_thangmay_dung();
    Motor_tamche_vao();
  }

  if(inputString=="tam che dung")
  {
    uart_debug.println("dung kich tam che");
    // Motor_thangmay_xuong();
    // reset_khaychuasanpham();
    Motor_tamche_dung();
  }

  if(inputString=="khoa hoc qua")
  {
    uart_debug.println("khoa hoc qua");
    // Motor_thangmay_len();
    Motor_khoacua_hocqua();
  }

  if(inputString=="mo hoc qua")
  {
    uart_debug.println("mo hoc qua");
    // Motor_thangmay_dung();
    Motor_mocua_hocqua();
  }

  if(inputString=="dung kich hoc qua")
  {
    uart_debug.println("dung kich hoc qua");
    // Motor_thangmay_xuong();
    // reset_khaychuasanpham();
    Motor_dungkich_hocqua();
  }

  if(inputString =="tat all")
  {
   for(int i=0; i<16; i++)
  {
      // MOTOR_COT.pinMode(i,OUTPUT);
      // MOTOR_HANG.pinMode(i,OUTPUT);
      MOTOR_COT.digitalWrite(i,HIGH);
      MOTOR_HANG.digitalWrite(i,HIGH);
  } 
  }

  else
  {
    char stChar;
		char stChar1;
		String a;
		String danh;
		int b;
		int c;
		String StringValue = inputString;
		String StringValue1 = inputString;
		stChar = inputString.charAt(0);
    stChar1 = inputString.charAt(3);
    if(stChar == 'H' && stChar1 =='C') //K0C2
    {
      if(ui8_trangthai_thangmay==1) 
      {
        uart_debug.println("THANG MAY DANG LOI");  
        return;
      }
      a = StringValue.substring(1,3);
			danh = StringValue.substring(4,6);
			b = (a.toInt());
			c = danh.toInt();
      uart_debug.println(b);
      uart_debug.println(c);	
      row_=b;
      column_=c;
      ui8_check_data=1;
      // ui32_tiemout_rskhay=millis()+30000;
    }

    if(stChar == 'h' && stChar1 =='c')
    {
      a = StringValue.substring(1,3);
			danh = StringValue.substring(4,6);
			b = (a.toInt());
			c = danh.toInt();
      uart_debug.println(b);
      uart_debug.println(c);	
      // row_=b;
      // column_=c;
      uart_debug.printf("test dong co tai hang %d va cot %d\n", b, c);
      set_motor(ROW[b+1],COL[c-1]);
    }    

    if(stChar=='N')
    {
      a = StringValue.substring(1,2);
      b = (a.toInt());
      ui8_songuoi_tuphanmem=b;
      uart_debug.print("so nguoi tu phan mem la: ");
      uart_debug.println(ui8_songuoi_tuphanmem);
    }

  }

}

/*---------------RUN MOTOR SAI MODULE THAN VE------------------*/
void runMotor(MOTOR &motorController, uint8_t tocdo, uint8_t chieuquay, uint16_t &lastspeed) {
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

/*-----------------DUNG MOTOR DUNG MODULE DONGCO--------------------*/
void stopMotor(MOTOR &motorController, uint16_t &lastspeed)
{
  motorController.Stop();
  lastspeed = 0;
}

void Motor_thangmay_len()
{
	vitrithangmay=1;
	running=df_run_up;
	runMotor(Motor_thangmay,80,chieuquay_thangmay_len,ui16_lastspeed);
}

void Motor_thangmay_xuong(uint8_t ui8_tocdo)
{
	vitrithangmay=1;
	running=df_run_down;
	runMotor(Motor_thangmay,ui8_tocdo,chieuquay_thangmay_xuong,ui16_lastspeed);
}

void Motor_thangmay_dung()
{
	running=df_run_stop;
	stopMotor(Motor_thangmay,ui16_lastspeed);
}

void Motor_tamche_ra()
{
	runMotor(Motor_tamche,100,chieuquay_tamche_ra,ui16_lastspeed);
  // runMotor(Motor_tamche,50,chieuquay_tamche_ra,ui16_lastspeed);
}

void Motor_tamche_vao()
{
	runMotor(Motor_tamche,100,chieuquay_tamche_vao,ui16_lastspeed);
  // runMotor(Motor_tamche,50,chieuquay_tamche_vao,ui16_lastspeed);
}

void Motor_tamche_dung()
{
	stopMotor(Motor_tamche,ui16_lastspeed);
}

void Motor_khoacua_hocqua()
{
	runMotor(Motor_khoatu,50,khoa_dong,ui16_lastspeed);
}

void Motor_mocua_hocqua()
{
	runMotor(Motor_khoatu,50,khoa_mo,ui16_lastspeed);
}

void Motor_dungkich_hocqua()
{
	stopMotor(Motor_khoatu,ui16_lastspeed);
}

int doccambien(int pin_cambien,uint32_t timeout,uint8_t muc_kiemtra)
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
	if(reading == muc_kiemtra){
        return 1;
    }	
  }

  lastButtonState[index] = reading;
  return 0;
}

void reset_khaychuasanpham()
{
	#ifndef use_elavator_
		uart_debug.println("WARNING!!!!!!!!");
		uart_debug.println("Debug no use elavator @ line 35 "); 
    	return;
  	#endif

	uart_debug.println("RESET KHAY CHUA SAN PHAM\n\r");
	int count = 0;
	Motor_thangmay_xuong(70);
	int reading = doccambien(CTHT_THANG_GOC,20,0);
	int encoder_= digitalRead(E_CHA);

	int err_=0;
	int buttonState=0;
	unsigned long currentMillis = millis();
	unsigned long previousMillis = currentMillis;        // will store last time LED was updated
	unsigned long timer_checkmotor = currentMillis+5000; 
	int time_out_=0;
	int status=0;

	while(reading==0)
  	{           
    reading = doccambien(CTHT_THANG_GOC,20,0);
    if(reading==1)
    {
      Motor_thangmay_dung();
      ui32_timeout_nhaqua=millis()+10000;
      ui8_check_khoacua=1;
      vitrithangmay=0;
      uart_debug.println("limDown_pin Pause\n\r");
      uart_debug.print("count=");     
      uart_debug.println(count);
      ui8_check_doilenh=0;
      // delay(50); 
      break;
    }
	
    encoder_ = digitalRead(E_CHA);
    // uart_debug.println(encoder_);
    if (encoder_ != buttonState) {
      buttonState = encoder_;
      if (buttonState == HIGH) {
        
        count++;
        //reset counter
        timer_checkmotor=currentMillis+5000;//sua cho nay
        if (count>10000){//sua cho nay
          Motor_thangmay_dung();
          uart_debug.println("ERR: max limit encoder\n\r");
          uart_debug.print("count=");
          uart_debug.println(count);
          err_=1;
          break;
        }
        // if(count<=30)
        // {
        //   uart_debug.println("giam toc do");
        //   Motor_thangmay_dung();
        //   err_=0;
        //   break;
        // }
      }
      else{
        
      }
    //   digitalWrite(ledPin, buttonState);
    }
    //check timer out
    currentMillis = millis();
    if (currentMillis - previousMillis >= 1000)//sua day
    {
      time_out_++;
      previousMillis=currentMillis;
      uart_debug.print("Time:\t");
          uart_debug.println(time_out_);
      if(time_out_>15){
          uart_debug.println("Time out\n\r");
          err_=2;
        break;
      }     
      //check motor
      if (currentMillis > timer_checkmotor)
      {            
        uart_debug.println("Elevator motor not running down\n\r");
       
        erro_status=thangmay;
        err_=2;
        break;    
      }                         
      if(status)
      {
        status=0;
        ledon;
      }
      else 
      {
        status=1;
        ledoff;
      }     
    }   
	// flash_led_red();
  }
    Motor_thangmay_dung();
    uart_debug.print("Err code=");  
    uart_debug.println(err_);

    if(err_==2)
    { 
      uart_debug.println("gui lenh loi thang may");
       ui8_phanhoi_huthangmay=1;
        ui8_trangthai_thangmay=1;
        EEPROM.put(0,ui8_trangthai_thangmay);
        EEPROM.commit();
        err_=0;
    }

    if(ui8_trangthai_thangmay==0)
    {
      uart_debug.println("done!\n\r");
      Motor_mocua_hocqua();
      // if(ui8_goilenh_xuong==1)
      // {
      // ui32_timeout_rsqua=millis()+20000;
      // ui8_every_ok=1;
      // Motor_khoacua_hocqua(255);
      delay(200);
      Motor_dungkich_hocqua();

      Motor_tamche_ra();
      ui8_solangoilenh=0;
      chay_tamche=0;
      ui32_timeout_chaytamngan=millis()+5000;
      ui8_dungtamche=1;
      ui8_dakhoa_cua=0;
      // ui8_hoanthanh=0;
    }
    

}

int get_vitri()
{
  switch (row_-1){
    case 0:
    return Vitrilaysanpham[0];
    break;
    
    case 1:
    return Vitrilaysanpham[1];
    break;
    
    case 2:
    return Vitrilaysanpham[2];
    break;    
    
    case 3:
    return Vitrilaysanpham[3];
    
    case 4:
    return Vitrilaysanpham[4];
	
	case 5:
    return Vitrilaysanpham[5];
  }
	return 0;
}

void gogo(int vitri_)
{
	uart_debug.println("Move thang may\n\r");
	int count=0;
	Motor_thangmay_len();
	// int reading = doccambien(limUp_pin,1);
	int encoder_ = digitalRead(E_CHA);
	int err_=0;
	int buttonState=0;
	unsigned long currentMillis = millis();
	unsigned long previousMillis = currentMillis;
	unsigned long timer_checkmotor = currentMillis+5000; 
	int time_out_=0;
	u32_timeflash=millis()+500;   

	while(1)
	{
		encoder_ = digitalRead(E_CHA);
    // uart_debug.println(encoder_);
		if(encoder_ != buttonState)
		{
			buttonState = encoder_;
			if(buttonState == HIGH)
			{
				count++;

				timer_checkmotor=currentMillis+5000;
				if(count>vitri_)
				{
					Motor_thangmay_dung();
					uart_debug.print("Da toi vi tri");
					uart_debug.print(" count=");
					uart_debug.println(count);
					count=0;
					err_=0;
					break;
				}
			}
		}

		currentMillis = millis();
		if (currentMillis - previousMillis >= 1000)
		{
			time_out_++;
			previousMillis=currentMillis;
			uart_debug.println(time_out_);
			if(time_out_>15)
			{
				uart_debug.println("Time out\n\r");
				err_=2;
				break;
			}      
		}
		//check motor
		if (currentMillis > timer_checkmotor)
		{            
			uart_debug.println("Elevator motor not running up\n\r");
      ui8_phanhoi_huthangmay=1;
      ui8_trangthai_thangmay=1;
      erro_status=thangmay;
			err_=2;
      EEPROM.put(0,ui8_trangthai_thangmay);
      EEPROM.commit();
			break;    
		}   
	
	// flash_led_red();
  	}
  Motor_thangmay_dung();
  uart_debug.print("Err code=");  
  uart_debug.println(err_);
  uart_debug.println("done!\n\r");
}

void move_thangmay()
{
  int sensor=get_vitri();
  uart_debug.println(sensor);
  uart_debug.println(ui8_vitridangchay);
  int vitri_cuoi=0;
  if(sensor)
  {
    // if(ui8_vitridangchay>sensor)
    // {
    //   uart_debug.println("chay len hang tren do o moi nhan cao hon");
    //   vitri_cuoi=ui8_vitridangchay-sensor;
    //   ui8_vitridangchay=sensor;
    //   gogo(-vitri_cuoi);
    // }

    if(ui8_vitridangchay<sensor && ui8_vitridangchay!=0)
    {
      uart_debug.println("chay xuong hang duoi do o moi nhan nho hon");
      vitri_cuoi=sensor-ui8_vitridangchay;
      ui8_vitridangchay=sensor;
      gogo(vitri_cuoi);
    }

    else if(ui8_vitridangchay==0)
    {
      uart_debug.println("chay tu diem goc len");
      ui8_vitridangchay=sensor;
      gogo(sensor);
    }

    else if(ui8_vitridangchay==sensor)
    {
      uart_debug.println("dung im");
      ui8_vitridangchay=sensor;
    }
     
  }
  else
  {
	  uart_debug.println("Undefined order elavator");
  }

}


void gogo_dixuong(int vitri_)
{
  uart_debug.println("Move thang  may\n\r");
  int count=vitri_;
  uart_debug.println(count);
  // motor_1_Down(50);
  Motor_thangmay_xuong(50);
  // int reading = doccambien(limDown_pin,0);
  int reading = doccambien(CTHT_THANG_GOC,20,0);
  int encoder_ = digitalRead(E_CHA);
  int err_=0;
  int buttonState=0;
  unsigned long currentMillis = millis();
  unsigned long previousMillis = currentMillis;
  unsigned long timer_checkmotor = currentMillis+10000; 
  int time_out_=0;
  u32_timeflash=millis()+500;   
  while(reading==0)
  {         
    // reading = doccambien(limDown_pin,0);
    int reading = doccambien(CTHT_THANG_GOC,20,0);
      // for(int speed=100;speed<=10;speed--)
      // {
      //   motor_1_Down(speed);
      //   delay(10);
      // }
      if(ui8_toivitri_antoan==0)
      {
      if(reading==1)
      {                
        // for(int speed=30;speed<=0;speed--)
        // {
        //   motor_1_Down(speed);
        //   delay(10);
        // }
        // motor_1_Dung();
        Motor_thangmay_dung();
        uart_debug.print("limDownp_pin Pause lan 1\n\r");
        uart_debug.print("count= ");
        uart_debug.println(count);
        ui8_check_doilenh=0;
      }  
    }
    
    
    encoder_ = digitalRead(E_CHA);
    if (encoder_ != buttonState && ui8_toivitri_antoan==0)  
    {
      buttonState = encoder_;
      if (buttonState == HIGH) 
      {

        count--;
        //reset counter
        timer_checkmotor=currentMillis+5000;
        // uart_debug.println("check encoder");
        // uart_debug.println(count);
        if (count<=700)
        {
          // for(int speed=50;speed<=30;speed--)
          // {
          //   motor_1_Down(speed);
          //   delay(10);
          // }
          // motor_1_Down(30);
          // motor_1_Dung();
          // for(int speed=30;speed<10;speed--)
          // {
            // Motor_thangmay_xuong(10);
          // }
          Motor_thangmay_dung();
          // ui32_timeout_vehome=millis()+300;
          ui8_toivitri_antoan=1;
          uart_debug.print("Da toi vi tri");
          uart_debug.print(" count=");
          uart_debug.println(count);
          count=0;
          err_=0;
          // break;
        }
      }
      // else{

      // }
      // digitalWrite(ledPin, buttonState);
    }

    // reading = doccambien(limDown_pin,0);
    reading = doccambien(CTHT_THANG_GOC,20,0);
    if(reading == 0)
    {
    if(ui8_toivitri_antoan==1)
    {
      // uart_debug.println("chay tiep doan nua");
      // // for(int speed=30;speed<=10;speed--)
      // // {
      //   motor_1_Down(2);
      // //   delay(10);
      // // }
      // reading = doccambien(limDown_pin,0);
      delay(500);
      Motor_thangmay_xuong(10);
      reading = doccambien(CTHT_THANG_GOC,20,0);
      if(reading==1)
      {                
        // for(int speed=30;speed<=0;speed--)
        // {
        //   motor_1_Down(speed);
        //   delay(10);
        // }
        // motor_1_Dung();
        Motor_thangmay_dung();
        // delay(10);
        // for(int speed=0;speed<=50;speed++)
        // {
        //   motor_1_Down(speed);
        //   delay(10);
        // }
        // delay(10);
        // motor_1_Dung();
        uart_debug.print("limDownp_pin Pause ham move xuong\n\r");
        uart_debug.print("count= ");
        uart_debug.println(count);
        ui8_check_doilenh=0;
        // if(ui8_goilenh_xuong==1)
        // {
    //       ui32_timeout_rsqua=millis()+20000;
    //       ui8_toivitri_antoan=0;
    //       ui8_every_ok=1;
    //       // motor_2_MO(255);
    //       // delay(100);
    //       // motor_2_dung();

    //       // motor_3_RA(255);
    //       Motor_mocua_hocqua();
    // // if(ui8_goilenh_xuong==1)
    // // {
    // // ui32_timeout_rsqua=millis()+20000;
    // // ui8_every_ok=1;
    // // Motor_khoacua_hocqua(255);
    //       delay(200);
    //       Motor_dungkich_hocqua();
    //       Motor_tamche_ra();
    //       ui8_solangoilenh=0;
    //       chay_tamche=0;
    //       ui8_dungtamche=1;
    //       ui32_timeout_chaytamngan=millis()+5000;
    //       ui8_dakhoa_cua=0;
          
    //       // uart_debug.println(digitalRead(NUT_RF));
    //       ui8_goilenh_xuong=0;
        // }
        break;
      }  
    }
    }
    
    //check timer out
    currentMillis = millis();
    if (currentMillis - previousMillis >= 2000)
    {
      time_out_++;
      previousMillis=currentMillis;
      uart_debug.println(time_out_);
      if(time_out_>15){
          uart_debug.println("Time out\n\r");
          err_=2;
        break;
      }      
    }
    //check motor
    if (currentMillis > timer_checkmotor)
    {            
      uart_debug.println("Elevator motor not running down\n\r");
      err_=2;
      break;    
    }   
	
	// flash_led_red();
  }
  // motor_1_Dung();
  Motor_thangmay_dung();
  uart_debug.print("Err code=");  
  uart_debug.println(err_);
  uart_debug.println("done!\n\r");

  if(err_==2)
  { 
    uart_debug.println("gui lenh loi thang may");
      ui8_phanhoi_huthangmay=1;
      ui8_trangthai_thangmay=1;
      EEPROM.put(0,ui8_trangthai_thangmay);
      EEPROM.commit();
      err_=0;
  }

  // if(ui8_goilenh_xuong==1)
  // {
  if(ui8_trangthai_thangmay==0)
  {
    ui32_timeout_rsqua=millis()+20000;
    ui8_every_ok=1;
    // motor_2_MO(255);
    // delay(100);
    // motor_2_dung();
    // motor_3_RA(255);
    ui8_toivitri_antoan=0;
    Motor_mocua_hocqua();
    // if(ui8_goilenh_xuong==1)
    // {
    // ui32_timeout_rsqua=millis()+20000;
    // ui8_every_ok=1;
    // Motor_khoacua_hocqua(255);
    delay(200);
    Motor_dungkich_hocqua();
    Motor_tamche_ra();
    ui8_solangoilenh=0;
    chay_tamche=0;
    ui8_dakhoa_cua=0;
    ui32_timeout_chaytamngan=millis()+5000;
    ui8_dungtamche=1;
    // uart_debug.println(digitalRead(NUT_RF));
    ui8_goilenh_xuong=0;
  }
    
  // }
}


void move_thangmay_xuong()
{
  int sensor=get_vitri();
  uart_debug.println(sensor);
  // int sensor=get_vitri();
  if(sensor)
  {
	  gogo_dixuong(sensor); 
  }
  else
  {
	  uart_debug.println("Undefined order elavator");
  }
}

void disable_row_col()
{  
  // RC_enable();
//   RC_disable(); 
  for (int i=0;i<10;i++){
    MOTOR_HANG.digitalWrite(ROW[i],1);
    MOTOR_COT.digitalWrite(COL[i],1);
  }
  //delay(2000);
  //RC_disable(); 
}

void set_motor(int row_pin_, int col_pin_)
{    
//   RC_enable();
  
  MOTOR_HANG.digitalWrite(row_pin_, LOW);
  MOTOR_COT.digitalWrite(col_pin_, LOW);  
}

int check_sku()
{
	int status = doccambien(CAMBIEN_ROTQUA,20,0);
	if(status == 1)
	{
		uart_debug.println("sense_pin ==LOW");
	  	return 1;  
	}
	return 0;
}

void drop_sku_process()
{
	int lanin=0;
	int motorsense;
	//read input values
	int sensorVal=0;
	int pre_=0; 
	// int u=0;
	int sku_=0;
	int t=0;
	int v=0;//VONG QUAY
	unsigned long u32wait;
	unsigned long u32checkmotor;
	unsigned long u32current;

	#ifndef use_elavator_
    	vitrithangmay=0;
  	#endif

	if(ui8_trangthai_cualayqua==0)
	{
		int tam=doccambien(CTHT_HOC_QUA,50,1);

		if(tam==0 && ui8_trangthaikhoacua==0)
		{
		Motor_khoacua_hocqua();
		delay(100);
		Motor_dungkich_hocqua();
		ui8_dakhoa_cua=1;
		// uart_debug.println(digitalRead(NUT_RF));
		erro_status=khoacua;
		ui8_respone2PC=1;
		ui8_trangthaikhoacua=1;
    ui8_phanhoi_cua=1;
		}

    else if(tam==1)
    {
      ui8_dakhoa_cua=1;
		// uart_debug.println(digitalRead(NUT_RF));
      erro_status=khoacua;
      ui8_respone2PC=1;
      ui8_trangthaikhoacua=1;
      ui8_phanhoi_cua=1;
    }
	}

	if(ui8_trangthaikhoacua==1 && ui8_phanhoi_cua==0)
  {
    if(ui8_solangoilenh<=1 && chay_tamche==0)
    {
      uart_debug.println("doi khoa cua chay vo");
      Motor_tamche_vao();
      ui32_timeout_chaytamngan=millis()+3500;
      chay_tamche=1;
    }

    if(chay_tamche==1 && ui32_timeout_chaytamngan<=millis())
    {
      Motor_tamche_dung();
      delay(100);
      uart_debug.print("Drop product at "); 
      uart_debug.println((row_));
      uart_debug.print(" ");
      uart_debug.println(column_);
      uart_debug.println("hoan thanh tam che");
      
        #ifdef use_elavator_
          move_thangmay();	
        #endif 
        ui8_tamchechay_xong=1;

      // if(ui8_trangthai_thangmay!=1)
      // {
          uart_debug.println("Truoc khi chay:\t");
  
        //   for(lanin=0;lanin<5;lanin++)
        //   {
        //     motorsense=analogRead(MOTOR_SENSE);
        //     uart_debug.print("S:\t");
        //     uart_debug.println(motorsense);
        //     delay(10);	  
        //   }
          disable_row_col();      

          set_motor(ROW[row_+1],COL[column_-1]);
          lanin=0;
          
          u32wait = millis()+10000;
          u32checkmotor  =millis()+8000;
          u32current=millis();
          erro_status=sku_empty;
          u32_timeflash=millis()+500;
          ui32_tiemout_rskhay=millis()+30000;
          while(u32current <u32wait)
          {
            u32current=millis();
          //check sku		

          int encoder_ = digitalRead(E_CHA);
          int buttonState=0;
          int count=0;
          if (encoder_ != buttonState) 
          {
            buttonState = encoder_;
            if (buttonState == HIGH) 
            {
              count++;
              if (count>=200)
              {
                gogo(200);
                uart_debug.print("comeback elevator");
                uart_debug.print(" count=");
                uart_debug.println(count);
                count=0;
                // err_=0;
                // break;
              }
            }
          }

          sku_=check_sku();
          if(sku_)
          {
            ui8_songuoi_quet++;
            uart_debug.println("Detect object\n\r");
            erro_status=sku_success;
            ui8_phanhoi_trangthainhaqua=1;
            break;
          }
            
          //check motor via MOTOR SENSE
            // motorsense=analogRead(MOTOR_SENSE);        
            // if (motorsense>200)
            // {      
            // // u32checkmotor=u32current;
            //   u32checkmotor=millis()+5000;
            // }
          //debug print
          if(lanin<5)
          {
            lanin++;
            uart_debug.print("A:\t");
            uart_debug.println(motorsense);
            delay(1);	  
          }
          //end check motor
            
          if(u32current>u32checkmotor)
          {           
              erro_status=sku_empty;
              ui8_respone2PC = 1;
              uart_debug.println("Motor not running");
              uart_debug.print("Plz check the motor at: ");
              uart_debug.println((row_));
              uart_debug.print(" ");
              uart_debug.println(column_);
              ui8_phanhoi_trangthainhaqua=1;
              // ui8_phanhoi_hudongconhaqua=1;
              // ui8_trangthai_dongconhaqua=1;
              break;
          }
          // flash_led_red();
          }
      // }
      
      
      if(ui8_songuoi_quet==ui8_songuoi_tuphanmem)
      {
          ui8_respone2PC = 1;
          ui32_timeduocguilenh = millis(); //danh sua
            uart_debug.println("tro ve thang may");
            disable_row_col(); 
            uart_debug.print("Drop Sku finished. Erro code: ");
            uart_debug.print(erro_status);
            uart_debug.print(" ");
            uart_debug.println(msg[erro_status]);
            ui8_vitridangchay=0;
            ui8_songuoi_quet=0;
            ui8_songuoi_tuphanmem=0;
            ui8_trangthaikhoacua=0;
            ui8_phanhoi_trangthainhaqua=1;
            ui32_timeout_khoacua=millis()+60000;
            ui32_timeout_comeback=millis()+tg_xuong;
            ui8_solangoilenh=0;
            chay_tamche=0;
            ui8_hoanthanh=1;
            start_drop_sku=false;
      }

      else if(ui8_songuoi_tuphanmem==0)
      {
        ui8_respone2PC = 1;
        ui32_timeduocguilenh = millis(); //danh sua
        uart_debug.println("tro ve thang may");
        disable_row_col(); 

        uart_debug.print("Drop Sku finished. Erro code: ");
        uart_debug.print(erro_status);
        uart_debug.print(" ");
        uart_debug.println(msg[erro_status]);
        ui8_phanhoi_trangthainhaqua=1;
        ui8_vitridangchay=0;
        ui8_songuoi_quet=0;
        ui8_songuoi_tuphanmem=0;
        ui8_trangthaikhoacua=0;
        ui32_timeout_khoacua=millis()+60000;
        ui32_timeout_comeback=millis()+tg_xuong;
        ui8_solangoilenh=0;
        chay_tamche=0;
        ui8_hoanthanh=1;
        start_drop_sku=false;
      }

      else
      {
        disable_row_col(); 
        uart_debug.println("dang o ham doi nha qua tiep");
        uart_debug.print("Drop Sku finished. Erro code: ");
        uart_debug.print(erro_status);
        uart_debug.print(" ");
        uart_debug.println(msg[erro_status]);
        ui8_phanhoi_trangthainhaqua=1;
        ui8_check_doilenh=1;
        ui32_tiemout_rskhay=millis()+20000;
        start_drop_sku=false;
      }
      }
        
      }

	
}

boolean getcheckmotor()
{  
  if(row_=='A' || row_=='B'|| row_=='E')return 1;
  return 0; 
}
boolean check_col_row()
{
  
#ifndef CHECK_COLUMN_ROW
   return 1;
 #endif

  
  if(row_==0)
  {
    if(column_>=1 & column_<=8)
    {
      return 1;
    }
  }
  else if(row_==1)
  {
    if(column_>=1 & column_<=2)
    {
      return 1;
    }
  }
  else if(row_==2)
  {
    if(column_>=1 & column_<=8)
    {
      return 1;
    }
  } 
  else if(row_==3)
  {
    if(column_>=1 & column_<=2)
    {
      return 1;
    }
  }
  else if(row_==4)
  {
    if(column_>=1 & column_<=5)
    {
      return 1;
    }
  } 
  return 0;
}

void check_kettay()
{
  int tam=doccambien(CTHT_HOC_QUA,50,1);
  // int tam1=check_sku(CB_HONGNGOAI_TRONGTHUNG,6);
  // int tam2=digitalRead(NUT_RF);
  // uart_debug.println(digitalRead(CB_HONGNGOAI_TRONGTHUNG));
  if(tam==1 && ui8_trangthai_cualayqua==0 && ui8_dakhoa_cua==0)
  {
      uart_debug.println();
      uart_debug.println("phat hien mo cua");
      ui8_trangthaikhoacua=0;
      ui8_every_ok=0;
      if(start_drop_sku==false)
      {
        erro_status=mocua;
        ui8_respone2PC=1;
      }
      
      ui8_trangthai_cualayqua=1;
      ui8_phanhoi_cua=1;
  }

  if(tam==0 && ui8_trangthai_cualayqua==1)
  {
    // uart_debug.println(digitalRead(NUT_RF));
    uart_debug.println("phat hien dong cua khoa cua");
    ui32_timeout_khoacua=millis()+15000;
    // ui8_trangthaikhoacua=1;
    if(start_drop_sku==false)
      {
        erro_status=dongcua;
        ui8_respone2PC=1;
      }
    ui8_trangthai_cualayqua=0;
    ui8_phanhoi_cua=1;
  }
}

void guilenhlenPC()
{
  if(ui32_timecho_guilenh +  200 <= millis())
  {
    PHANHOI(ui8_phanhoi_dungFunction, 0x00, ui8_data_rec[1], respondtoPC(ui8_function, ui8_status));
    PHANHOI(ui8_phanhoi_saiCRC, 0x01, ui8_data_rec[1], send_data_khinhansailenh(ui8_function,ui8_status));
    PHANHOI(ui8_phanhoi_saiFunction, 0x02, ui8_data_rec[1], send_data_khinhansailenh(ui8_function,ui8_status));
    PHANHOI(ui8_phanhoi_trunglenh, 0x03, ui8_data_rec[1], send_data_khinhansailenh(ui8_function,ui8_status));
    PHANHOI(ui8_phanhoi_trangthainhaqua,0x00,0x18,send_state_to_PC(0x18));
    PHANHOI(ui8_phanhoi_cua,0x00,0xFA,send_state_to_PC(ui8_function));
    PHANHOI(ui8_phanhoi_huthangmay,0x00,0x06,send_state_to_PC(ui8_function));
    PHANHOI(ui8_phanhoi_hudongconhaqua,0x00,0x06,send_state_to_PC(ui8_function));

  }
}

void tatled()
{
	neopixelWrite(ledpixel,0,0,0); // 
}

void led_chopcoi()
  {
	if(millis()>=ui32_timeout_chopled)
	{
		if(led_status==1)
		{
			neopixelWrite(ledpixel,0,255,32); // Red
			led_status=0;
			ui32_timeout_chopled=millis()+100;		
		}

		else
		{
			neopixelWrite(ledpixel,0,0,0); // Red
			led_status=1;
			ui32_timeout_chopled=millis()+100;
		}
	}
  }

void led_pixel()
  {
	if(millis()>=ui32_timeout_chopled)
	{
		if(led_status==1)
		{
			neopixelWrite(ledpixel,0,0,255); // Red
			led_status=0;
			ui32_timeout_chopled=millis()+900;		
		}

		else
		{
			neopixelWrite(ledpixel,0,0,0); // Red
			led_status=1;
			ui32_timeout_chopled=millis()+200;
		}
	}
	// neopixelWrite(ledpixel,RGB_BRIGHTNESS,0,0); // Red
  }


void led_thangxuong()
  {
	if(millis()>=ui32_timeout_chopled)
	{
		if(led_status==1)
		{
			neopixelWrite(ledpixel,0,255,255); // Red
			led_status=0;
			ui32_timeout_chopled=millis()+100;		
		}

		else
		{
			neopixelWrite(ledpixel,0,0,0); // Red
			led_status=1;
			ui32_timeout_chopled=millis()+600;
		}
	}
	// neopixelWrite(ledpixel,RGB_BRIGHTNESS,0,0); // Red
  }

 
void ledbaodo()
{
  if(millis()>=ui32_timeout_chopled)
	{
		if(led_status==1)
		{
			neopixelWrite(ledpixel,255,0,0); // Red
			led_status=0;
			ui32_timeout_chopled=millis()+500;		
		}

		else
		{
			neopixelWrite(ledpixel,0,0,0); // Red
			led_status=1;
			ui32_timeout_chopled=millis()+200;
		}
	}
}

void Task1core(void *parameter)
{
	int count2 = 0;
	
	for(;;)
	{
		xSemaphoreTake(batton, portMAX_DELAY);
		readSerial();
		uart_debugserial();
    uart_debug.print("analog: ");
    uart_debug.println(analogRead(5));
		xSemaphoreGive(batton);
		vTaskDelay(1);
	}
}

void Task2core(void *parameter)
{
	for(;;)
	{
		xSemaphoreTake(batton, portMAX_DELAY);
    nhaqua();
    check_kettay();
    guilenhlenPC();

  if(ui8_dungtamche==1 && ui32_timeout_chaytamngan<=millis())
  {
    uart_debug.println("dung dong co tam che");
    // motor_3_Dung();
    Motor_tamche_dung();
    ui8_dungtamche=0;
  }

    if(ui8_check_doilenh==1)
    {
      if((ui32_tiemout_rskhay<=millis()))
      {
        // led_thangxuong();
        ui8_vitridangchay=0;
        ui8_songuoi_quet=0;
        ui8_songuoi_tuphanmem=0;
        uart_debug.println("ko thay phan hoi lenh");
        ui8_solangoilenh=0;
        chay_tamche=0;
        ui8_hoanthanh=1;
        ui32_timeout_comeback=millis()+tg_xuong;
        ui8_check_doilenh=0;
      }
      
    }

    if(ui8_hoanthanh==1 && ui32_timeout_comeback<=millis())
    {
      tatled();
      reset_khaychuasanpham(); //danh cho may so 2
      // move_thangmay_xuong();
      ui8_hoanthanh=0;
    }

    if(start_drop_sku==true)
    {
      // led_chopcoi();
      tatled();
      drop_sku_process();
    }

    else if(start_drop_sku==false && ui8_trangthai_thangmay==0 && ui8_check_doilenh==0)
    {
      led_pixel();
    }

    else if(ui8_trangthai_thangmay==1)
    {
      ledbaodo();
    }

    if(wait_off)
    {
        if(millis() > temp_main)
        {
          ledoff;
          wait_off=0;
          temp_main=millis()+1000;
          tempus = temp_main;
        }
    }
    else if (millis() > temp_main) 
    {        
      ledon;
      wait_off=1;
      temp_main=millis()+500;        
      tempus = temp_main;
      if(temp_serial++>10)
      {
       uart_debug.print("SP0 count \t@");
       uart_debug.println(counter_test);    
        counter_test++;
        temp_serial=0;
      }    
    }

		xSemaphoreGive(batton);
		vTaskDelay(1);
	}
}
