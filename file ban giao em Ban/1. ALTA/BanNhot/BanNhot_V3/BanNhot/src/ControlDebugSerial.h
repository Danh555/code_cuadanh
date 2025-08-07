#ifndef CONTROLDEBUGSERIAL_H
#define CONTROLDEBUGSERIAL_H

#include <Arduino.h>
#include <HardwareSerial.h>

// Định nghĩa để bật hoặc tắt debug
#define DEBUG_SERIAL 1 // Đặt 1 để bật, 0 để tắt

#if DEBUG_SERIAL
class ControlDebugSerial
{
private:
    HardwareSerial &_serial; // Chỉ sử dụng HardwareSerial

public:
    // Constructor: Nhận đối tượng HardwareSerial
    ControlDebugSerial(HardwareSerial &serialInstance) : _serial(serialInstance) {}

    // Khởi động Serial
    void begin(unsigned long baudRate)
    {
        _serial.begin(baudRate);
    }

    // Hàm in printf
    void printf(const char *format, ...)
    {
        char buffer[128]; // Kích thước buffer tùy chỉnh
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        _serial.print(buffer);
    }

    // Hàm print và println
    void print(const char *message)
    {
        _serial.print(message);
    }

    void println(const char *message)
    {
        _serial.println(message);
    }

    void print(int value)
    {
        _serial.print(value);
    }

    void println(int value)
    {
        _serial.println(value);
    }

    void print(float value)
    {
        _serial.print(value);
    }

    void println(float value)
    {
        _serial.println(value);
    }

    // Hàm ghi dữ liệu thô
    void write(const uint8_t *data, size_t length)
    {
        _serial.write(data, length);
    }
};

#else

// Nếu DEBUG_SERIAL tắt, định nghĩa lớp Dummy không thực hiện gì
class ControlDebugSerial
{
public:
    ControlDebugSerial(HardwareSerial &) {}
    void begin(unsigned long) {}
    void printf(const char *, ...) {}
    void print(const char *) {}
    void println(const char *) {}
    void print(int) {}
    void println(int) {}
    void print(float) {}
    void println(float) {}
    void write(const uint8_t *, size_t) {}
};

#endif

#endif // CONTROLDEBUGSERIAL_H
