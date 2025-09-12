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
  scale.set_offset(offset_scale);
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
    // if(test_can==0) return;
    float weight = fabs(scale.get_units(5) / calibration_factor); // luôn dương
    weights=weight/1000.0,1;
    Serial.print("Khối lượng: ");
    Serial.print(weights); // đổi ra gram
    Serial.println(" kg");
    
    if(weights < 0.2)
    {
      ui8_prewest=0;
      ui8_moichao = 1;
      return;
    }

    if(fabs(weights - ui8_prewest) > 1)
    {
      ui8_batdauhienthi = 1;
      Serial_debug.println("nhay vao bat man hinh");
      ui8_prewest=weights;
    }

   
    // Serial.println(" kg");
    // test_can=0;

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


    Serial.println(F("Can: nhap \"w_ok\""));
    Serial_debug.println(F("Vao che do calib can: nhap \"calib\""));
    Serial_debug.println(F("Khai bao khoi luong: \"nhap so\""));
    Serial_debug.println(F("Tinh chinh gia tri Scale: nhap so co dau \"+\" hoac \"-\" phia truoc so"));

	doc_eeprom_offsetloadcell();
	init_scale(); // Khởi tạo cân
	doc_eeprom_w();

	lcd.setCursor(0,0);
  	lcd.print("KHOI LUONG LA:");	
	setup_pixel();

	ui32_timeoutkl=millis()+500; // Biến thời gian khóa không cho đo liên tục
} 


// void serialEvent() {
//   while (Serial_debug.available()) 
//   {
//     // get the new byte:
//     char inChar = (char)Serial_debug.read();
//     // add it to the inputString:
//     inputString += inChar;
//     // if the incoming character is a newline, set a flag so the main loop can
//     // do something about it:
//     if (inChar == '\n') {
//       stringComplete = true;
//     }
//   }
// }

void serialEvent()
{
	//inputString = "";
	while (Serial.available()){
		char inChar = (char)Serial.read();
		//Serial.print(inChar);
		if (inChar == '\n') {
			stringComplete = true;
			//Serial.print("Chuoi String: ");
			//Serial.println(inputString);
		}
		if(first_char){
			//Serial.print("Ki tu dau tien.");
			
			if((inChar == '+') || (inChar == '-')){
				// Serial.print("First char: ");
				// Serial.println(inChar);
				isNum = true;
			}
			if(isDigit(inChar)){
				// Serial.print("First char: ");
				// Serial.println(inChar);
				isKg = true;
			}
		}	
		if(stringComplete == false){
			if(calibLoadcell){
				if(isNum){
					inputNum += inChar;
				}	
				else if(isKg){
					inputKg += inChar;
					
				}
				else{
					inputString += inChar;
				}
			}
			else if(calibHight){
				//if(isKg){
					inputMet += inChar;
				//}
			}
			else if(input_time){
				
				inputTime += inChar;
				
			}
			else {
				inputString += inChar;
			}
			first_char = false;	
		}	
	}
}

void Serial_process()
{
  // Serial_debug.println(inputString);

  if(inputString=="batdaucan\n")
  {
    // Serial_debug.println("bat dau qua trinh can");
    calibrateOffset();
  }

  if(inputString=="calib")
  {
    Serial_debug.println("calib can");
    scale_value_calib = array_scale[0];
    Serial.print(F("Input text:  "));
    Serial.println(inputString);
    calibLoadcell = true;
    Serial.print(F("Calib Loadcell:  "));
    Serial.println(calibLoadcell);
    Serial.print(F("scale_value_calib:  "));
    Serial.println(scale_value_calib);
    
  }

  if(inputString == "exitcalib")
  {
			if(calibLoadcell){
				Serial.println(F("Thoat calib !"));
				calibLoadcell = false;
				doc_eeprom_w();
				
			}
			Serial.print(F("Input text:  "));
			Serial.println(inputString);
			
			Serial.print(F("Calib Loadcell:  "));
			Serial.println(calibLoadcell);
			
		}

  if (inputString == "w_reset"){
				// buzzer_calib();
				scale.power_up();
				scale.tare();
				ghi_eeprom_offsetloadcell();
				// inputString = "";
  }

  if(inputString == "w_ok")
  {
				//buzzer();
				measure_w();
				// inputString = "";
				// ok = true;
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

/* ---------------------- Ghi giá trị offset loadcell vào Eeprom ----------------------------*/
void ghi_eeprom_offsetloadcell() {
	
	long offset_value = scale.get_offset();
	byte* z = (byte*)(void*)&offset_value;
	for(unsigned int t = 0; t<sizeof(offset_value); t++){
		add_eeprom = t+28;
		EEPROM.write(add_eeprom, *z);
		
		//Serial.println(*q, HEX);
		delay(5);
		z++;
		
	}
	Serial.print (F("Gia tri offset ghi vao EEPROM = "));
	Serial.println (offset_value);
}
/* ---------------------- Đọc giá trị offset loadcell từ Eeprom ----------------------------*/
void doc_eeprom_offsetloadcell() {
	long offset_read ;
	byte* w = (byte*)(void*)&offset_read;
	for(uint16_t i = 0;i<sizeof(offset_read); i++){
		add_eeprom = i+100;
		*w = EEPROM.read(add_eeprom);
		
		delay(5);
		// Serial.print(*p, HEX);
		w++;
	}
	offset_scale = offset_read;
	Serial.print (F("Gia tri offset doc EEPROM khi khoi dong = "));
	Serial.println (offset_scale);
	
}

void doc_eeprom_w(){
	//Serial.println(F("Bat dau doc EEPROM !"));
	struct myObject 
	{
		double getvalue1;
		float scalevalue1;
		double getvalue2;
		float scalevalue2;
		double getvalue3;
		float scalevalue3;
		double getvalue4;
		float scalevalue4;
		double getvalue5;
		float scalevalue5;
	};
	unsigned int address_doc = 0;
	myObject data_eeprom;
	EEPROM.get(address_doc, data_eeprom); // Doc du lieu tu EEPROM.
	array_get [0] = data_eeprom.getvalue1;
	array_scale [0] = data_eeprom.scalevalue1;
	array_get [1] = data_eeprom.getvalue2;
	array_scale [1] = data_eeprom.scalevalue2;
	array_get [2] = data_eeprom.getvalue3;
	array_scale [2] = data_eeprom.scalevalue3;
	array_get [3] = data_eeprom.getvalue4;
	array_scale [3] = data_eeprom.scalevalue4;
	array_get [4] = data_eeprom.getvalue5;
	array_scale [4] = data_eeprom.scalevalue5;
	Serial.print(F("Get_value"));
	Serial.print(F("\t \t"));
	Serial.print(F("array_scale_calib"));
	Serial.println();
	for(byte i = 0; i<5; i++)
  {
		Serial.print(array_get [i]);
		Serial.print(F("\t"));
		Serial.print(array_scale [i]);
		Serial.println();
	}
	Serial.println("Da doc xong EEPROM !");
}

void hienthi_khoiluong(float ui8_kl)
{
  if(ui8_batdauhienthi==0) return;
//   ui32_timeoutkl=millis();

  Serial_debug.println("bat dau hien thi khoi luong");
  Serial_debug.println(ui8_kl);
  Serial_debug.println(sizeof(String(ui8_kl))-3);
  char kl[20];
  char kl_num[10];
  dtostrf(ui8_kl, sizeof(String(ui8_kl))-3, 2, kl_num);   // 5: độ dài chuỗi, 2: số sau dấu thập phân
  sprintf(kl, "%sKG", kl_num);
  Serial_debug.println(kl);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("KHOI LUONG LA:");
  
  if(ui8_kl>0 && ui8_kl<1)
  {
	lcd.setCursor(14,0);
  	lcd.print(kl);	
  }

  else if(ui8_kl>=1)
  {
	lcd.setCursor(14,0);
  	lcd.print(kl);	
	lcd.setCursor(8,1);
  	lcd.print("OVERLOAD");
  }

  else 
  {
	lcd.setCursor(14,0);
  	lcd.print("      ");
  }
  

//   ui32_timeout_hienthi = millis() + 5000; // Thời gian hiển thị là 5 giây
//   ui8_moichao = 1;
  ui32_timeoutkl=millis()+1000;
  ui8_batdauhienthi=0;
}

void hienthi_moichao()
{
  if(ui8_moichao==1 && (millis() - ui32_timeout_hienthi < 5000))
  {
    lcd.clear();
    // lcd.setCursor(0, 0);
    // lcd.print("MOI BAN DAT BAO BI");
    ui8_moichao=0;
    // lcd.setCursor(0, 1);
    // lcd.print("DU AN CAN BAO BI");
    // lcd.setCursor(0, 2);
    // lcd.print("ALTA MEDIA COMPANY");
    // lcd.setCursor(0, 3);
    // lcd.print("AUTHOR DANHNGUYEN");
  }
}

void calib_can()
{
  if(calibLoadcell)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DANG CALIB CAN");
    if(isNum){
			Serial.print(F("input = "));
			Serial.println(inputNum);
			Serial.print(F("Chuoi nhan duoc la so:"));
			Serial.println(inputNum.toInt());
			scale_value_calib += inputNum.toInt(); 	//Chuyển String thành số Int.
			Serial.print(F("Gia tri sau thay doi: "));
			Serial.println(scale_value_calib);
			inputString = "";
			inputNum = "";
			isNum = false;
			stringComplete = false;
			//first_char = true;
		}
		else if(isKg){
			float kg;
			Serial.print(F("input = "));
			Serial.println(inputKg);
			Serial.print(F("Chuoi nhan duoc la khoi luong:"));
			Serial.println(inputKg.toFloat());
			kg =  inputKg.toFloat(); 	//Chuyển String thành số Int.
			scale_value_calib = float(get_value_sub/kg);
			Serial.print(F("Scale_value tinh duoc: "));
			Serial.println(scale_value_calib);
			inputString = "";
			inputKg = "";
			isKg = false;
			stringComplete = false;
			//first_char = true;
		}

    else {
			Serial.print(F("input = "));
			Serial.println(inputString);
			Serial.print(F("Chuoi nhan duoc la text:"));
			Serial.println(inputString);
			thuc_hien();
			inputString = "";
			stringComplete = false;
			
		}

    scale.set_scale(scale_value_calib);

		doc_analog = scale.read_average(5);
		get_value_sub = scale.get_value(5);
		
		khoiluong = scale.get_units(5);
		Serial.print(doc_analog);
		Serial.print(F("\t\t"));
		Serial.print(get_value_sub);
		Serial.print(F("\t\t"));
		
		Serial.print(khoiluong, 2);	
		Serial.print(F(" Kg \t"));
		Serial.print (F("scale value = "));
		Serial.println(scale_value_calib);
		scale.power_down();			        // put the ADC in sleep mode
		delay(1000);
		scale.power_up();
    
  }
}

void measure_w()
{
	if(ui32_timeoutkl > millis() || ui8_batdauhienthi==1 || ui8_calib==1) return;

	unsigned long time_now = millis();
	unsigned long time_do = millis();
	unsigned long time_wait = millis();
	const byte duration = 150;
	boolean status_bip = true;
	float a = 0, sub = 0, temp;
	boolean first = false;
	float scale_sample, get_sample;
	SimpleKalmanFilter bo_loc(2, 2, 0.1);
	bo_loc.deleteEstimate();
	scale.power_up();
	while(!first)
	{
		if((millis()-time_wait) >= 800){
			get_sample = scale.get_value(5);
			Serial.print(get_sample);
			Serial.print("\t");
			if(get_sample <= array_get[0]){
				scale_value = array_scale[0];
				scale.set_scale(scale_value);
			}
			else if((array_get[0] < get_sample) && (get_sample<= array_get[1])){
				scale_value = array_scale[1];
				scale.set_scale(scale_value);
			}
			else if(((array_get[1] < get_sample) && (get_sample<= array_get[2])) || (get_sample > array_get[2])){
				scale_value = array_scale[2];
				scale.set_scale(scale_value);
			}
			else if(((array_get[2] < get_sample) && (get_sample<= array_get[3])) || ((get_sample > array_get[3]) && (get_sample <= array_get[4])))
			{
				scale_value = array_scale[3];
				scale.set_scale(scale_value);
			}
			// else if(((array_get[3] < get_sample) && (get_sample<= array_get[4])) || (get_sample > array_get[4]))
			// {
			// 	scale_value = array_scale[4];
			// 	scale.set_scale(scale_value);
			// }
			Serial.print("gia tri scale value: ");
			Serial.println(scale_value);
			temp = scale.get_units(5);
			bo_loc.update(temp);
			first = true;
		}
	}
	if(first)
	{
		for (byte i = 0; i<lando; i++)
		{
			// if((millis() - time_now) >= duration){
			// 	status_bip =!status_bip;
			// 	time_now = millis();
			// }
			a = scale.get_units(1);
			//Serial.print("lan do  ");
			//Serial.print(i+1);
			//Serial.print(": ");
			//Serial.print(a,2);
			//Serial.print(";\t");
			sub =  bo_loc.updateEstimate(a);
			//Serial.print("Gtri loc Kalman: ");
			//Serial.println(sub);
			weights = sub;
			// weightsTemp=sub;
			//delay(1);
		}
		

		if(weights<=0.1)
		{
			Serial.print(F("Khoi luong tam thoi: "));
			Serial.print(weightsTemp);	
			Serial.println(F(" Kg"));

			weights=0;
			if(fabs(weights - weightsTemp) > 0.01)
			{
				// weightsTemp=weights;
				ui8_batdauhienthi = 1;
			}
			
			Serial.print(F("Khoi luong: "));
			Serial.print(weights);	
			Serial.println(F(" Kg"));
			weightsTemp=weights;
			first = false;
			ui32_timeoutkl=millis() + 1000; // Biến thời gian
		}
		if(weights>0.1)
		{
			Serial.print(F("Khoi luong tam thoi khi so ky lon hon 0: "));
			Serial.print(weightsTemp);	
			Serial.println(F(" Kg"));

			if(fabs(weights - weightsTemp) > 0.01)
			{
				// weightsTemp=weights;
				Serial_debug.println("phat hien so can khac");
				ui8_batdauhienthi = 1;
			}
			Serial.print(F("Khoi luong: "));
			Serial.print(weights);	
			Serial.println(F(" Kg"));
			weightsTemp=weights;
			first = false;
			ui32_timeoutkl=millis() + 1000; // Biến thời gian
		}
		
	}
	
}

void thuc_hien(){
	if(inputString == "save 1"){
		address = 0;
		myObject Save1 = {get_value_sub, scale_value_calib};
		EEPROM.put(address, Save1);
		Serial.print(F("Dia chi bat dau: "));
		Serial.println(address);
		Serial.println(F("Da ghi du lieu thu 1 !"));
	}
	else if(inputString == "save 2"){
		address = 8;
		myObject Save2 = {get_value_sub, scale_value_calib};
		EEPROM.put(address, Save2);
		Serial.print(F("Dia chi bat dau: "));
		Serial.println(address);
		Serial.println(F("Da ghi du lieu thu 2 !"));
	}
	else if(inputString == "save 3"){
		address = 16;
		myObject Save3 = {get_value_sub, scale_value_calib};
		EEPROM.put(address, Save3);
		Serial.print(F("Dia chi bat dau: "));
		Serial.println(address);
		Serial.println(F("Da ghi du lieu thu 3 !"));
	}

	else if(inputString == "save 4"){
		address = 24;
		myObject Save4 = {get_value_sub, scale_value_calib};
		EEPROM.put(address, Save4);
		Serial.print(F("Dia chi bat dau: "));
		Serial.println(address);
		Serial.println(F("Da ghi du lieu thu 4 !"));
	}

	else if(inputString == "save 5"){
		address = 32;
		myObject Save5 = {get_value_sub, scale_value_calib};
		EEPROM.put(address, Save5);
		Serial.print(F("Dia chi bat dau: "));
		Serial.println(address);
		Serial.println(F("Da ghi du lieu thu 5 !"));
	}

	else if(inputString == "readAll"){
		unsigned int getAddress = 0;
		struct getAlldata {
			float getValue1;
			float getScale1;
			float getValue2;
			float getScale2;
			float getValue3;
			float getScale3;
			float getValue4;
			float getScale4;
			float getValue5;
			float getScale5;
			
		};
		getAlldata Data;
		EEPROM.get(getAddress, Data);
		Serial.println(F("Du lieu doc duoc: "));
		// Cap du lieu 1
		Serial.print(F("Get value 1:  "));
		Serial.print(Data.getValue1);
		Serial.print(F("\t"));
		Serial.print(F("Get Get scale 1:  "));
		Serial.println(Data.getScale1);
		// Cap du lieu 2
		Serial.print(F("Get value 2:  "));
		Serial.print(Data.getValue2);
		Serial.print(F("\t"));
		Serial.print(F("Get Get scale 2:  "));
		Serial.println(Data.getScale2);
		// // Cap du lieu 3
		Serial.print(F("Get value 3:  "));
		Serial.print(Data.getValue3);
		Serial.print(F("\t"));
		Serial.print(F("Get Get scale 3:  "));
		Serial.println(Data.getScale3);

		Serial.print(F("Get value 4:  "));
		Serial.print(Data.getValue4);
		Serial.print(F("\t"));
		Serial.print(F("Get Get scale 4:  "));
		Serial.println(Data.getScale4);

		Serial.print(F("Get value 5:  "));
		Serial.print(Data.getValue5);
		Serial.print(F("\t"));
		Serial.print(F("Get Get scale 5:  "));
		Serial.println(Data.getScale5);
		
	}
}

void loop() {
  // put your main code here, to run repeatedly:
    // serialEvent();
   if (stringComplete==true) 
	{
		Serial.print(F("stringComplete :"));
		Serial.println(inputString);

		if(inputString == "calib_can")
		{
			scale_value_calib = array_scale[0];
			Serial.print(F("Input text:  "));
			Serial.println(inputString);
			calibLoadcell = true;
			Serial.print(F("Calib Loadcell:  "));
			Serial.println(calibLoadcell);
			Serial.print(F("scale_value_calib:  "));
			Serial.println(scale_value_calib);
			ui8_calib=1;
			inputString = "";
			stringComplete = false;
		}

		if(inputString == "exitcalib")
		{
			if(calibLoadcell)
			{
				Serial.println(F("Thoat calib !"));
				if(khoiluong<0.1)
				{
					khoiluong=0;
				}
				calibLoadcell = false;
				doc_eeprom_w();
				lcd.clear();
				lcd.setCursor(4, 0);
				lcd.print("CALIB DONE");
				// lcd.setCursor(0, 1);
				// lcd.print(F("SO KY CALIB: "));
				// lcd.print(khoiluong);
				// lcd.print(F(" Kg"));
				weights=0;
				weightsTemp=0;
				scale.power_down();		
				ui32_timeout_hienthi = millis() + 6000; // Thời gian hiển thị là 5 giây
				ui32_timeoutkl=millis()+5000; // Biến thời gian khóa không cho đo liên tục
				ui8_calib=0;
				ui8_moichao = 1;
			}
			Serial.print(F("Input text:  "));
			Serial.println(inputString);
			
			Serial.print(F("Calib Loadcell:  "));
			Serial.println(calibLoadcell);
			
			
			inputString = "";
			stringComplete = false;
		}

		if(!calibLoadcell && !input_time)
		{
			if(inputString == "w_ok"){
				//buzzer();
				measure_w();
			} 
			inputString = "";	
			stringComplete = false;
		}
		first_char = true;
	}

	if(calibLoadcell)
	{
		scale.power_up();
		Serial.println(F("Dang calib can !"));
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("DANG CALIB CAN.......");
		if(isNum){
			Serial.print(F("input = "));
			Serial.println(inputNum);
			Serial.print(F("Chuoi nhan duoc la so:"));
			Serial.println(inputNum.toInt());
			scale_value_calib += inputNum.toInt(); 	//Chuyển String thành số Int.
			Serial.print(F("Gia tri sau thay doi: "));
			Serial.println(scale_value_calib);
			inputString = "";
			inputNum = "";
			isNum = false;
			stringComplete = false;
			//first_char = true;
		}
		else if(isKg){
			float kg;
			Serial.print(F("input = "));
			Serial.println(inputKg);
			Serial.print(F("Chuoi nhan duoc la khoi luong:"));
			Serial.println(inputKg.toFloat());
			kg =  inputKg.toFloat(); 	//Chuyển String thành số Int.
			scale_value_calib = float(get_value_sub/kg);
			Serial.print(F("Scale_value tinh duoc: "));
			Serial.println(scale_value_calib);
			inputString = "";
			inputKg = "";
			isKg = false;
			stringComplete = false;
			//first_char = true;
		}
		else {
			Serial.print(F("input = "));
			Serial.println(inputString);
			Serial.print(F("Chuoi nhan duoc la text:"));
			Serial.println(inputString);
			thuc_hien();
			inputString = "";
			stringComplete = false;
			
		}
		
		scale.set_scale(scale_value_calib);

		doc_analog = scale.read_average(5);
		get_value_sub = scale.get_value(5);
		
		khoiluong = scale.get_units(5);
		Serial.print(doc_analog);
		Serial.print(F("\t\t"));
		Serial.print(get_value_sub);
		Serial.print(F("\t\t"));
		
		Serial.print(khoiluong, 2);	
		Serial.print(F(" Kg \t"));
		Serial.print (F("scale value = "));
		Serial.println(scale_value_calib);
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(F("SO KY CALIB: "));
		lcd.print(khoiluong);
		lcd.print(F(" Kg"));
		scale.power_down();			        // put the ADC in sleep mode
		delay(1000);
		scale.power_up();
	}
	scale.power_down();			        // put the ADC in sleep mode
	//   hienthi_khoiluong(ui8_khoiluong);
	sangled();
  	measure_w();
	hienthi_khoiluong(weights);
	// hienthi_moichao();
	}

void setup_pixel()
{
	pixels1.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
	pixels1.show();            // Turn OFF all pixels ASAP
	pixels1.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void tatled()
{
	for(int led1_chopchop=0;led1_chopchop<LED_COUNT;led1_chopchop++)
	{
		pixels1.setPixelColor(led1_chopchop,pixels1.Color(0,0,0));
	}
	pixels1.show();
	// digitalWrite(LED_ONBOARD,0);
}

void ledred()
{
	for(int led1_chopchop=0;led1_chopchop<LED_COUNT;led1_chopchop++)
	{
		pixels1.setPixelColor(led1_chopchop,pixels1.Color(255,0,0));
	}
	pixels1.show();
	// digitalWrite(LED_ONBOARD,0);
}

void ledgreen()
{
	for(int led1_chopchop=0;led1_chopchop<LED_COUNT;led1_chopchop++)
	{
		pixels1.setPixelColor(led1_chopchop,pixels1.Color(0,255,0));
	}
	pixels1.show();
	// digitalWrite(LED_ONBOARD,1);
}	

void ledblue()
{
	for(int led1_chopchop=0;led1_chopchop<LED_COUNT;led1_chopchop++)
	{
		pixels1.setPixelColor(led1_chopchop,pixels1.Color(0,0,255));
	}
	pixels1.show();
	// digitalWrite(LED_ONBOARD,1);
}

void sangled()
{
	if(weights>1)
	{
		// Serial_debug.println("sang led xanh");
		ledred();	
		// led_overload();
		// ui32_timeoutkl=millis()+1000;
	}
	else 
	{
		// Serial_debug.println("tat led");
		tatled();
	}
}

void led_overload()
{
	if(millis()>=time_chopchop)
	if(led_state==1)
	{
		ledred();
		led_state=0;
		time_chopchop=millis()+500;
	}

	else
	{
		tatled();
		led_state=1;
		time_chopchop=millis()+200;
	}
}




