#ifndef READSENSORDEBOUNCE_H
#define READSENSORDEBOUNCE_H

/*
 * File: ReadSensorDeboune.h
 * Date: 24/
 * Description: This is file to declare struct Sensor and read debounce Sensor hight or low
 */

#include <Arduino.h>

class Sensor
{
private:
    const uint8_t _pin;
    bool _lastState;
    bool _tempState;
    uint32_t _lastDebounceTime;
    uint8_t _logicLevel; // Mức logic (HIGH hoặc LOW) sẽ được lưu trữ tại đây

public:
    bool state;

    // Constructor mặc định, nếu không chọn mức thì mặc định là mức cao (HIGH)
    Sensor(uint8_t Pin);

    // Constructor cho phép chọn mức logic (HIGH hoặc LOW)
    Sensor(uint8_t Pin, uint8_t logicLevel);

    void setup();
    void clearState();
    void readDebounce(int delay);
};

// Constructor mặc định, gán mức cao (HIGH) khi không chọn mức
Sensor::Sensor(uint8_t Pin)
    : _pin(Pin), state(0), _lastState(0), _tempState(0), _lastDebounceTime(0), _logicLevel(HIGH) // Mặc định là HIGH
{
}

// Constructor có tham số để người dùng chọn mức logic (HIGH hoặc LOW)
Sensor::Sensor(uint8_t Pin, uint8_t logicLevel)
    : _pin(Pin), state(0), _lastState(0), _tempState(0), _lastDebounceTime(0), _logicLevel(logicLevel)
{
}

void Sensor::setup()
{
    pinMode(_pin, INPUT);
}

void Sensor::clearState()
{
    this->state = 0;
    this->_lastState = 0;
    this->_tempState = 0;
    this->_lastDebounceTime = 0;
}

void Sensor::readDebounce(int delay)
{
    // Sử dụng toán tử ba ngôi để đọc mức cao hoặc thấp
    bool _reading = (_logicLevel == HIGH) ? digitalRead(_pin) : !digitalRead(_pin);

    // Xử lý debounce
    if (_reading != _tempState)
    {
        _lastDebounceTime = millis();
        _tempState = _reading;
    }
    if ((millis() - _lastDebounceTime) >= (unsigned long)delay)
    {
        state = _reading;
    }
}

#endif // READSENSORDEBOUNCE_H
