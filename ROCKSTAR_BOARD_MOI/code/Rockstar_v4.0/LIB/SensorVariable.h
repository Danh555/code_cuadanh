#pragma once
#include "Arduino.h"
#include <stdio.h>
#include <stdint.h>

/*------------CAM BIEN CUA MAY-----------------*/
#define S1 10
#define S2 11
#define S3 12
#define S4 13
#define S5 14
#define S6 15
#define S7 16
#define S8 17

#define CTHT_THANG_GOC S1
#define CTHT_HOC_QUA S2
#define CAMBIEN_ROTQUA 36

unsigned char CB[8]={CTHT_THANG_GOC,CTHT_HOC_QUA,CAMBIEN_ROTQUA,S4,S5,S6,S7,S8};

#define socambien 20
int lastButtonState_new[socambien];
int lastCBState_new[socambien]; 
unsigned long lastDebounceTime_new[socambien] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};



void setup_cambien()
{

    for(int i=0; i<8;i++)
    {
        pinMode(CB[i],INPUT_PULLUP);
    }
    // pinMode(CTHT_HOC_QUA,INPUT_PULLUP);
    // pinMode(CTHT_THANG_GOC,INPUT_PULLUP);
    // pinMode(CAMBIEN_ROTQUA,INPUT_PULLUP);
}