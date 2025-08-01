/*
 * File: SerialComunicationv2.cpp
 * Date: 250113
 * Author: banban0141
 * Description: This is file to declare class Serial
 * Funtion: Read and write Serial according to protocol of AltaMedia
 * Addition:
 * - 241025 : Thêm contruction để có thể sử dụng với board không có chân DE ( Chân DE kích tự động)
 * - 250113 : Thay đổi cách truyền nhận từ dùng timeout để ngắt lệnh thành dùng Byte stuffing để truyền lệnh
 */
#include <Arduino.h>
#include "SerialComunicationv2.h"

#define START_BYTE 0xAA
#define STOP_BYTE 0xFF
#define ESCAPE_BYTE 0x7D

SerialUSB::SerialUSB(HardwareSerial &serialPort, uint8_t slaveID) : _serial(serialPort), slaveID(slaveID),
                                                                    startRead(false), isReading(false), completeRead(false), incorrectCRC(false), isSent(false), isReceived(false),
                                                                    timeSend(0), incorrectFuntion(0), timeReadChar(0), byteIndex(0), data_rec({0}) {}

void SerialUSB::begin(uint32_t baudrate)
{
    _serial.begin(baudrate);
}

void SerialUSB::receive(uint8_t *data)
{
    if (_serial.available())
    {
        timeReadChar = millis();
        while (_serial.available())
        {
            unsigned char inChar = (unsigned char)_serial.read();
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

    if (isReading && ((millis() - timeReadChar) > 10))
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
                isReceived = true;
            }

            byteIndex = 0;
            completeRead = false;
            startRead = false;
        }
    }
}

void SerialUSB::send(uint8_t *data)
{
    uint8_t dataSize = *(data + 0);

    *(data + 0) = slaveID;
    uint16_t crc_pro = calcCRC16((dataSize + 1), data);
    *(data + (dataSize + 1)) = (uint8_t)(crc_pro >> 8);
    *(data + (dataSize + 2)) = (uint8_t)(crc_pro & 0x00FF);

    for (int i = 0; i <= dataSize + 2; i++)
    {
        _serial.write(*(data + i));
    }

    isSent = true;

    timeSend = millis();
}

uint16_t SerialUSB::calcCRC16(uint8_t u8length, uint8_t *regs)
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

RS485_Master::RS485_Master(HardwareSerial &serialPort, uint8_t TXpin, uint8_t RXpin, uint8_t DEpin) : _serial(serialPort), TXpin(TXpin), RXpin(RXpin), DEpin(DEpin),
                                                                                                      startRead(false), isReading(false), completeRead(false), incorrectCRC(false),
                                                                                                      isSent(false), isReceived(false), timeSend(0), incorrectFuntion(0),
                                                                                                      timeReadChar(0), byteIndex(0), data_rec({0}) {}

RS485_Master::RS485_Master(HardwareSerial &serialPort, uint8_t TXpin, uint8_t RXpin) : _serial(serialPort), TXpin(TXpin), RXpin(RXpin), DEpin(0xFF),
                                                                                       startRead(false), isReading(false), completeRead(false), incorrectCRC(false),
                                                                                       isSent(false), isReceived(false), timeSend(0), incorrectFuntion(0),
                                                                                       timeReadChar(0), byteIndex(0), data_rec({0}) {}

void RS485_Master::begin(uint32_t baudrate)
{
    _serial.begin(baudrate, SERIAL_8N1, RXpin, TXpin);

    if (DEpin != 0xFF)
    {
        pinMode(DEpin, OUTPUT);
        digitalWrite(DEpin, LOW);
    }
}
void RS485_Master::receive(uint8_t *data, uint8_t slaveID)
{
    bool escapeFlag = false;
    bool startReceived = false;
    uint8_t byteIndex = 0;
    if (_serial.available())
    {
        timeBeginRead = millis();
        while (_serial.available())
        {
            unsigned char inChar = (unsigned char)_serial.read();

            if (inChar == ESCAPE_BYTE &&
                escapeFlag == false)
            {
                escapeFlag = true;
                continue;
            }

            if (inChar == START_BYTE &&
                startReceived != true &&
                escapeFlag == false)
            {
                startReceived = true;
                continue;
            }

            if (startReceived)
            {
                if (escapeFlag == false &&
                    inChar == STOP_BYTE)
                {
                    completeRead = true;
                    escapeFlag = false;
                    break;
                }

                data_rec[byteIndex] = inChar;
                byteIndex++;
                escapeFlag = false;
            }
            if (millis() - timeBeginRead > 50)
            {
                byteIndex = 0;
                completeRead = false;
                startRead = false;
                escapeFlag = false;
                isReceived = false;
                return;
            }
        }
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
                isReceived = true;
            }

            byteIndex = 0;
            completeRead = false;
            startRead = false;
        }
    }
}

void RS485_Master::send(uint8_t *datasend, uint8_t slaveID)
{
    uint8_t data[256] = {0};
    uint8_t dataSize = *(datasend + 0);
    memcpy(data + 1, datasend + 1, dataSize);
    *(data + 0) = slaveID;
    uint16_t crc_pro = calcCRC16(dataSize + 1, (data + 0));
    *(data + (dataSize + 1)) = (uint8_t)(crc_pro >> 8);
    *(data + (dataSize + 2)) = (uint8_t)(crc_pro & 0x00FF);

    if (DEpin != 0xFF)
    {
        digitalWrite(DEpin, HIGH);
    }

    _serial.write(START_BYTE);
    for (int i = 0; i < dataSize + 3; i++)
    {
        if (data[i] == START_BYTE || data[i] == STOP_BYTE || data[i] == ESCAPE_BYTE)
        {
            _serial.write(ESCAPE_BYTE); // Escape byte
        }
        Serial.print(*(data + i), HEX);
        Serial.printf(" ");
        _serial.write(*(data + i));
    }
    _serial.write(STOP_BYTE);
    Serial.println();

    isSent = true;

    if (DEpin != 0xFF)
    {
        _serial.flush();
        digitalWrite(DEpin, LOW);
    }
    timeSend = millis();
}

uint16_t RS485_Master::calcCRC16(uint8_t u8length, uint8_t *regs)
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

RS485_Slave::RS485_Slave(HardwareSerial &serialPort, uint8_t seftSlaveID, uint8_t TXpin, uint8_t RXpin, uint8_t DEpin) : _serial(serialPort), slaveID(seftSlaveID), TXpin(TXpin), RXpin(RXpin), DEpin(DEpin),
                                                                                                                         startRead(false), isReading(false), completeRead(false), incorrectCRC(false),
                                                                                                                         isSent(false), isReceived(false), timeSend(0), incorrectFuntion(0),
                                                                                                                         timeReadChar(0), byteIndex(0), data_rec({0})
{
}

RS485_Slave::RS485_Slave(HardwareSerial &serialPort, uint8_t seftSlaveID, uint8_t TXpin, uint8_t RXpin) : _serial(serialPort), slaveID(seftSlaveID), TXpin(TXpin), RXpin(RXpin), DEpin(0xFF),
                                                                                                          startRead(false), isReading(false), completeRead(false), incorrectCRC(false),
                                                                                                          isSent(false), isReceived(false), timeSend(0), incorrectFuntion(0),
                                                                                                          timeReadChar(0), byteIndex(0), data_rec({0})
{
}
void RS485_Slave::begin(uint32_t baudrate)
{
    _serial.begin(baudrate, SERIAL_8N1, RXpin, TXpin);

    if (DEpin != 0xFF)
    {
        pinMode(DEpin, OUTPUT);
        digitalWrite(DEpin, LOW);
    }
}
void RS485_Slave::receive(uint8_t *data)
{
    bool escapeFlag = false;
    bool startReceived = false;
    uint8_t byteIndex = 0;
    if (_serial.available())
    {
        timeBeginRead = millis();
        while (_serial.available())
        {
            unsigned char inChar = (unsigned char)_serial.read();

            if (inChar == ESCAPE_BYTE &&
                escapeFlag == false)
            {
                escapeFlag = true;
                continue;
            }

            if (inChar == START_BYTE &&
                startReceived != true &&
                escapeFlag == false)
            {
                startReceived = true;
                continue;
            }

            if (startReceived)
            {
                if (escapeFlag == false &&
                    inChar == STOP_BYTE)
                {
                    completeRead = true;
                    escapeFlag = false;
                    break;
                }

                data_rec[byteIndex] = inChar;
                byteIndex++;
                escapeFlag = false;
            }
            if (millis() - timeBeginRead > 50)
            {
                byteIndex = 0;
                completeRead = false;
                startRead = false;
                escapeFlag = false;
                isReceived = false;
                return;
            }
        }
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
                isReceived = true;
            }

            byteIndex = 0;
            completeRead = false;
            startRead = false;
        }
    }
}
void RS485_Slave::send(uint8_t *datasend)
{

    uint8_t dataSize = *(datasend + 0);

    uint8_t data[dataSize + 2] = {0};
    memcpy(data + 1, datasend + 1, dataSize);
    *(data + 0) = slaveID;
    uint16_t crc_pro = calcCRC16(dataSize + 1, (data + 0));
    *(data + (dataSize + 1)) = (uint8_t)(crc_pro >> 8);
    *(data + (dataSize + 2)) = (uint8_t)(crc_pro & 0x00FF);

    if (DEpin != 0xFF)
    {
        digitalWrite(DEpin, HIGH);
    }

    _serial.write(START_BYTE);
    for (int i = 0; i < dataSize + 3; i++)
    {
        if (data[i] == START_BYTE || data[i] == STOP_BYTE || data[i] == ESCAPE_BYTE)
        {
            _serial.write(ESCAPE_BYTE); // Escape byte
        }
        _serial.write(*(data + i));
    }
    _serial.write(STOP_BYTE);

    isSent = true;
    if (DEpin != 0xFF)
    {
        _serial.flush();
        digitalWrite(DEpin, LOW);
    }
    timeSend = millis();
}

uint16_t RS485_Slave::calcCRC16(uint8_t u8length, uint8_t *regs)
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
