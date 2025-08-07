#include "LIB/GlobalVariable.h"

void scan_i2c()
{
  byte error, address;
  int nDevices;
  Serial_debug.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial_debug.print("I2C device found at address 0x");
      if (address<16) {
        Serial_debug.print("0");
      }
      Serial_debug.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial_debug.print("Unknow error at address 0x");
      if (address<16) {
        Serial_debug.print("0");
      }
      Serial_debug.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial_debug.println("No I2C devices found\n");
  }
  else {
    Serial_debug.println("done\n");
  }
  delay(1000);          
}

void init_scale()
{
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();
}

void calibrateOffset()
{
  Serial_debug.println("The software is adjusting offset...");
  delay(10);
  
  scale.tare(); 
  offset = scale.read_average(20);

  // EEPROM.put(OFFSET_ADDR, offset);
  // EEPROM.commit(); // Save the offset to EEPROM
  Serial_debug.println("Adjust done!");

  scale.set_offset(offset);
}

void hienthi_lcd_test()
{
    // set cursor to first column, first row
  lcd.setCursor(0, 0);
  // print message
  lcd.print("Hello, World!");
  delay(1000);
  // clears the display to print new message
  lcd.clear();
  // set cursor to first column, second row
  lcd.setCursor(0,1);
  lcd.print("Hello, World!");
  delay(1000);
  lcd.clear();
}

void start_locadcell()
{
    float fl_CurrentWeith = scale.get_units();
    if(fl_CurrentWeith<10)
    {
      ui2_detect = false;
    } 
    else if((isDetected < fl_CurrentWeith) && (fl_CurrentWeith < ScaleLimited) && ui2_detect == false)  //The object is detected
    {
      //Send a signal to main board to start pouring 
      //Check input of 10th time to confirm the object is detected if all times are the same
      float ConfirmWeith;
      uint8_t cnt = 0;
      while(cnt < 10)
      {
        ConfirmWeith = scale.get_units();
        if(ConfirmWeith > isDetected)
        {
          cnt++;
          Serial_debug.print("cnt: ");
          Serial_debug.println(cnt);
          delay(200);
        }
        else
        {
          cnt = 0;
          break;
        }
      }
      if(cnt >= 10)
      {
        ConfirmWeith = scale.get_units();
        if((ConfirmWeith > isDetected) && (ui2_detect == false))
        {
          //Send command to slave to start pouring
          // Send_data_EspNow(SLAVE_ID, OBJECT_DETECTED, 4);
          Serial_debug.println("The object is detected...");
          ui2_detect = true;
          cnt = 0;
        }
      }
    }
    else if(fl_CurrentWeith > ScaleLimited)
    {
      float Weith_exceed_limit = scale.get_units();
      while (Weith_exceed_limit > ScaleLimited)
      {
        /* 
          not thing to do 
          wait until the object is removed from the scale
        */
        Weith_exceed_limit = scale.get_units();
        if(Weith_exceed_limit < 10) 
        {
          break;
        }
      }
    }
    /* 
      Handle the pouring process, read the value continuously during the pouring process
      If the weith is not enough, continue pouring, if it is enough, stop pouring
      If the object is not detected, stop pouring
    */
    if(ui2_detect)  
    {
      float ConfirmWeith_object_detected = scale.get_units();
      // Serial_debug.println("so 1");
      while(ConfirmWeith_object_detected < ScaleLimited) //The weith is not enough
      {
        ConfirmWeith_object_detected = scale.get_units(); 
        // Serial_debug.println("so 2");
        // BlinkLed(); //blink led to show the status of the device
        if(ConfirmWeith_object_detected > ScaleLimited) //The weith is enough
        {
          /* Stop pouring */
          Serial_debug.println("so 3");
          // Send_data_EspNow(SLAVE_ID, STOP_POURING, 4); 
          // ui8_batdaucan=0;
          // ui2_ena_scale = false;
          ui2_detect = false; //reset the object detected flag
          // Serial_debug.println("Stop pouring so 3...");
          Serial_debug.println(ConfirmWeith_object_detected);
          break;
        }
        if(ConfirmWeith_object_detected < isDetected) //There is no object on the scale
        {
          /* Stop pouring */
          Serial_debug.println("so 4");
          // Send_data_EspNow(SLAVE_ID, STOP_POURING, 4);
          // ui8_batdaucan=0;
          // ui2_ena_scale = false;
          ui2_detect = false; //reset the object detected flag
          // Serial_debug.println("Stop pouring so 4..."); 
          Serial_debug.println(ConfirmWeith_object_detected);
          break;
        }
        else 
        {
          Serial_debug.print("ConfirmWeith_object_detected: ");
          Serial_debug.println(ConfirmWeith_object_detected);
          ui2_detect = false; //reset the object detected flag 
        }
      }
    }
}

void setup() {
    // put your setup code here, to run once:
    Wire.begin();
    Serial_debug.begin(115200);

    // inputString.reserve(200);

    scan_i2c();

    // initialize LCD
    lcd.init();
    // turn on LCD backlight                      
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("DU AN CAN BAO BI ");
    // lcd.setCursor(2,1);
    // lcd.print("BAO BI");
    lcd.setCursor(0,1);
    lcd.print("ALTA MEDIA COMPANY");
    lcd.setCursor(0,2);
    lcd.print("AUTHOR: DANHNGYYEN");
    delay(5000);
    lcd.clear();
    ui8_moichao = 1;
    ui32_timeout_hienthi = millis() + 500; // Thời gian hiển thị là 5 giây
} 


void serialEvent() {
  while (Serial_debug.available()) 
  {
    // get the new byte:
    char inChar = (char)Serial_debug.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
}
}

void Serial_process()
{
  // Serial_debug.println(inputString);

  if(inputString=="batdaucan\n")
  {
    Serial_debug.println("bat dau qua trinh can");
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
    if (stChar == 'C') // K0C2
    {
      a = StringValue.substring(1,sizeof(inputString));
      Serial_debug.print("so can nang nhan duoc la: ");
      // Serial_debug.println(a);   
      ui8_khoiluong=a.toFloat();
       Serial_debug.println(ui8_khoiluong);
      // Serial_debug.print("so can nang nhan duoc la: ");
      // Serial_debug.println(a);
      // lcd.clear();
      // lcd.setCursor(3,1);
      // lcd.print("KHOI LUONG LA:");
      // lcd.setCursor(17,1);
      // lcd.print(a);
      ui8_batdauhienthi=1;

    }
  }

}

void hienthi_khoiluong(float ui8_kl)
{
  if(ui8_batdauhienthi==0) return;
  Serial_debug.println(ui8_kl);
  Serial_debug.println(sizeof(String(ui8_kl))-3);
  char kl[20];
  char kl_num[10];
  dtostrf(ui8_kl, sizeof(String(ui8_kl))-3, 1, kl_num);   // 5: độ dài chuỗi, 2: số sau dấu thập phân
  sprintf(kl, "%sKG", kl_num);
  Serial_debug.println(kl);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("TIEN HANH DO");
  lcd.setCursor(0,1);
  lcd.print("KHOI LUONG LA:");
  lcd.setCursor(14,1);
  lcd.print(kl);
  ui32_timeout_hienthi = millis() + 5000; // Thời gian hiển thị là 5 giây
  ui8_moichao = 1;
  ui8_batdauhienthi=0;
}

void hienthi_moichao()
{
  if(ui8_moichao==1 && (millis() - ui32_timeout_hienthi < 5000))
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MOI BAN DAT BAO BI");
    ui8_moichao=0;
    // lcd.setCursor(0, 1);
    // lcd.print("DU AN CAN BAO BI");
    // lcd.setCursor(0, 2);
    // lcd.print("ALTA MEDIA COMPANY");
    // lcd.setCursor(0, 3);
    // lcd.print("AUTHOR DANHNGUYEN");
  }

}

void loop() {
  // put your main code here, to run repeatedly:
    // serialEvent();
   if (stringComplete) {
    Serial.println(inputString);
    // clear the string:
    Serial_process();
    inputString = "";
    stringComplete = false;
  }

  hienthi_khoiluong(ui8_khoiluong);
  hienthi_moichao();

}
