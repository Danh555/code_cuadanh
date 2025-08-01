/*
* File: PROTOCOL.h
* Author: NGUYEN CONG DANH
* Date: 20/09/2023
* Description: This is library for PROTOCOL 
*/

#pragma once
#include "Arduino.h"
#include <stdio.h>
#include <stdint.h>

//hàm sử dụng trong thư viện này

void respondtoPC(uint8_t function_code, uint8_t status);
void send_state_to_PC(uint8_t function_code);
// void send_cnt_to_PC(uint8_t thungrac);
void send_data_khinhansailenh(uint8_t function_code_error, uint8_t status);
void send_firmware();
void send_state_array_to_PC(uint8_t function_code, uint8_t vitri_khay);

void respondtoPC(uint8_t function_code, uint8_t status){
	
    uint8_t sizeofdata = 8;
    uint8_t* data_send = (uint8_t*)malloc(sizeofdata * sizeof(uint8_t));
    *(data_send) = ui8_slaveID;
    *(data_send+1) = function_code;
    *(data_send+2) = ui16_self_order_command >> 8;
    *(data_send+3) = ui16_self_order_command & (0x00FF);
    *(data_send+4) = 0x01;
    *(data_send+5) = status;
    uint16_t ui16_crc_pro = calcCRC16(6, data_send);
    *(data_send+6) = ui16_crc_pro >> 8;
	*(data_send+7) = ui16_crc_pro & 0x00FF;
    
    for(int i=0; i<8; i++){
        Serial.write(*(data_send+i));
    }
    if(ui16_self_order_command < 0xFFFF)
        ++ui16_self_order_command;
    else ui16_self_order_command = 0x0001;
    free(data_send);
    // ui32_timecho_guilenh = millis();
}


// void send_state_array_to_PC(uint8_t function_code, uint8_t vitri_khay)
// {
//     uint8_t sizeofdata = 10;
//     uint8_t* data_send = (uint8_t*)malloc(sizeofdata * sizeof(uint8_t));

//     *(data_send) = ui8_slaveID;
//     *(data_send+1) = function_code;
//     *(data_send+2) = ui16_self_order_command >> 8;
//     *(data_send+3) = ui16_self_order_command & (0x00FF);
//     *(data_send+4) = 0x01;

//     uint8_t bytecount = 5;

    

//     uint16_t ui16_crc_pro = calcCRC16(bytecount, data_send);
//     *(data_send+bytecount) = ui16_crc_pro >> 8;
// 	*(data_send+bytecount+1) = ui16_crc_pro & 0x00FF;
// 	bytecount += 2;
    
//     for(int i=0; i<bytecount; i++)
//     {
//         Serial.write(*(data_send+i));
//     }
//     if(ui16_self_order_command < 0xFFFF)
//         ++ui16_self_order_command;
//     else ui16_self_order_command = 0x0001;
//     free(data_send);
// }

/************************HÀM GỬI TRẠNG THÁI LÊN PC***********************/
void send_state_to_PC(uint8_t function_code){
	
    uint8_t sizeofdata = 10;
    uint8_t* data_send = (uint8_t*)malloc(sizeofdata * sizeof(uint8_t));
	
    *(data_send) = ui8_slaveID;
    *(data_send+1) = function_code;
    *(data_send+2) = ui16_self_order_command >> 8;
    *(data_send+3) = ui16_self_order_command & (0x00FF);
    *(data_send+4) = 0x01;
	
	uint8_t bytecount = 5;
	
	// if(function_code == 0x12)
	// {
	// 	*(data_send+4) = 0x02;
	// 	if(ui8_phanhoi_trangthai_thungracchai == 1)
	// 	{
	// 		if(ui8_trangthaithungracchai == DAY)
	// 		{
	// 			*(data_send+5) = 0x01;
	// 			*(data_send+6) = 0xFF;
	// 			uart_debug.println("THUNG RAC CHAI DAY");
	// 		}
	// 		else{
	// 			*(data_send+5) = 0x01;
	// 			*(data_send+6) = 0x00;
	// 			uart_debug.println("THUNG RAC CHAI BINH THUONG");
	// 		} 
				
	// 		ui8_phanhoi_trangthai_thungracchai = 0;
	// 	}
	// 	else if(ui8_phanhoi_trangthai_thungraclon == 1)
	// 	{
	// 		if(ui8_trangthaithungraclon == DAY){
	// 			*(data_send+5) = 0x02;
	// 			*(data_send+6) = 0xFF;
	// 			uart_debug.println("THUNG RAC LON DAY");
	// 		}
				
	// 		else{
	// 			*(data_send+5) = 0x02;
	// 			*(data_send+6) = 0x00;
	// 			uart_debug.println("THUNG RAC LON BINH THUONG");
	// 		}
				
	// 		ui8_phanhoi_trangthai_thungraclon = 0;
	// 	}

	// 	else if(ui8_phanhoi_trangthai_thungrackhac == 1)
	// 	{
	// 		if(ui8_trangthaithungrackhac == DAY){
	// 			*(data_send+5) = 0x03;
	// 			*(data_send+6) = 0xFF;
	// 			uart_debug.println("THUNG RAC KHAC DAY");
	// 		}
				
	// 		else{
	// 			*(data_send+5) = 0x03;
	// 			*(data_send+6) = 0x00;
	// 			uart_debug.println("THUNG RAC KHAC BINH THUONG");
	// 		}
				
	// 		ui8_phanhoi_trangthai_thungrackhac = 0;
	// 	}

	// 	bytecount = 7;
	// }
	
	// else if(function_code == 0x08)
	// {
	// 	if(ui8_phathien_lon == 1){
	// 		*(data_send+5) = 0xFF;
	// 	}
			
	// 	// else if (ui8_cochaimoi == 1){
	// 	// 	*(data_send+5) = 0x01;	
	// 	// }
			
	// 	else 
	// 		*(data_send+5) = 0x00;
		
	// 	bytecount++;
	// }

	// else if(function_code == 0x21)
	// {
	// 	*(data_send+4) = 0x02;
	// 	if(ui8_phanhoi_trangthai_resetthungrac_chai == 1)
	// 	{
	// 		// if(ui8_trangthaithungracchai == KHONGDAY)
	// 		// {
	// 			*(data_send+5) = 0x01;
	// 			*(data_send+6) = 0x00;
	// 			uart_debug.println("DA RESET TRANG THAI THUNG RAC CHAI");	
	// 		// }
			
	// 		ui8_phanhoi_trangthai_resetthungrac_chai = 0;
	// 	}
	// else if(ui8_phanhoi_trangthai_resetthungrac_lon == 1)
	// {
	// 	// if(ui8_phanhoi_trangthai_resetthungrac_lon == 1)
	// 	// {
	// 	*(data_send+5) = 0x02;
	// 	*(data_send+6) = 0x00;	
	// 	uart_debug.println("DA RESET TRANG THAI THUNG RAC LON");
	// 	// }
	// 	ui8_phanhoi_trangthai_resetthungrac_lon = 0;
	// }

	// else if(ui8_phanhoi_trangthai_resetthungrac_khac == 1)
	// {
	// 	// if(ui8_phanhoi_trangthai_resetthungrac_lon == 1)
	// 	// {
	// 	*(data_send+5) = 0x03;
	// 	*(data_send+6) = 0x00;	
	// 	uart_debug.println("DA RESET TRANG THAI THUNG RAC khac");
	// 	// }
	// 	ui8_phanhoi_trangthai_resetthungrac_khac = 0;
	// }

	// 	// bytecount++;
	// 	bytecount = 7;
	// }

	// // else if(function_code == 0x22)
	// // {
	// // 	if(ui8_phanhoi_trangthai_vehome_motornhavat == 1)
	// // 	{
	// // 		if(ui8_trangthai_nhavat == KHONGNHA)
	// // 		{
	// // 			*(data_send+5) = 0x01;
	// // 			*(data_send+6) = 0xFF;	
	// // 			uart_debug.println("DONG CO NHA VAT QUAY DUOC MOT VONG");
	// // 		}
	// // 		ui8_phanhoi_trangthai_vehome_motornhavat == 0;
	// // 	}
	// // 	bytecount++;
	// // }


	// else if(function_code == 0xFA)
	// {
	// 	*(data_send+4) = 0x02;
	// 	if(ui8_phanhoi_mocuamay == 1)
	// 	{
	// 		if(ui8_trangthaicua_may == 1)
	// 		{
	// 			*(data_send+5) = 0x02;
	// 			*(data_send+6) = 0xFF;
	// 		}

	// 		else if(ui8_trangthaicua_may == 0)
	// 		{
	// 			*(data_send+5) = 0x02;
	// 			*(data_send+6) = 0x00;	
	// 		}
			
	// 		ui8_phanhoi_mocuamay = 0;
	// 	}
	// 	bytecount = 7;
	// }
	
	// else
	// {
	// 	if(ui8_trangthaivat == COVAT || ui8_kiemtramotoreplon == LOI || ui8_kiemtramotornghienchai == LOI || ui8_loidongco_nangha == LOI || ui8_kiemtramotorbangtai == LOI) 
	// 	{
	// 		*(data_send+5) = 0xFF;
	// 	}
	// 	else 
	// 	{
	// 		*(data_send+5) = 0x00; 
	// 	}
	// 	bytecount++;
	// }
	
    
    uint16_t ui16_crc_pro = calcCRC16(bytecount, data_send);
    *(data_send+bytecount) = ui16_crc_pro >> 8;
	*(data_send+bytecount+1) = ui16_crc_pro & 0x00FF;
	bytecount += 2;
    
    for(int i=0; i<bytecount; i++)
    {
        Serial.write(*(data_send+i));
    }
    if(ui16_self_order_command < 0xFFFF)
        ++ui16_self_order_command;
    else ui16_self_order_command = 0x0001;
    free(data_send);
    // ui32_timecho_guilenh = millis();
}

// void send_cnt_to_PC(uint8_t thungrac){
	
//     uint8_t sizeofdata = 9;
//     uint8_t* data_send = (uint8_t*)malloc(sizeofdata * sizeof(uint8_t));
//     *(data_send) = ui8_slaveID;
//     *(data_send+1) = 0x20;
//     *(data_send+2) = ui16_self_order_command >> 8;
//     *(data_send+3) = ui16_self_order_command & (0x00FF);
//     *(data_send+4) = 0x02;
// 	uint8_t phantram = 0;
	
// 	if(thungrac == THUNGRACCHAI){
// 		*(data_send+5) = 0x01;
// 		phantram = (uint8_t)((uint32_t)(ui16_sochai_trongthungrac * 100) / (uint16_t)SOCHAITOIDA);
// 		uart_debug.printf("PHAN TRAM TRONG THUNG RAC CHAI = %d\n", phantram);
// 	}

// 	else if(thungrac == THUNGRACKHAC)
// 	{
// 		*(data_send+5) = 0x03;
// 		phantram = (uint8_t)((uint32_t)(ui16_vat_trongthungrackhac * 100) / (uint16_t)SOLONTOIDA);
// 		uart_debug.printf("PHAN TRAM TRONG THUNG RAC khac = %d\n", phantram);
// 	}

// 	else{
// 		*(data_send+5) = 0x02;
// 		phantram = (uint8_t)((uint32_t)(ui16_solon_trongthungrac * 100) / (uint16_t)SOLONTOIDA);
// 		uart_debug.printf("PHAN TRAM TRONG THUNG RAC LON = %d\n", phantram);
// 	}
	
// 	*(data_send+6) = phantram;
    
//     uint16_t ui16_crc_pro = calcCRC16(7, data_send);
//     *(data_send+7) = ui16_crc_pro >> 8;
// 	*(data_send+8) = ui16_crc_pro & 0x00FF;
    
//     for(int i=0; i<sizeofdata; i++){
//         Serial.write(*(data_send+i));
//     }
//     if(ui16_self_order_command < 0xFFFF)
//         ++ui16_self_order_command;
//     else ui16_self_order_command = 0x0001;
//     free(data_send);
//     // ui32_timecho_guilenh = millis();
// }


void send_data_khinhansailenh(uint8_t function_code_error, uint8_t status)
{
	uint8_t sizeofdata = 9;
  uint8_t* data_send = (uint8_t*)malloc(sizeofdata * sizeof(uint8_t));
 	*(data_send) = ui8_slaveID;
 	*(data_send+1) = 0xFE;
 	*(data_send+2) = ui16_self_order_command >> 8;
  *(data_send+3) = ui16_self_order_command & (0x00FF);
  *(data_send+4) = 0x02;

  *(data_send+5) = function_code_error;
  *(data_send+6) = status;

   uint16_t ui16_crc_pro = calcCRC16(7, data_send);
  *(data_send+7) = ui16_crc_pro >> 8;
	*(data_send+8) = ui16_crc_pro & 0x00FF;
  
  for(int i=0; i<sizeofdata; i++)
  {
      Serial.write(*(data_send+i));
  }

  free(data_send);  
//   ui32_timecho_guilenh = millis();
}

// void send_firmware()
// {
// 	uint8_t sizeofdata = 11;
//   uint8_t* data_send = (uint8_t*)malloc(sizeofdata * sizeof(uint8_t));
//  	*(data_send) = ui8_slaveID;
//  	*(data_send+1) = 0xFB;
//  	*(data_send+2) = ui16_self_order_command >> 8;
//   *(data_send+3) = ui16_self_order_command & (0x00FF);
//   *(data_send+4) = 0x04;

//   uint8_t firmware_hi = 0;
// 	uint8_t firmware_lo = 0;
// 	uint8_t version_hi = 0;
// 	uint8_t version_lo = 0;

// 	str = firmware;
// 	data_hi = str.substring(5,6);
// 	data_lo = str.substring(6,8);

// 	firmware_hi	= uint8_t(data_hi.toInt());
// 	firmware_lo	= uint8_t(data_lo.toInt());

// 	*(data_send+5) = firmware_hi;
// 	*(data_send+6) = firmware_lo;

// 	*(data_send+7) = 0x00;
// 	*(data_send+8) = 0x03;

// 	uint16_t ui16_crc_pro = calcCRC16(9, data_send);
//   *(data_send+9) = ui16_crc_pro >> 8;
// 	*(data_send+10) = ui16_crc_pro & 0x00FF;
  
//   for(int i=0; i<sizeofdata; i++)
//   {
//       Serial.write(*(data_send+i));
//   }

//   free(data_send);  	
// //   ui32_timecho_guilenh = millis();

// }
