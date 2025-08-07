
// Slave1 là điều khiển bộ nghiền và bộ phân loại

#include "RVM_V3_slave.h"

uint8_t selfSlave1ID = 0x01; // Đặt Slave ID
RS485_Slave RS485(Serial1, selfSlave1ID, RS485_TX_PIN, RS485_RX_PIN);

static TimerHandle_t auto_reload_timer = NULL;

void setup()
{
    SerialDebug.begin(115200);
    RS485.begin(115200);
    SerialDebug.onEvent(usbEventCallback);

    SerialDebug.println("\nRVM_Slave1 [V3.1]");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    if (!EEPROM.begin(1024))
    {
        SerialDebug.println("Failed to initialise EEPROM");
        return;
    }

    uint16_t value = EEPROM.read(EEPROM_ADD::ADD_COUNT_RESET0) | (EEPROM.read(EEPROM_ADD::ADD_COUNT_RESET1) << 8);
    SerialDebug.print("\nSố lần reset: ");
    SerialDebug.println(++value);
    EEPROM.write(EEPROM_ADD::ADD_COUNT_RESET0, value);
    EEPROM.write(EEPROM_ADD::ADD_COUNT_RESET1, (value >> 8) & 0xFF);
    EEPROM.commit();

#if 1
    Motor_M1.setup();
    Motor_M2.setup();
    Motor_M3.setup();
    Motor_M4.setup();
    Motor_M5.setup();

#endif

    SerialDebug.print("\nRVM_Slave [");
    for (uint8_t i = 0; i < 3; i++)
    {
        if (i == 2)
        {
            SerialDebug.print((firmware[i] * 255 + firmware[i + 1]), DEC);
            SerialDebug.println("]");
        }
        else
        {
            SerialDebug.print(firmware[i], DEC);
            SerialDebug.print(".");
        }
    }

    CB_Encoder.setup();
    CB_PhanLoai1_Trai.setup();
    CB_PhanLoai1_Phai.setup();
    CB_Rac1.setup();
    CB_Rac2.setup();

    pinMode(LED_ONBOARD, OUTPUT);

    // GET_EEPROM();
    // SET_ERROR_EEPROM();

    vTaskDelay(200 / portTICK_PERIOD_MS);

    xTaskCreatePinnedToCore( // Use xTaskCreate() in vanilla FreeRTOS
        taskControlAll,      // Function to be called
        "taskControlAll",    // Name of task
        10240,               // Stack size (bytes in ESP32, words in FreeRTOS)
        NULL,                // Parameter to pass
        2,                   // Task priority (must be same to prevent lockup)
        NULL,                // Task handle
        1);                  // Run on one core for demo purposes (ESP32 only)

    xTaskCreatePinnedToCore(taskCommunicateToR485Master, "taskCommunicateToR485Master", 10240, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(taskControlMotor, "taskControlMotor", 10240, NULL, 1, NULL, 1);

    xTaskCreatePinnedToCore(taskDebug, "taskDebug", 2048, NULL, 1, NULL, 1);

    // xTaskCreatePinnedToCore(taskTest, "taskTest", 10240, NULL, 1, NULL, 1);

    // Create an auto-reload timer
    auto_reload_timer = xTimerCreate(
        "Auto-reload timer",    // Name of timer
        2 / portTICK_PERIOD_MS, // Period of timer (in ticks)
        pdTRUE,                 // Auto-reload
        (void *)1,              // Timer ID
        timerReadSensor);       // Callback function

    xTimerStart(auto_reload_timer, portMAX_DELAY);
    // Delete "setup and loop" task
    vTaskDelete(NULL);
}

void timerReadSensor(void *pvParameters)
{
    CB_Encoder.readDebounce(10);
    CB_PhanLoai1_Trai.readDebounce(10);
    CB_PhanLoai1_Phai.readDebounce(10);
    CB_Rac1.readDebounce(2000);
    CB_Rac2.readDebounce(2000);
}

void taskCommunicateToR485Master(void *pvParameters)
{
    while (1)
    {

        if (ui8_update_firmware == 1)
        {
            receiveFirmware();
        }

        blink(1000);

        if (ui8_phanhoi_TrangthaiLoi == 1 || ui8_phanhoi_firmware == 1)
            ui8_trangthaiRS485 = trangthaiRS485::DANGGUI;
        else
            ui8_trangthaiRS485 = trangthaiRS485::DANGNHAN;

        // Xử lý khi ở trạng thái nhận dữ liệu đến master
        if (ui8_trangthaiRS485 == trangthaiRS485::DANGNHAN)
        {
            // SerialDebug.println("\ndang nhan");
            RS485.receive(buffer_receive);
#ifdef ShowSerial
            if (RS485.isReceived)
            {
                SerialDebug.print("\nReceive: ");
                for (uint8_t i = 0; i <= buffer_receive[0]; i++)
                {
                    SerialDebug.print(buffer_receive[i], HEX);
                    SerialDebug.print(" ");
                }
            }
#endif
            processRS485();
        }

        // Xử lý khi ở trạng thái gửi dữ liệu đến master
        else if (ui8_trangthaiRS485 == trangthaiRS485::DANGGUI)
        {
            SerialDebug.println("\ndang gui");
            if (ui8_phanhoi_TrangthaiLoi == 1)
            {
                phanhoi_trangthailoi();
                ui8_phanhoi_TrangthaiLoi = 0;
            }
            if (ui8_phanhoi_firmware == 1)
            {
                phanhoi_firmware();
                ui8_phanhoi_firmware = 0;
            }
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void taskControlMotor(void *pvParameters)
{
    while (1)
    {

#if 1
        for (CoCauMotorDC *Motor : MotorDCArray)
        {
            if ((*Motor).TrangThai == STOP && (*Motor).YeuCau == STOP)
            {
            }
            else if ((*Motor).TrangThai == STOP && (*Motor).YeuCau == FORWARD)
            {
                (*Motor).TrangThai = FORWARD;
                (*Motor).timeBegin = millis();
                (*Motor).timeGiaToc = millis();
                (*Motor).TrangThaiTocDo = TrangThaiTocDoMotor::GIATOC;
                (*Motor).Tocdo = TOCDOBANDAU;
            }
            else if ((*Motor).TrangThai == STOP && (*Motor).YeuCau == REVERSE)
            {
                (*Motor).TrangThai = REVERSE;
                (*Motor).timeBegin = millis();
                (*Motor).timeGiaToc = millis();
                (*Motor).TrangThaiTocDo = TrangThaiTocDoMotor::GIATOC;
                (*Motor).Tocdo = TOCDOBANDAU;
            }
            else if ((*Motor).TrangThai == FORWARD && (*Motor).YeuCau == STOP)
            {
                (*Motor).Stop();
                (*Motor).TrangThai = STOP;
            }
            else if ((*Motor).TrangThai == FORWARD && (*Motor).YeuCau == FORWARD)
            {
                if ((*Motor).TrangThaiTocDo == TrangThaiTocDoMotor::ONDINH)
                    continue;
                else
                    (*Motor).TangTocForward();
            }
            else if ((*Motor).TrangThai == FORWARD && (*Motor).YeuCau == REVERSE)
            {
                (*Motor).Stop();
                (*Motor).TrangThai = STOP;
            }
            else if ((*Motor).TrangThai == REVERSE && (*Motor).YeuCau == STOP)
            {
                (*Motor).Stop();
                (*Motor).TrangThai = STOP;
            }
            else if ((*Motor).TrangThai == REVERSE && (*Motor).YeuCau == FORWARD)
            {
                (*Motor).Stop();
                (*Motor).TrangThai = STOP;
            }
            else if ((*Motor).TrangThai == REVERSE && (*Motor).YeuCau == REVERSE)
            {
                if ((*Motor).TrangThaiTocDo == TrangThaiTocDoMotor::ONDINH)
                    continue;
                else
                    (*Motor).TangTocReverse();
            }
        }
#endif

#if 1
        for (CoCauMotorServo *Motor : MotorServoArray)
        {
            if ((*Motor).TrangThai == STOP)
                (*Motor).Stop();

            if ((*Motor).TrangThai == (*Motor).YeuCau)
            {
                continue;
            }
            else if ((*Motor).YeuCau == FORWARD)
            {
                (*Motor).runForward();
                (*Motor).TrangThai = FORWARD;
            }
            else if ((*Motor).YeuCau == REVERSE)
            {
                (*Motor).runReverse();
                (*Motor).TrangThai = REVERSE;
            }
            else if ((*Motor).YeuCau == STOP)
            {
                (*Motor).Stop();
                (*Motor).TrangThai = STOP;
            }
        }
#endif
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void taskControlAll(void *pvParameters)
{
    while (1)
    {
        kiemtra_hoatdong_phanloai();
        kiemtra_hoatdong_nghienchai();
        kiemtra_thungrac();

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    //
}

void taskDebug(void *pvParameters)
{
    while (1)
    {
        DebugSerial();

        vTaskDelay(2 / portTICK_PERIOD_MS);
    }
}
void taskTest(void *pvParameters)
{
    while (1)
    {

        for (Sensor *Sen : CB_Array)
        {
            SerialDebug.print((*Sen).state);
            SerialDebug.print(" ");
        }
        SerialDebug.println();
        vTaskDelay(1000 / portTICK_PERIOD_MS);

#if 0
        MotorPhanLoai1.YeuCau = TrangThaiMotorPhanLoai1::LEFT;
        vTaskDelay(100 / portTICK_PERIOD_MS);
        MotorPhanLoai2.YeuCau = TrangThaiMotorPhanLoai2::LEFT;
        SerialDebug.println("FORWARD");
        vTaskDelay(10000 / portTICK_PERIOD_MS);

        MotorPhanLoai2.YeuCau = TrangThaiMotorPhanLoai2::STOP;
        vTaskDelay(100 / portTICK_PERIOD_MS);
        MotorPhanLoai1.YeuCau = TrangThaiMotorPhanLoai1::STOP;
        SerialDebug.println("Dung lai");
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        MotorPhanLoai1.YeuCau = TrangThaiMotorPhanLoai1::RIGHT;
        vTaskDelay(100 / portTICK_PERIOD_MS);
        MotorPhanLoai2.YeuCau = TrangThaiMotorPhanLoai2::RIGHT;
        SerialDebug.println("REVERSE");
        vTaskDelay(10000 / portTICK_PERIOD_MS);

        MotorPhanLoai2.YeuCau = TrangThaiMotorPhanLoai2::STOP;
        vTaskDelay(100 / portTICK_PERIOD_MS);
        MotorPhanLoai1.YeuCau = TrangThaiMotorPhanLoai1::STOP;
        SerialDebug.println("Dung lai");
        vTaskDelay(5000 / portTICK_PERIOD_MS);

#endif
    }
}

static void usbEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == ARDUINO_HW_CDC_EVENTS)
    {
        switch (event_id)
        {
        case ARDUINO_HW_CDC_RX_EVENT:
            DebugSerial();
            break;

        default:
            break;
        }
    }
}

void GET_EEPROM()
{
    EpLon.trangthaiLoi = EEPROM.read(EEPROM_ADD::ADD_ERROR_EPLON);
    NghienChai.trangthaiLoi = EEPROM.read(EEPROM_ADD::ADD_ERROR_NGHIENCHAI);
    PhanLoai.trangthaiLoi = EEPROM.read(EEPROM_ADD::ADD_ERROR_KETPHANLOAI);
}

void ERASE_ERROR_EEPROM()
{
    EEPROM.write(EEPROM_ADD::ADD_ERROR_EPLON, 0);
    EEPROM.write(EEPROM_ADD::ADD_ERROR_NGHIENCHAI, 0);
    EEPROM.write(EEPROM_ADD::ADD_ERROR_KETPHANLOAI, 0);
    EEPROM.commit();
}

void SET_ERROR_EEPROM()
{
    EEPROM.write(EEPROM_ADD::ADD_ERROR_EPLON, EpLon.trangthaiLoi);
    EEPROM.write(EEPROM_ADD::ADD_ERROR_NGHIENCHAI, NghienChai.trangthaiLoi);
    EEPROM.write(EEPROM_ADD::ADD_ERROR_KETPHANLOAI, PhanLoai.trangthaiLoi);
    EEPROM.commit();
}

#if 1
void processRS485()
{
    if (RS485.incorrectCRC == true)
    {
        SerialDebug.println("Incorrect CRC");
        RS485.incorrectCRC = false;
        return;
    }
    if (RS485.isReceived == true)
    {
        RS485.isReceived = false;
        memcpy(order_command, (buffer_receive + 2), 2);
        if (order_command[0] == last_order_command[0] && order_command[1] == last_order_command[1])
        {
            // return;
        }
        memcpy(last_order_command, order_command, 2);

        uint8_t size = buffer_receive[0] - 4;
        uint8_t function = buffer_receive[1];
        uint8_t bytecount = buffer_receive[4];

        uint8_t data[buffer_receive[0] - 4];
        memcpy(data, buffer_receive + 5, buffer_receive[0] - 4);

        switch (function)
        {

        case codeMaster::PHANLOAI:
        {

            if (size != 1 || // Check size
                (data[0] != vitrithungracphanloai::CAN &&
                 data[0] != vitrithungracphanloai::OTHER &&
                 data[0] != vitrithungracphanloai::BOTTLE))
            {
                SerialDebug.println("Incorrect command phan loai");
                break;
            }
            else
            {
                if (data[0] == vitrithungracphanloai::BOTTLE)
                {
                    SerialDebug.println("Nhan dien BOTTLE");
                }
                else if (data[0] == vitrithungracphanloai::CAN)
                {
                    SerialDebug.println("Nhan dien CAN");
                }
                else if (data[0] == vitrithungracphanloai::OTHER)
                {
                    SerialDebug.println("Nhan dien OTHER");
                }
                PhanLoai.vitriYeucau = data[0];
                // SerialDebug.println("correct command phan loai");
                xuly_yeucau_phanloai();
                break;
            }
        }

        case codeMaster::NGHIENCHAI:
        {

            if (size != 1 || // Check size
                (data[0] != TrangThaiMotor::STOP &&
                 data[0] != TrangThaiMotor::FORWARD &&
                 data[0] != TrangThaiMotor::REVERSE))
            {

                SerialDebug.println(" Incorrect command nghien chai");
                break;
            }
            else
            {
                NghienChai.trangthaiYeuCau = data[0];
                xuly_yeucau_nghienchai();
                break;
            }
        }

        case codeMaster::check_error:
        {
            if (size != 1 || // Check size
                data[0] != 0x00)
            {
                SerialDebug.println(" Incorrect command check error");
                break;
            }
            else
            {
                ui8_phanhoi_TrangthaiLoi = 1;
                break;
            }
        }

        case codeMaster::erase_error:
        {

            if (size != 1 || // Check size
                data[0] != 0x00)
            {
                SerialDebug.println(" Incorrect command erase error");
                break;
            }
            else
            {
                // ERASE_ERROR_EEPROM();
                // PhanLoai.trangthaiLoi = TrangThaiLoi::KHONGLOI;
                // NghienChai.trangthaiLoi = TrangThaiLoi::KHONGLOI;
                // SerialDebug.println("Da xoa loi");
                xuly_yeucau_xoaloi();
                break;
            }
        }

        case codeMaster::admin_control_motor:
        {
            if (size == 2 && // Check size
                (data[0] == 0xFF &&
                 data[1] == 0xFF))
            {
                SerialDebug.println("Exit admin control motor");
                ui8_admin_control_motor = false;
                break;
            }
            else if (size != 2 || // Check size
                     (data[1] != TrangThaiMotor::STOP &&
                      data[1] != TrangThaiMotor::FORWARD &&
                      data[1] != TrangThaiMotor::REVERSE))
            {
                SerialDebug.println(" Incorrect command admin control motor");
                break;
            }
            else
            {
                ui8_admin_control_motor = true;
                SerialDebug.println("Correct command admin control motor");
                switch (data[0])
                {
                case admin_control_motor::PHANLOAI1:
                {
                    // SerialDebug.println(" correct command admin control phanloai 1");
                    MotorPhanLoai1.YeuCau = data[1];
                    break;
                }
                case admin_control_motor::PHANLOAI2:
                {
                    // SerialDebug.println(" correct command admin control phanloai 2");
                    MotorPhanLoai2.YeuCau = data[1];
                    break;
                }
                case admin_control_motor::NGHIENCHAI:
                {
                    // SerialDebug.println(" correct command nghien chai");
                    MotorNghienChai.YeuCau = data[1];
                    break;
                }
                default:
                    SerialDebug.println(" Incorrect command admin control motor *");
                    break;
                }
                break;
            }
        }

        case codeMaster::check_firmware:
        {
            if (size != 1 || // Check size
                data[0] != 0x00)
            {
                SerialDebug.println("Incorrect command get firmware");
                break;
            }
            else
            {
                ui8_phanhoi_firmware = true;
                break;
            }
        }

        case codeMaster::reset:
        {
            if (size != 1 || // Check size
                data[0] != 0x00)
            {
                SerialDebug.println("Incorrect command reset");
                break;
            }
            else
            {
                ESP.restart();
                break;
            }
        }
        case codeMaster::update_firmware:
        {
            if (size != 1 || // Check size
                data[0] != 0x00)
            {
                SerialDebug.println("Incorrect command update firmware");
                break;
            }
            else
            {
                ui8_update_firmware = true;
                break;
            }
        }

        default:
            SerialDebug.println(" Incorrect command all");
            break;
        }
    }
}
#endif

void goHome()
{
    PhanLoai.vitriYeucau = vitrithungracphanloai::CAN;
    xuly_yeucau_phanloai();
    NghienChai.trangthaiYeuCau = TrangThaiMotor::FORWARD;
    xuly_yeucau_nghienchai();
}

// Xử lý yêu cầu phân loại từ board Master (Hàm chạy 1 lần)
void xuly_yeucau_phanloai()
{
    SerialDebug.println("Yeu Cau Phanloai");
    if (PhanLoai.trangthaiLoi == TrangThaiLoi::KHONGLOI)
    {
        PhanLoai.beginTime = millis();

        if (PhanLoai.vitriYeucau == PhanLoai.vitriHientai)
        {
            PhanLoai.phanloai1_doneFlag = 1;
            PhanLoai.phanloai2_doneFlag = 1;
            PhanLoai.dangxulyFlag = 0;
            return;
        }

        switch (PhanLoai.vitriYeucau)
        {
        // Nếu yêu cầu về thùng rác chai
        case vitrithungracphanloai::BOTTLE:
        {

            if (PhanLoai.vitriPhanloai1 != vitricocauphanloai::RIGHT)
            {

                MotorPhanLoai1.YeuCau = TrangThaiMotorPhanLoai1::RIGHT;
                PhanLoai.phanloai1_doneFlag = 0;
            }
            else if (PhanLoai.vitriPhanloai1 == vitricocauphanloai::RIGHT)
            {

                PhanLoai.phanloai1_doneFlag = 1;
            }
            PhanLoai.phanloai2_doneFlag = 1;
            PhanLoai.vitriHientai = vitrithungracphanloai::BOTTLE;
            PhanLoai.dangxulyFlag = 1;
            break;
        }

        case vitrithungracphanloai::CAN:
        {
            // Nếu vị trí 1 hiện tại khác bên trái thì cho chạy
            if (PhanLoai.vitriPhanloai1 != vitricocauphanloai::LEFT)
            {
                MotorPhanLoai1.YeuCau = TrangThaiMotorPhanLoai1::LEFT;
                PhanLoai.phanloai1_doneFlag = 0;
            }
            else
                PhanLoai.phanloai1_doneFlag = 1;

            // Nếu vị trí 2 hiện tại khác bên trái thì cho chạy
            if (PhanLoai.vitriPhanloai2 != vitricocauphanloai::LEFT)
            {
                MotorPhanLoai2.YeuCau = TrangThaiMotorPhanLoai2::LEFT;
                PhanLoai.phanloai2_doneFlag = 0;
            }
            else
                PhanLoai.phanloai2_doneFlag = 1;

            PhanLoai.vitriHientai = vitrithungracphanloai::CAN;
            PhanLoai.dangxulyFlag = 1;
            break;
        }

        case vitrithungracphanloai::OTHER:
        {
            if (PhanLoai.vitriPhanloai1 != vitricocauphanloai::LEFT)
            {
                MotorPhanLoai1.YeuCau = TrangThaiMotorPhanLoai1::LEFT;
                PhanLoai.phanloai1_doneFlag = 0;
            }
            else
                PhanLoai.phanloai1_doneFlag = 1;

            if (PhanLoai.vitriPhanloai2 != vitricocauphanloai::RIGHT)
            {
                MotorPhanLoai2.YeuCau = TrangThaiMotorPhanLoai2::RIGHT;
                PhanLoai.phanloai2_doneFlag = 0;
            }
            else
                PhanLoai.phanloai2_doneFlag = 1;

            PhanLoai.vitriHientai = vitrithungracphanloai::OTHER;
            PhanLoai.dangxulyFlag = 1;
            break;
        }

        default:
            break;
        }
    }
}

// Xử lý yêu cầu nghiền chai (Hàm chạy  1 lần)
void xuly_yeucau_nghienchai()
{

    if (NghienChai.trangthaiLoi == TrangThaiLoi::KHONGLOI)
    {
        NghienChai.beginTime = millis();
        NghienChai.lastCBstate = CB_Encoder.state;
        // MotorNghienChai.YeuCau = NghienChai.trangthaiYeuCau;
        NghienChai.dangRunFlag = 1;

        if (NghienChai.trangthaiYeuCau == TrangThaiMotorNghien::FORWARD ||
            NghienChai.trangthaiYeuCau == TrangThaiMotorNghien::REVERSE)
        {
            SerialDebug.print("Yeu Cau Nghien Chai :");
            SerialDebug.println(NghienChai.trangthaiYeuCau);

            NghienChai.yeucauStopFlag = 0;
            NghienChai.lastChangeTime = millis();
            MotorNghienChai.YeuCau = NghienChai.trangthaiYeuCau;
        }
        else if (NghienChai.trangthaiYeuCau == TrangThaiMotorNghien::STOP)
        {
            if (MotorNghienChai.TrangThai == STOP)
            {
                NghienChai.yeucauStopFlag = 0;
                NghienChai.dangRunFlag = 0;
                MotorNghienChai.TrangThai = STOP;
                MotorNghienChai.YeuCau = STOP;
                MotorNghienChai.Stop();
                return;
            }
            if (NghienChai.yeucauStopFlag == 1)
            {
                return;
            }
            SerialDebug.println("Yeu Cau Stop Nghien Chai");
            NghienChai.yeucauStopFlag = 1;
            NghienChai.lastChangeTime = millis();
            NghienChai.beginStopTime = millis();
        }
    }
}

// Xử lý yêu cầu ép (Hàm chạy  1 lần)
void xuly_yeucau_xoaloi()
{
    // ERASE_ERROR_EEPROM();
    ui2_trangthaiRac1 = 0;
    ui2_trangthaiRac2 = 0;
    PhanLoai.trangthaiLoi = TrangThaiLoi::KHONGLOI;
    PhanLoai.vitriHientai = vitrithungracphanloai::NOWHERE;
    PhanLoai.vitriYeucau = vitrithungracphanloai::NOWHERE;
    PhanLoai.vitriPhanloai1 = vitricocauphanloai::NOWHERE;
    PhanLoai.vitriPhanloai2 = vitricocauphanloai::NOWHERE;
    PhanLoai.dangxulyFlag = 0;
    PhanLoai.phanloai1_doneFlag = 1;
    PhanLoai.phanloai2_doneFlag = 1;

    NghienChai.trangthaiLoi = TrangThaiLoi::KHONGLOI;
    NghienChai.trangthaiYeuCau = TrangThaiMotorNghien::STOP;
    NghienChai.yeucauStopFlag = 0;
    NghienChai.dangRunFlag = 0;

    SerialDebug.println("Da xoa loi");
}

// Kiểm tra hoàn thành phân loại (Hàm để trong loop)
void kiemtra_hoatdong_phanloai()
{
    if (ui8_admin_control_motor)
    {
        SerialDebug.print("admin control motor");
        return;
    }

    if (PhanLoai.trangthaiLoi == 1 &&
        MotorPhanLoai1.TrangThai == STOP && MotorPhanLoai1.YeuCau == STOP &&
        MotorPhanLoai2.TrangThai == STOP && MotorPhanLoai2.YeuCau == STOP)
    {
        // SerialDebug.print("Loi phan loai");
        return;
    }

    if (EpLon.trangthaiLoi == 1 &&
        (MotorPhanLoai1.TrangThai != STOP || MotorPhanLoai1.YeuCau != STOP ||
         MotorPhanLoai2.TrangThai != STOP || MotorPhanLoai2.YeuCau != STOP))
    {
        MotorPhanLoai1.TrangThai = STOP;
        MotorPhanLoai1.YeuCau = STOP;
        MotorPhanLoai1.Stop();
        MotorPhanLoai2.TrangThai = STOP;
        MotorPhanLoai2.YeuCau = STOP;
        MotorPhanLoai2.Stop();
        SerialDebug.print("Loi phan loai");
        return;
    }

    if (PhanLoai.phanloai1_doneFlag == 0)
    {
        // SerialDebug.print("Phan loai 1 chua done");
        if (millis() - PhanLoai.beginTime > PhanLoai.timeError_phanloai1)
        {
            SerialDebug.print("Phan loai 1 loi");
            PhanLoai.phanloai1_doneFlag = 1;
            PhanLoai.phanloai2_doneFlag = 1;

            MotorPhanLoai1.YeuCau = TrangThaiMotorPhanLoai1::STOP;
            MotorPhanLoai2.YeuCau = TrangThaiMotorPhanLoai2::STOP;
            PhanLoai.trangthaiLoi = TrangThaiLoi::COLOI;
            SET_ERROR_EEPROM();
        }
        if (MotorPhanLoai1.YeuCau == TrangThaiMotorPhanLoai1::LEFT &&
            CB_PhanLoai1_Trai.state == 1)
        {
            PhanLoai.phanloai1_doneFlag = 1;
            MotorPhanLoai1.YeuCau = TrangThaiMotorPhanLoai1::STOP;
        }
        if (MotorPhanLoai1.YeuCau == TrangThaiMotorPhanLoai1::RIGHT &&
            CB_PhanLoai1_Phai.state == 1)
        {
            PhanLoai.phanloai1_doneFlag = 1;
            MotorPhanLoai1.YeuCau = TrangThaiMotorPhanLoai1::STOP;
        }
    }

    if (PhanLoai.phanloai2_doneFlag == 0)
    {
        // SerialDebug.print("Phan loai 2 chua done");
        if (millis() - PhanLoai.beginTime > PhanLoai.timeStop_phanloai2)
        {
            SerialDebug.print("Stop phanloai 2");
            MotorPhanLoai2.YeuCau = TrangThaiMotorPhanLoai2::STOP;
            PhanLoai.phanloai2_doneFlag = 1;
        }
    }
    if (PhanLoai.dangxulyFlag == 1 &&
        PhanLoai.phanloai1_doneFlag == 1 &&
        PhanLoai.phanloai2_doneFlag == 1)
    {
        PhanLoai.dangxulyFlag = 0;
    }
}

// Kiểm tra hoàn thành nghiền chai (Hàm để trong loop)
void kiemtra_hoatdong_nghienchai()
{
    if (ui8_admin_control_motor == 1)
        return;

    if (NghienChai.trangthaiLoi == 1 && MotorNghienChai.TrangThai == STOP && MotorNghienChai.YeuCau == STOP)
    {
        SerialDebug.print("Loi nghien chai");
        return;
    }

    if (NghienChai.trangthaiLoi == 1 && (MotorNghienChai.TrangThai != STOP || MotorNghienChai.YeuCau != STOP))
    {
        MotorNghienChai.TrangThai = STOP;
        MotorNghienChai.YeuCau = STOP;
        MotorNghienChai.Stop();
        return;
    }

    if (NghienChai.dangRunFlag == 1)
    {

        // Kiểm tra lỗi nghiền chai
        if (NghienChai.lastCBstate != CB_Encoder.state)
        {
            NghienChai.lastCBstate = CB_Encoder.state;
            NghienChai.lastChangeTime = millis();
        }
        if (millis() - NghienChai.lastChangeTime > NghienChai.timeError)
        {
            NghienChai.dangRunFlag = 0;
            MotorNghienChai.YeuCau = TrangThaiMotorNghien::STOP;
            NghienChai.trangthaiLoi = TrangThaiLoi::COLOI;
            SET_ERROR_EEPROM();
            return;
        }

        // Kiểm tra dừng nghiền chai
        if (NghienChai.yeucauStopFlag == 1 &&
            millis() - NghienChai.beginStopTime > NghienChai.timeWaitStop)
        {
            NghienChai.dangRunFlag = 0;
            MotorNghienChai.YeuCau = TrangThaiMotorNghien::STOP;
            NghienChai.yeucauStopFlag = 0;
            return;
        }

        if (NghienChai.yeucauStopFlag == 1 &&
            (MotorNghienChai.TrangThai == TrangThaiMotorNghien::STOP ||
             MotorNghienChai.YeuCau == TrangThaiMotorNghien::STOP))
        {
            NghienChai.dangRunFlag = 0;
            MotorNghienChai.YeuCau = TrangThaiMotorNghien::STOP;
            NghienChai.trangthaiYeuCau = TrangThaiMotorNghien::STOP;
            NghienChai.yeucauStopFlag = 0;
            return;
        }

        // Nếu quá thời gian mà không thấy tín hiệu phân loại thì tắt để tránh trường hợp không gửi lệnh stop xuống
        if (millis() - PhanLoai.beginTime > NghienChai.timeAutoStop &&
            millis() - NghienChai.beginTime > NghienChai.timeAutoStop)
        {
            NghienChai.dangRunFlag = 0;
            NghienChai.yeucauStopFlag = 0;
            MotorNghienChai.YeuCau = TrangThaiMotorNghien::STOP;
            SerialDebug.print("\nAuto stop nghien chai\n");
        }
    }
}

// Kiểm tra thùng rác 3 (Hàm để trong loop)
void kiemtra_thungrac()
{
    if (ui2_trangthaiRac1 == false &&
        CB_Rac1.state == 1)
    {
        ui2_trangthaiRac1 = true;
    }
    if (ui2_trangthaiRac2 == false &&
        CB_Rac2.state == 1)
    {
        ui2_trangthaiRac2 = true;
    }
}

void kiemtra_tatdongco()
{
    if (PhanLoai.trangthaiLoi == TrangThaiLoi::KHONGLOI ||
        EpLon.trangthaiLoi == TrangThaiLoi::KHONGLOI ||
        NghienChai.trangthaiLoi == TrangThaiLoi::KHONGLOI)
    {
        ui2_tatDongCo_doLoi_flag = 0;
    }

    if ((PhanLoai.trangthaiLoi == TrangThaiLoi::COLOI ||
         NghienChai.trangthaiLoi == TrangThaiLoi::COLOI ||
         EpLon.trangthaiLoi == TrangThaiLoi::COLOI) &&
        ui2_tatDongCo_doLoi_flag == 0)
    {
        ui2_tatDongCo_doLoi_flag = 1;
        MotorPhanLoai1.YeuCau = TrangThaiMotorPhanLoai1::STOP;
        MotorPhanLoai2.YeuCau = TrangThaiMotorPhanLoai2::STOP;
        MotorNghienChai.YeuCau = TrangThaiMotorNghien::STOP;
        NghienChai.dangRunFlag = 0;
        PhanLoai.dangxulyFlag = 0;
        EpLon.dangxulyFlag = 0;
    }
}

void phanhoi_trangthailoi()
{

    uint8_t trangthai_Phanloai = ((PhanLoai.dangxulyFlag == 1) ? 0x02 : PhanLoai.trangthaiLoi);
    uint8_t trangthai_Nghien = (NghienChai.trangthaiLoi);

    uint8_t bytecount = 4;
    uint8_t datasize = bytecount + 4;
    raiseOrderCommand(self_order_command);

    uint8_t data[datasize + 1] = {datasize,
                                  codeMaster::check_error,
                                  self_order_command[0],
                                  self_order_command[1],
                                  bytecount,
                                  trangthai_Phanloai,
                                  trangthai_Nghien,
                                  ui2_trangthaiRac1,
                                  ui2_trangthaiRac2};

    memcpy(buffer_send, data, datasize + 1);
    RS485.send(buffer_send);
#ifdef ShowSerial

    SerialDebug.printf("\nguiphanhoichoMaster: %D %D %D %D\n",
                       trangthai_Phanloai, trangthai_Nghien, ui2_trangthaiRac1, ui2_trangthaiRac2);
    // SerialDebug.printf("cam bien :%d %d %d %d %d \n", CB_PhanLoai1_Trai.state, CB_PhanLoai1_Trai.state, CB_PhanLoai1_Trai.state, CB_Encoder.state, CB_Rac1.state, CB_Rac2.state);
#endif
}

void phanhoi_firmware()
{
    uint8_t bytecount = 4;
    uint8_t datasize = bytecount + 4;
    raiseOrderCommand(self_order_command);

    uint8_t data[datasize + 1] = {datasize,
                                  codeMaster::check_firmware,
                                  self_order_command[0],
                                  self_order_command[1],
                                  bytecount,
                                  firmware[0],
                                  firmware[1],
                                  firmware[2],
                                  firmware[3]};

    memcpy(buffer_send, data, datasize + 1);
    RS485.send(buffer_send);
}

void raiseOrderCommand(uint8_t *order)
{
    (*(order + 1))++;
    *(order + 0) = (*(order + 1) == 0x00) ? ((*(order + 0)) + 1) : (*(order + 0));
}

String inputString = "";
bool stringComplete = false;
void docserial()
{
    while (SerialDebug.available())
    {
        // get the new byte:
        char inChar = (char)SerialDebug.read();
        if (inChar == '\n')
        {
            stringComplete = true;
        }
        else
        {
            inputString += inChar;
        }
    }
}

void DebugSerial()
{
    docserial();
    if (stringComplete)
    {

        serialDebugProcess();

        inputString = "";
        stringComplete = false;
    }
}

void serialDebugProcess()
{
    SerialDebug.print("\n test bang debug :");
    SerialDebug.println(inputString);

    if (inputString == "phanloai_chai")
    {
        SerialDebug.println(" Vitri Rac Chai");
        PhanLoai.vitriYeucau = vitrithungracphanloai::BOTTLE;
        xuly_yeucau_phanloai();
    }

    else if (inputString == "phanloai_lon")
    {

        SerialDebug.println(" Vitri Rac Lon");
        PhanLoai.vitriYeucau = vitrithungracphanloai::CAN;
        xuly_yeucau_phanloai();
    }

    else if (inputString == "phanloai_khac")
    {

        SerialDebug.println(" Vitri Rac Khac");
        PhanLoai.vitriYeucau = vitrithungracphanloai::OTHER;
        xuly_yeucau_phanloai();
    }

    else if (inputString == "nghienchai_fw")
    {
        SerialDebug.println("Nghien chai fw");

        NghienChai.trangthaiYeuCau = TrangThaiMotorNghien::FORWARD;
        xuly_yeucau_nghienchai();
    }

    else if (inputString == "nghienchai_rv")
    {
        SerialDebug.println("Nghien chai rv");

        NghienChai.trangthaiYeuCau = TrangThaiMotorNghien::REVERSE;
        xuly_yeucau_nghienchai();
    }

    else if (inputString == "nghienchai_stop")
    {
        SerialDebug.println("Nghien chai stop");

        NghienChai.trangthaiYeuCau = TrangThaiMotorNghien::STOP;
        xuly_yeucau_nghienchai();
    }

    else if (inputString == "xoaloi")
    {
        xuly_yeucau_xoaloi();
        SerialDebug.println("Xoaloi");
    }

    else if (inputString == "reset")
    {
        SerialDebug.println("Reset");
        ESP.restart();
    }
    else if (inputString == "pl1_trai")
    {
        ui8_admin_control_motor = true;
        SerialDebug.println("pl1 xoay trai");
        MotorPhanLoai1.YeuCau = TrangThaiMotorPhanLoai1::LEFT;
    }

    else if (inputString == "pl1_phai")
    {
        ui8_admin_control_motor = true;
        SerialDebug.println("pl1 xoay phai");
        MotorPhanLoai1.YeuCau = TrangThaiMotorPhanLoai1::RIGHT;
    }

    else if (inputString == "pl1_stop")
    {
        ui8_admin_control_motor = true;
        SerialDebug.println("pl1 stop");
        MotorPhanLoai1.YeuCau = TrangThaiMotorPhanLoai1::STOP;
    }
    else if (inputString == "pl2_trai")
    {
        ui8_admin_control_motor = true;
        SerialDebug.println("pl2 xoay trai");
        MotorPhanLoai2.YeuCau = TrangThaiMotorPhanLoai2::LEFT;
    }
    else if (inputString == "pl2_phai")
    {
        ui8_admin_control_motor = true;
        SerialDebug.println("pl2 xoay phai");
        MotorPhanLoai2.YeuCau = TrangThaiMotorPhanLoai2::RIGHT;
    }
    else if (inputString == "pl2_stop")
    {
        ui8_admin_control_motor = true;
        SerialDebug.println("pl2 stop");
        MotorPhanLoai2.YeuCau = TrangThaiMotorPhanLoai2::STOP;
    }

    else if (inputString == "admin_nghien_fw")
    {
        ui8_admin_control_motor = true;
        SerialDebug.println("admin_nghien_fw ");
        MotorNghienChai.YeuCau = TrangThaiMotorNghien::FORWARD;
    }

    else if (inputString == "admin_nghien_stop")
    {
        ui8_admin_control_motor = true;
        SerialDebug.println("admin_nghien_stop ");
        MotorNghienChai.YeuCau = TrangThaiMotorNghien::STOP;
    }

    else if (inputString == "end_admin")
    {
        ui8_admin_control_motor = false;
        SerialDebug.println("end admin control");
    }
}

// Nhận firmware qua UART và cập nhật
void receiveFirmware()
{

    uint8_t buffer[BUFFER_SIZE + 1024]; // DATA_SIZE + HEADER_SIZE
    size_t totalReceived = 0;
    bool updateStarted = false;
    uint32_t expectedSize = 0;
    bool firmwareInvalid = false;
    bool firmwareWrongSize = false;
    bool timeoutReceive = false;

    bool receivedEOP = false;
    bool receivedEOF = false;
    uint8_t counter = 0;

    /* Nhận thông tin tổng số byte */
    Serial1.readBytes((uint8_t *)&expectedSize, sizeof(expectedSize));
    Serial.printf("Expected firmware size: %u bytes\n", expectedSize);

    // Bắt đầu cập nhật
    if (!updateStarted)
    {
        if (expectedSize == 0)
        {
            Serial.println(" No firmware to update.");
            ui8_update_firmware = false;
            return;
        }
        if (!Update.begin(expectedSize))
        {
            Serial.println("Not enough space for firmware update.");
            ui8_update_firmware = false;
            return;
        }
        updateStarted = true;
        Serial.println("Firmware update started...");
    }
    uint32_t timer0 = millis();
    uint32_t timer1 = millis();

    while (true)
    {
        /* Kiểm tra thời gian không có dữ liệu để break while */
        if (millis() - timer1 > 5000)
        {
            timeoutReceive = true;
            break;
        }
        /* Nếu có dữ liệu */
        if (Serial1.available())
        {
            timer1 = millis();
            int len = Serial1.readBytes(buffer, BUFFER_SIZE);

            /* Kiểm tra checksum */
            uint8_t checksum = calculateChecksum(buffer + 3, uint16_t(len - HEADER_SIZE));

            Serial.printf("Checksum: %02X\n", checksum);
            if (buffer[0] == 'B' && buffer[1] == 'O' && buffer[2] == 'P')
            {
                Serial.println("Received Begin Page Maker ");
            }
            if (buffer[len - 3] == 'E' && buffer[len - 2] == 'O' && buffer[len - 1] == 'P')
            {
                Serial.println("Received End Page Maker ");
            }

            /*Kiểm tra Begin và End Maker*/
            if (buffer[0] == 'B' && buffer[1] == 'O' && buffer[2] == 'P' &&
                buffer[len - 3] == 'E' && buffer[len - 2] == 'O' && buffer[len - 1] == 'P')
            {
                receivedEOP = true;
            }
            else if (buffer[0] == 'B' && buffer[1] == 'O' && buffer[2] == 'P' &&
                     buffer[len - 3] == 'E' && buffer[len - 2] == 'O' && buffer[len - 1] == 'F')
            {
                receivedEOF = true;
            }

            /*Nếu nhận được dữ liệu hợp lệ*/
            if ((receivedEOP || receivedEOF) &&
                checksum == buffer[len - 4])
            {
                receivedEOP = false; /* Reset EOP để nhận trong while tiếp theo , EOF thì không không reset để ngắt while*/
                counter = 0;
                /*Ghi dữ liệu vào Update */
                size_t written = Update.write(buffer + 3, len - HEADER_SIZE);

                /*Kiểm tra ghi dữ liệu thành công*/
                if (written == len - HEADER_SIZE) /*Nếu thành công thì gửi phản hồi về*/
                {
                    // digitalWrite(RS485_DE_PIN, HIGH);
                    uint8_t code[3] = {'R', 'O', 'K'};
                    Serial1.write(code, sizeof(code));
                    Serial1.flush();
                    // digitalWrite(RS485_DE_PIN, LOW);
                    Serial.println("Sent respond OK");
                }
                else /* Nếu không thành công thì không phản hồi*/
                {
                    Serial.println("Error writing firmware data!");
                    break;
                    firmwareInvalid = true;
                }
            }
            else if (counter <= 3) /* Nếu dữ liệu không hợp lệ*/
            {
                Serial.printf("Received error %d bytes \n", len - HEADER_SIZE);
                counter++;
                // digitalWrite(RS485_DE_PIN, HIGH);
                uint8_t code[3] = {'E', 'R', 'R'};
                Serial1.write(code, sizeof(code));
                Serial1.flush();
                // digitalWrite(RS485_DE_PIN, LOW);
                Serial.println("Request resend -------------------------------------------------------------------------------------------------------------");
                continue;
            }
            else if (counter > 3)
            {
                firmwareInvalid = true;
                break;
            }

            totalReceived += len - HEADER_SIZE;
            Serial.printf("Received %d bytes|| Total received: %d \n", len - HEADER_SIZE, totalReceived);

            /*Kiểm tra khi nhận tất cả dữ liệu thì break while*/
            if (receivedEOF)
            {
                /* Nếu không nhận được đúng size như ban đầu gửi thì báo lỗi*/
                if (totalReceived != expectedSize)
                    firmwareWrongSize = true;

                Serial.println("Received End File Maker ");
                break;
            }

            Serial.printf("Time: %d ms\n", millis() - timer1);
        }
    }

    Serial.printf("Total time: %d ms\n", millis() - timer0);

    /*Kiểm tra các lỗi để không tiến hành cập nhật*/
    if (timeoutReceive || firmwareInvalid || firmwareWrongSize)
    {
        Serial.println("Firmware update failed.");
        Serial.printf("timeoutReceive: %d\n", timeoutReceive);
        Serial.printf("firmwareInvalid: %d\n", firmwareInvalid);
        Serial.printf("firmwareWrongSize: %d\n", firmwareWrongSize);
        Update.abort();
    }
    /*Tiến hành cập nhật và khởi động lại*/
    else if (updateStarted)
    {
        Serial.println("Firmware transfer completed.");
        Update.end();
        Serial.println("Firmware update completed! Restarting...");
        ESP.restart();
        Serial.println("Error");
    }

    ui8_update_firmware = false;
}

uint8_t calculateChecksum(const uint8_t *data, uint16_t length)
{
    if (data == NULL || length == 0) // Kiểm tra con trỏ NULL hoặc độ dài bằng 0
    {
        return 0; // Trả về 0 để tránh lỗi
    }

    uint8_t checksum = 0;
    for (int i = 0; i < length; i++)
    {
        checksum += data[i]; // Tổng tất cả các byte
    }
    return checksum; // Trả về checksum (mod 256 tự động do kiểu uint8_t)
}

void loop()
{
}

void blink(int time)
{
    static uint32_t ui32_time_blink = 0;
    static bool led_status = 0;
    if (millis() - ui32_time_blink > time)
    {
        ui32_time_blink = millis();
        digitalWrite(LED_ONBOARD, led_status = !led_status);
    }
}