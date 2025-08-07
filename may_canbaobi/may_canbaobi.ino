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
          // ui2_detect = false; //reset the object detected flag
          Serial_debug.println("Stop pouring so 3...");
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
          // ui2_detect = false; //reset the object detected flag
          Serial_debug.println("Stop pouring so 4..."); 
          Serial_debug.println(ConfirmWeith_object_detected);
          break;
        }
        else 
        {
          // Serial_debug.print("ConfirmWeith_object_detected: ");
          // Serial_debug.println(ConfirmWeith_object_detected);
        }
      }
    }
}

void setup() {
    // put your setup code here, to run once:
    Wire.begin();
    Serial_debug.begin(115200);
    scan_i2c();

    // initialize LCD
    lcd.init();
    // turn on LCD backlight                      
    lcd.backlight();
} 

void loop() {
  // put your main code here, to run repeatedly:

}
