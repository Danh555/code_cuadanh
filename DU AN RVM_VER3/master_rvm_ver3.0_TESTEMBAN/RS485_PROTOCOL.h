#include "SerialComunicationv2.h"
#include "include/GlobalVariables.h"

#define BUFFER_SIZE 10240 // Kích thước gói dữ liệu

const uint8_t BEGIN_PAGE_MARKER[] = {'B', 'O', 'P'};
const uint8_t END_PAGE_MARKER[] = {'E', 'O', 'P'};
const uint8_t BEGIN_FILE_MARKER[] = {'B', 'O', 'F'};
const uint8_t END_FILE_MARKER[] = {'E', 'O', 'F'};

RS485_Master RS485(Serial1, TX_PIN_485, RX_PIN_485);

uint8_t self_order_command[2] = {0x00, 0x00};
uint8_t ui8_buffer_send_slave[15];
namespace codeSlaveRS485
{
    const uint8_t PHANLOAI = 0x01;
    const uint8_t NGHIENCHAI = 0x02;
    const uint8_t EPLON = 0x03;
    const uint8_t check_error = 0x04;
    const uint8_t erase_error = 0x05;
    const uint8_t admin_control_motor = 0x06;
    const uint8_t check_fw = 0x07;
    const uint8_t update_firmware = 0x08;
    const uint8_t DONGMOCUA=0x09;
    const uint8_t DONGCUA=0x10;
    const uint8_t PHATQUA=0x11;
    const uint8_t RESET=0x12;
};

void raiseOrderCommand(uint8_t *order)
{
    (*(order + 1))++;
    *(order + 0) = (*(order + 1) == 0x00) ? ((*(order + 0))+1) : (*(order + 0));
}

void admin_control_RS485(uint8_t Motor, uint8_t YeuCau,uint8_t slave)
{
    uint8_t bytecount = 2;
    uint8_t datasize = bytecount + 4;
    raiseOrderCommand(self_order_command);
    uint8_t data[datasize + 1] = {datasize,
                                  codeSlaveRS485::admin_control_motor,
                                  self_order_command[0],
                                  self_order_command[1],
                                  bytecount,
                                  Motor,
                                  YeuCau};
    memcpy(ui8_buffer_send_slave, data, datasize + 1);
    RS485.send(ui8_buffer_send_slave, slave);
}

void check_error_slave_RS485(uint8_t slave)
{
    uint8_t bytecount = 1;
    uint8_t datasize = bytecount + 4;
    raiseOrderCommand(self_order_command);
    uint8_t data[datasize + 1] = {datasize,
                                  codeSlaveRS485::check_error,
                                  self_order_command[0],
                                  self_order_command[1],
                                  bytecount,
                                  0x00};
    memcpy(ui8_buffer_send_slave, data, datasize + 1);
    RS485.send(ui8_buffer_send_slave, slave);
}

void erase_error_slave_RS485(uint8_t slave)
{
    uint8_t bytecount = 1;
    uint8_t datasize = bytecount + 4;
    raiseOrderCommand(self_order_command);
    uint8_t data[datasize + 1] = {datasize,
                                  codeSlaveRS485::erase_error,
                                  self_order_command[0],
                                  self_order_command[1],
                                  bytecount,
                                  0x00};
    memcpy(ui8_buffer_send_slave, data, datasize + 1);
    RS485.send(ui8_buffer_send_slave, slave);
}

void check_firmware_slave_RS485(uint8_t slave)
{
    uint8_t bytecount = 1;
    uint8_t datasize = bytecount + 4;
    raiseOrderCommand(self_order_command);
    uint8_t data[datasize + 1] = {datasize,
                                  codeSlaveRS485::check_fw,
                                  self_order_command[0],
                                  self_order_command[1],
                                  bytecount,
                                  0x00};
    memcpy(ui8_buffer_send_slave, data, datasize + 1);
    RS485.send(ui8_buffer_send_slave, slave);
}

void reset_slave(uint8_t slave)
{
    uint8_t bytecount = 1;
    uint8_t datasize = bytecount + 4;
    raiseOrderCommand(self_order_command);
    uint8_t data[datasize + 1] = {datasize,
                                  codeSlaveRS485::RESET,
                                  self_order_command[0],
                                  self_order_command[1],
                                  bytecount,
                                  0x00};
    memcpy(ui8_buffer_send_slave, data, datasize + 1);
    RS485.send(ui8_buffer_send_slave, slave);
}

void dongmocua_trenduoi(uint8_t Vitri,uint8_t trangthai,uint8_t slave)
{
    uint8_t bytecount = 2;
    uint8_t datasize = bytecount + 4;
    raiseOrderCommand(self_order_command);
    uint8_t data[datasize + 1] = {datasize,
                                  codeSlaveRS485::DONGMOCUA,
                                  self_order_command[0],
                                  self_order_command[1],
                                  bytecount,
                                  Vitri,
                                  trangthai};
    memcpy(ui8_buffer_send_slave, data, datasize + 1);
    RS485.send(ui8_buffer_send_slave, slave);
}

void dongcua_trenduoi(uint8_t Vitri,uint8_t slave)
{
    uint8_t bytecount = 1;
    uint8_t datasize = bytecount + 4;
    raiseOrderCommand(self_order_command);
    uint8_t data[datasize + 1] = {datasize,
                                  codeSlaveRS485::DONGCUA,
                                  self_order_command[0],
                                  self_order_command[1],
                                  bytecount,
                                  Vitri};
    memcpy(ui8_buffer_send_slave, data, datasize + 1);
    RS485.send(ui8_buffer_send_slave, slave);
}

// void check_firmware_slave_RS485()
// {
//     uint8_t bytecount = 1;
//     uint8_t datasize = bytecount + 4;
//     raiseOrderCommand(self_order_command);
//     uint8_t data[datasize + 1] = {datasize,
//                                   codeSlaveRS485::check_fw,
//                                   self_order_command[0],
//                                   self_order_command[1],
//                                   bytecount,
//                                   0x00};
//     memcpy(ui8_buffer_send_slave, data, datasize + 1);
//     RS485.send(ui8_buffer_send_slave, slave1);
// }

void control_BoPhanLoai(uint8_t Vitri,uint8_t slave)
{
    uint8_t bytecount = 1;
    uint8_t datasize = bytecount + 4;
    raiseOrderCommand(self_order_command);
    uint8_t data[datasize + 1] = {datasize,
                                  codeSlaveRS485::PHANLOAI,
                                  self_order_command[0],
                                  self_order_command[1],
                                  bytecount,
                                  Vitri};
    memcpy(ui8_buffer_send_slave, data, datasize + 1);
    RS485.send(ui8_buffer_send_slave, slave);
}

void control_BoEp(uint8_t Chieuchay,uint8_t slave)
{
    // if(millis()<ui32_timeout_ep) return;
    uint8_t bytecount = 1;
    uint8_t datasize = bytecount + 4;
    raiseOrderCommand(self_order_command);
    uint8_t data[datasize + 1] = {datasize,
                                  codeSlaveRS485::EPLON,
                                  self_order_command[0],
                                  self_order_command[1],
                                  bytecount,
                                  Chieuchay};
    memcpy(ui8_buffer_send_slave, data, datasize + 1);
    RS485.send(ui8_buffer_send_slave, slave);
}

void control_BoNghien(uint8_t Chieuchay,uint8_t slave)
{
    uint8_t bytecount = 1;
    uint8_t datasize = bytecount + 4;
    raiseOrderCommand(self_order_command);
    uint8_t data[datasize + 1] = {datasize,
                                  codeSlaveRS485::NGHIENCHAI,
                                  self_order_command[0],
                                  self_order_command[1],
                                  bytecount,
                                  Chieuchay};
    memcpy(ui8_buffer_send_slave, data, datasize + 1);
    RS485.send(ui8_buffer_send_slave, slave);
}

void control_mayphatqua(uint8_t vitrikhay, uint8_t vitri_o, uint8_t slave)
{
    uint8_t bytecount = 2;
    uint8_t datasize = bytecount + 4;
    raiseOrderCommand(self_order_command);
    uint8_t data[datasize + 1] = {datasize,
                                  codeSlaveRS485::PHATQUA,
                                  self_order_command[0],
                                  self_order_command[1],
                                  bytecount,
                                  vitrikhay,
                                  vitri_o};
    memcpy(ui8_buffer_send_slave, data, datasize + 1);
    RS485.send(ui8_buffer_send_slave, slave);
}


void update_firmware_slave_RS485(uint8_t slave)
{
  uint8_t bytecount = 1;
  uint8_t datasize = bytecount + 4;
  raiseOrderCommand(self_order_command);
  uint8_t data[datasize + 1] = {datasize,
                                codeSlaveRS485::update_firmware,
                                self_order_command[0],
                                self_order_command[1],
                                bytecount,
                                0x00};
  memcpy(ui8_buffer_send_slave, data, datasize + 1);
  RS485.send(ui8_buffer_send_slave, slave);
}