
/*
 * Dùng cầu phân áp kéo xuông GND (dùng trở 4.7K và 10K), không kéo lên mức cao
 * Dùng để giao tiếp trên 1 dây giữa hai thiết bị với mức logic khác nhau 3.3V với 5V
 * Hoặc có thể dùng giữa 2 thiết bị với có logic 3.3V và điện trở kéo lên
 * Do không kéo lên 3.3V nên cần phải kích HIGH chân SGN_PIN ở trạng thái chờ
 * Giữ chân CTR_PIN ở LOW, kích HIGH là reset board RESET
 */

#define S13 24
#define S14 25

#define SGN_PIN S13
#define CTR_PIN S14

uint32_t ui32_time_send_PWM = 0;
uint32_t ui32_time_receive_PWM = 0;

int reset_variable = 0; // debug

/*--- timesend = count * micros * 2 ---*/
void sendPWM(int count, int micros)
{
  pinMode(SGN_PIN, OUTPUT);
  for (int i = 0; i < count; i++)
  {
    digitalWrite(SGN_PIN, LOW);
    delayMicroseconds(micros);
    digitalWrite(SGN_PIN, HIGH);
    delayMicroseconds(micros);
  }
}

uint8_t check_PWM_respond(int count, uint32_t timeout_ms)
{
  pinMode(SGN_PIN, INPUT);
  uint8_t state = 1;
  uint32_t timein = millis();
  for (int i = 0; i < count; i++)
  {
    while (state)
    {
      if (millis() - timein > timeout_ms)
      {
        reset_variable++; // debug
        pinMode(SGN_PIN, OUTPUT);
        digitalWrite(SGN_PIN, HIGH);
        return 0;
      }
      state = digitalRead(SGN_PIN);
    }
    while (!state)
    {
      if (millis() - timein > timeout_ms)
      {
        reset_variable++; // debug
        pinMode(SGN_PIN, OUTPUT);
        digitalWrite(SGN_PIN, HIGH);
        return 0;
      }
      state = digitalRead(SGN_PIN);
    }
  }
  pinMode(SGN_PIN, OUTPUT);
  digitalWrite(SGN_PIN, HIGH);
  return 1;
}

void resetBoardReset()
{
  digitalWrite(CTR_PIN, HIGH);
  delay(100);
  digitalWrite(CTR_PIN, LOW);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Master Ready");

  
  pinMode(SGN_PIN, OUTPUT);
  digitalWrite(SGN_PIN, HIGH);
  pinMode(CTR_PIN, OUTPUT);
  digitalWrite(CTR_PIN, LOW);

  ui32_time_send_PWM = millis();
  ui32_time_receive_PWM = millis();
}

void loop()
{
  if (millis() - ui32_time_send_PWM > 1000)
  {
    ui32_time_send_PWM = millis();
    sendPWM(5, 100);
    int respond = check_PWM_respond(5, 5);
    if (respond == 1)
    {
      ui32_time_receive_PWM = millis();
    }

    Serial.print(respond); // Debug
    Serial.println(reset_variable);     // Debug
  }

  if (millis() - ui32_time_receive_PWM > 600000)
  {
    resetBoardReset();
    ui32_time_receive_PWM = millis();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
// /*
//  * Dùng pinMode(Input) thay thế cho digitalWrite(HIGH), dùng chế độ input để thả nổi, chân sẽ tự động kéo lên 3.3V
//  * Dùng để giao tiếp trên 1 dây giữa hai thiết bị với mức logic khác nhau 3.3V với 5V
//  *
//  *
//  *
//  */
// #define PIN 4
// int a = 0;

// void setup()
// {
//   Serial.begin(115200);
//   Serial.println("Master Ready");
//   pinMode(PIN, OUTPUT);
// }
// /*--- timesend = count * micros * 2 ---*/
// void sendPWM(int count, int micros)
// {
//   pinMode(PIN, OUTPUT);
//   // digitalWrite(PIN, HIGH);
//   for (int i = 0; i < count; i++)
//   {
//     digitalWrite(PIN, LOW);
//     delayMicroseconds(micros);
//     // digitalWrite(PIN, HIGH);
//     pinMode(PIN, INPUT);
//     delayMicroseconds(micros);
//     pinMode(PIN, OUTPUT);
//   }
// }

// uint8_t check_PWM_respond(int count, uint32_t timeout_ms)
// {
//   pinMode(PIN, INPUT);
//   uint8_t state = 1;
//   uint32_t timein = millis();
//   for (int i = 0; i < count; i++)
//   {
//     while (state)
//     {
//       if (millis() - timein > timeout_ms)
//       {
//         a = 1;
//         return 0;
//       }
//       state = digitalRead(PIN);
//     }
//     while (!state)
//     {
//       if (millis() - timein > timeout_ms)
//       {
//         a = 1;
//         return 0;
//       }
//       state = digitalRead(PIN);
//     }
//   }
//   return 1;
// }

// void loop()
// {
//   sendPWM(5, 50);

//   int respond = check_PWM_respond(5, 4);

//   Serial.printf("%d %d\n", respond, a);
//   delay(2000);
// }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// #define PIN 4

// void setup()
// {
//   Serial.begin(115200);
//   Serial.println("Master Ready");
//   pinMode(PIN, OUTPUT);
// }

// void loop()
// {
//   digitalWrite(PIN, LOW);
//   delay(1);
//   digitalWrite(PIN, HIGH);
//   pinMode(PIN, INPUT);
//   bool error = 0;
//   uint8_t state = 1;
//   uint32_t time1 = millis();
//   while (state)
//   {
//     if (millis() - time1 > 5)
//     {
//       error = 1;
//       break;
//     }
//     state = digitalRead(PIN);
//   }
//   while (!state)
//   {
//     if (millis() - time1 > 5)
//     {
//       error = 1;
//       break;
//     }
//     state = digitalRead(PIN);
//   }
//   pinMode(PIN, OUTPUT);
//   Serial.printf("%d\n", error);
//   delay(2000);
// }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
//
// #define ONE_WIRE_PIN 4

// // Gửi một bit qua 1-Wire
// void oneWireWriteBit(uint8_t bit)
// {
//   pinMode(ONE_WIRE_PIN, OUTPUT);   // Chuyển PD4 về đầu ra
//   digitalWrite(ONE_WIRE_PIN, LOW); // Kéo xuống mức thấp
//   if (bit)
//   {
//     delayMicroseconds(6);             // Đợi 6 µs
//     digitalWrite(ONE_WIRE_PIN, HIGH); // Trả lại mức cao
//     delayMicroseconds(64);            // Đợi 64 µs
//   }
//   else
//   {
//     delayMicroseconds(60);            // Đợi 60 µs
//     digitalWrite(ONE_WIRE_PIN, HIGH); // Trả lại mức cao
//     delayMicroseconds(10);            // Đợi 10 µs
//   }
// }

// // Đọc một bit qua 1-Wire
// uint8_t oneWireReadBit(void)
// {
//   uint8_t bit = 0;

//   delayMicroseconds(10);

//   bit = digitalRead(ONE_WIRE_PIN); // Đọc giá trị

//   delayMicroseconds(60); // Đợi 55 µs để kết thúc chu kỳ đọc
//   return bit;
// }

// // Gửi một byte qua 1-Wire
// void masterWriteByte(uint8_t byte)
// {
//   pinMode(ONE_WIRE_PIN, OUTPUT);
//   digitalWrite(ONE_WIRE_PIN, LOW);
//   delayMicroseconds(2000);
//   digitalWrite(ONE_WIRE_PIN, HIGH);
//   delayMicroseconds(10);
//   for (uint8_t i = 0; i < 8; i++)
//   {
//     oneWireWriteBit(byte & 0x01);
//     byte >>= 1;
//   }
// }

// // Đọc một byte qua 1-Wire
// uint8_t masterReadByte(void)
// {
//   digitalWrite(ONE_WIRE_PIN, LOW);
//   pinMode(ONE_WIRE_PIN, INPUT);
//   uint32_t time1r = millis();
//   uint8_t byte = 0;
//   uint8_t temp = 1;

//   while (1)
//   {
//     if (millis() - time1r > 5)
//       return 0;
//     temp = digitalRead(ONE_WIRE_PIN);
//     if (temp == 0)
//     {
//       delayMicroseconds(1500);
//       temp = digitalRead(ONE_WIRE_PIN);
//       if (temp == 0)
//       {
//         while (temp == 0)
//         {
//           temp = digitalRead(ONE_WIRE_PIN);
//         }
//       }
//       else
//         return 0;
//     }
//     for (uint8_t i = 0; i < 8; i++)
//     {
//       byte >>= 1;
//       if (oneWireReadBit())
//       {
//         byte |= 0x80;
//       }
//     }
//     return byte;
//   }
// }

// void slaveWriteByte(uint8_t byte)
// {
//   pinMode(ONE_WIRE_PIN, OUTPUT);
//   digitalWrite(ONE_WIRE_PIN, LOW);
//   delayMicroseconds(2000);
//   digitalWrite(ONE_WIRE_PIN, HIGH);
//   delayMicroseconds(10);
//   for (uint8_t i = 0; i < 8; i++)
//   {
//     oneWireWriteBit(byte & 0x01);
//     byte >>= 1;
//   }
// }
// uint8_t slaveReadByte(void)
// {
//   uint32_t time1r = millis();
//   uint8_t byte = 0;
//   uint8_t temp = 1;
//   pinMode(ONE_WIRE_PIN, INPUT);
//   while (1)
//   {
//     if (millis() - time1r > 1000)
//       return 0;
//     temp = digitalRead(ONE_WIRE_PIN);
//     if (temp == 0)
//     {
//       delayMicroseconds(1500);
//       temp = digitalRead(ONE_WIRE_PIN);
//       if (temp == 0)
//       {
//         while (temp == 0)
//         {
//           temp = digitalRead(ONE_WIRE_PIN);
//         }
//       }
//       else
//         return 0;
//     }
//     for (uint8_t i = 0; i < 8; i++)
//     {
//       byte >>= 1;
//       if (oneWireReadBit())
//       {
//         byte |= 0x80;
//       }
//     }
//     return byte;
//   }
// }

// void setup()
// {
//   Serial.begin(115200);
//   Serial.println("Master Ready");
// }

// void loop()
// {
//   masterWriteByte(0xAA);
//   uint8_t code = masterReadByte();

//   Serial.printf("code :%d \n", code);
//   delay(2000);
// }
