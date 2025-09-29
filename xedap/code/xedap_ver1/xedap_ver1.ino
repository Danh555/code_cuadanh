#define encoder_pin 3
#define so_dia_banhxe 1
#define duong_kinh 0.040
#define so_pi 3.14


#define button_1 2 
#define button_2 1 
#define button_3 0 
#define button_4 6 

unsigned char CB[]={
		button_1,
        button_2,
        button_3,
        button_4
	};

//---------------Variable for doccambien function----------------//
#define socambien 20
int lastButtonState[socambien];
int lastCBState[socambien]; 
unsigned long lastDebounceTime[socambien] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


volatile unsigned long dem = 0;
int tam=0;
int rpm = 0;
float tocdo = 0;
float tocdo_km=0;

int timecho = 1000;
uint32_t ui32_thoigian=0;
uint32_t ui32_hientai=0;

// HWCDC Serial;

// static void usbEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
//   if (event_base == ARDUINO_HW_CDC_EVENTS) {
//     switch (event_id) {
//       case ARDUINO_HW_CDC_RX_EVENT:
//         //Serial0.println("\nCDC EVENT:: ARDUINO_HW_CDC_RX_EVENT");
//         // sends all bytes read from USB Hardware Serial to UART0
//         while (Serial.available()) 
//         {
//         //   Serial.write(Serial.read());
//           String input = Serial.readString();

//         }
//         break;
//         default: break;
//         }
//   }
// }

// void dem_xung()
// {
//     dem++;
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

void IRAM_ATTR pulseISR() {
  dem++;
}

void speed()
{
    ui32_thoigian = millis();
    if(ui32_thoigian - ui32_hientai >= timecho)
    {
        ui32_hientai = ui32_thoigian;
        noInterrupts();
        tam = dem;
        dem=0;
        interrupts();
        rpm = (tam/so_dia_banhxe)*60;
            /*
            * Đĩa encoder có 20 xung, chúng ta đo được 120 xung/s
            * vậy lấy 120/20 = 6 vòng/s
            * ta được: 6*60 = số vòng quay / phút (RPM)
            */
        tocdo = (float(tam/so_dia_banhxe) * float(1.1)); //m/s
        tocdo_km=tocdo*3.6;
        /*
        * dem/20 là số vòng/s
        * 0.025 là đường kính vòng tròn (đơn vị: m)
        * 3.14 là số pi
        */
        // tam = dem;
        // dem=0;
        
        // Serial.print("\t\t\t\t"); Serial.print("RPM: "); Serial.print(rpm);
        // Serial.print("   "); Serial.print("M/s: "); Serial.println(tocdo);
        Serial.print ("$");
        Serial.print (tocdo_km);
        Serial.println ("#");
    }
}

void setup()
{
    // Serial.onEvent(usbEventCallback);
    Serial.begin(115200);

    pinMode(encoder_pin,INPUT);
    attachInterrupt(digitalPinToInterrupt(encoder_pin),pulseISR,FALLING);
    
}

void loop()
{
    speed();
}