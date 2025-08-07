
#include "src/SerialComunication.h"
#include "src/ReadSensorDebounce.h"

#define relay 3

#define relay_power 2

#define LED_ONBOARD 13

#define TIMERELAY 50

uint32_t ui32_time_blink;
bool led_status = 0;

SerialUSB SerialPC(Serial, 0x05);
Sensor CB_power_relay(relay_power);

uint8_t buffer_receive[100];
uint8_t buffer_send[100];
uint8_t ui8_control_relay = 0;
uint8_t ui8_number_relay = 0;
uint32_t ui32_time_start_relay = 0;

bool ui2_respond_confirmReceived = false;
bool ui2_funtion_self_respond = false;
bool ui2_respond_incorrectCRC = false;
bool ui2_respond_function_notExist = false;
bool ui2_respond_repeatedCommand = false;
bool ui2_respond_firmware = false;
bool ui2_reset = false;
uint8_t order_command[2] = {0x00, 0x00};
uint8_t last_order_command[2] = {0x00, 0x00};

void setup()
{
    SerialPC.begin(115200);
    pinMode(relay, OUTPUT);
    digitalWrite(relay, LOW);

    pinMode(relay_power, INPUT_PULLUP);

    pinMode(LED_ONBOARD, OUTPUT);
    digitalWrite(LED_ONBOARD, HIGH);

    ui32_time_blink = millis();
}

void loop()
{

    CB_power_relay.readSensorDebounce(100);

    SerialPC.receive(buffer_receive);
    processSerial();

    processRelay();

    blink(1000);
}
void blink(int time)
{
    if (millis() - ui32_time_blink > time)
    {
        ui32_time_blink = millis();
        led_status = !led_status;
        digitalWrite(LED_ONBOARD, led_status);
    }
}
void processRelay()
{

    if (ui8_number_relay != 0 &&
        ui8_control_relay == 0 &&
        millis() - ui32_time_start_relay > 1000)
    {
        ui8_control_relay = 1;
        ui8_number_relay--;
    }
    if (ui8_control_relay == 1)
    {
        digitalWrite(relay, HIGH);
        ui8_control_relay = 2;
        ui32_time_start_relay = millis();
    }
    if (ui8_control_relay == 2)
    {
        if (millis() - ui32_time_start_relay > TIMERELAY)
        {
            digitalWrite(relay, LOW);
            ui8_control_relay = 0;
        }
    }
}

void processSerial()
{
    if (SerialPC.incorrectCRC == true)
    {
        ui2_respond_incorrectCRC = true;
        SerialPC.incorrectCRC = false;
        return;
    }
    if (SerialPC.isReceived == true)
    {
        SerialPC.isReceived = false;
        memcpy(order_command, (buffer_receive + 2), 2);
        if (order_command[0] == last_order_command[0] && order_command[1] == last_order_command[1])
        {
        }

        switch (buffer_receive[1])
        {
        case 0x01:
        {
            memcpy(last_order_command, order_command, 2);
            if (buffer_receive[6] == 0xFF)
            {
                phanhoi_ok();
                ui8_number_relay += buffer_receive[5];
            }

            break;
        }

        case 0x02:
        {
            memcpy(last_order_command, order_command, 2);
            if (buffer_receive[5] == 0x01)
            {
                phanhoi_trangthai_relay();
            }

            break;
        }

        default:
            ui2_respond_function_notExist = 1; // Nếu không nhận được lệnh hợp lệ
            break;
        }

        if (ui2_respond_function_notExist == 0 && ui2_funtion_self_respond == 0) // Nếu nhận được lệnh hợp lệ
        {
            ui2_respond_confirmReceived = 1;
        }
    }
}

void phanhoi_ok()
{
    uint8_t bytecount = 1;
    uint8_t datasize = bytecount + 4;
    // raiseOrderCommand(order_command);

    uint8_t data[datasize + 1] = {datasize,
                                  0x01,
                                  order_command[0],
                                  order_command[1],
                                  bytecount,
                                  0x00};

    memcpy(buffer_send, data, datasize + 1);
    SerialPC.send(buffer_send);
}

void phanhoi_trangthai_relay()
{
    uint8_t bytecount = 1;
    uint8_t datasize = bytecount + 4;
    // raiseOrderCommand(order_command);

    uint8_t data[datasize + 1] = {datasize,
                                  0x02,
                                  order_command[0],
                                  order_command[1],
                                  bytecount,
                                  (CB_power_relay.state == 1) ? 0x01 : 0x00};

    memcpy(buffer_send, data, datasize + 1);
    SerialPC.send(buffer_send);
}

void raiseOrderCommand(uint8_t *order)
{
    (*(order + 1))++;
    *(order + 0) = (*(order + 1) == 0x00) ? ((*(order + 0)) + 1) : (*(order + 0));
}
