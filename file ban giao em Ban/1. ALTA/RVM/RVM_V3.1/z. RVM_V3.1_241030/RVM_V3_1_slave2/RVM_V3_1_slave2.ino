
#include "RVM_V3_slave.h"

uint8_t selfSlave2ID = 0x02; // Đặt Slave ID
RS485_Slave RS485(Serial1, selfSlave2ID, RS485_TX_PIN, RS485_RX_PIN);

static TimerHandle_t auto_reload_timer = NULL;

void setup()
{
    SerialDebug.begin(115200);

    RS485.begin(115200);

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

    SerialDebug.print("\nRVM_Slave [");
    for (uint8_t i = 0; i < 3; i++)
    {
        SerialDebug.print(firmware[i], HEX);
        if (i == 2)
            SerialDebug.println("]");
        else
            SerialDebug.print(".");
    }

    // GET_EEPROM();
    // SET_ERROR_EEPROM();

    vTaskDelay(200 / portTICK_PERIOD_MS);

    // xTaskCreatePinnedToCore( // Use xTaskCreate() in vanilla FreeRTOS
    //     taskControlAll,      // Function to be called
    //     "taskControlAll",    // Name of task
    //     10240,               // Stack size (bytes in ESP32, words in FreeRTOS)
    //     NULL,                // Parameter to pass
    //     2,                   // Task priority (must be same to prevent lockup)
    //     NULL,                // Task handle
    //     1);                  // Run on one core for demo purposes (ESP32 only)

    xTaskCreatePinnedToCore(taskCommunicateToR485Master, "taskCommunicateToR485Master", 10240, NULL, 1, NULL, 1);
    // xTaskCreatePinnedToCore(taskControlMotor, "taskControlMotor", 10240, NULL, 1, NULL, 1);

    // xTaskCreatePinnedToCore(taskDebug, "taskDebug", 2048, NULL, 1, NULL, 1);

    // xTaskCreatePinnedToCore(taskTest, "taskTest", 10240, NULL, 1, NULL, 1);

    // Create an auto-reload timer
    auto_reload_timer = xTimerCreate(
        "Auto-reload timer",    // Name of timer
        1 / portTICK_PERIOD_MS, // Period of timer (in ticks)
        pdTRUE,                 // Auto-reload
        (void *)1,              // Timer ID
        timerReadSensor);       // Callback function

    xTimerStart(auto_reload_timer, portMAX_DELAY);
    // Delete "setup and loop" task
    vTaskDelete(NULL);
}

void timerReadSensor(void *pvParameters)
{
}

void taskCommunicateToR485Master(void *pvParameters)
{
    while (1)
    {
        if (ui8_phanhoi_TrangthaiLoi == 1 || ui8_phanhoi_firmware == 1)
            ui8_trangthaiRS485 = trangthaiRS485::DANGGUI;
        else
            ui8_trangthaiRS485 = trangthaiRS485::DANGNHAN;

        // Xử lý khi ở trạng thái nhận dữ liệu đến master
        if (ui8_trangthaiRS485 == trangthaiRS485::DANGNHAN)
        {
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

#if 0
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

#if 0
        for (CoCauMotorServo *Motor : MotorServoArray)
        {
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
        kiemtra_hoatdong_eplon();
        // kiemtra_tatdongco();
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

            if (buffer_receive[0] - 4 != 1 || // Check size
                (buffer_receive[5] != vitrithungracphanloai::CAN &&
                 buffer_receive[5] != vitrithungracphanloai::OTHER &&
                 buffer_receive[5] != vitrithungracphanloai::BOTTLE))
            {
                SerialDebug.println("Incorrect command phan loai");
                break;
            }
            else
            {
                if (buffer_receive[5] == vitrithungracphanloai::BOTTLE)
                {
                    SerialDebug.println("Nhan dien BOTTLE");
                }
                else if (buffer_receive[5] == vitrithungracphanloai::CAN)
                {
                    SerialDebug.println("Nhan dien CAN");
                }
                else if (buffer_receive[5] == vitrithungracphanloai::OTHER)
                {
                    SerialDebug.println("Nhan dien OTHER");
                }
                PhanLoai.vitriYeucau = buffer_receive[5];
                // SerialDebug.println("correct command phan loai");
                xuly_yeucau_phanloai();
                break;
            }
        }

        case codeMaster::EPLON:
        {
            if (buffer_receive[0] - 4 != 1 || // Check size
                (buffer_receive[5] != TrangThaiMotor::STOP &&
                 buffer_receive[5] != TrangThaiMotor::FORWARD &&
                 buffer_receive[5] != TrangThaiMotor::REVERSE))
            {

                SerialDebug.println(" Incorrect command ep lon");
                break;
            }
            else
            {
                EpLon.trangthaiYeuCau = buffer_receive[5];
                xuly_yeucau_eplon();

                break;
            }
        }

        case codeMaster::NGHIENCHAI:
        {

            if (buffer_receive[0] - 4 != 1 || // Check size
                (buffer_receive[5] != TrangThaiMotor::STOP &&
                 buffer_receive[5] != TrangThaiMotor::FORWARD &&
                 buffer_receive[5] != TrangThaiMotor::REVERSE))
            {

                SerialDebug.println(" Incorrect command nghien chai");
                break;
            }
            else
            {
                NghienChai.trangthaiYeuCau = buffer_receive[5];
                xuly_yeucau_nghienchai();
                break;
            }
        }

        case codeMaster::check_error:
        {
            if (buffer_receive[0] - 4 != 1 || // Check size
                buffer_receive[5] != 0x00)
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

            if (buffer_receive[0] - 4 != 1 || // Check size
                buffer_receive[5] != 0x00)
            {
                SerialDebug.println(" Incorrect command erase error");
                break;
            }
            else
            {
                // ERASE_ERROR_EEPROM();
                PhanLoai.trangthaiLoi = TrangThaiLoi::KHONGLOI;
                NghienChai.trangthaiLoi = TrangThaiLoi::KHONGLOI;
                EpLon.trangthaiLoi = TrangThaiLoi::KHONGLOI;
                SerialDebug.println("Da xoa loi");
                break;
            }
        }

        case codeMaster::admin_control_motor:
        {
            if (buffer_receive[0] - 4 == 2 && // Check size
                (buffer_receive[5] == 0xFF &&
                 buffer_receive[6] == 0xFF))
            {
                SerialDebug.println("Exit admin control motor");
                ui8_admin_control_motor = false;
                break;
            }
            else if (buffer_receive[0] - 4 != 2 || // Check size
                     (buffer_receive[6] != TrangThaiMotor::STOP &&
                      buffer_receive[6] != TrangThaiMotor::FORWARD &&
                      buffer_receive[6] != TrangThaiMotor::REVERSE))
            {
                SerialDebug.println(" Incorrect command admin control motor");
                break;
            }
            else
            {
                ui8_admin_control_motor = true;
                SerialDebug.println("Correct command admin control motor");
                switch (buffer_receive[5])
                {
                case admin_control_motor::PHANLOAI1:
                {
                    // SerialDebug.println(" correct command admin control phanloai 1");
                    MotorPhanLoai1.YeuCau = buffer_receive[6];
                    break;
                }
                case admin_control_motor::PHANLOAI2:
                {
                    // SerialDebug.println(" correct command admin control phanloai 2");
                    MotorPhanLoai2.YeuCau = buffer_receive[6];
                    break;
                }
                case admin_control_motor::EPLON:
                {
                    // SerialDebug.print(" correct command eplon ");
                    SerialDebug.println(buffer_receive[6]);
                    MotorEpLon.YeuCau = buffer_receive[6];
                    break;
                }
                case admin_control_motor::NGHIENCHAI:
                {
                    // SerialDebug.println(" correct command nghien chai");
                    MotorNghienChai.YeuCau = buffer_receive[6];
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
            if (buffer_receive[0] - 4 != 1 || // Check size
                buffer_receive[5] != 0x00)
            {
                SerialDebug.println("Incorrect command firmware");
                break;
            }
            else
            {
                ui8_phanhoi_firmware = true;
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
    EpLon.trangthaiYeuCau = TrangThaiMotor::FORWARD;
    xuly_yeucau_eplon();
    NghienChai.trangthaiYeuCau = TrangThaiMotor::FORWARD;
    xuly_yeucau_nghienchai();
}

// Xử lý yêu cầu phân loại từ board Master (Hàm chạy 1 lần)
void xuly_yeucau_phanloai()
{
    SerialDebug.println("Yeu Cau Phanloai");
    if (PhanLoai.trangthaiLoi == TrangThaiLoi::KHONGLOI)
    {

        if (PhanLoai.vitriYeucau == PhanLoai.vitriHientai)
        {
            PhanLoai.phanloai1_doneFlag = 1;
            PhanLoai.phanloai2_doneFlag = 1;
            PhanLoai.dangxulyFlag = 0;
            return;
        }

        PhanLoai.beginTime = millis();
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

// Xử lý yêu cầu ép (Hàm chạy  1 lần)
void xuly_yeucau_eplon()
{
    SerialDebug.println("Yeu Cau ep lon");
    if (EpLon.trangthaiLoi == TrangThaiLoi::KHONGLOI)
    {
        EpLon.beginTime = millis();
        EpLon.lastCBstate = CB_DauEp.state;
        EpLon.chovatFlag = 1;
        EpLon.dangxulyFlag = 1;

        // MotorEpLon.YeuCau = EpLon.trangthaiYeuCau; //Phải đợi có vật mới bắt đầu chạy động cơ ép
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

        if (NghienChai.trangthaiYeuCau == TrangThaiMotorNghien::FORWARD)
        {
            SerialDebug.println("Yeu Cau Nghien Chai");
            NghienChai.yeucauStopFlag = 0;
            NghienChai.lastChangeTime = millis();
            MotorNghienChai.YeuCau = TrangThaiMotorNghien::FORWARD;
        }
        else if (NghienChai.trangthaiYeuCau == TrangThaiMotorNghien::STOP)
        {
            SerialDebug.println("Yeu Cau Stop Nghien Chai");
            NghienChai.yeucauStopFlag = 1;
            NghienChai.beginStopTime = millis();
        }
    }
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

// Kiểm tra hoàn thành ép lon (Hàm để trong loop)
void kiemtra_hoatdong_eplon()
{
    if (ui8_admin_control_motor)
        return;

    if (EpLon.trangthaiLoi == 1 && MotorEpLon.TrangThai == STOP && MotorEpLon.YeuCau == STOP)
        return;

    if (EpLon.trangthaiLoi == 1 && (MotorEpLon.TrangThai != STOP || MotorEpLon.YeuCau != STOP))
    {
        EpLon.count = 0;
        MotorEpLon.TrangThai = STOP;
        MotorEpLon.YeuCau = STOP;
        MotorEpLon.Stop();
        return;
    }

    if (EpLon.dangxulyFlag == 1)
    {
        if (EpLon.chovatFlag == 1) // Nếu có cờ chờ vật thì phải có vật mới bắt đầu chạy
        {
            if (millis() - EpLon.beginTime > EpLon.timeWait) // Nếu quá thời gian chờ vật thì báo bình thường hoặc báo lỗi
            {
                EpLon.dangxulyFlag = 0;
            }

            if (CB_LongEp1.state == 1 || CB_LongEp2.state == 1) // đã có vật trong lồng thì không chờ nữa
            {
                EpLon.lastChangeTime = millis();
                MotorEpLon.YeuCau = EpLon.trangthaiYeuCau;
                EpLon.chovatFlag = 0;
            }
        }
        else if (EpLon.chovatFlag == 0) // Nếu không có cờ chờ vật thì cứ chạy lồng ép bình thường
        {
            if (EpLon.count > 10) // Nếu ép 10 lần mà CB vẫn có vật
            {
                EpLon.trangthaiLoi = TrangThaiLoi::COLOI;
                MotorEpLon.YeuCau = TrangThaiMotorEp::STOP;
                EpLon.dangxulyFlag = 0;
                SET_ERROR_EEPROM();
                EpLon.count = 0;
                return;
            }

            if (EpLon.lastCBstate == 1)
            {
                if (millis() - EpLon.lastChangeTime > EpLon.timeErrorGoAway) // Nếu quá thời gian chạy ra mà cảm biến vẫn bắt được đầu ép
                {
                    MotorEpLon.YeuCau = TrangThaiMotorEp::STOP;
                    EpLon.count = 0;
                    EpLon.trangthaiLoi = TrangThaiLoi::COLOI;
                    EpLon.dangxulyFlag = 0;
                    SET_ERROR_EEPROM();
                    return;
                }
                if (CB_DauEp.state == 0)
                {
                    EpLon.lastCBstate = 0;
                    EpLon.lastChangeTime = millis();
                }
            }
            if (EpLon.lastCBstate == 0)
            {

                if (CB_DauEp.state == 1 && (CB_LongEp1.state == 1 || CB_LongEp2.state == 1))
                {
                    EpLon.count++;
                    SerialDebug.print(EpLon.count);
                    EpLon.lastCBstate = 1;
                    EpLon.lastChangeTime = millis();
                }

                // Hoàn thành chu trình ép
                if (CB_DauEp.state == 1 && CB_LongEp1.state == 0 && CB_LongEp2.state == 0)
                {
                    MotorEpLon.YeuCau = TrangThaiMotorEp::STOP;
                    EpLon.count = 0;
                    EpLon.dangxulyFlag = 0;
                    return;
                }

                if (millis() - EpLon.lastChangeTime > EpLon.timeErrorGoBack)
                {
                    MotorEpLon.YeuCau = TrangThaiMotorEp::STOP;
                    EpLon.trangthaiLoi = TrangThaiLoi::COLOI;
                    EpLon.dangxulyFlag = 0;
                    EpLon.count = 0;
                    SET_ERROR_EEPROM();
                    return;
                }
            }
        }
    }
}

// Kiểm tra hoàn thành nghiền chai (Hàm để trong loop)
void kiemtra_hoatdong_nghienchai()
{
    if (ui8_admin_control_motor)
        return;

    if (NghienChai.trangthaiLoi == 1 && MotorNghienChai.TrangThai == STOP && MotorNghienChai.YeuCau == STOP)
        return;

    if (NghienChai.trangthaiLoi == 1 && (MotorNghienChai.TrangThai != STOP || MotorNghienChai.YeuCau != STOP))
    {
        MotorNghienChai.TrangThai = STOP;
        MotorNghienChai.YeuCau = STOP;
        MotorNghienChai.Stop();
        return;
    }

    if (NghienChai.dangRunFlag == 1)
    {
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
            MotorNghienChai.YeuCau = TrangThaiMotorNghien::STOP;
            NghienChai.trangthaiYeuCau = TrangThaiMotorNghien::STOP;
            NghienChai.dangRunFlag = 0;
            NghienChai.yeucauStopFlag = 0;
            return;
        }

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

        // // Nếu quá thời gian mà không thấy tín hiệu phân loại thì tắt để tránh trường hợp không gửi lệnh stop xuống
        // if (millis() - PhanLoai.beginTime > NghienChai.timeAutoStop &&
        //     millis() - NghienChai.beginTime > NghienChai.timeAutoStop)
        // {
        //     NghienChai.dangRunFlag = 0;
        //     MotorNghienChai.YeuCau = TrangThaiMotorNghien::STOP;
        // }
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
        MotorEpLon.YeuCau = TrangThaiMotorEp::STOP;
        NghienChai.dangRunFlag = 0;
        PhanLoai.dangxulyFlag = 0;
        EpLon.dangxulyFlag = 0;
    }
}

void phanhoi_trangthailoi()
{

    uint8_t trangthai_Phanloai = ((PhanLoai.dangxulyFlag == 1) ? 0x02 : PhanLoai.trangthaiLoi);
    uint8_t trangthai_Nghien = (NghienChai.trangthaiLoi);
    uint8_t trangthai_Ep = ((EpLon.dangxulyFlag == 1) ? 0x02 : EpLon.trangthaiLoi);

    uint8_t bytecount = 3;
    uint8_t datasize = bytecount + 4;
    raiseOrderCommand(self_order_command);

    uint8_t data[datasize + 1] = {datasize,
                                  codeMaster::check_error,
                                  self_order_command[0],
                                  self_order_command[1],
                                  bytecount,
                                  trangthai_Phanloai,
                                  trangthai_Nghien,
                                  trangthai_Ep};

    memcpy(buffer_send, data, datasize + 1);
    RS485.send(buffer_send);
#ifdef ShowSerial

    SerialDebug.printf("\nguiphanhoichoMaster: %D %D %D\n", trangthai_Phanloai, trangthai_Nghien, trangthai_Ep);
#endif
}

void phanhoi_firmware()
{
    uint8_t bytecount = 3;
    uint8_t datasize = bytecount + 4;
    raiseOrderCommand(self_order_command);

    uint8_t data[datasize + 1] = {datasize,
                                  codeMaster::check_firmware,
                                  self_order_command[0],
                                  self_order_command[1],
                                  bytecount,
                                  firmware[0],
                                  firmware[1],
                                  firmware[2]};

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

    else if (inputString == "eplon_fw")
    {
        SerialDebug.println("Eplon fw");

        EpLon.trangthaiYeuCau = TrangThaiMotorEp::FORWARD;
        xuly_yeucau_eplon();
    }

    else if (inputString == "nghienchai_fw")
    {
        SerialDebug.println("Nghien chai fw");

        NghienChai.trangthaiYeuCau = TrangThaiMotorNghien::FORWARD;
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
        PhanLoai.trangthaiLoi = TrangThaiLoi::KHONGLOI;
        NghienChai.trangthaiLoi = TrangThaiLoi::KHONGLOI;
        EpLon.trangthaiLoi = TrangThaiLoi::KHONGLOI;
        PhanLoai.vitriHientai = vitrithungracphanloai::NOWHERE;
        PhanLoai.vitriYeucau = vitrithungracphanloai::NOWHERE;
        PhanLoai.vitriPhanloai1 = vitricocauphanloai::NOWHERE;
        PhanLoai.vitriPhanloai2 = vitricocauphanloai::NOWHERE;

        SerialDebug.println("Xoaloi");
        // ERASE_ERROR_EEPROM();
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

    else if (inputString == "end_admin")
    {
        ui8_admin_control_motor = false;
        SerialDebug.println("end admin control");
    }
}

void loop()
{
}
