#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

String text = "Xin chao ban, day la chu cuon 1 dong tren LCD 20x4";
String buffer;
int pos = 0;

unsigned long previousMillis = 0;
const long interval = 2000; // tốc độ cuộn (ms)

const int buttonPin = A3;
bool scrolling = false;   // Trạng thái có cuộn chữ hay không
bool lastButtonState = HIGH;

void setup() {
  lcd.init();
  lcd.backlight();

  pinMode(buttonPin, INPUT_PULLUP);

  // Tạo buffer có khoảng trắng ở đầu/cuối
  buffer = String(' ', 20) + text + String(' ', 20);

  lcd.setCursor(0, 0);
  lcd.print("Nhan nut de chay");  // thông báo ban đầu
}

void loop() {
  // Đọc nút (phát hiện nhấn xuống)
  bool buttonState = digitalRead(buttonPin);
  if (lastButtonState == HIGH && buttonState == LOW) { 
    scrolling = true;   // Bắt đầu cuộn
    pos = 0;
    // Xóa chỉ dòng 1 (20 ký tự trắng)
    lcd.setCursor(0, 0);
    lcd.print("                    ");
  }
  lastButtonState = buttonState;

  // Nếu đang cuộn
  if (scrolling) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      lcd.setCursor(0, 0);
      lcd.print(buffer.substring(pos, pos + 20));

      pos++;
      if (pos > buffer.length() - 20) {
        scrolling = false; // kết thúc cuộn

        // Xóa dòng 1 rồi in lại thông báo
        lcd.setCursor(0, 0);
        lcd.print("                    ");
        lcd.setCursor(0, 0);
        lcd.print("Nhan nut de chay");
      }
    }
  }
}
