/*
 * File: Motor_BTS7960.h
 * Author: Banban0141
 * Date: 10/09/2023
 * Description: This is library for BTS7960 control motor via PCF8574
 */
#ifndef __MOTOR_BTS7960_H__
#define __MOTOR_BTS7960_H__

#include "PCF8574.h"
#include "Arduino.h"
#include <stdio.h>
#include <stdint.h>

class Motor_BTS7960
{
private:
    /* data */
    uint8_t _EN;
    uint8_t _L_PWM;
    uint8_t _R_PWM;
    PCF8574 &_pcf;

public:
    Motor_BTS7960(PCF8574 &pcf, uint8_t EN, uint8_t L_PWM, uint8_t R_PWM);
    void setup();
    void TurnLeft(uint8_t pwm);
    void TurnRight(uint8_t pwm);
    void Stop();
};

Motor_BTS7960::Motor_BTS7960(PCF8574 &pcf, uint8_t EN, uint8_t L_PWM, uint8_t R_PWM)
    : _pcf(pcf), _EN(EN), _L_PWM(L_PWM), _R_PWM(R_PWM)
{
}

void Motor_BTS7960::setup()
{
    pinMode(_EN, OUTPUT);
    _pcf.pinMode(_L_PWM, OUTPUT);
    _pcf.pinMode(_R_PWM, OUTPUT);
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

#endif //__MOTOR_BTS7960_H__