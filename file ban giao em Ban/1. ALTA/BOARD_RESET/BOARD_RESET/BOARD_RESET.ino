/*
 * URL: https://raw.githubusercontent.com/tenbaht/sduino/master/package_sduino_stm8_index.json
 * MCU :STM8S003F3
 * Dùng Arduino IDE với  Board Manager là SDUINO (STM8S103F3 Breakout Board)
 * Build File HEX từ Arduino IDE và nạp với STVP
 * Nối chân CTR với GND khi nạp code
 *
 */

#define LED_RUN PD2
#define LED_STT PD3

#define numSW 6
#define SW1 PC6 // PinID = 0
#define SW2 PC5 // PinID = 1
#define SW3 PC4 // PinID = 2
#define SW4 PC3 // PinID = 3
#define SW5 PB4 // PinID = 4
#define SW6 PB5 // PinID = 5

#define RST_MCU PC7

#define SIGNAL PD4
#define ONE_WIRE_PIN PD4 // chân SIGNAL với PD4

uint8_t SW[numSW] = {SW1, SW2, SW3, SW4, SW5, SW6};

bool tempState[numSW] = {0, 0, 0, 0, 0, 0};
bool lastState[numSW] = {0, 0, 0, 0, 0, 0};
bool state[numSW] = {0, 0, 0, 0, 0, 0};
bool change[numSW] = {0, 0, 0, 0, 0, 0};
uint32_t lastDebounceTime[numSW] = {0, 0, 0, 0, 0, 0};

uint32_t time_delay = 500;

uint32_t ui32_time_blink_run = 0;
uint32_t ui32_time_blink_stt = 0;
bool led_status_run = 0;
bool led_status_stt = 0;

uint32_t ui32_time_reset = 0;
uint32_t ui32_time_check = 0;

uint8_t a;
uint8_t received;
uint8_t oder_reset_raise = 0;

/**********************Reset MCU**********************************/
void resetMCU()
{
  digitalWrite(RST_MCU, HIGH);
  delay(1000);
  digitalWrite(RST_MCU, LOW);
}

/**********************Blink đèn onboard ( ms)**************************/
void blink_RUN(int time)
{
  if (millis() - ui32_time_blink_run > time)
  {
    ui32_time_blink_run = millis();
    led_status_run = !led_status_run;
    digitalWrite(LED_RUN, led_status_run);
  }
}
void blink_STT(int time)
{
  if (millis() - ui32_time_blink_stt > time)
  {
    ui32_time_blink_stt = millis();
    led_status_stt = !led_status_stt;
    digitalWrite(LED_STT, led_status_stt);
  }
}

/**********************Đọc trang thái cảm biến trong khoảng delay************************/
void readSensorDebounce(uint16_t delay, uint8_t pinID)
{
  bool reading = digitalRead(SW[pinID]);
  if (reading != tempState[pinID])
  {
    lastDebounceTime[pinID] = millis();
    tempState[pinID] = reading;
  }
  if ((millis() - lastDebounceTime[pinID]) >= delay)
  {
    state[pinID] = reading;
  }
  if (state[pinID] != lastState[pinID])
  {
    lastState[pinID] = state[pinID];
    change[pinID] = 1;
  }
}

uint32_t time_setup()
{
  if ((state[0] * 32 + state[1] * 16 + state[2] * 8 + state[3] * 4 + state[4] * 2 + state[5]) == 0)
    return 500;
  else if (state[0] == 1)
    return ((state[1] * 16 + state[2] * 8 + state[3] * 4 + state[4] * 2 + state[5]) * 60000);
  else
    return ((state[1] * 16 + state[2] * 8 + state[3] * 4 + state[4] * 2 + state[5]) * 1000);
}

/**********************Hiển thị số được cài đặt của SƯ_6P thông qua số lần nhấp nháy************************/
unsigned long previousMillis = 0;
unsigned long interval = 250;  // 0.25 giây (300ms)
unsigned long waitTime = 5000; // 5 giây (5000ms)
bool LED_STT_State = LOW;      // Trạng thái của LED
int blinkCount = 0;            // Biến đếm số lần nhấp nháy
bool inBlinkCycle = true;      // Trạng thái đang trong chu kỳ nhấp nháy

void blinkNum(int timedelay)
{
  int n = ((timedelay >= 1000) ? (timedelay / 1000) : 0);
  unsigned long currentMillis = millis(); // Lấy thời gian hiện tại

  if (inBlinkCycle)
  {
    // Kiểm tra nếu đã đủ thời gian để thay đổi trạng thái LED
    if (blinkCount < n)
    {
      if (currentMillis - previousMillis >= interval)
      {
        previousMillis = currentMillis; // Cập nhật thời gian lần cuối

        // Đổi trạng thái của LED
        LED_STT_State = !LED_STT_State;
        digitalWrite(LED_STT, LED_STT_State);

        // Tăng biến đếm mỗi khi LED chuyển từ OFF sang ON
        if (LED_STT_State == HIGH)
        {
          blinkCount++;
        }
      }
    }
    else
    {
      // Đã nhấp nháy đủ số lần, chuyển sang trạng thái chờ
      inBlinkCycle = false;
      previousMillis = currentMillis; // Cập nhật thời gian để bắt đầu thời gian chờ
    }
  }
  else
  {
    // Đang trong thời gian chờ 5 giây
    if (currentMillis - previousMillis >= waitTime)
    {
      blinkCount = 0;      // Đặt lại biến đếm
      inBlinkCycle = true; // Quay trở lại chu kỳ nhấp nháy
    }
  }
}

void sendPWM(int count, int micros)
{
  pinMode(ONE_WIRE_PIN, OUTPUT);
  for (int i = 0; i < count; i++)
  {
    digitalWrite(ONE_WIRE_PIN, LOW);
    delayMicroseconds(micros);
    digitalWrite(ONE_WIRE_PIN, HIGH);
    delayMicroseconds(micros);
  }
}

uint8_t check_PWM(int count, uint32_t timeout_ms)
{

  uint8_t state = 0;
  uint32_t timein = millis();
  for (int i = 0; i < count; i++)
  {
    while (!state)
    {
      if (millis() - timein > timeout_ms)
      {
        return 0;
      }
      state = digitalRead(ONE_WIRE_PIN);
    }
    if ((count - i) == 1)
    {
      a = !a;
      return 1;
    }

    while (state)
    {
      if (millis() - timein > timeout_ms)
      {
        return 0;
      }
      state = digitalRead(ONE_WIRE_PIN);
    }
  }
  return 1;
}

void checkOderResetALL()
{
  if (millis() - ui32_time_check < 20)
  {
    oder_reset_raise++;
  }
  else
    oder_reset_raise = 0;

  if (oder_reset_raise == 4)
  {
    oder_reset_raise = 0;
    resetMCU();
  }
}

void setup()
{

  pinMode(LED_RUN, OUTPUT);
  pinMode(LED_STT, OUTPUT);
  pinMode(RST_MCU, OUTPUT);

  //---------Pinmode SW-6P---------------//
  for (int i = 0; i < numSW; i++)
  {
    pinMode(SW[i], INPUT);
  }
  //---------Set up Switch 6P để lấy thời gian---------------//
  for (int i = 0; i < numSW; i++)
  {
    state[i] = digitalRead(SW[i]);
  }
  time_delay = time_setup();

  //---------Set up chân 1-Wire---------------//
  pinMode(ONE_WIRE_PIN, INPUT); // Khởi tạo PD4 là input
  // digitalWrite(ONE_WIRE_PIN, HIGH); // Mức ban đầu là cao

  for (int i = 0; i < 5; i++)
  {
    digitalWrite(LED_STT, HIGH);
    delay(100);
    digitalWrite(LED_STT, LOW);
    delay(100);
  }
  ui32_time_check = millis();
}

void loop()
{
  // blink_STT(1000);
  // blinkNum(time_delay);
  blink_RUN(1000);

  if (millis() - ui32_time_check > time_delay)
  {
    ui32_time_check = millis();
    resetMCU();
  }

  uint8_t state = digitalRead(ONE_WIRE_PIN);
  if (state == 0)
  {
    received = check_PWM(5, 5);
  }

  if (received == 1)
  {
    delayMicroseconds(100);
    sendPWM(5, 100);
    received = 0;
    pinMode(ONE_WIRE_PIN, INPUT);

    checkOderResetALL();

    ui32_time_check = millis();
  }
}

/*
 *
 *
 *
 *
 *
 *  Recycle Bin
 */

//---------Reset MCU Hardware(void Setup)-----------------------------------------------------------------------//
// digitalWrite(RST_MCU, HIGH);
// delay(1000);
// digitalWrite(RST_MCU, LOW);

//---------Loop----------------------------------------------------------------------------------------------//
// for (int i = 0; i < numSW; i++)
// {
//   readSensorDebounce(1000, i);
// }

// for (int i = 0; i < numSW; i++)
// {
//   if (change[i] == 1)
//   {
//     change[i] = 0;
//     time_delay = time_setup();

//     previousMillis = 0;
//     interval = 250;      // 0.25 giây (300ms)
//     waitTime = 5000;     // 5 giây (5000ms)
//     LED_STT_State = LOW; // Trạng thái của LED
//     blinkCount = 0;      // Biến đếm số lần nhấp nháy
//     inBlinkCycle = true;
//   }
// }

//---------Loop----------------------------------------------------------------------------------------------//
// void loop() {
//   // Ví dụ: Gửi lệnh "SKIP ROM" (0xCC) và "CONVERT T" (0x44) tới cảm biến DS18B20
//   oneWireWriteByte(0xCC);  // Gửi lệnh SKIP ROM
//   oneWireWriteByte(0x44);  // Gửi lệnh CONVERT T

//   delay(750);  // Đợi chuyển đổi

//   // Đọc dữ liệu từ cảm biến
//   uint8_t temp_lsb = oneWireReadByte();
//   uint8_t temp_msb = oneWireReadByte();

//   // Xử lý dữ liệu nhiệt độ (code để xử lý thêm ở đây)

//   delay(1000);  // Đợi 1 giây trước khi lặp lại
// }