// Slave 2 điều khiển bộ ép

#include "RVM_V3_slave.h"

uint8_t selfSlave2ID = 0x02; // Đặt Slave ID
RS485_Slave RS485(Serial1, selfSlave2ID, RS485_TX_PIN, RS485_RX_PIN);

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

    CB_DauEp.setup();
    CB_LongEp1.setup();
    CB_LongEp2.setup();
    CB_Rac3.setup();

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
    CB_DauEp.readDebounce(10);
    CB_LongEp1.readDebounce(100);
    CB_LongEp2.readDebounce(100);
    CB_Rac3.readDebounce(2000);
}

void taskCommunicateToR485Master(void *pvParameters)
{
    while (1)
    {
        blink(1000);

        if (ui8_update_firmware == 1)
        {
            receiveFirmware();
        }

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

#if 1
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
        kiemtra_hoatdong_eplon();
        kiemtra_thungrac3();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
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
        vTaskDelay(1000 / portTICK_PERIOD_MS);
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

        case codeMaster::EPLON:
        {
            if (size != 1 || // Check size
                (data[0] != TrangThaiMotor::STOP &&
                 data[0] != TrangThaiMotor::FORWARD &&
                 data[0] != TrangThaiMotor::REVERSE))
            {

                SerialDebug.println(" Incorrect command ep lon");
                break;
            }
            else
            {
                EpLon.trangthaiYeuCau = data[0];
                xuly_yeucau_eplon();

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

                case admin_control_motor::EPLON:
                {
                    // SerialDebug.print(" correct command eplon ");
                    SerialDebug.println(data[1]);
                    MotorEpLon.YeuCau = data[1];
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

        default:
            SerialDebug.println(" Incorrect command all");
            break;
        }
    }
}
#endif

void goHome()
{
    SerialDebug.println("Yeu Cau Go Home");
    EpLon.trangthaiYeuCau = TrangThaiMotor::FORWARD;

    if (EpLon.trangthaiLoi == TrangThaiLoi::KHONGLOI)
    {
        EpLon.beginTime = millis();
        EpLon.lastCBstate = CB_DauEp.state;
        EpLon.dangxulyFlag = 1;
        EpLon.lastChangeTime = millis();
        MotorEpLon.YeuCau = EpLon.trangthaiYeuCau;
        EpLon.chovatFlag = 0;
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

// Xử lý yêu cầu ép (Hàm chạy  1 lần)
void xuly_yeucau_xoaloi()
{
    ERASE_ERROR_EEPROM();
    ui2_trangthaiRac3 = 0;
    PhanLoai.trangthaiLoi = TrangThaiLoi::KHONGLOI;
    NghienChai.trangthaiLoi = TrangThaiLoi::KHONGLOI;
    EpLon.trangthaiLoi = TrangThaiLoi::KHONGLOI;
    SerialDebug.println("Da xoa loi");
    goHome();
}

// Kiểm tra thùng rác 3 (Hàm để trong loop)
void kiemtra_thungrac3()
{
    if (ui2_trangthaiRac3 == 0 &&
        CB_Rac3.state == 1)
    {
        ui2_trangthaiRac3 = 1;
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
#if defined(CoVatMoiChayEpLon)
                EpLon.dangxulyFlag = 0;
#elif defined(GuiLenhLaEpLon)
                EpLon.lastChangeTime = millis();
                MotorEpLon.YeuCau = TrangThaiMotorEp::FORWARD;
                EpLon.chovatFlag = 0;
#endif
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
            if (EpLon.count >= 3) // Nếu ép 10 lần mà CB vẫn có vật
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
        MotorEpLon.YeuCau = TrangThaiMotorEp::STOP;
        NghienChai.dangRunFlag = 0;
        PhanLoai.dangxulyFlag = 0;
        EpLon.dangxulyFlag = 0;
    }
}

void phanhoi_trangthailoi()
{

    uint8_t trangthai_Ep = ((EpLon.dangxulyFlag == 1) ? 0x02 : EpLon.trangthaiLoi);

    uint8_t bytecount = 2;
    uint8_t datasize = bytecount + 4;
    raiseOrderCommand(self_order_command);

    uint8_t data[datasize + 1] = {datasize,
                                  codeMaster::check_error,
                                  last_order_command[0],
                                  last_order_command[1],
                                  bytecount,
                                  trangthai_Ep,
                                  ui2_trangthaiRac3};

    memcpy(buffer_send, data, datasize + 1);
    RS485.send(buffer_send);
#ifdef ShowSerial

    SerialDebug.printf("\nguiphanhoichoMaster: %D %D\n", trangthai_Ep, ui2_trangthaiRac3);
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
void serialDebugProcess()
{
    SerialDebug.print("\n test bang debug :");
    SerialDebug.println(inputString);

    if (inputString == "eplon_fw")
    {
        SerialDebug.println("Eplon fw");

        EpLon.trangthaiYeuCau = TrangThaiMotorEp::FORWARD;
        xuly_yeucau_eplon();
    }

    else if (inputString == "xoaloi")
    {

        SerialDebug.println("Xoaloi");
        xuly_yeucau_xoaloi();
    }
    else if (inputString == "admin_ep_fw")
    {
        ui8_admin_control_motor = true;
        MotorEpLon.YeuCau = TrangThaiMotorEp::FORWARD;
        SerialDebug.println(" admin control");
    }

    else if (inputString == "end_admin")
    {
        ui8_admin_control_motor = false;
        SerialDebug.println("end admin control");
    }
    else if (inputString == "reset")
    {
        SerialDebug.println("Reset");
        ESP.restart();
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
    SerialDebug.printf("Expected firmware size: %u bytes\n", expectedSize);

    // Bắt đầu cập nhật
    if (!updateStarted)
    {
        if (expectedSize == 0)
        {
            SerialDebug.println(" No firmware to update.");
            ui8_update_firmware = false;
            return;
        }
        if (!Update.begin(expectedSize))
        {
            SerialDebug.println("Not enough space for firmware update.");
            ui8_update_firmware = false;
            return;
        }
        updateStarted = true;
        SerialDebug.println("Firmware update started...");
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

            SerialDebug.printf("Checksum: %02X\n", checksum);
            if (buffer[0] == 'B' && buffer[1] == 'O' && buffer[2] == 'P')
            {
                SerialDebug.println("Received Begin Page Maker ");
            }
            if (buffer[len - 3] == 'E' && buffer[len - 2] == 'O' && buffer[len - 1] == 'P')
            {
                SerialDebug.println("Received End Page Maker ");
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
                    SerialDebug.println("Sent respond OK");
                }
                else /* Nếu không thành công thì không phản hồi*/
                {
                    SerialDebug.println("Error writing firmware data!");
                    break;
                    firmwareInvalid = true;
                }
            }
            else if (counter <= 3) /* Nếu dữ liệu không hợp lệ*/
            {
                SerialDebug.printf("Received error %d bytes \n", len - HEADER_SIZE);
                counter++;
                // digitalWrite(RS485_DE_PIN, HIGH);
                uint8_t code[3] = {'E', 'R', 'R'};
                Serial1.write(code, sizeof(code));
                Serial1.flush();
                // digitalWrite(RS485_DE_PIN, LOW);
                SerialDebug.println("Request resend -------------------------------------------------------------------------------------------------------------");
                continue;
            }
            else if (counter > 3)
            {
                firmwareInvalid = true;
                break;
            }

            totalReceived += len - HEADER_SIZE;
            SerialDebug.printf("Received %d bytes|| Total received: %d \n", len - HEADER_SIZE, totalReceived);

            /*Kiểm tra khi nhận tất cả dữ liệu thì break while*/
            if (receivedEOF)
            {
                /* Nếu không nhận được đúng size như ban đầu gửi thì báo lỗi*/
                if (totalReceived != expectedSize)
                    firmwareWrongSize = true;

                SerialDebug.println("Received End File Maker ");
                break;
            }

            SerialDebug.printf("Time: %d ms\n", millis() - timer1);
        }
    }

    SerialDebug.printf("Total time: %d ms\n", millis() - timer0);

    /*Kiểm tra các lỗi để không tiến hành cập nhật*/
    if (timeoutReceive || firmwareInvalid || firmwareWrongSize)
    {
        SerialDebug.println("Firmware update failed.");
        SerialDebug.printf("timeoutReceive: %d\n", timeoutReceive);
        SerialDebug.printf("firmwareInvalid: %d\n", firmwareInvalid);
        SerialDebug.printf("firmwareWrongSize: %d\n", firmwareWrongSize);
        Update.abort();
    }
    /*Tiến hành cập nhật và khởi động lại*/
    else if (updateStarted)
    {
        SerialDebug.println("Firmware transfer completed.");
        Update.end();
        SerialDebug.println("Firmware update completed! Restarting...");
        ESP.restart();
        SerialDebug.println("Error");
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