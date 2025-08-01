

#pragma once
#include "Arduino.h"
#include <stdio.h>
#include <stdint.h>

class DONGCO
{
    private:
        /* data */
        uint8_t _DIR;
        uint8_t _PWM;
    public:
        DONGCO(uint8_t DIR, uint8_t PWM);
        void TurnLeft(uint8_t pwm);
        void TurnRight(uint8_t pwm);
        void Stop();
};

DONGCO::DONGCO(uint8_t DIR, uint8_t PWM)
{
    this->_DIR = DIR;
    this->_PWM = PWM;
    pinMode(_DIR, OUTPUT);
    pinMode(_PWM, OUTPUT);
}

void DONGCO::TurnLeft(uint8_t pwm)
{
    analogWrite(_PWM, pwm);
    digitalWrite(_DIR, 0);
}

void DONGCO::TurnRight(uint8_t pwm)
{
    analogWrite(_PWM, pwm);
    digitalWrite(_DIR, 1);
}

void DONGCO::Stop()
{
    analogWrite(_PWM, 0);
}