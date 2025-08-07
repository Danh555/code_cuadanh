#include "src/SerialComunication.h"
#include "src/Motor_BTS7960.h"
#include "hardware_config.h"

uint8_t selfSlaveID = 0x01; // Đặt Slave ID
RS485_Slave RS485(Serial1, selfSlaveID, RS485_TX_PIN, RS485_RX_PIN, RS485_DE_PIN);
uint8_t i = 0;
uint8_t dataReceive[30];

void setup()
{
    Serial.begin(115200);

    RS485.begin(115200);

    Serial.println("Slave is ready!");

    if (!pcf_PortA.begin())
    {
        Serial.println("KHONG BAT DAU VOI MODULE IO PORT A");
    }
    for (int i = 0; i < 8; i++)
    {
        pcf_PortA.pinMode(i, INPUT);
    }
    delay(50);

    if (!pcf_PortB.begin())
    {
        Serial.println("KHONG BAT DAU VOI MODULE IO PORT B");
    }
    for (int i = 0; i < 8; i++)
    {
        pcf_PortB.pinMode(i, INPUT);
    }

    if (!pcf_PortRelay.begin())
    {
        Serial.println("KHONG BAT DAU VOI MODULE IO PORT RELAY");
    }
    delay(50);

    if (!pcf_Motor.begin())
    {

        Serial.println("KHONG BAT DAU VOI MODULE MOTOR");
    }

    pinMode(EN_M1, OUTPUT);
    pcf_Motor.pinMode(PWML_M1, OUTPUT);
    pcf_Motor.pinMode(PWMR_M1, OUTPUT);

    pinMode(EN_M2, OUTPUT);
    pcf_Motor.pinMode(PWML_M2, OUTPUT);
    pcf_Motor.pinMode(PWMR_M2, OUTPUT);

    pinMode(EN_M3, OUTPUT);
    pcf_Motor.pinMode(PWML_M3, OUTPUT);
    pcf_Motor.pinMode(PWMR_M3, OUTPUT);
    delay(50);

    pinMode(EN_M4, OUTPUT);
    pcf_Motor.pinMode(PWML_M4, OUTPUT);
    pcf_Motor.pinMode(PWMR_M4, OUTPUT);
    delay(50);

    pinMode(EN_M5, OUTPUT);
    pcf_PortRelay.pinMode(PWML_M5, OUTPUT);
    pcf_PortRelay.pinMode(PWMR_M5, OUTPUT);
    delay(50);
}

void loop()
{

    RS485.receive(dataReceive);

    if (RS485.dataReceived)
    {
        Serial.print("Receive: ");
        for (uint8_t i = 0; i <= dataReceive[0] + 2; i++)
        {
            Serial.print(dataReceive[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
        RS485.dataReceived = false;

        // uint8_t dataToSend[10];
        // memset(dataToSend, 0, 10);
        // dataToSend[0] = 6;    // Kích thước dữ liệu (2 byte dữ liệu)
        // dataToSend[1] = 0xFF; // Function code (ví dụ: đọc dữ liệu)
        // dataToSend[2] = 0xFF; // Tham số hoặc địa chỉ
        // dataToSend[3] = 0xFF;
        // dataToSend[4] = 0xFA;
        // dataToSend[5] = i;
        // i++;
        // RS485.send(dataToSend);

        Motor_M3.TurnLeft(150);
        delay(2000);
        Motor_M3.Stop();
        delay(2000);
        Motor_M3.TurnRight(150);
        delay(2000);
        Motor_M3.Stop();
        delay(2000);
    }

    delay(100);
}