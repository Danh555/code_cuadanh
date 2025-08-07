/*
 * File: Motor_DriverDIY.h
 * Author: Banban0141
 * Date: 24/10/25
 * Description: This is library for control motor using driver is designed by Mr.Than
 */
#ifndef __MOTOR_DRIVERDIY_H__
#define __MOTOR_DRIVERDIY_H__

#include "Arduino.h"
#include <stdio.h>
#include <stdint.h>

class Motor_DriverDIY
{
private:
    uint8_t _CONTROL_PIN;
    uint8_t _PWM_PIN;

public:
    Motor_DriverDIY(uint8_t CONTROL_PIN, uint8_t PWM_PIN);
    void setup();
    void TurnLeft(uint8_t pwm);
    void TurnRight(uint8_t pwm);
    void Stop();
};

Motor_DriverDIY::Motor_DriverDIY(uint8_t CONTROL_PIN, uint8_t PWM_PIN)
    : _CONTROL_PIN(CONTROL_PIN), _PWM_PIN(PWM_PIN)
{
}

void Motor_DriverDIY::setup()
{
    pinMode(_CONTROL_PIN, OUTPUT);
    pinMode(_PWM_PIN, OUTPUT);
}
void Motor_DriverDIY::TurnLeft(uint8_t pwm)
{
    digitalWrite(_CONTROL_PIN, 0);
    analogWrite(_PWM_PIN, pwm);
}

void Motor_DriverDIY::TurnRight(uint8_t pwm)
{
    digitalWrite(_CONTROL_PIN, 1);
    analogWrite(_PWM_PIN, pwm);
}

void Motor_DriverDIY::Stop()
{
    digitalWrite(_CONTROL_PIN, 0);
    analogWrite(_PWM_PIN, 0);
}

#endif //__MOTOR_DRIVERDIY_H__