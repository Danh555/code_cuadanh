#ifndef CONTROLDEBUGSERIAL_H
#define CONTROLDEBUGSERIAL_H

#include <Arduino.h>
#include <HardwareSerial.h>

// Định nghĩa để bật hoặc tắt debug
#define DEBUG_SERIAL 1 // Đặt 1 để bật, 0 để tắt

#if DEBUG_SERIAL
class ControlDebugSerial
{
public:
    // Khởi động Serial
    void begin(unsigned long baudRate)
    {
        Serial.begin(baudRate);
    }

    // Hàm in printf
    void printf(const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        Serial.printf(format, args);
        va_end(args);
    }

    // Hàm ghi dữ liệu thô
    void write(const uint8_t *data, size_t length)
    {
        Serial.write(data, length);
    }
};

// Khởi tạo đối tượng ControlDebugSerial
ControlDebugSerial debug;

#else

// Nếu DEBUG_SERIAL tắt, định nghĩa lớp Dummy không thực hiện gì
class ControlDebugSerial
{
public:
    void begin(unsigned long) {}
    void printf(const char *, ...) {}
    void write(const uint8_t *, size_t) {}
};

// Khởi tạo đối tượng ControlDebugSerial không hoạt động
ControlDebugSerial debug;

#endif

void setup()
{
    debug.begin(115200); // Chỉ thực hiện nếu DEBUG_SERIAL = 1
    debug.printf("Debugging is enabled!\n");
}

void loop()
{
    debug.printf("Current time: %lu ms\n", millis());
    const char message[] = "Hello, ControlDebugSerial!\n";
    debug.write((const uint8_t *)message, sizeof(message) - 1);
    delay(1000);
}

#endif // READSERIAL_H