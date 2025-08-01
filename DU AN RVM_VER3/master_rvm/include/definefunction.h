
#pragma once

/*********************************MACRO FUNCTION***********************************/

#define START_USER()\
	if(ui8_trangthaimay > 0) {\
		if(motor_nangha.ui8_kiemtra_trangthailoi_motorDC == TrangthaiLoi::LOI){\
			ui8_function = 0x15;\
			send_state_to_PC(ui8_function);\
			Serial_Debug.printf("MOTOR NANG HA DANG LOI\n");\
			delay(500);\
		}\
		if(motor_ep.ui8_kiemtra_trangthailoi == TrangthaiLoi::LOI){\
			ui8_function = 0x14;\
			send_state_to_PC(ui8_function);\
			Serial_Debug.printf("MOTOR EP LON DANG LOI\n");\
			delay(500);\
		}\
		if(motor_nghien.ui8_kiemtra_trangthailoi == TrangthaiLoi::LOI){\
			ui8_function = 0x16;\
			send_state_to_PC(ui8_function);\
			Serial_Debug.printf("MOTOR NGHIEN CHAI DANG LOI\n");\
			delay(500);\
		}\
		if(motor_phanloai.ui8_kiemtra_trangthailoi == TrangthaiLoi::LOI){\
			ui8_function = 0x17;\
			send_state_to_PC(ui8_function);\
			Serial_Debug.printf("MOTOR PHAN LOI DANG LOI\n");\
			delay(500);\
		}\
		return;\
	}\
	if(ui8_startprocess == 1) return;\
	if(motor_cua.ui8_trangthaimotor != TrangThaiMotorDC::NANGLEN){\
		stop_Motor_Cua();\
		run_Motor_Cua_Nanglen();\
	}\
	TATLED();\
	BATTAT_LED_NHANDIEN(TrangThaiLed::OFF);\
	ui8_startprocess = 1;\
	ui8_is_stopuser = 0;\
	Serial_Debug.printf("CHO PHEP USER MOI\n");
	
#define STOP_USER()\
	if(ui8_startprocess == 0) return;\
	ui8_is_stopuser = 1;\
	Serial_Debug.printf("KET THUC USER\n");
	
#define SETTRANGTHAIMAY()\
	ui8_trangthaimay = ui8_trangthaithungracchai + ui8_trangthaithungraclon + ui8_trangthaithungrackhac\
	+ motor_nghien.ui8_kiemtra_trangthailoi + motor_ep.ui8_kiemtra_trangthailoi + motor_nangha.ui8_kiemtra_trangthailoi_motorDC + motor_phanloai.ui8_kiemtra_trangthailoi;\
	Serial_Debug.printf("TRANG THAI MAY LA: %d\n", ui8_trangthaimay);\
	
#define BATLED(red, green, blue)\
	analogWrite(led_red,red);\
	analogWrite(led_green,green);\
	analogWrite(led_blue,blue);\

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
	if(biendieukien != 0)\
	{\
		ui8_status = status;\
		ui8_function = function;\
		guilenh;\
		biendieukien = TrangThaiLenhPhanHoi::HOANTHANH;\
		return;\
	}


#define KIEMTRAMOTOR(trangthai, chaymotor, bienphanhoi, stopmotor, bienhoanthanh, cambien)\
	if(trangthai != TrangthaiLoi::LOI){\
		if(ui8_is_loop == 0){\
			int tam = digitalRead(cambien);\
			if(tam == HIGH){\
				while(1){\
					int chay = chaymotor();\
					if(chay == 1) {\
						bienhoanthanh = 1;\
						break;\
					}\
					else if(chay == 0){\
						stopmotor();\
						trangthai = TrangthaiLoi::LOI;\
						SETTRANGTHAIMAY();\
						bienphanhoi = TrangThaiLenhPhanHoi::DUNG;\
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
				trangthai = TrangthaiLoi::LOI;\
				stopmotor();\
				SETTRANGTHAIMAY();\
				bienphanhoi = TrangThaiLenhPhanHoi::DUNG;\
				bienhoanthanh = 1;\
			}\
		}\
	}\
	else stopmotor();