#include "include/GlobalVariables.h"
#include "include/SensorMotorDefine.h" //MOTOR VA SENSOR DUOC DEFINE TRONG NAY. THAY DOI DEFINE COCACOLA HOAC HEINEKEN TUONG UNG CHO MAY
// #include "lib/DONGCO.h"
#include "lib/CRC16.h"
#include <EEPROM.h>
#include "SerialComunication.h"
#include <Wire.h> 
#include "RS485_PROTOCOL.h"
#include "include/definefunction.h"
// #include "lib/PROTOCOL.h"
#include <Adafruit_PCF8574.h>
#include <Adafruit_PCF8575.h>

Adafruit_PCF8575 pcf_PortA;
// Adafruit_PCF8574 pcf_PortA;
// Adafruit_PCF8574 pcf_PortB;
// Adafruit_PCF8574 pcf_PortC;
// Adafruit_PCF8574 pcf_PortD;
// Adafruit_PCF8574 pcf_PortE;

// TwoWire I2C_1 = TwoWire(0); // Kênh I2C đầu tiên
// TwoWire I2C_2 = TwoWire(1); // Kênh I2C thứ hai
// TwoWire I2C_3 = TwoWire(2); // Kênh I2C đầu tiên
// TwoWire I2C_4 = TwoWire(3); // Kênh I2C thứ hai
// TwoWire I2C_5 = TwoWire(4); // Kênh I2C đầu tiên
// TwoWire I2C_6 = TwoWire(5); // Kênh I2C thứ hai
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
  
  Wire.begin(14,13);
  // I2C_2.begin(12,11);
  // I2C_3.begin(10,9);
  // I2C_4.begin(1,2);
  // I2C_5.begin(40,41);
	// delay(50);
  config();

  if(!pcf_PortA.begin(addr_ExIO_portA, &Wire)){
		Serial_Debug.printf("KHONG BAT DAU VOI MODULE MOTOR IO PORT A\n");
	}
	delay(50);
	
	// if(!pcf_PortB.begin(addr_ExIO_portB, &I2C_2)){
	// 	Serial_Debug.println("KHONG BAT DAU VOI MODULE IO PORT B");
	// }
	// delay(50);
	
	// if(!pcf_PortC.begin(addr_ExIO_portC, &I2C_3)){
	// 	Serial_Debug.println("KHONG BAT DAU VOI MODULE IO PORT C");
	// }
	// delay(50);
	
	// if(!pcf_PortD.begin(addr_ExIO_portD, &I2C_4)){
	// 	Serial_Debug.println("KHONG BAT DAU VOI MODULE IO PORT D");
	// }

  // if(!pcf_PortE.begin(addr_ExIO_portE, &I2C_5)){
	// 	Serial_Debug.println("KHONG BAT DAU VOI MODULE IO PORT E");
	// }
	// delay(50);
	
	for(int i=0; i<16; i++){
		pcf_PortA.pinMode(i, OUTPUT);
    pcf_PortA.digitalWrite(i, HIGH);

    // pcf_PortB.pinMode(i, OUTPUT);
    // pcf_PortB.digitalWrite(i, HIGH);

    // pcf_PortC.pinMode(i, OUTPUT);
    // pcf_PortC.digitalWrite(i, HIGH);

    // pcf_PortD.pinMode(i, OUTPUT);
    // pcf_PortD.digitalWrite(i, HIGH);

    // pcf_PortE.pinMode(i, OUTPUT);
    // pcf_PortE.digitalWrite(i, HIGH);
	}
  
	delay(50);
  
  // digitalWrite(RL[0],1);
  // pcf_PortA.digitalWrite(5, LOW);

}

void loop() {
  // put your main code here, to run repeatedly:
  // if(ui8_check_slave==0)
  // {
    rs485();
  // }
  
  //  if()
  //  {
      BATLED_RUN();
  //  }


    if(ui8_coquaroi==1)
    { 
      // if(ui8_dangchopled==1) return;
      int tam = nhaqua(vitri_khay,vitri_o);
      if(tam==1)
      {
        ui32_timeout_reset=millis()+1000;
        erro_status=sku_success;
        ui8_trangthaimaythanhcong=1;
        ui8_timeout_ledthanhcong=millis()+8000;
        ui8_khongcoquaroi=1;
        Serial_Debug.print("nha thanh cong\n");
        ui8_coquaroi=0;
        Serial_Debug.print("Drop Sku finished. Erro code: ");
	      Serial_Debug.print(erro_status);
        // LED_THANHCONG();
      }

      else if(tam==0)
      {
        ui32_timeout_reset=millis()+1000;
        ui32_timeout_resetledthatbai=millis()+8000;
        erro_status=motor_erro;
        ui8_trangthaimaythatbai=1;
        ui8_khongcoquaroi=1;
        Serial_Debug.print("nha that bai\n");
        ui8_coquaroi=0;
        Serial_Debug.print("Drop Sku finished. Erro code: ");
      	Serial_Debug.print(erro_status);
        // LED_THATBAI();
      }

    } 
    check_thanhcongthatbai();
    // if(erro_status==sku_success)
    // {
    //   LED_THANHCONG();
    // }

    // else if(erro_status==motor_erro)
    // {
    //   LED_THATBAI();
    // }

    if(ui8_khongcoquaroi==1){
        if(ui32_timeout_reset<=millis())
        {
          Serial_Debug.print("reset bien");
          erro_status=sku_empty;
          ui8_khongcoquaroi=0;
        }     
    }

    // if(ui8_tatledthanhcong==1)
    // {
    //   if(ui8_timeout_ledthanhcong<=millis())
    //   {
    //     digitalWrite(RL[1],0);
    //     ui8_tatledthanhcong=0;
    //   }
    // }
      // erro_status = sku_success;
      // ui8_check_slave=0;
      // LED_THANHCONG();
}
   

void check_thanhcongthatbai()
{
  int tam=doccambien(cambien_roiqua1,10);
  int tam1=doccambien(cambien_roiqua2,10);
  // if((tam==1 || tam1==1) && ui8_trangthaimaythanhcong==1)
  // {
  //   LED_THANHCONG();
  // }
  // else if((tam==0 || tam1==0) && ui8_trangthaimaythanhcong==1)
  // {
  //   ui8_trangthaimaythanhcong=0;
  //   // ui8_tatledthanhcong=1;
  //   digitalWrite(RL[1],0);
  //   // ui8_timeout_ledthanhcong=millis()+3000;
  // }
  if(ui8_trangthaimaythanhcong==1)
  {
    LED_THANHCONG();
    ui8_dangchopled=1;
    if(ui8_timeout_ledthanhcong<=millis())
    {
      ui8_trangthaimaythanhcong=0;
      digitalWrite(RL[1],0);
      // digitalWrite(RL[1],0);
    }
  }
  if(ui8_trangthaimaythatbai==1)
  {
    LED_THATBAI();
    ui8_dangchopled=1;
    if(ui32_timeout_resetledthatbai<=millis())
    {
      ui8_trangthaimaythatbai=0;
      digitalWrite(RL[0],0);
      // digitalWrite(RL[1],0);
    }
  }
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
          if(input=="nhaqua")
          {
            Serial_Debug.printf("nha qua\n");
            digitalWrite(RL[0],0);
            digitalWrite(RL[1],0);
            ui8_khongcoquaroi=0;
            ui8_coquaroi=1;
            // // kich_mocua(mocua_tren,cambien_cuatren);
            // nhaqua(2,1);
            // pcf_PortA.digitalWrite(8,0);
          }

          if(input=="reset")
          {
            ESP.restart();
          }

          if(input=="nhaqua1")
          {
            digitalWrite(RL[0],0);
            digitalWrite(RL[1],0);
            ui8_khongcoquaroi=0;
            ui8_coquaroi=0;
            nhaqua(1,1);
           
          }

          if(input=="nhaqua2")
          {
            digitalWrite(RL[0],0);
            digitalWrite(RL[1],0);
            ui8_khongcoquaroi=0;
            ui8_coquaroi=0;
            nhaqua(2,2);
          }

          if(input=="nhaqua3")
          {
            digitalWrite(RL[0],0);
            digitalWrite(RL[1],0);
            ui8_khongcoquaroi=0;
            ui8_coquaroi=0;
            nhaqua(2,3);
          }
          if(input=="nhaqua4")
          {
            digitalWrite(RL[0],0);
            digitalWrite(RL[1],0);
            ui8_khongcoquaroi=0;
            ui8_coquaroi=0;
            nhaqua(1,5);
          }
          
          if(input=="nhaqua5")
          {
            digitalWrite(RL[0],0);
            digitalWrite(RL[1],0);
            ui8_khongcoquaroi=0;
            ui8_coquaroi=0;
            nhaqua(3,5);
          }
          // if(input=="kichrole1")
          // {
          //   Serial_Debug.printf("kich mo cua\n");
          //   kich_mocua(mocua_duoi,cambien_cuaduoi);
          // }
        
        }
        break;
        default: break;
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

int nhaqua(uint8_t vitri_hang,uint8_t vitri_o)
{
  static uint8_t chophepnhavat = 0;
	static uint8_t dachaymotor = 0;
	static uint8_t dungdongco = 0; 
	static uint8_t khacvitrihome = 0;

  int hang=vitri_hang-1;
  int o=vitri_o-1;
  // int roiqua = 0;
	// int roiqua2 = 0;
	unsigned long u32wait;
	unsigned long u32checkmotor;
	unsigned long u32current;
  // pcf_PortA.digitalWrite(KHAY_1[o],0);
  // Serial_Debug.printf("vi tri o: %d\n",vitri_o);
  erro_status = sku_empty;
  
  if(dachaymotor==0)
  {
    if(vitri_hang==1)
    {
    pcf_PortA.digitalWrite(KHAY_1[o],0);
    Serial_Debug.printf("kich port khay 1 port A\n");
    }
    else if(vitri_hang==2)
    {

        pcf_PortA.digitalWrite(KHAY_2[o],0);
        Serial_Debug.printf("kich port khay 2 port A\n");
    }
    
    else if(vitri_hang==3)
    {
      pcf_PortA.digitalWrite(KHAY_3[o],0);
      Serial_Debug.printf("kich port khay 3 port A\n");
    }

    ui32_timeout_loinhaqua = millis();
    dachaymotor = 1;
    int roiqua = doccambien(cambien_roiqua1,20);
	  int roiqua2 = doccambien(cambien_roiqua2,20);

    if(roiqua==0 || roiqua2==0)
    {
      khacvitrihome = 1;
    }
  }

  if(ui32_timeout_loinhaqua<millis())
  {
    int roiqua = doccambien(cambien_roiqua1,20);
	  int roiqua2 = doccambien(cambien_roiqua2,20);
    if(roiqua==1 || roiqua2==1)
    {
      if(khacvitrihome == 1 || (khacvitrihome == 0 && (ui32_timeout_loinhaqua + 2200 <= millis())))
      {
        Serial_Debug.printf("nha thanh cong");
        if(vitri_hang==1)
        {
          pcf_PortA.digitalWrite(KHAY_1[o],1);
          Serial_Debug.printf("tat kich port khay 1 port A\n");
        }
        else if(vitri_hang==2)
        {

            pcf_PortA.digitalWrite(KHAY_2[o],1);
            Serial_Debug.printf("tat kich port khay 2 port A\n");
        }
        
        else if(vitri_hang==3)
        {
          pcf_PortA.digitalWrite(KHAY_3[o],1);
          Serial_Debug.printf("tat kich port khay 3 port A\n");
        }
        dachaymotor = 0;
        chophepnhavat = 0;
        dungdongco = 0;
        khacvitrihome = 0;
        return 1;
      }
      
    }

    else if(chophepnhavat==0)
		{
			if(ui32_timeout_loinhaqua+2200<=millis())
			{
				chophepnhavat=1;
				ui32_timeout_nhaqua = millis();
				Serial_Debug.println("CHO PHEP NHA VAT XUONG");
			}
		}

		else if(chophepnhavat==1)
		{

			if(ui32_timeout_nhaqua+5000<millis())
			{
				//LOI
        if(vitri_hang==1)
        {
          pcf_PortA.digitalWrite(KHAY_1[o],1);
          Serial_Debug.printf("tat kich port khay 1 port A\n");
        }
        else if(vitri_hang==2)
        {

            pcf_PortA.digitalWrite(KHAY_2[o],1);
            Serial_Debug.printf("tat kich port khay 2 port A\n");
        }
        
        else if(vitri_hang==3)
        {
          pcf_PortA.digitalWrite(KHAY_3[o],1);
          Serial_Debug.printf("tat kich port khay 3 port A\n");
        }
				chophepnhavat = 0;
				dachaymotor = 0;
				dungdongco = 0; 
				khacvitrihome = 0;
				// hoanthanh_kiemtra_nhavat = 0;
				Serial_Debug.printf("DONG CO NHA VAT KHONG NHA\n");
				// kiemtracuaket();
				// ui8_trangthai_nhavat=TrangThaiRoiVat::KHONGNHA;
				return 0;
			}
    }
  }
  

  // u32wait = millis()+timeout_wait;
	// u32checkmotor = millis()+10000;
	// u32current=millis();
	// ui32_timeout_nhaqua = millis()+5000;
	// ui32_timeout_loinhaqua = millis()+timeout_loinhaqua;


  // while(u32current < u32wait)
	// {
	// u32current = millis();
	// roiqua = doccambien(cambien_roiqua1,20);
	// roiqua2 = doccambien(cambien_roiqua2,20);
	// if(roiqua == 1 || roiqua2 == 1)
	// {
	// 	Serial_Debug.println("co qua roi");
	// 	erro_status = sku_success;
	// 	u32checkmotor = millis()+5000;
	// 	// ui8_phanhoi_coquaroi = 1;
	// 	ui8_coquaroi = 1;
  //   // digitalWrite(RL[0],1);
	// 	ui32_timeout_nhaqua = millis()+5000;
    
	// 	break;
	// }

  //   if(u32current > u32checkmotor)
  //   {
  //     Serial_Debug.println("Motor not running");
  //     // ui8_phanhoi_khongcoquaroi = 1;
  //     ui8_khongcoquaroi = 1;
  //     ui32_timeout_nhaqua = millis()+5000;
  //     // ui8_check_slave=0;
  //     erro_status = motor_erro;
  //     // digitalWrite(RL[1],1);
  //   //   Serial_Debug.print("Plz check the motor at: ");
  //   //   Serial_Debug.write(ui8_row);
  //   //   Serial_Debug.print(" ");
  //   //   Serial_Debug.println(ui8_col);
  //     break;
  //   }
	// }
	// disable_row_col(clock_pin,data_pin,lat_pin);

  
	// status_drop_product=0;
  return -1;
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

void LED_THANHCONG()
{
  // int tam = digitalRead(ledon_board1);
  int tam = digitalRead(RL[1]);
  static uint32_t thoigianled = 0;
  static const uint16_t thoigianled_binhthuong = 500;
  if(millis() - thoigianled >= thoigianled_binhthuong){
    digitalWrite(RL[1], !tam);
    thoigianled = millis();
  }
}

void LED_THATBAI()
{
  // int tam = digitalRead(ledon_board1);
  int tam = digitalRead(RL[0]);
  static uint32_t thoigianled = 0;
  static const uint16_t thoigianled_binhthuong = 300;
  if(millis() - thoigianled >= thoigianled_binhthuong){
    digitalWrite(RL[0], !tam);
    thoigianled = millis();
  }
}

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
        Serial_Debug.print("\nReceive: ");
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

        case codeMaster::PHATQUA:
        {
          if (buffer_receive[0] - 4 != 2)
            {
                Serial_Debug.println(" Incorrect command check error");
                break;
            }
          
          else
          {
            // if(buffer_receive[5]==)
            // {
              Serial_Debug.printf("nhan du lieu mo qua\n");
            //   ui8_mocuatren=buffer_receive[5];
            // }
            // else if(buffer_receive[5]==vitricua::CUADUOI)
            // {
            //   Serial_Debug.printf("mo cua duoi\n");
            //   ui8_mocuaduoi=buffer_receive[5];
            // }
            // xulyyeucau_mocua();
            // if(ui8_dangchopled==1)
            // {
            if(ui8_coquaroi==1) return;
              ui8_trangthaimaythanhcong=0;
              ui8_trangthaimaythatbai=0;
              ui32_timeout_resetledthatbai=millis()+100;
              ui8_timeout_ledthanhcong=millis()+100;
              // ui8_dangchopled=1;
            // } 
              
              digitalWrite(RL[0],0);
              digitalWrite(RL[1],0);
              ui8_check_slave=1;
              
              vitri_khay = buffer_receive[5];
              vitri_o = buffer_receive[6];
              
              ui8_coquaroi=1;  
              
              
              // nhaqua(vitri_khay,vitri_o);
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
        //     xulyyeucau_dongcua();
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
  // if(ui8_check_slave==1) return;
   if (ui8_phanhoi_TrangthaiLoi == 1 )
            ui8_trangthaiRS485 = trangthaiRS485::DANGGUI;
        else
            ui8_trangthaiRS485 = trangthaiRS485::DANGNHAN; 

    if (ui8_trangthaiRS485 == trangthaiRS485::DANGNHAN)
        {
            // Serial_Debug.println("\ndang nhan");
            RS485.receive(buffer_receive);
// #ifdef ShowSerial
            if (RS485.isReceived)
            {
                Serial_Debug.print("\nReceive: ");
                for (uint8_t i = 0; i <= buffer_receive[0]; i++)
                {
                    Serial_Debug.print(buffer_receive[i], HEX);
                    Serial_Debug.print(" ");
                }
            }
// #endif
            processRS485();
        }

        // Xử lý khi ở trạng thái gửi dữ liệu đến master
        else if (ui8_trangthaiRS485 == trangthaiRS485::DANGGUI)
        {
            Serial_Debug.println("\ndang gui");
            if (ui8_phanhoi_TrangthaiLoi == 1)
            {
                phanhoi_trangthairotqua();
                ui8_phanhoi_TrangthaiLoi = 0;
            }
        }
}

void phanhoi_trangthairotqua()
{
    // uint8_t trangthai_Phanloai = ((PhanLoai.dangxulyFlag == 1) ? 0x02 : PhanLoai.trangthaiLoi);
    // uint8_t trangthai_Nghien = (NghienChai.trangthaiLoi);
    // uint8_t trangthai_Ep = ((EpLon.dangxulyFlag == 1) ? 0x02 : EpLon.trangthaiLoi);
    // uint8_t trangthai_cuatren=ui8_trangthai_cuatren;
    // uint8_t trangthai_cuaduoi=ui8_trangthai_cuaduoi;
    // uint8_t trangthai_rotqua=erro_status;
    Serial_Debug.println("loi motor");
    Serial_Debug.println(erro_status);
    uint8_t bytecount = 1;
    uint8_t datasize = bytecount + 4;
    raiseOrderCommand(self_order_command);

    uint8_t data[datasize + 1] = {datasize,
                                  codeMaster::check_error,
                                  self_order_command[0],
                                  self_order_command[1],
                                  bytecount,
                                  erro_status
                                  };

    memcpy(buffer_send, data, datasize + 1);
    RS485.send(buffer_send);
    // Serial_Debug.print("\nReceive SLAVE: ");
        // ui8_chophanhoi=0;
        // for (uint8_t i = 0; i <= sizeof(buffer_send); i++)
        // {
        //   Serial_Debug.print(buffer_send[i], HEX);
        //   Serial_Debug.print(" ");
        // }
    // erro_status = sku_empty;
#ifdef ShowSerial
    SerialDebug.printf("\nguiphanhoichoMaster: %D %D %D\n", trangthai_Phanloai, trangthai_Nghien, trangthai_Ep);
#endif
}


