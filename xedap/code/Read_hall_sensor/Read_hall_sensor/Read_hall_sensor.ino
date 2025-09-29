const int hallSensor = 3;
unsigned int count = 0;
unsigned int countTotal = 0;
unsigned long timeCheck = 0;

void setup() {
  pinMode(hallSensor, INPUT); // Cấu hình chân cảm biến với điện trở kéo lên
  attachInterrupt(digitalPinToInterrupt(hallSensor), countPulse, FALLING); // Gắn ngắt cho cảm biến
  Serial.begin(115200); // Khởi tạo giao tiếp Serial
  timeCheck  = millis();
  Serial.println("Start");
}
void countPulse (){
  count ++;
}
void loop() {
  if ((millis()- timeCheck) >= 1000)
  {
    noInterrupts();
    unsigned int count_t = count;
    count = 0;
    interrupts();
    countTotal += count_t;
    // Serial.print ("countTotal: ");
    // Serial.println (countTotal);
    // Serial.print ("count: ");
    Serial.print ("$");
    Serial.print (count_t);
    Serial.println ("#");
    timeCheck = millis();

  }
  

}
