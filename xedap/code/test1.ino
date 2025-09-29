#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

int enco = 2;
int dem = 0;
int rpm = 0;
float tocdo = 0;

int timecho = 1000;
unsigned long thoigian;
unsigned long hientai;

void dem_xung()
{
  dem++; //đếm xung
}

void setup() 
{
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("RPM: ");
  lcd.setCursor(0,1);
  lcd.print("M/s: ");  
  
  pinMode(enco, INPUT);
  attachInterrupt(0, dem_xung, RISING); //Ngắt ngoài
}

void loop() 
{
  thoigian = millis();
//  Serial.print("Time: "); Serial.print(thoigian); Serial.print("   ");
//  Serial.print("Hiện tại: "); Serial.println(hientai);
  
  if (thoigian - hientai >= timecho) //millis thay delay
  {    
    hientai = thoigian;
    rpm = (dem/20)*60;    
        /*
         * Đĩa encoder có 20 xung, chúng ta đo được 120 xung/s
         * vậy lấy 120/20 = 6 vòng/s
         * ta được: 6*60 = số vòng quay / phút (RPM)
         */
    tocdo = float(dem/20)*float(0.025*3.14); // m/s
    /*
     * dem/20 là số vòng/s
     * 0.025 là đường kính vòng tròn (đơn vị: m)
     * 3.14 là số pi
     */
    //Serial.print("\t\t\t\t"); Serial.print("Số xung/s: "); Serial.println(dem);
    dem = 0; 
    Serial.print("\t\t\t\t"); Serial.print("RPM: "); Serial.print(rpm);
    Serial.print("   "); Serial.print("M/s: "); Serial.println(tocdo);
  }

  if (rpm >= 100)
  {
    lcd.setCursor(5,0);
    lcd.print(rpm);
    lcd.setCursor(8,0);
    lcd.print("        ");
  }
  else
  {
    lcd.setCursor(5,0);
    lcd.print(rpm);
    lcd.setCursor(6,0);
    lcd.print("        ");    
  }
  
  lcd.setCursor(5,1);
  lcd.print(tocdo);  
}
