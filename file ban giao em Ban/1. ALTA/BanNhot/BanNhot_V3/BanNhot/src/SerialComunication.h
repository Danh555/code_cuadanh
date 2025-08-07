#ifndef SERIALCOMUNICATION_H
#define SERIALCOMUNICATION_H

/*
 * File: SerialComunication.h
 * Date: 24/06/07
 * Description: This is file to declare class Serial
 * Funtion: Read and write Serial according to protocol of AltaMedia
 */

#include <Arduino.h>
#include <HardwareSerial.h>

class SerialComunication
{
public:
    SerialComunication(HardwareSerial &serialPort, uint8_t slaveID);
    void begin(uint32_t baudrate);

    /*
     * Function: receive
     * Description: This function receives data from the serial port according to protocol of AltaMedia
     * Parameters after receive:
     *      *data:
     *        *(data + 0)   - Size of data
     *        *(data + 1)   - Function
     *        *(data + 2)   - Command order number
     *        *(data + 3)   - Command order number
     *        *(data + 4)   - Data
     *        *(data + ...)   - Data
     * Note:
     *      - Program will check Slave ID and CRC16
     *      - *data will not have Slave ID and CRC16
     */
    void receive(uint8_t *data);

    /*
     * Function: send
     * Description: This function sends data to the serial port according to protocol of AltaMedia
     * Parameters send:
     *      *data:
     *        *(data + 0)   - Size of data
     *        *(data + 1)   - Function
     *        *(data + 2)   - Command order number
     *        *(data + 3)   - Command order number
     *        *(data + 4)   - Data
     *        *(data + ...)  - Data
     * Note:
     *      - Program send *(data + 0) is Slave ID to the serial port (according to protocol of AltaMedia)
     *      - Program will calculate CRC16 and send it to the serial port
     *      - After send, Command order number will be + 1
     */
    void send(uint8_t *data);

    bool dataReceived;
    bool dataSent;
    bool incorrectCRC;
    uint8_t incorrectFuntion;
    uint32_t timeSend;

private:
    HardwareSerial &serial;

    bool startRead;
    bool isReading;
    bool completeRead;

    uint8_t slaveID;
    uint8_t byteIndex;
    // uint8_t status;
    uint8_t data_rec[100];

    uint32_t timeReadChar;
    uint16_t calcCRC16(uint8_t u8length, uint8_t *regs);
};

#endif // READSERIAL_H
