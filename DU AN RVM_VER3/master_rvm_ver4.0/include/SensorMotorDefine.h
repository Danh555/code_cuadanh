#define MASTER

#ifdef MASTER
	#define cambien_cua1 PIN_CAMBIEN::PIN_S1
	#define cambien_cua2 PIN_CAMBIEN::PIN_S2
	#define cambien_cua3 PIN_CAMBIEN::PIN_S3
	#define cambien_cua4 PIN_CAMBIEN::PIN_S4
	#define cambien_kimloai1 PIN_CAMBIEN::PIN_S5
	#define cambien_kimloai2 PIN_CAMBIEN::PIN_S6
	#define cambien_nangha PIN_CAMBIEN::PIN_S7
	#define cambien_vat1 PIN_CAMBIEN::PIN_S8
	#define cambien_vat2 PIN_CAMBIEN::PIN_S9
	#define cambien_duphong PIN_CAMBIEN::PIN_S10

	#define led_white PIN_LED::PIN_W
	#define led_blue PIN_LED::PIN_B
	#define led_green PIN_LED::PIN_G
	#define led_red PIN_LED::PIN_R

	#define ledon_board1 45
	#define ledon_board2 46

	#define nutbam 0

	unsigned char CB[]={
		cambien_cua1,
		cambien_cua2,
		cambien_cua3,
		cambien_cua4,
		cambien_kimloai1,
		cambien_kimloai2,
		cambien_nangha,
		cambien_vat1,
		cambien_vat2,
		cambien_duphong
	};

	void config()
	{
		
		for(int i=0; i<sizeof(CB);i++)
		{
			pinMode(CB[i],INPUT_PULLUP);
		}

		pinMode(nutbam,INPUT_PULLUP);
		pinMode(ledon_board1,OUTPUT);
		pinMode(ledon_board2,OUTPUT);
	}

#else
	#define cambien_cuatren PIN_CAMBIEN::PIN_S1
	#define cambien_cuaduoi PIN_CAMBIEN::PIN_S2
	// #define cambien_cua3 PIN_CAMBIEN::PIN_S3
	// #define cambien_cua4 PIN_CAMBIEN::PIN_S4
	// #define cambien_kimloai1 PIN_CAMBIEN::PIN_S5
	// #define cambien_kimloai2 PIN_CAMBIEN::PIN_S6
	// #define cambien_nangha PIN_CAMBIEN::PIN_S7
	// #define cambien_vat1 PIN_CAMBIEN::PIN_S8
	// #define cambien_vat2 PIN_CAMBIEN::PIN_S9
	// #define cambien_duphong PIN_CAMBIEN::PIN_S10

	#define mocua_tren PIN_ROLE::PIN_ROLE1
	#define mocua_duoi PIN_ROLE::PIN_ROLE2
	#define ledon_board1 39
	// #define ledon_board2 46

	// #define nutbam 0

	unsigned char CB[]={
		cambien_cuatren,
		cambien_cuaduoi

	};

	unsigned char RL[]={
		mocua_tren,
		mocua_duoi
	};


	void config()
	{
		
		for(int i=0; i<sizeof(CB);i++)
		{
			pinMode(CB[i],INPUT_PULLUP);
		}

		for(int i=0; i<sizof(RL);i++)
		{
			pinMode(RL[i] ,OUTPUT);
			digitalWrite(RL[i],0);
		}

		// pinMode(nutbam,INPUT_PULLUP);
		pinMode(ledon_board1,OUTPUT);
		// pinMode(ledon_board2,OUTPUT);
	}
#endif

// DONGCO Motor_cua(DIR_MOTOR1,PWM_MOTOR1);
// DONGCO Motor_bangtai(DIR_MOTOR2,PWM_MOTOR2);
// DONGCO Motor_xoaychai(DIR_MOTOR3,PWM_MOTOR3);
// DONGCO Motor_nangha(DIR_MOTOR4,PWM_MOTOR4);


void checkpulse()
{
	int reading = digitalRead(pinA);
	if (reading != lastButtonState[0])
	{
		lastDebounceTime[0] = millis();	
	} 

	if ((millis() - lastDebounceTime[0]) > 200) 
	{
		if(reading == 1)
		{
			int reading2= digitalRead(pinB);
			if(reading2 != lastButtonState[1])
			{
				lastDebounceTime[1] = millis();	
			}	

			if(millis() - lastDebounceTime[1] > 200)
			{
				if(reading2 == 1)
				{
					// xu ly tang
					encoderPos++;
				}
			}
			lastButtonState[1] = reading2;
		}

		if(reading==0)
		{
			encoderPos--;
		}

		if (encoderPos < 0) 
          {
          flag_encoderPos = 3;
          encoderPos=0;
          //Serial.println("encode < 0");
          }
      if (encoderPos > 8) 
          {
            flag_encoderPos = 0;
            encoderPos = 0;
            //Serial.println("encode > 8");
          }
      if(encoderPos >=0 && encoderPos < 2) 
          {
            flag_encoderPos= 0;
          //  Serial.println(" 0< encode<2");
          }
      if(encoderPos >=2 && encoderPos < 4)
          { flag_encoderPos= 1;
        //  Serial.println(" 2< encode<4");
          }
      if(encoderPos >=4 && encoderPos < 6) 
          {
            flag_encoderPos= 2;
          //  Serial.println(" 4< encode<6");
          }
      if(encoderPos >=6 && encoderPos < 8) 
          {
            flag_encoderPos= 3;
          //  Serial.println(" 6< encode<8");
          }

		   Serial.print("so xung hien tai: ");
		   Serial.println(encoderPos);
       		lastReadMs = millis()+ 500; 
	}
	  lastButtonState[0] = reading;

	   
}