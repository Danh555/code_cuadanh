/*
 * Dùng cầu phân áp kéo xuông GND, không kéo lên
 * Dùng để giao tiếp trên 1 dây giữa hai thiết bị với mức logic khác nhau 3.3V với 5V
 * Hoặc có thể dùng giữa 2 thiết bị với có logic 3.3V và điện trở kéo lên
 *
 *
 */

#define PIN 4

int respond;
bool a = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("Slave Ready");
  pinMode(PIN, INPUT);
}
/*--- timesend = count * micros * 2 ---*/
void sendPWM(int count, int micros)
{
  pinMode(PIN, OUTPUT);
  for (int i = 0; i < count; i++)
  {
    digitalWrite(PIN, LOW);
    delayMicroseconds(micros);
    digitalWrite(PIN, HIGH);
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
      state = digitalRead(PIN);
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
      state = digitalRead(PIN);
    }
  }
  return 1;
}

void loop()
{
  uint8_t state = digitalRead(PIN);
  if (state == 0)
  {
    respond = check_PWM(5, 4);
  }

  if (respond == 1)
  {
    delayMicroseconds(100);
    sendPWM(5, 50);

    Serial.printf("%d %d\n ", respond, a);
    respond = 0;
  }

  pinMode(PIN, INPUT);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// /*
//  * Dùng pinMode(Input) thay thế cho digitalWrite(HIGH), dùng chế độ input để thả nổi, chân sẽ tự động kéo lên 3.3V
//  * Dùng để giao tiếp trên 1 dây giữa hai thiết bị với mức logic khác nhau 3.3V với 5V
//  *
//  *
//  *
//  */

// #define PIN 4

// int respond;
// bool a = 0;

// void setup()
// {
//   Serial.begin(115200);
//   Serial.println("Slave Ready");
//   pinMode(PIN, INPUT);
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

// uint8_t check_PWM(int count, uint32_t timeout_ms)
// {

//   uint8_t state = 0;
//   uint32_t timein = millis();
//   for (int i = 0; i < count; i++)
//   {
//     while (!state)
//     {
//       if (millis() - timein > timeout_ms)
//       {
//         return 0;
//       }
//       state = digitalRead(PIN);
//     }
//     if ((count - i) == 1)
//     {
//       a = !a;
//       return 1;
//     }

//     while (state)
//     {
//       if (millis() - timein > timeout_ms)
//       {
//         return 0;
//       }
//       state = digitalRead(PIN);
//     }
//   }
//   return 1;
// }

// void loop()
// {
//   uint8_t state = digitalRead(PIN);
//   if (state == 0)
//   {
//     respond = check_PWM(5, 4);
//   }

//   if (respond == 1)
//   {
//     delayMicroseconds(100);
//     sendPWM(5, 50);

//     Serial.printf("%d %d\n ", respond, a);
//     respond = 0;
//   }

//   pinMode(PIN, INPUT);
// }
////////// chỉ kích xung lên xuống 1 lần/////////////////////////////////////////////////////////
// #define PIN 4

// uint8_t state = 1;
// uint8_t error = 0;
// uint32_t time1;
// void setup()
// {
//   Serial.begin(115200);
//   Serial.println("Slave Ready");
//   pinMode(PIN, INPUT);
//   time1 = millis();
// }

// void loop()
// {
//   state = digitalRead(PIN);
//   if (state == 0)
//   {
//     time1 = millis();
//     while (!state)
//     {
//       if (millis() - time1 > 5)
//       {
//         error = 1;
//         break;
//       }
//       state = digitalRead(PIN);
//     }
//     pinMode(PIN, OUTPUT);
//     digitalWrite(PIN, LOW);
//     delay(2);
//     digitalWrite(PIN, HIGH);
//     pinMode(PIN, INPUT);
//     Serial.printf("%d\n", error);
//   }
// }

////////////////////////////////////////////////////////////////////////
//
//
//
//
////
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
//   Serial.println("Slave Ready");
// }

// void loop()
// {
//   uint8_t code = slaveReadByte();

//   if (code != 0 && code != 255)
//   {
//     Serial.printf("code :%d \n", code);
//     slaveWriteByte(0x55);
//   }
// }
