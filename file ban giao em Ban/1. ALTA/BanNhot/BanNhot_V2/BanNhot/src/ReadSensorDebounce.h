#ifndef READSENSORDEBOUNCE_H
#define READSENSORDEBOUNCE_H

/*
 * File: ReadSensorDeboune.h
 * Date: 24/06/07
 * Description: This is file to declare struct Sensor and read debounce Sensor hight or low
 */

#include <Arduino.h>

struct Sensor
{
    const uint8_t Pin;
    bool state;
    bool change;
    bool lastState;
    bool tempState;
    uint32_t lastDebounceTime;

    // Contructor
    Sensor(uint8_t Pin)
        : Pin(Pin), state(0), change(0), lastState(0), tempState(0), lastDebounceTime(0) {}

    Sensor(uint8_t Pin, bool state, bool lastState, bool tempState, bool change, uint32_t lastDebounceTime)
        : Pin(Pin), state(state), lastState(lastState), tempState(tempState), change(change), lastDebounceTime(lastDebounceTime) {}

    // Function
    void clearState()
    {
        this->state = 0;
        this->change = 0;
        this->lastState = 0;
        this->tempState = 0;
        this->lastDebounceTime = 0;
    }

    /**********************Đọc trang thái cảm biến trong khoảng delay************************/
    void readSensorDebounce(int delay)
    {
        bool reading = !digitalRead(Pin);
        if (reading != tempState)
        {
            lastDebounceTime = millis();
            tempState = reading;
        }
        if ((millis() - lastDebounceTime) >= delay)
        {
            state = reading;
        }
        if (state != lastState)
        {
            lastState = state;
            change = 1;
        }
    }
};

#endif // READSENSORDEBOUNCE_H
