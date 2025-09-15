const int buttonPin = A3;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 1050;

unsigned long lastPressTime = 0;
int pressCount = 0;
unsigned long doubleClickDelay = 400;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP); // dùng trở kéo lên trong Arduino
  Serial.begin(115200);
  Serial.println("Chuong trinh bat dau...");
}

void loop() {
  int reading = digitalRead(buttonPin);

  // Debug trạng thái nút
   Serial.println(reading);  // bỏ comment dòng này nếu muốn xem trạng thái nút

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (lastButtonState == HIGH && reading == LOW) {
      pressCount++;
      lastPressTime = millis();
      Serial.println("Da nhan nut!");
    }
  }

  if (pressCount > 0 && (millis() - lastPressTime > doubleClickDelay)) {
    if (pressCount == 1) {
      Serial.println("A");   // Nhấn 1 lần
    } else if (pressCount == 2) {
      Serial.println("B");   // Nhấn 2 lần
    } else {
      Serial.print("Nhan ");
      Serial.print(pressCount);
      Serial.println(" lan");
    }
    pressCount = 0;
  }

  lastButtonState = reading;
}
