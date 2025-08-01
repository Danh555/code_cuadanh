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
		
		for(int i=0; i<11;i++)
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

		for(int i=0; i<sizeof(RL);i++)
		{
			pinMode(RL[i] ,OUTPUT);
			digitalWrite(RL[i],0);
		}
		pinMode(ledon_board1,OUTPUT);
	}
#endif
