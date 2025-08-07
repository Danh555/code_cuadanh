#ifndef __HARDWARE_CONFIG_H__
#define __HARDWARE_CONFIG_H__

// #define RVM_HEINEKEN
// #define mayFPT

#include "src/PCF8574.h"
#include "src/Motor_BTS7960.h"
#include "src/Input_PCF8574.h"
#include "Adafruit_PWMServoDriver.h"

#define addr_ExIO_portA 0x20
#define addr_ExIO_portB 0x24
#define addr_ExIO_Motor 0x22    //(Motor from 1 to 4)
#define addr_ExIO_OutputRL 0x26 //(Relay and Motor 5)
#define addr_Output_PWM 0x40

#define EN_M1 13
#define EN_M2 14
#define EN_M3 27
#define EN_M4 26
#define EN_M5 25

// Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define cambien_vat PIN_A1
#define cambien_nhavat PIN_A2
#define cambien_roichai PIN_A3

#ifdef mayFPT
#define cambien_thungracchai PIN_A4
#define cambien_thungraclon PIN_B8
#else
#define cambien_thungracchai PIN_B1
#define cambien_thungraclon PIN_B2
#endif

#define cambien_encodernghien PIN_A5
#define cambien_longep1 PIN_A6
#define cambien_longep2 PIN_A7
#define cambien_hopep PIN_A8
// #define cambien_thungracchai PIN_B1

#define cambien_cua1 PIN_B3
#define cambien_cua2 PIN_B4
#define cambien_cua3 PIN_B5
#define cambien_cuamay PIN_B6
#define cambien_cuathungrac PIN_B7
#define nutnhan 0

#define motor_nghien_thuan RL1
#define motor_nghien_nghich RL2
#define Motor_nhavat_thuan RL3
#define Motor_nhavat_nghich RL4
#define motor_ep_thuan RL5
// #define motor_ep_nghich RL4

#define RS485_RX_PIN 32
#define RS485_TX_PIN 33
#define RS485_DE_PIN 4

#define LED_ONBOARD 2

#define RE PWM1
#define GR PWM2
#define BL PWM3
#define LED_NHANDIEN PWM4

enum PortA_Pin
{
    PIN_A8 = 0,
    PIN_A7,
    PIN_A6,
    PIN_A5,
    PIN_A4,
    PIN_A3,
    PIN_A2,
    PIN_A1
};

enum PortB_Pin
{
    PIN_B8 = 0,
    PIN_B7,
    PIN_B6,
    PIN_B5,
    PIN_B4,
    PIN_B3,
    PIN_B2,
    PIN_B1
};

enum PortMotor_Pin
{
    PWMR_M1 = 0,
    PWML_M1,
    PWMR_M2,
    PWML_M2,
    PWMR_M3,
    PWML_M3,
    PWMR_M4,
    PWML_M4
};

enum PortRelay_Pin
{
    RL1 = 0,
    RL2,
    RL3,
    RL4,
    RL5,
    PWMR_M5,
    PWML_M5,
};

enum PortPWM_Pin
{
    PWM1 = 0,
    PWM2,
    PWM3,
    PWM4,
    PWM5,
    PWM6,
    PWM7,
    PWM8
};

Adafruit_PWMServoDriver pwm_PortLED = Adafruit_PWMServoDriver();

PCF8574 pcf_Motor(addr_ExIO_Motor, &Wire);
PCF8574 pcf_PortA(addr_ExIO_portA, &Wire);
PCF8574 pcf_PortB(addr_ExIO_portB, &Wire);
PCF8574 pcf_PortRelay(addr_ExIO_OutputRL, &Wire);

Motor_BTS7960 Motor_M1(pcf_Motor, EN_M1, PWML_M1, PWMR_M1);
Motor_BTS7960 Motor_M2(pcf_Motor, EN_M2, PWML_M2, PWMR_M2);
Motor_BTS7960 Motor_M3(pcf_Motor, EN_M3, PWML_M3, PWMR_M3);
Motor_BTS7960 Motor_M4(pcf_Motor, EN_M4, PWML_M4, PWMR_M4);
Motor_BTS7960 Motor_M5(pcf_PortRelay, EN_M5, PWML_M5, PWMR_M5);

Input_PCF8574 CamBien1(pcf_PortA, PIN_A1);
Input_PCF8574 CamBien2(pcf_PortA, PIN_A2);
Input_PCF8574 CamBien3(pcf_PortA, PIN_A3);
Input_PCF8574 CamBien4(pcf_PortA, PIN_A4);
Input_PCF8574 CamBien5(pcf_PortA, PIN_A5);
Input_PCF8574 CamBien6(pcf_PortA, PIN_A6);
Input_PCF8574 CamBien7(pcf_PortA, PIN_A7);
Input_PCF8574 CamBien8(pcf_PortA, PIN_A8);

Input_PCF8574 CamBien9(pcf_PortB, PIN_B1);
Input_PCF8574 CamBien10(pcf_PortB, PIN_B2);
Input_PCF8574 CamBien11(pcf_PortB, PIN_B3);
Input_PCF8574 CamBien12(pcf_PortB, PIN_B4);
Input_PCF8574 CamBien13(pcf_PortB, PIN_B5);
Input_PCF8574 CamBien14(pcf_PortB, PIN_B6);
Input_PCF8574 CamBien15(pcf_PortB, PIN_B7);
Input_PCF8574 CamBien16(pcf_PortB, PIN_B8);

Input_PCF8574 *CambienArray[] = {&CamBien1, &CamBien2, &CamBien3, &CamBien4, &CamBien5, &CamBien6, &CamBien7, &CamBien8, &CamBien9, &CamBien10, &CamBien11, &CamBien12, &CamBien13, &CamBien14, &CamBien15, &CamBien16};

#endif //__HARDWARE_CONFIG_H__