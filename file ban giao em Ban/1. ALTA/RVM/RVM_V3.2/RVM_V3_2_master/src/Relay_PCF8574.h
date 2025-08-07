/*
 * File: Relay_PCF8574.h
 * Author: Banban0141
 * Date: 24/09/24
 * Description: This is library for relay control via PCF8574
 */
#ifndef __Relay_PCF8574_H__
#define __Relay_PCF8574_H__

#include "PCF8574.h"
#include "Arduino.h"
#include <stdio.h>
#include <stdint.h>

class Relay_PCF8574
{
private:
    /* data */
    uint8_t _pin;

    PCF8574 &_pcf;

public:
    Relay_PCF8574(PCF8574 &pcf, uint8_t PIN);

    void setup();
    void ON();
    void OFF();
};

Relay_PCF8574::Relay_PCF8574(PCF8574 &pcf, uint8_t PIN)
    : _pcf(pcf), _pin(PIN)
{
}

void Relay_PCF8574::setup()
{
    _pcf.pinMode(_pin, OUTPUT);
}

void Relay_PCF8574::ON()
{
    _pcf.digitalWrite(_pin, 1);
}

void Relay_PCF8574::OFF()
{
    _pcf.digitalWrite(_pin, 0);
}

#endif //__Relay_PCF8574_H__