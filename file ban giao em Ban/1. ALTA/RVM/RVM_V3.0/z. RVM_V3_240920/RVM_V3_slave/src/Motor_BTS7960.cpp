
#include "Motor_BTS7960.h"

Motor_BTS7960::Motor_BTS7960(PCF8574 &pcf, uint8_t EN, uint8_t L_PWM, uint8_t R_PWM)
    : _pcf(pcf), _EN(EN), _L_PWM(L_PWM), _R_PWM(R_PWM)
{
    pinMode(_EN, OUTPUT);
}

void Motor_BTS7960::TurnLeft(uint8_t pwm)
{
    _pcf.digitalWrite(_R_PWM, 0);
    delayMicroseconds(100);
    _pcf.digitalWrite(_L_PWM, 1);
    analogWrite(_EN, pwm);
}

void Motor_BTS7960::TurnRight(uint8_t pwm)
{
    _pcf.digitalWrite(_R_PWM, 1);
    delayMicroseconds(100);
    _pcf.digitalWrite(_L_PWM, 0);
    analogWrite(_EN, pwm);
}

void Motor_BTS7960::Stop()
{
    analogWrite(_EN, 0);
    _pcf.digitalWrite(_R_PWM, 0);
    delayMicroseconds(100);
    _pcf.digitalWrite(_L_PWM, 0);
}
