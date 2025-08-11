#include "HX711.h"

HX711 scale;

float calibration_factor = 420.0; // Giá trị mặc định
String inputString = ""; 
boolean stringComplete = false;

void setup() {
  Serial.begin(115200);
  scale.begin(4, 3); // DOUT = 4, SCK = 5 (thay theo phần cứng của bạn)

  Serial.println("HX711 Auto Positive Weight");
  Serial.println("Lệnh:");
  Serial.println("  calib <trong_luong_gram> : Calib với mẫu");
  Serial.println("  reset : Reset về 0");
  
  scale.tare(); // Xóa khối lượng ban đầu
}

void loop() {
  if (stringComplete) {
    stringComplete = false;
    inputString.trim();
    
    if (inputString.startsWith("calib")) {
      float known_weight = inputString.substring(6).toFloat(); // gram
      if (known_weight > 0) {
        long reading = scale.read_average(10);
        calibration_factor = fabs(reading / (known_weight / 1000.0)); // kg
        Serial.print("Calibration factor mới: ");
        Serial.println(calibration_factor);
      }
    } 
    else if (inputString == "reset") {
      scale.tare();
      Serial.println("Đã reset về 0");
    }
    
    inputString = "";
  }

  // Đọc giá trị trọng lượng
  float weight = fabs(scale.get_units(5) / calibration_factor); // luôn dương
  Serial.print("Khối lượng: ");
  Serial.print(weight/1000.0,1); // đổi ra gram
  Serial.println(" kg");

  delay(500);
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
  }
}
