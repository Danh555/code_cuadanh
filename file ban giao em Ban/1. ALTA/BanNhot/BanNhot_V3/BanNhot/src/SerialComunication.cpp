/*
 * File: SerialComunication.cpp
 * Date: 24/10/25
 * Description: This is file to declare class Serial
 * Funtion: Read and write Serial according to protocol of AltaMedia
 */
#include <Arduino.h>
#include "SerialComunication.h"
// #include "CRC16.h"

SerialComunication::SerialComunication(HardwareSerial &serialPort, uint8_t slaveID) : serial(serialPort), slaveID(slaveID),
                                                                                      startRead(false), isReading(false), completeRead(false), incorrectCRC(false), dataSent(false), dataReceived(false),
                                                                                      timeSend(0), incorrectFuntion(0), timeReadChar(0), byteIndex(0), data_rec({0}) {}

void SerialComunication::begin(uint32_t baudrate)
{
    serial.begin(baudrate);
}
void SerialComunication::receive(uint8_t *data)
{
    if (serial.available())
    {
        timeReadChar = millis();
        while (serial.available())
        {
            unsigned char inChar = (unsigned char)serial.read();
            if (!startRead)
            {
                isReading = true;
                startRead = true;
                byteIndex = 0;
            }
            if (startRead && (millis() - timeReadChar) < 10)
            {
                data_rec[byteIndex] = inChar;
                byteIndex++;
                timeReadChar = millis();
            }
        }
    }

    if (isReading && ((millis() - timeReadChar) > 50))
    {
        isReading = false;
        completeRead = true;
    }
    if (completeRead)
    {
        if (byteIndex != 0 && data_rec[0] != slaveID)
        {
            byteIndex = 0;
            completeRead = false;
            startRead = false;
        }
        else if (byteIndex != 0 && data_rec[0] == slaveID)
        {
            uint8_t crcReceiveHi = data_rec[byteIndex - 1];
            uint8_t crcReceiveLo = data_rec[byteIndex - 2];

            uint16_t crc16Calculate = calcCRC16(byteIndex - 2, data_rec);
            uint8_t crcCalculateLo = crc16Calculate >> 8;
            uint8_t crcCalculateHi = crc16Calculate & 0x00FF;

            if (crcReceiveHi != crcCalculateHi || crcReceiveLo != crcCalculateLo)
            {
                incorrectFuntion = data_rec[1];
                incorrectCRC = 1;
            }
            else if (crcReceiveHi == crcCalculateHi && crcReceiveLo == crcCalculateLo)
            {
                memcpy(data, data_rec, (byteIndex - 2));
                *(data + 0) = byteIndex - 3;
                dataReceived = true;
            }

            byteIndex = 0;
            completeRead = false;
            startRead = false;
        }
    }
}

void SerialComunication::send(uint8_t *data)
{
    // uint8_t dataSize = *(data + 0);

    // uint16_t crc_pro = calcCRC16(dataSize, (data + 1));
    // *(data + (dataSize + 1)) = (uint8_t)(crc_pro >> 8);  
    // *(data + (dataSize + 2)) = (uint8_t)(crc_pro & 0x00FF);

    // Serial.write(slaveID);

    uint8_t dataSize = *(data + 0);
    *(data + 0) = slaveID;
    uint16_t crc_pro = calcCRC16(dataSize + 1, (data + 0));
    *(data + (dataSize + 1)) = (uint8_t)(crc_pro >> 8);
    *(data + (dataSize + 2)) = (uint8_t)(crc_pro & 0x00FF);

    for (int i = 0; i < dataSize + 3; i++)
    {
        Serial.write(*(data + i));
    }

    dataSent = true;

    timeSend = millis();
}

uint16_t SerialComunication::calcCRC16(uint8_t u8length, uint8_t *regs)
{
    unsigned int temp, temp2, flag;
    temp = 0xFFFF;
    for (unsigned char i = 0; i < u8length; i++)
    {
        temp = temp ^ regs[i];
        for (unsigned char j = 1; j <= 8; j++)
        {
            flag = temp & 0x0001;
            temp >>= 1;
            if (flag)
                temp ^= 0xA001;
        }
    }
    // Reverse byte order.
    temp2 = temp >> 8;
    temp = (temp << 8) | temp2;
    temp &= 0xFFFF;
    // the returned value is already swapped
    // crcLo byte is first & crcHi byte is last
    return temp;
}

// void SerialComunication::process(uint8_t* data, size_t length) {
//     // Xử lý dữ liệu nhận được trong mảng data với kích thước length
//     // Thêm code xử lý tại đây.
// }
