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
    const uint8_t _pin;
    bool _lastState;
    bool _tempState;
    uint32_t _lastDebounceTime;
    uint8_t _logicLevel; // Mức logic (HIGH hoặc LOW) sẽ được lưu trữ tại đây
    PCF8574 &_pcf;

public:
    bool state;

    // Constructor mặc định, nếu không chọn mức thì mặc định là mức cao (HIGH)
    Input_PCF8574(PCF8574 &pcf, uint8_t Pin);

    // Constructor cho phép chọn mức logic (HIGH hoặc LOW)
    Input_PCF8574(PCF8574 &pcf, uint8_t Pin, uint8_t logicLevel);

    void setup();
    void clearState();
    void readDebounce(int delay);
};

// Constructor mặc định, gán mức cao (HIGH) khi không chọn mức
Input_PCF8574::Input_PCF8574(PCF8574 &pcf, uint8_t Pin)
    : _pcf(pcf), _pin(Pin), state(0), _lastState(0), _tempState(0), _lastDebounceTime(0), _logicLevel(HIGH) // Mặc định là HIGH
{
}

// Constructor có tham số để người dùng chọn mức logic (HIGH hoặc LOW)
Input_PCF8574::Input_PCF8574(PCF8574 &pcf, uint8_t Pin, uint8_t logicLevel)
    : _pcf(pcf), _pin(Pin), state(0), _lastState(0), _tempState(0), _lastDebounceTime(0), _logicLevel(logicLevel)
{
}

void Input_PCF8574::setup()
{
    _pcf.pinMode(_pin, INPUT);
}

void Input_PCF8574::clearState()
{
    this->state = 0;
    this->_lastState = 0;
    this->_tempState = 0;
    this->_lastDebounceTime = 0;
}

void Input_PCF8574::readDebounce(int delay)
{
    // Sử dụng toán tử ba ngôi để đọc mức cao hoặc thấp
    bool reading = (_logicLevel == HIGH) ? _pcf.digitalRead(_pin) : !_pcf.digitalRead(_pin);

    // Xử lý debounce
    if (reading != _tempState)
    {
        _lastDebounceTime = millis();
        _tempState = reading;
    }
    if ((millis() - _lastDebounceTime) >= (unsigned long)delay)
    {
        state = reading;
    }
}

#endif // __INPUT_PCF8574_H__
