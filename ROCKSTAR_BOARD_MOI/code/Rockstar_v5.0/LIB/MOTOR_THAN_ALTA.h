/*
* File: MOTOR.h
* Author: HOANG QUOC BINH
* Date: 30/03/2023
* Description: This is library for MOTOR control motor via PCF8574
*/
#pragma once
#include "Arduino.h"
#include <stdio.h>
#include <stdint.h>


class MOTOR
{
private:
    /* data */
    uint8_t _PWM;
    uint8_t _DIR;
    // uint8_t _R_DIR;
public:
    MOTOR(uint8_t PWM, uint8_t DIR);
    void TurnLeft(uint8_t pwm);
	void TurnRight(uint8_t pwm);
	void Stop();
};

MOTOR::MOTOR(uint8_t PWM, uint8_t DIR){
    this->_DIR = DIR;
    // this->_R_DIR = R_DIR;
    this->_PWM = PWM;
    pinMode(_PWM, OUTPUT);
}
void MOTOR::TurnLeft(uint8_t pwm){
    analogWrite(_PWM, pwm);
    // digitalWrite(_R_DIR, 0);
    // delayMicroseconds(100);
    digitalWrite(_DIR, 1);
}

void MOTOR::TurnRight(uint8_t pwm){
    analogWrite(_PWM, pwm);
    // digitalWrite(_R_DIR, 1);
    // delayMicroseconds(100);
    digitalWrite(_DIR, 0);
}

void MOTOR::Stop(){
	analogWrite(_PWM, 0);
    // pcf.digitalWrite(_R_DIR, HIGH);
    digitalWrite(_DIR, LOW);
}