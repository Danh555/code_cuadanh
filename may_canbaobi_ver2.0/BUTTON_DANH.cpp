#include "BUTTON_DANH.h"

void setting_Button(Button *btn, int pin, unsigned long debounceDelay)
{
    btn->pin = pin;
    btn->state; // Assuming pull-up resistor, button not pressed
    btn->lastState = HIGH;
    btn->lastDebounceTime = 0;
    btn->debounceDelay = debounceDelay;
    pinMode(pin, INPUT_PULLUP); // Use internal pull-up resistor    
}

int readButton(Button *btn, uint32_t timeout)
{
    int reading = digitalRead(btn->pin);
    int pressed = 0;

    if (reading != btn->lastState) 
    {
        btn->lastDebounceTime = millis();
    }

    if((millis() - btn->lastDebounceTime) > timeout) 
    {
        if (reading != btn->state) 
        {
            btn->state = reading;
            if(btn->state == LOW)
            {
                pressed = 1; // Button is pressed
            } 
            else
            {
                pressed = 0; // Button is released
            }
        }
        
    }

    btn->lastState = reading;
    return  pressed;
}