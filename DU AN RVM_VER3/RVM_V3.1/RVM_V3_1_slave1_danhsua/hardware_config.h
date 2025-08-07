#ifndef __HARDWARE_CONFIG_H__
#define __HARDWARE_CONFIG_H__

#include "src/ReadSensorDebounce.h"
#include "src/Motor_DriverDIY.h"
#include "RVM_V3_slave.h"

#define RS485_RX_PIN 48
#define RS485_TX_PIN 38

#define SEN1 15
#define SEN2 7
#define SEN3 6
#define SEN4 5
#define SEN5 4

#define CONTROL_PIN1 40
#define PWM1 41

#define CONTROL_PIN2 1
#define PWM2 2

#define CONTROL_PIN3 10
#define PWM3 9

#define CONTROL_PIN4 12
#define PWM4 11

#define CONTROL_PIN5 14
#define PWM5 13

#define Relay1 47
#define Relay2 21

#define LED_ONBOARD 39

#define bootPin 0

Sensor CB_Encoder(SEN2, LOW);
Sensor CB_PhanLoai1_Trai(SEN3, LOW);
Sensor CB_PhanLoai1_Phai(SEN4, LOW);

Sensor CB_DauEp(SEN1, LOW);
Sensor CB_LongEp1(SEN4, LOW);
Sensor CB_LongEp2(SEN5, LOW);

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
    const uint8_t DUNGLAI = TrangThaiMotor::STOP;
    const uint8_t NANGLEN = TrangThaiMotor::FORWARD;
    const uint8_t HAXUONG = TrangThaiMotor::REVERSE;
};

namespace TrangThaiMotorBangTai
{
    const uint8_t DUNGLAI = TrangThaiMotor::STOP;
    const uint8_t CHAYVAO = TrangThaiMotor::FORWARD;
    const uint8_t CHAYRA = TrangThaiMotor::REVERSE;
}

namespace TrangThaiMotorNangBangTai
{
    const uint8_t DUNGLAI = TrangThaiMotor::STOP;
    const uint8_t NANGLEN = TrangThaiMotor::FORWARD;
    const uint8_t HAXUONG = TrangThaiMotor::REVERSE;
}

namespace TrangThaiMotorXoayChai
{
    const uint8_t STOP = TrangThaiMotor::STOP;
    const uint8_t LEFT = TrangThaiMotor::FORWARD;
    const uint8_t RIGHT = TrangThaiMotor::REVERSE;
}

namespace TrangThaiMotorPhanLoai1
{
    const uint8_t STOP = TrangThaiMotor::STOP;
    const uint8_t LEFT = TrangThaiMotor::FORWARD;
    const uint8_t RIGHT = TrangThaiMotor::REVERSE;
}

namespace TrangThaiMotorPhanLoai2
{
    const uint8_t STOP = TrangThaiMotor::STOP;
    const uint8_t LEFT = TrangThaiMotor::FORWARD;
    const uint8_t RIGHT = TrangThaiMotor::REVERSE;
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

//-----------Struc CoCauMotorDC và khởi tại đối tượng motor---------------//
#define TOCDOBANDAU 50     // Tốc độ ban đầu khi tăng tốc động cơ DC 50/255
#define BUOCNHAYTANGTOC 10 // Số bước nhảy đến tốc độ tối đa
#define THOIGIANBUOC 100   // Thời gian giữa hai bước nhảy tăng tốc

Motor_DriverDIY Motor_M1(CONTROL_PIN1, PWM1);
Motor_DriverDIY Motor_M2(CONTROL_PIN2, PWM2);
Motor_DriverDIY Motor_M3(CONTROL_PIN3, PWM3);
Motor_DriverDIY Motor_M4(CONTROL_PIN4, PWM4);
Motor_DriverDIY Motor_M5(CONTROL_PIN5, PWM5);

#if 1
struct CoCauMotorDC
{
    Motor_DriverDIY motor;
    unsigned long timeBegin;
    // unsigned long timedelay;

    uint8_t TrangThai;
    uint8_t YeuCau;
    // uint8_t trangthaiLoi;

    uint8_t TrangThaiTocDo;
    unsigned long timeGiaToc;

    int Tocdo;
    int TocdoMax;

    bool DieuKhienNguoc;

    // Constructor truyền tham số
    CoCauMotorDC(Motor_DriverDIY &Motor) : motor(Motor)
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

    CoCauMotorDC(Motor_DriverDIY &Motor, bool doichieu) : motor(Motor)
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

    CoCauMotorDC(Motor_DriverDIY &Motor, bool doichieu, uint8_t setTocDo) : motor(Motor), TocdoMax(setTocDo)
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

CoCauMotorDC MotorPhanLoai1(Motor_M1, true);
CoCauMotorDC MotorPhanLoai2(Motor_M2, false);

#endif

struct CoCauMotorServo
{
    uint8_t relayForward;
    uint8_t relayReverse;

    uint8_t TrangThai;
    uint8_t YeuCau;
    // uint8_t trangthaiLoi;

    CoCauMotorServo(uint8_t relayForward, uint8_t relayReverse) : relayForward(relayForward), relayReverse(relayReverse)
    {
        TrangThai = TrangThaiMotor::STOP;
        YeuCau = TrangThaiMotor::STOP;
    }
    // Hàm thành viên
    void runForward()
    {
        digitalWrite(relayForward, HIGH);
        digitalWrite(relayReverse, LOW);
    }

    void runReverse()
    {
        digitalWrite(relayForward, LOW);
        digitalWrite(relayReverse, HIGH);
    }

    void Stop()
    {
        digitalWrite(relayForward, LOW);
        digitalWrite(relayReverse, LOW);
    }
};

CoCauMotorServo MotorNghienChai(Relay1, Relay2);
CoCauMotorServo MotorEpLon(Relay1, Relay2);

#endif //__HARDWARE_CONFIG_H__