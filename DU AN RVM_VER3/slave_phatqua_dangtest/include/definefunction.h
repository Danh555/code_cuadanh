
#pragma once

/*********************************MACRO FUNCTION***********************************/

#define START_USER()\
	if(ui8_trangthaimay > 0) {\
		if(ui8_loidongco_nangha == LOI){\
			ui8_function = 0x15;\
			send_state_to_PC(ui8_function);\
			uart_debug.println("MOTOR NANG HA DANG LOI");\
			delay(500);\
		}\
		if(ui8_kiemtramotoreplon == LOI){\
			ui8_function = 0x14;\
			send_state_to_PC(ui8_function);\
			uart_debug.println("MOTOR EP LON DANG LOI");\
			delay(500);\
		}\
		if(ui8_kiemtramotornghienchai == LOI){\
			ui8_function = 0x16;\
			send_state_to_PC(ui8_function);\
			uart_debug.println("MOTOR NGHIEN CHAI DANG LOI");\
			delay(500);\
		}\
		if(ui8_kiemtramotorphanloai == LOI){\
			ui8_function = 0x17;\
			send_state_to_PC(ui8_function);\
			uart_debug.println("MOTOR PHAN LOI DANG LOI");\
			delay(500);\
		}\
		if(ui8_trangthaithungracchai == DAY){\
			ui8_function = 0x12;\
			send_state_to_PC(ui8_function);\
			ui8_phanhoi_trangthai_thungracchai = 1;\
			uart_debug.println("THUNG RAC CHAI DANG DAY");\
			delay(500);\
		}\
		if(ui8_trangthaithungraclon == DAY){\
			ui8_function = 0x12;\
			send_state_to_PC(ui8_function);\
			ui8_phanhoi_trangthai_thungraclon = 1;\
			uart_debug.println("MOTOR NHA VAT DANG LOI");\
			delay(500);\
		}\
		return;\
	}\
	if(ui8_startprocess == 1) return;\
	if(ui8_trangthaimotorcua != NANGLEN){\
		stop_Motor_Cua();\
		run_Motor_Cua_Nanglen();\
	}\
	TATLED();\
	BATTAT_LED_NHANDIEN(4096,0);\
	ui8_startprocess = 1;\
	ui8_is_stopuser = 0;\
	uart_debug.println("CHO PHEP USER MOI");
	
#define STOP_USER()\
	if(ui8_startprocess == 0) return;\
	ui8_is_stopuser = 1;\
	uart_debug.println("KET THUC USER");
	
#define SETTRANGTHAIMAY()\
	ui8_trangthaimay = ui8_trangthaithungracchai + ui8_trangthaithungraclon + ui8_trangthaithungrackhac\
	+ ui8_loidongco_nangha + ui8_kiemtramotoreplon + ui8_kiemtramotornghienchai + ui8_kiemtramotorphanloai;\
	uart_debug.print("TRANG THAI MAY LA: ");\
	uart_debug.println(ui8_trangthaimay);
	
#define XOALOI(bienkiemtra, add_EEPROM)\
	bienkiemtra = KHONGLOI;\
	SETTRANGTHAIMAY();\
	EEPROM.put(add_EEPROM, bienkiemtra);\
	EEPROM.commit();

#define BATLED(red, green, blue)\
	pwm.setPWM(RE, 0, red);\
	pwm.setPWM(GR, 0, green);\
	pwm.setPWM(BL, 0, blue);\

#define XULYLED(timecount, timeled, ledstate, ledcolor)\
	if(millis() - timecount >= timeled)\
	{\
		ledstate = 1-ledstate;\
		if(ledstate == 1)\
		{\
			ledcolor();\
		}\
		else TATLED();\
		timecount = millis();\
	}

#define PHANHOI(biendieukien, status, function, guilenh)\
	if(biendieukien == 1)\
	{\
		ui8_status = status;\
		ui8_function = function;\
		guilenh;\
		biendieukien = 0;\
		return;\
	}


#define KIEMTRAMOTOR(trangthai, chaymotor, bienphanhoi, stopmotor, bienhoanthanh, cambien)\
	if(trangthai != LOI){\
		if(ui8_is_loop == 0){\
			int tam = pcf_PortB.digitalRead(cambien);\
			if(tam == HIGH){\
				while(1){\
					int chay = chaymotor();\
					if(chay == 1) {\
						bienhoanthanh = 1;\
						break;\
					}\
					else if(chay == 0){\
						stopmotor();\
						trangthai = LOI;\
						SETTRANGTHAIMAY();\
						bienphanhoi = 1;\
						bienhoanthanh = 1;\
						break;\
					}\
				}\
			}\
			else bienhoanthanh = 1;\
		}\
		else{\
			int tam = chaymotor();\
			if(tam == 1) bienhoanthanh = 1;\
			else if(tam == 0){\
				trangthai = LOI;\
				stopmotor();\
				SETTRANGTHAIMAY();\
				bienphanhoi = 1;\
				bienhoanthanh = 1;\
			}\
		}\
	}\
	else stopmotor();