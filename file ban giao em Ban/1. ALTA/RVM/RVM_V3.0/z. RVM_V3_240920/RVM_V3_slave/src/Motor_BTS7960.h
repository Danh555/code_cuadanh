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
    void TurnLeft(uint8_t pwm);
    void TurnRight(uint8_t pwm);
    void Stop();
};

#endif //__MOTOR_BTS7960_H__