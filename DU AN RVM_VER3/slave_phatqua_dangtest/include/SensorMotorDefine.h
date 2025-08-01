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
	#define cambien_roiqua1 PIN_CAMBIEN::PIN_S1
	#define cambien_roiqua2 PIN_CAMBIEN::PIN_S2

	#define hang1_vitriqua_1 PIN_A1
	#define hang1_vitriqua_2 PIN_A2
	#define hang1_vitriqua_3 PIN_A3
	#define hang1_vitriqua_4 PIN_A4
	#define hang1_vitriqua_5 PIN_A5


	#define hang2_vitriqua_1 PIN_A6
	#define hang2_vitriqua_2 PIN_A7
	#define hang2_vitriqua_3 PIN_A8
	#define hang2_vitriqua_4 PIN_A9
	#define hang2_vitriqua_5 PIN_A10

	#define hang3_vitriqua_1 PIN_A11
	#define hang3_vitriqua_2 PIN_A12
	#define hang3_vitriqua_3 PIN_A13
	#define hang3_vitriqua_4 PIN_A14
	#define hang3_vitriqua_5 PIN_A15

	// #define hang2_vitriqua_8 PIN_B8

	// #define hang3_vitriqua_1 PIN_B1
	// #define hang3_vitriqua_2 PIN_B2
	// #define hang3_vitriqua_3 PIN_B3
	// #define hang3_vitriqua_4 PIN_B4
	// #define hang3_vitriqua_5 PIN_B5
	// #define hang3_vitriqua_6 PIN_B6
	// #define hang3_vitriqua_7 PIN_B7
	// #define hang3_vitriqua_8 PIN_B8

	// #define hang4_vitriqua_1 PIN_C1
	// #define hang4_vitriqua_2 PIN_C2
	// #define hang4_vitriqua_3 PIN_C3
	// #define hang4_vitriqua_4 PIN_C4
	// #define hang4_vitriqua_5 PIN_C5
	// #define hang4_vitriqua_6 PIN_C6
	// #define hang4_vitriqua_7 PIN_C7
	// #define hang4_vitriqua_8 PIN_C8

	// #define hang5_vitriqua_1 PIN_D1
	// #define hang5_vitriqua_2 PIN_D2
	// #define hang5_vitriqua_3 PIN_D3
	// #define hang5_vitriqua_4 PIN_D4
	// #define hang5_vitriqua_5 PIN_D5
	// #define hang5_vitriqua_6 PIN_D6
	// #define hang5_vitriqua_7 PIN_D7
	// #define hang5_vitriqua_8 PIN_D8

	// #define cambien_cua3 PIN_CAMBIEN::PIN_S3
	// #define cambien_cua4 PIN_CAMBIEN::PIN_S4
	// #define cambien_kimloai1 PIN_CAMBIEN::PIN_S5
	// #define cambien_kimloai2 PIN_CAMBIEN::PIN_S6
	// #define cambien_nangha PIN_CAMBIEN::PIN_S7
	// #define cambien_vat1 PIN_CAMBIEN::PIN_S8
	// #define cambien_vat2 PIN_CAMBIEN::PIN_S9
	// #define cambien_duphong PIN_CAMBIEN::PIN_S10

	#define led1 PIN_ROLE::PIN_ROLE1
	#define led2 PIN_ROLE::PIN_ROLE2
	#define ledon_board1 39

	unsigned char CB[]={
		cambien_roiqua1,
		cambien_roiqua2

	};

	unsigned char RL[]={
		led1,
		led2
	};


	unsigned char KHAY_1[]={
		hang1_vitriqua_1,
		hang1_vitriqua_2,
		hang1_vitriqua_3,
		hang1_vitriqua_4,
		hang1_vitriqua_5
	};

	unsigned char KHAY_2[]={
		hang2_vitriqua_1,
		hang2_vitriqua_2,
		hang2_vitriqua_3,
		hang2_vitriqua_4,
		hang2_vitriqua_5
	};

	unsigned char KHAY_3[]={
		hang3_vitriqua_1,
		hang3_vitriqua_3,
		hang3_vitriqua_2,
		hang3_vitriqua_4,
		hang3_vitriqua_5
	};

	// unsigned char KHAY_4[]={
	// 	hang4_vitriqua_1,
	// 	hang4_vitriqua_2,
	// 	hang4_vitriqua_3,
	// 	hang4_vitriqua_4,
	// 	hang4_vitriqua_5,
	// 	hang4_vitriqua_6,
	// 	hang4_vitriqua_7,
	// 	hang4_vitriqua_8
	// };

	// unsigned char KHAY_5[]={
	// 	hang5_vitriqua_1,
	// 	hang5_vitriqua_2,
	// 	hang5_vitriqua_3,
	// 	hang5_vitriqua_4,
	// 	hang5_vitriqua_5,
	// 	hang5_vitriqua_6,
	// 	hang5_vitriqua_7,
	// 	hang5_vitriqua_8
	// };

	
	unsigned char KHAY[3][5]={
		KHAY_1[5],
		KHAY_2[5],
		KHAY_3[5]
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
		digitalWrite(ledon_board1,LOW);
	}
#endif
