#ifndef BUTTON_DANH_H
#define BUTTON_DANH_H

#include <Arduino.h>

typedef struct {
    int pin;                        // Chân nút bấm
    int state;                      // Trạng thái hiện tại của nút
    int lastState;                  // Trạng thái trước đó của nút
    unsigned long lastDebounceTime; // Thời gian lần cuối trạng thái thay đổi
    unsigned long debounceDelay;    // Thời gian chống dội
} Button;

void setting_Button(Button *btn, int pin, unsigned long debounceDelay);// KHỞI TẠO NÚT NHẤN

int readButton(Button *btn, uint32_t timeout=0); // ĐỌC TRẠNG THÁI NÚT (TRUE=PHÁT HIỆN NHẤN XUỐNG)

#endif