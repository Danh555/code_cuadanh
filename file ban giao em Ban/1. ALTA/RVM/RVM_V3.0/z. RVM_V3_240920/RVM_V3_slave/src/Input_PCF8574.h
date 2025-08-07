/*
 * File: Input_PCF8574.h
 * Author: Banban0141
 * Date: 10/09/2023
 * Description: This is library for Input expander via PCF8574
 */
#ifndef __INPUT_PCF8574_H__
#define __INPUT_PCF8574_H__

#include "PCF8574.h"
#include "Arduino.h"
#include <stdio.h>
#include <stdint.h>

class Input_PCF8574
{
private:
    const uint8_t Pin;
    bool lastState;
    bool tempState;
    uint32_t lastDebounceTime;
    uint8_t logicLevel; // Mức logic (HIGH hoặc LOW) sẽ được lưu trữ tại đây
    PCF8574 &pcf;

public:
    bool state;

    // Constructor mặc định, nếu không chọn mức thì mặc định là mức cao (HIGH)
    Input_PCF8574(PCF8574 &pcf, uint8_t Pin);

    // Constructor cho phép chọn mức logic (HIGH hoặc LOW)
    Input_PCF8574(PCF8574 &pcf, uint8_t Pin, uint8_t logicLevel);

    void clearState();
    void readDebounce(int delay);
};

// Constructor mặc định, gán mức cao (HIGH) khi không chọn mức
Input_PCF8574::Input_PCF8574(PCF8574 &pcf, uint8_t Pin)
    : pcf(pcf), Pin(Pin), state(0), lastState(0), tempState(0), lastDebounceTime(0), logicLevel(HIGH) // Mặc định là HIGH
{
}

// Constructor có tham số để người dùng chọn mức logic (HIGH hoặc LOW)
Input_PCF8574::Input_PCF8574(PCF8574 &pcf, uint8_t Pin, uint8_t logicLevel)
    : pcf(pcf), Pin(Pin), state(0), lastState(0), tempState(0), lastDebounceTime(0), logicLevel(logicLevel)
{
}

void Input_PCF8574::clearState()
{
    this->state = 0;
    this->lastState = 0;
    this->tempState = 0;
    this->lastDebounceTime = 0;
}

void Input_PCF8574::readDebounce(int delay)
{
    // Sử dụng toán tử ba ngôi để đọc mức cao hoặc thấp
    bool reading = (logicLevel == HIGH) ? pcf.digitalRead(Pin) : !pcf.digitalRead(Pin);

    // Xử lý debounce
    if (reading != tempState)
    {
        lastDebounceTime = millis();
        tempState = reading;
    }
    if ((millis() - lastDebounceTime) >= (unsigned long)delay)
    {
        state = reading;
    }
}

#endif // __INPUT_PCF8574_H__
