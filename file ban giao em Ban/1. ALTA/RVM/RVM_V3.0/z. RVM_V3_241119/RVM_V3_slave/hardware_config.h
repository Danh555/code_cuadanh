#ifndef __HARDWARE_CONFIG_H__
#define __HARDWARE_CONFIG_H__

#include "src/PCF8574.h"
#include "src/Motor_BTS7960.h"
#include "src/Input_PCF8574.h"
#include "src/Relay_PCF8574.h"
#include "RVM_V3_slave.h"

#define RS485_RX_PIN 32
#define RS485_TX_PIN 33
#define RS485_DE_PIN 4

#define EN_M1 13
#define EN_M2 14
#define EN_M3 27
#define EN_M4 26
#define EN_M5 25

#define nutnhan 0

#define addr_ExIO_portA 0x20
#define addr_ExIO_portB 0x24
#define addr_ExIO_Motor 0x22    //(Motor from 1 to 4)
#define addr_ExIO_OutputRL 0x26 //(Relay and Motor 5)
#define addr_Output_PWM 0x40

#define LED_ONBOARD 2

#define RE PWM1
#define GR PWM2
#define BL PWM3
#define LED_NHANDIEN PWM4

//-----------Định nghĩa chân Port------------//

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

//----------Trạng thái động cơ------------//
#if 1
enum TrangThaiMotor : uint8_t
{
    STOP = 0,
    FORWARD = 1,
    REVERSE = 2
};

enum TrangThaiLoi : uint8_t
{
    KHONGLOI = 0,
    COLOI = 1
};

enum TrangThaiTocDoMotor
{
    ONDINH = 0,
    GIATOC = 1

};

namespace TrangThaiMotorCua
{
    const uint8_t CUA_DUNGLAI = TrangThaiMotor::STOP;
    const uint8_t CUA_NANGLEN = TrangThaiMotor::FORWARD;
    const uint8_t CUA_HAXUONG = TrangThaiMotor::REVERSE;
};

namespace TrangThaiMotorBangTai
{
    const uint8_t BANGTAI_DUNGLAI = TrangThaiMotor::STOP;
    const uint8_t BANGTAI_CHAYVAO = TrangThaiMotor::FORWARD;
    const uint8_t BANGTAI_CHAYRA = TrangThaiMotor::REVERSE;
}

namespace TrangThaiMotorNangBangTai
{
    const uint8_t NANGBANGTAI_DUNGLAI = TrangThaiMotor::STOP;
    const uint8_t NANGBANGTAI_NANGLEN = TrangThaiMotor::FORWARD;
    const uint8_t NANGBANGTAI_HAXUONG = TrangThaiMotor::REVERSE;
}

namespace TrangThaiMotorXoayChai
{
    const uint8_t XOAYCHAI_STOP = TrangThaiMotor::STOP;
    const uint8_t XOAYCHAI_LEFT = TrangThaiMotor::FORWARD;
    const uint8_t XOAYCHAI_RIGHT = TrangThaiMotor::REVERSE;
}

namespace TrangThaiMotorPhanLoai1
{
    const uint8_t PHANLOAI1_STOP = TrangThaiMotor::STOP;
    const uint8_t PHANLOAI1_LEFT = TrangThaiMotor::FORWARD;
    const uint8_t PHANLOAI1_RIGHT = TrangThaiMotor::REVERSE;
}

namespace TrangThaiMotorPhanLoai2
{
    const uint8_t PHANLOAI2_STOP = TrangThaiMotor::STOP;
    const uint8_t PHANLOAI2_LEFT = TrangThaiMotor::FORWARD;
    const uint8_t PHANLOAI2_RIGHT = TrangThaiMotor::REVERSE;
}
namespace TrangThaiMotorNghien
{
    const uint8_t STOP = TrangThaiMotor::STOP;
    const uint8_t FORWARD = TrangThaiMotor::FORWARD;
    const uint8_t REVERSE = TrangThaiMotor::REVERSE;
}
namespace TrangThaiMotorEp
{
    const uint8_t STOP = TrangThaiMotor::STOP;
    const uint8_t FORWARD = TrangThaiMotor::FORWARD;
    const uint8_t REVERSE = TrangThaiMotor::REVERSE;
}

#endif

/*--- Khởi tạo đối tượng IC PCF8574 ---*/
PCF8574 pcf_Motor(addr_ExIO_Motor, &Wire);
PCF8574 pcf_PortA(addr_ExIO_portA, &Wire);
PCF8574 pcf_PortB(addr_ExIO_portB, &Wire);
PCF8574 pcf_PortRelay(addr_ExIO_OutputRL, &Wire);

/*--- Khởi tạo đối tượng motor ---*/
Motor_BTS7960 Motor_M1(pcf_Motor, EN_M1, PWML_M1, PWMR_M1);
Motor_BTS7960 Motor_M2(pcf_Motor, EN_M2, PWML_M2, PWMR_M2);
Motor_BTS7960 Motor_M3(pcf_Motor, EN_M3, PWML_M3, PWMR_M3);
Motor_BTS7960 Motor_M4(pcf_Motor, EN_M4, PWML_M4, PWMR_M4);
Motor_BTS7960 Motor_M5(pcf_PortRelay, EN_M5, PWML_M5, PWMR_M5);

/*--- Khởi tạo đối tượng relay ---*/
Relay_PCF8574 Relay1(pcf_PortRelay, RL1);
Relay_PCF8574 Relay2(pcf_PortRelay, RL2);
Relay_PCF8574 Relay3(pcf_PortRelay, RL3);
Relay_PCF8574 Relay4(pcf_PortRelay, RL4);
Relay_PCF8574 Relay5(pcf_PortRelay, RL5);

/*--- Khởi tạo đối tượng cảm biến ---*/
Input_PCF8574 CB_PhanLoai1_Trai(pcf_PortA, PIN_A1, LOW);
Input_PCF8574 CB_PhanLoai1_Phai(pcf_PortA, PIN_A2, LOW);
Input_PCF8574 CB_LongEp1(pcf_PortA, PIN_A3, LOW);
Input_PCF8574 CB_LongEp2(pcf_PortA, PIN_A4, LOW);
Input_PCF8574 CB_DauEp(pcf_PortA, PIN_A5, LOW);
Input_PCF8574 CB_Encoder(pcf_PortA, PIN_A6, LOW);

Input_PCF8574 CB_PhanLoai2_Trai(pcf_PortA, PIN_A7, LOW);
Input_PCF8574 CB_PhanLoai2_Phai(pcf_PortA, PIN_A8, LOW);

Input_PCF8574 *CB_Array[] = {&CB_Encoder, &CB_PhanLoai1_Trai, &CB_PhanLoai1_Phai, &CB_DauEp, &CB_LongEp1, &CB_LongEp2, &CB_PhanLoai2_Trai, &CB_PhanLoai2_Phai};

//-----------Struc CoCauMotorDC và khởi tại đối tượng motor---------------//
#define TOCDOBANDAU 50     // Tốc độ ban đầu khi tăng tốc động cơ DC 50/255
#define BUOCNHAYTANGTOC 10 // Số bước nhảy đến tốc độ tối đa
#define THOIGIANBUOC 100   // Thời gian giữa hai bước nhảy tăng tốc

#if 1
struct CoCauMotorDC
{
    Motor_BTS7960 &motor;
    unsigned long timeBegin;
    unsigned long timedelay;

    uint8_t TrangThai;
    uint8_t YeuCau;
    // uint8_t trangthaiLoi;

    uint8_t TrangThaiTocDo;
    unsigned long timeGiaToc;

    int Tocdo;
    int TocdoMax;

    bool DieuKhienNguoc;

    // Constructor truyền tham số
    CoCauMotorDC(Motor_BTS7960 &Motor, unsigned long timedelayMacDinh) : motor(Motor), timedelay(timedelayMacDinh)
    {
        timeBegin = 0;
        timeGiaToc = 0;
        TrangThai = TrangThaiMotor::STOP;
        YeuCau = TrangThaiMotor::STOP;
        // trangthaiLoi = TrangThaiLoi::KHONGLOI;
        TrangThaiTocDo = TrangThaiTocDoMotor::ONDINH;
        Tocdo = 0;
        TocdoMax = 255;
        DieuKhienNguoc = false;
    }

    CoCauMotorDC(Motor_BTS7960 &Motor, unsigned long timedelayMacDinh, bool doichieu) : motor(Motor), timedelay(timedelayMacDinh)
    {
        timeBegin = 0;
        timeGiaToc = 0;
        TrangThai = TrangThaiMotor::STOP;
        YeuCau = TrangThaiMotor::STOP;
        // trangthaiLoi = TrangThaiLoi::KHONGLOI;
        TrangThaiTocDo = TrangThaiTocDoMotor::ONDINH;
        Tocdo = 0;
        TocdoMax = 255;
        DieuKhienNguoc = doichieu;
    }

    CoCauMotorDC(Motor_BTS7960 &Motor, unsigned long timedelayMacDinh, bool doichieu, uint8_t setTocDo) : motor(Motor), timedelay(timedelayMacDinh), TocdoMax(setTocDo)
    {
        timeBegin = 0;
        timeGiaToc = 0;
        TrangThai = TrangThaiMotor::STOP;
        YeuCau = TrangThaiMotor::STOP;
        // trangthaiLoi = TrangThaiLoi::KHONGLOI;
        TrangThaiTocDo = TrangThaiTocDoMotor::ONDINH;
        Tocdo = 0;
        DieuKhienNguoc = doichieu;
    }
    // Hàm thành viên
    void runForward(uint8_t speed)
    {
        if (DieuKhienNguoc)
            motor.TurnLeft(speed);
        else
            motor.TurnRight(speed);
    }

    void runReverse(uint8_t speed)
    {
        if (DieuKhienNguoc)
            motor.TurnRight(speed);
        else
            motor.TurnLeft(speed);
    }

    void Stop()
    {
        motor.Stop();
        Tocdo = 0;
    }

    void TangTocForward()
    {
        if (millis() - timeGiaToc > THOIGIANBUOC && TrangThaiTocDo == TrangThaiTocDoMotor::GIATOC)
        {
            Serial.println(Tocdo);
            timeGiaToc = millis();
            if (Tocdo < TocdoMax)
            {
                (Tocdo + (TocdoMax / BUOCNHAYTANGTOC + 1)) >= TocdoMax ? Tocdo = TocdoMax : Tocdo += (TocdoMax / BUOCNHAYTANGTOC + 1);
                runForward(Tocdo);
            }
            else if (Tocdo >= TocdoMax)
            {
                Tocdo = TocdoMax;
                runForward(Tocdo);
                TrangThaiTocDo = TrangThaiTocDoMotor::ONDINH;
            }
        }
    }

    void TangTocReverse()
    {
        if (millis() - timeGiaToc > THOIGIANBUOC && TrangThaiTocDo == TrangThaiTocDoMotor::GIATOC)
        {
            Serial.println(Tocdo);
            timeGiaToc = millis();
            if (Tocdo < TocdoMax)
            {
                (Tocdo + (TocdoMax / BUOCNHAYTANGTOC + 1)) >= TocdoMax ? Tocdo = TocdoMax : Tocdo += (TocdoMax / BUOCNHAYTANGTOC + 1);
                runReverse(Tocdo);
            }
            else if (Tocdo >= TocdoMax)
            {
                Tocdo = TocdoMax;
                runReverse(Tocdo);
                TrangThaiTocDo = TrangThaiTocDoMotor::ONDINH;
            }
        }
    }
};

CoCauMotorDC MotorPhanLoai1(Motor_M1, 10000);
CoCauMotorDC MotorPhanLoai2(Motor_M2, 10000, true);

CoCauMotorDC *MotorDCArray[] = {&MotorPhanLoai1, &MotorPhanLoai2};

#endif

struct CoCauMotorServo
{
    Relay_PCF8574 &relayForward;
    Relay_PCF8574 &relayReverse;

    // unsigned long timeBegin;
    // unsigned long timedelay;

    uint8_t TrangThai;
    uint8_t YeuCau;
    // uint8_t trangthaiLoi;

    CoCauMotorServo(Relay_PCF8574 &relayForward, Relay_PCF8574 &relayReverse) : relayForward(relayForward), relayReverse(relayReverse)
    {
        TrangThai = TrangThaiMotor::STOP;
        YeuCau = TrangThaiMotor::STOP;
        // trangthaiLoi = TrangThaiLoi::KHONGLOI;
    }
    // Hàm thành viên
    void runForward()
    {
        relayForward.ON();
        relayReverse.OFF();
    }

    void runReverse()
    {
        relayForward.OFF();
        relayReverse.ON();
    }

    void Stop()
    {
        relayForward.OFF();
        relayReverse.OFF();
    }
};

CoCauMotorServo MotorNghienChai(Relay1, Relay2);
CoCauMotorServo MotorEpLon(Relay3, Relay4);

CoCauMotorServo *MotorServoArray[] = {&MotorNghienChai, &MotorEpLon};

#endif //__HARDWARE_CONFIG_H__