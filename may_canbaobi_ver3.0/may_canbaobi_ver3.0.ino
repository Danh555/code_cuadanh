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
  EEPROM.get(64, offset);
  EEPROM.get(128,ui8_bienluumau);
  EEPROM.get(138,ui8_gioihan_do);
  Serial_debug.print("Offset read from EEPROM: ");
  Serial_debug.println(offset);
  scale.set_offset(offset);
//   scale.tare();
}

void calibrateOffset()
{
	if(ui8_tienhanh_tare==0) return;
	scale.power_up();
  Serial_debug.println("The software is adjusting offset...");
  delay(10);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("CALIB PROCESS.......");
//   scale.set_offset(0);
  scale.tare(10); 
  offset = scale.read_average(20);
//   offset = scale.get_offset();
  EEPROM.put(64, offset);
//   EEPROM.commit(); // Save the offset to EEPROM
  Serial_debug.println("Adjust done!");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("CALIB DONE!");
  delay(1000);
  lcd.clear();
lcd.setCursor(0,0);
lcd.print("SETTING MODE:");
lcd.setCursor(0,1);
lcd.print("1: CHOOSE COLOR");
lcd.setCursor(0,2);
lcd.print("2: CALIB LOADCELL");
lcd.setCursor(0,3);
lcd.print("HOLD BUTTON: EXIT");
  scale.power_down();
//   ui8_calib=0;
  scale.set_offset(offset);
  Serial_debug.print("New offset: ");
  Serial_debug.println(offset);

  ui8_tienhanh_tare=0;
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

	pinMode(buttonPin, INPUT_PULLUP);
    scan_i2c();

    // initialize LCD
    lcd.init();
    // turn on LCD backlight                      
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("WELCOME TO USE");
    lcd.setCursor(0,1);
    lcd.print("ALTA MEDIA COMPANY");
    lcd.setCursor(0,2);
    lcd.print("AUTHOR: DANHNGYYEN");
    delay(3000);
    lcd.clear();
    ui8_moichao = 1;
    ui32_timeout_hienthi = millis() + 500; // Thời gian hiển thị là 5 giây
	ui8_landau=1;

    Serial.println(F("Can: nhap \"w_ok\""));
    Serial_debug.println(F("Vao che do calib can: nhap \"calib\""));
    Serial_debug.println(F("Khai bao khoi luong: \"nhap so\""));
    Serial_debug.println(F("Tinh chinh gia tri Scale: nhap so co dau \"+\" hoac \"-\" phia truoc so"));

	doc_eeprom_offsetloadcell();
	init_scale(); // Khởi tạo cân
	doc_eeprom_w();
	// tatled();

	// lcd.setCursor(0,0);
  	// lcd.print("KHOI LUONG LA:");	
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
	if(ui8_mode_calib==0) return;
	while(Serial.available()){
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
		float offsetvalue;
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
  lcd.setCursor(4,0);
  lcd.print("WEIGHT:");

  if(ui8_kl>=ui8_gioihan_do)
  {
	lcd.setCursor(11,0);
  	lcd.print(kl);	
	lcd.setCursor(6,1);
  	lcd.print("OVERLOAD");
  }
  else 
  {
	lcd.setCursor(11,0);
  	lcd.print(kl);	
  }

//   ui32_timeout_hienthi = millis() + 5000; // Thời gian hiển thị là 5 giây
//   ui8_moichao = 1;
//   ui32_timeoutkl=millis()+1000;
  ui32_tg_bamnut=millis() + 500;
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
    lcd.print("CALIB PROCESS.......");
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
	if(ui32_timeoutkl > millis() || ui8_batdauhienthi==1 || ui8_calib==1 || ui8_tienhanh_tare==1 || ui8_bamnut==1) return;

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
		if((millis()-time_wait) >= 500){
			get_sample = scale.get_value(5);
			// Serial.print(get_sample);
			// Serial.print("\t");
			if(get_sample <= array_get[0]){
				Serial_debug.println("lay gia tri scale 0");
				scale_value = array_scale[0];
				scale.set_scale(scale_value);
			}
			else if((array_get[0] < get_sample) && (get_sample<= array_get[1]))
			{
				Serial_debug.println("lay gia tri scale 1");
				scale_value = array_scale[1];
				scale.set_scale(scale_value);
			}
			else if(((array_get[1] < get_sample) && (get_sample<= array_get[2])))
			{
				Serial_debug.println("lay gia tri scale 2");
				scale_value = array_scale[2];
				scale.set_scale(scale_value);
			}
			else if(((array_get[2] < get_sample) && (get_sample<= array_get[3])))
			{
				Serial_debug.println("lay gia tri scale 3");
				scale_value = array_scale[3];
				scale.set_scale(scale_value);
			}
			else 
			{
				Serial_debug.println("lay gia tri scale 4");
				scale_value = array_scale[4];
				scale.set_scale(scale_value);	
			}
			temp = scale.get_units(2);
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
			a = scale.get_units(2);
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
		scale.power_down();
		

		if(weights<=0.05)
		{
			// Serial.print(F("Khoi luong tam thoi: "));
			// Serial.print(weightsTemp);	
			// Serial.println(F(" Kg"));
			weights=0;
			if(fabs(weights - weightsTemp) > 0.01 || ui8_landau==1)
			{
				// weightsTemp=weights;
				ui8_hienthiled=1;
				ui8_batdauhienthi = 1;
				ui8_landau=0;
			}
			
			Serial.print(F("Khoi luong: "));
			Serial.print(weights);	
			Serial.println(F(" Kg"));
			weightsTemp=weights;
			first = false;
			ui32_timeoutkl=millis() + 3000; // Biến thời gian
			// ui32_tg_bamnut=millis() + 2000;
			ui8_bamnut=1;
		}
		if(weights>0.05)
		{
			// Serial.print(F("Khoi luong tam thoi khi so ky lon hon 0: "));
			// Serial.print(weightsTemp);	
			// Serial.println(F(" Kg"));

			if(fabs(weights - weightsTemp) > 0.01)
			{
				// weightsTemp=weights;
				ui8_hienthiled=1;
				Serial_debug.println("phat hien so can khac");
				ui8_batdauhienthi = 1;
			}
			Serial.print(F("Khoi luong: "));
			Serial.print(weights);	
			Serial.println(F(" Kg"));
			weightsTemp=weights;
			first = false;
			ui32_timeoutkl=millis() + 3000; // Biến thời gian
			// ui32_tg_bamnut=millis() + 2000;
			ui8_bamnut=1;
		}
		
	}
	
}

void thuc_hien(){
	if(inputString == "save 1")
	{
		address = 0;
		myObject Save1 = {get_value_sub, scale_value_calib};
		EEPROM.put(address, Save1);
		Serial.print(F("Dia chi bat dau: "));
		Serial.println(address);
		Serial.println(F("Da ghi du lieu thu 1 !"));
	}
	else if(inputString == "save 2")
	{
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

int serial_printf(const char *fmt, ...) {
  char buf[64];  // Tăng/giảm tùy bộ nhớ (Uno chỉ có 2KB RAM)
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);
  return Serial.print(buf);
}

void Serial_test()
{

	if (stringComplete==true) 
	{
		Serial.print(F("stringComplete :"));
		Serial.println(inputString);
		// serial_printf("stringComplete : %s", inputString.c_str());

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
			ui8_bamnut=0;
			inputString = "";
			stringComplete = false;
		}

		if(inputString == "test calib")
		{
			// buzzer_calib();
			// calibrateOffset();
			// ui8_calib=1;
			ui8_tienhanh_tare=1;
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
				// ui8_bamnut=1;
				weights=0;
				weightsTemp=0;
				scale.power_down();		
				ui32_timeout_hienthi = millis() + 6000; // Thời gian hiển thị là 5 giây
				ui32_timeoutkl=millis()+5000; // Biến thời gian khóa không cho đo liên tục
				ui8_calib=0;
				ui8_landau=1;
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
		lcd.print("CALIB PROCESS.......");
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
}

void loop() {
  // put your main code here, to run repeatedly:
	if(ui8_mode_calib==1)
	{
		Serial_test();
	}

	if(ui8_mode_calib==0)
	{
		sangled(ui8_bienluumau);
		measure_w();
		hienthi_khoiluong(weights);	
	}

	calibrateOffset();
	button_chucnang();
	if(ui8_solan_bamnut>0 && pressStartTime<=millis())
	{
		int reading = digitalRead(buttonPin);
		if (reading == LOW) 
		{ // Button is pressed
			Serial_debug.println("THOAT");
			lcd.clear();
			lcd.setCursor(0,0);
			lcd.print("EXIT SETTING MODE");
			ui8_landau=1;
			// ui8_solan_bamnut++;
			weights=0;
			weightsTemp=0;
			ui8_modesetting=0;
			ui8_calib=0;
			ui8_bamnut=0;
			ui8_solan_bamnut=0;
			check_nutthoat=0;
			ui8_mode_calib=0;

		}
		else
		{
			if(ui8_solan_bamnut==1)
			{
				Serial.println("Ban bam nut 1 lan");
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("CHOOSE COLLOR");
				lcd.setCursor(0,1);
				lcd.print("1: RED 2: GREEN");
				lcd.setCursor(0,2);
				lcd.print("3: BLUE 4: LIMIT");
				ui8_nut1_bam=1;
				ui32_lasttime_bamnut1=millis()+10000;
				ui8_solan_bamnut=0;
				check_nutthoat=0;
			}

			if(ui8_solan_bamnut==2)
			{
				Serial.println("Ban bam nut 2 lan");
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("MODE CALIB");
				ui8_tienhanh_tare=1;
				ui8_solan_bamnut=0;
				check_nutthoat=0;
			}

			if(ui8_solan_bamnut==3)
			{
				Serial.println("Ban bam nut 3 lan");
				Serial_debug.println("thoat chế độ settings");
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("MODE CALIB");
				// ui8_landau=1;
				// // ui8_solan_bamnut++;
				// weights=0;
				// weightsTemp=0;
				// ui8_modesetting=0;
				// ui8_calib=0;
				// ui8_bamnut=0;
				ui8_solan_bamnut=0;
				// check_nutthoat=0;
				// ui8_mode_calib=0;
				ui8_mode_calib=1;
			}
		}

	}
	// button_chucnang_TEST();
	// hienthi_moichao();
	if(ui8_nut1_bam==1 && ui32_lasttime_bamnut1<=millis())
	{
		Serial_debug.println("het thoi gian chon mau");
		lcd.clear();
		lcd.setCursor(0,0);		
		lcd.print("BACK SETTING MODE");
		delay(2000);
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("SETTING MODE:");
		lcd.setCursor(0,1);
		lcd.print("1: CHOOSE COLOR");
		lcd.setCursor(0,2);
		lcd.print("2: CALIB LOADCELL");
		lcd.setCursor(0,3);
		lcd.print("HOLD BUTTON: EXIT");
		ui8_nut1_bam=0;
	}

	if(ui8_solan_bamnut_nut1>0 && pressStartTime<=millis())
	{
		int reading = digitalRead(buttonPin);
		if (reading == LOW) 
		{ // Button is pressed
			Serial_debug.println("THOAT CHON MAU");
			// lcd.clear();
			// lcd.setCursor(0,0);
			// lcd.print("EXIT SETTING MODE");
			ui32_lasttime_bamnut1=millis()+100;
			ui8_solan_bamnut_nut1=0;
			// ui8_mode_calib=0;

		}
		else
		{
			if(ui8_solan_bamnut_nut1==1)
			{
				Serial.println("Sang led do");
				ui8_bienluumau=1;
				EEPROM.put(128,ui8_bienluumau);
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("CHOOSE LED RED");
				ui32_lasttime_bamnut1=millis()+2000;
				ui8_solan_bamnut_nut1=0;
			}

			if(ui8_solan_bamnut_nut1==2)
			{
				Serial.println("Sang led xanh");
				ui8_bienluumau=2;
				EEPROM.put(128,ui8_bienluumau);
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("CHOOSE LED GREEN");
				ui32_lasttime_bamnut1=millis()+2000;
				ui8_solan_bamnut_nut1=0;
			}

			if(ui8_solan_bamnut_nut1==3)
			{
				Serial.println("Sang led xanh");
				ui8_bienluumau=3;
				EEPROM.put(128,ui8_bienluumau);
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("CHOOSE LED BLUE");
				ui32_lasttime_bamnut1=millis()+2000;
				ui8_solan_bamnut_nut1=0;
			}

			if(ui8_solan_bamnut_nut1==4)
			{
				Serial.println("setting overload");
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("LIMITED WEIGHT: ");		
				// lcd.clear();
				// lcd.setCursor(0,0);
				// lcd.print("BACK SETTING MODE");
				ui32_lasttime_bamnut1=millis()+60000;
				pressStartTime=millis()+20000;
				ui8_setting_gioihan=1;
				// ui8_nut1_bam=0;
				ui8_solan_bamnut_nut1=0;
				
				
			}
		}
		
	}

	if(ui8_setting_gioihan>0 && pressStartTime<=millis())
	{
		// int reading = digitalRead(buttonPin);
		// if (reading == LOW) 
		// { // Button is pressed
			EEPROM.put(138,ui8_gioihan_do);
			Serial_debug.println("THOAT CHON MAU");
			ui32_lasttime_bamnut1=millis()+100;
			ui8_setting_gioihan=0;
			// ui8_gioihan_do=0;
		// }

	}

	if(ui8_modesetting==1 && ui32_timeout_modesetting<=millis())
	{
		Serial_debug.println("het thoi gian settings");
		lcd.clear();
		lcd.setCursor(0,0);		
		lcd.print("EXIT SETTING MODE");
		ui8_landau=1;
		weights=0;
		weightsTemp=0;
		ui8_modesetting=0;
		ui8_calib=0;
		ui8_bamnut=0;
		ui8_solan_bamnut=0;
		ui8_nut1_bam=0;
		ui8_solan_bamnut_nut1=0;
		ui8_mode_calib=0;
	}
}


void setup_pixel()
{
	pixels1.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
	pixels1.show();            // Turn OFF all pixels ASAP
	pixels1.setBrightness(155); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void tatled()
{
	for(uint16_t led1_chopchop=0;led1_chopchop<LED_COUNT;led1_chopchop++)
	{
		// Serial_debug.println(led1_chopchop);
		pixels1.setPixelColor(led1_chopchop,pixels1.Color(0,0,0));
		// delay(100);
		
	}
	pixels1.show();
	
	// digitalWrite(LED_ONBOARD,0);
}

void ledred()
{
	// pixels1.clear(); // Set all pixel colors to 'off'
	for(uint16_t led1_chopchop=0;led1_chopchop<LED_COUNT;led1_chopchop++)
	{
		pixels1.setPixelColor(led1_chopchop,pixels1.Color(255,0,0));
		// delay(10);
	}
	pixels1.show();
	// digitalWrite(LED_ONBOARD,0);
}

void ledgreen()
{
	for(uint16_t led1_chopchop=0;led1_chopchop<LED_COUNT;led1_chopchop++)
	{
		pixels1.setPixelColor(led1_chopchop,pixels1.Color(0,255,0));
		delay(10);
	}
	pixels1.show();
	// digitalWrite(LED_ONBOARD,1);
}	

void ledblue()
{
	for(uint16_t led1_chopchop=0;led1_chopchop<LED_COUNT;led1_chopchop++)
	{
		pixels1.setPixelColor(led1_chopchop,pixels1.Color(0,0,255));
		delay(10);
	}
	pixels1.show();
	// digitalWrite(LED_ONBOARD,1);
}

void sangled(uint8_t collor)
{
	// if(ui8_mode_calib==1) return;
	if(weights>ui8_gioihan_do)
	{
		if(collor==1)
		{
			// Serial_debug.println("sang led do");
			ledred();
		}
		else if(collor==2)
		{
			// Serial_debug.println("sang led xanh");
			ledgreen();	
		}
		else if(collor==3)
		{
			// Serial_debug.println("sang led xanh");
			ledblue();	
		}

		// ui8_hienthiled=0;
	}
	else if(weights<=ui8_gioihan_do)
	{
		// Serial_debug.println("tat led");
		tatled();
		// ui8_hienthiled=0;
		// ledblue();
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


void button_chucnang()
{
	if(ui8_bamnut==0 && ui32_tg_bamnut<=millis()) return;
	// ui8_solan_bamnut=0;
	int reading = digitalRead(buttonPin);

	// Chống dội
	if(ui8_modesetting==0)
	{
		if (reading != lastButtonState) 
		{
			lastDebounceTime = millis();
		}
			if((millis() - lastDebounceTime) > debounceDelay-200) 
			{
				if (reading != buttonState) {
					buttonState = reading;
					if (buttonState == LOW) 
					{
						// Nút được nhấn
						weights=0;
						weightsTemp=0;
						Serial_debug.println("Nút nhấn bấm vào chế độ settings");
						lcd.clear();
						lcd.setCursor(0,0);
						lcd.print("SETTING MODE:");
						lcd.setCursor(0,1);
						lcd.print("1: CHOOSE COLOR");
						lcd.setCursor(0,2);
						lcd.print("2: CALIB LOADCELL");
						lcd.setCursor(0,3);
						lcd.print("HOLD BUTTON: EXIT");
						ui32_timeout_modesetting=millis()+60000; // Thời gian chờ vào chế độ settings
						// ui8_solan_bamnut++;
						ui8_modesetting=1;
						ui8_calib=1;
						
						// ui8_tienhanh_tare=1;
					}
					
				}
				else 
				{
					// lcd.clear();
					// lcd.setCursor(0,0);
					// lcd.print("KHONG CO AI BAM SETTINGS MODE");
					// Serial_debug.println("Nút nhấn bấm vào chế độ settings KHONG DUOC BAM THOAT");
					ui32_timeoutkl=millis()+100;
					ui8_modesetting=0;
					ui8_calib=0;
					ui8_bamnut=0;
				}
			}
	}
	

	if(ui8_modesetting==1 && ui8_nut1_bam==0 && ui8_setting_gioihan==0)
	{
		int reading = digitalRead(buttonPin);

		if (reading != lastButtonState) 
		{
			lastClickTime = millis();
		}

		if((millis() - lastClickTime) > 50)
		{	
			if (reading != buttonState) 
			{
				buttonState = reading;
				if(buttonState == LOW) 
				{
					// Nút được nhấn
					ui8_solan_bamnut++;
					if(ui8_solan_bamnut>3)
					{
						ui8_solan_bamnut=0;
					}
					lastClickTime = millis();
					pressStartTime=millis()+1500;
					ui32_timeout_modesetting=millis()+60000; // Thời gian chờ vào chế độ settings
					Serial_debug.print("So lan bam nut: ");
					Serial_debug.println(ui8_solan_bamnut);
				}
			}
		}

		lastButtonState = reading;
	}
	if(ui8_modesetting==1 && ui8_nut1_bam==1 && ui8_setting_gioihan==0)
	{
		int reading = digitalRead(buttonPin);

		if (reading != lastButtonState) 
		{
			lastClickTime = millis();
		}

		if((millis() - lastClickTime) > 50)
		{	
			if (reading != buttonState) 
			{
				buttonState = reading;
				if(buttonState == LOW) 
				{
					// Nút được nhấn
					ui8_solan_bamnut_nut1++;
					if(ui8_solan_bamnut_nut1>4)
					{
						ui8_solan_bamnut_nut1=0;
					}
					lastClickTime = millis();
					pressStartTime=millis()+1500;
					ui32_timeout_modesetting=millis()+60000; // Thời gian chờ vào chế độ settings
					Serial_debug.print("So lan bam nut 1: ");
					Serial_debug.println(ui8_solan_bamnut_nut1);
				}
			}
		}
		lastButtonState = reading;
	}

	if(ui8_setting_gioihan==1 && ui8_modesetting==1)
	{
		int reading = digitalRead(buttonPin);

		if (reading != lastButtonState) 
		{
			lastClickTime = millis();
		}

		if((millis() - lastClickTime) > 50)
		{	
			if (reading != buttonState) 
			{
				buttonState = reading;
				if(buttonState == LOW) 
				{
					// Nút được nhấn
					ui8_gioihan_do=ui8_gioihan_do+2;
					lcd.clear();
					lcd.setCursor(0,0);
					lcd.print("LIMITED WEIGHT: ");
					lcd.setCursor(16,0);
					lcd.print(ui8_gioihan_do);
					if(ui8_gioihan_do>40)
					{
						ui8_gioihan_do=0;
					}
					lastClickTime = millis();
					pressStartTime=millis()+10000;
					ui32_timeout_modesetting=millis()+60000; // Thời gian chờ vào chế độ settings
					Serial_debug.print("So lan bam nut chinh overload: ");
					Serial_debug.println(ui8_gioihan_do);
				}
			}
		}
		lastButtonState = reading;
	}

	lastButtonState = reading;
}

// void button_nut1()
// {

// }

void hienthi_chaychu()
{
	 unsigned long currentMillis = millis();

	if (!isPaused) {
		if (currentMillis - previousMillis >= scrollInterval) 
		{
			previousMillis = currentMillis;

			// Lấy 16 ký tự để in ra LCD
			String window = buffer.substring(position, position + 16);
			lcd.setCursor(0, 0);
			lcd.print("               ");
			lcd.setCursor(0, 0);
			lcd.print(window);

			// Khi tới chính giữa thì pause
			if (position == 13) 
			{  // chỉnh số này để canh vị trí dừng
				isPaused = true;
				pauseStart = currentMillis;
			} 
			else 
			{
				position++;
				if (position > buffer.length() - 16) 
				{
					position = 0;
				}
			}
		}
	} 
	else 
	{
		// Nếu đang pause
		if (currentMillis - pauseStart >= pauseTime) 
		{
			isPaused = false;   // hết pause, tiếp tục chạy
			position++;
		}
	}
}




