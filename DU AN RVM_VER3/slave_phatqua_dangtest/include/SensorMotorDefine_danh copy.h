
// #define RVM_HEINEKEN
// #define RVM_COCACOLA

#ifdef RVM_HEINEKEN

	BTS7960 Motor_xoaychai(EN_M1, PWML_M1, PWMR_M1);
	BTS7960 Motor_nhavat(EN_M2, PWML_M2, PWMR_M2);
	BTS7960 Motor_cua(EN_M3, PWML_M3, PWMR_M3);

	//MOTOR CHI SU DUNG CHO MAY RVM COCA
	BTS7960 Motor_bangtai(EN_M4, PWML_M4, PWMR_M4);

	#define cambien_vat PIN_A1
	#define cambien_nhavat PIN_A2
	#define cambien_longep1 PIN_A3
	#define cambien_longep2 PIN_A4
	#define cambien_hopep PIN_A5
	#define cambien_thungraclon PIN_A6
	#define cambien_cua1 PIN_A7
	#define cambien_cua2 PIN_A8
	#define cambien_cua3 PIN_B1
	#define	nutnhan 0

	//NHUNG CAM BIEN NAY CHI SU DUNG TRONG RVM COCA
	#define cambien_roichai PIN_B2
	#define cambien_roilon PIN_B3
	#define cambien_encodernghien PIN_B4
	#define cambien_thungracchai PIN_B5


	#define LED_NHANDIEN RL1
	#define motor_ep_thuan RL2
	#define motor_ep_nghich RL3

	//OUTPUT CHI SU DUNG CHO MAY RVM COCA
	#define motor_nghien_thuan RL4
	#define motor_nghien_nghich RL5
	#define LED_ONBOARD 2

	#define RE PWM1
	#define BL PWM3
	#define GR PWM2


#elif defined RVM_COCACOLA

	BTS7960 Motor_xoaychai(EN_M1, PWML_M1, PWMR_M1);
	BTS7960 Motor_nhavat(EN_M2, PWML_M2, PWMR_M2);
	BTS7960 Motor_bangtai(EN_M3, PWML_M3, PWMR_M3);
	BTS7960 Motor_cua(EN_M4, PWML_M4, PWMR_M4);

	#define cambien_vat PIN_A1
	#define cambien_nhavat PIN_A2
	#define cambien_roichai PIN_A3
	#define cambien_roilon PIN_A4
	#define cambien_encodernghien PIN_A5
	#define cambien_longep1 PIN_A6
	#define cambien_longep2 PIN_A7
	#define cambien_hopep PIN_A8
	#define cambien_thungracchai PIN_B1
	#define cambien_thungraclon PIN_B2
	#define cambien_cua1 PIN_B3
	#define cambien_cua2 PIN_B4
	#define cambien_cua3 PIN_B5
	#define cambien_cuamay PIN_B6
	#define	nutnhan 0

	#define LED_NHANDIEN RL1
	#define motor_nghien_thuan RL2
	#define motor_nghien_nghich RL3
	#define motor_ep_thuan RL4
	#define motor_ep_nghich RL5

	#define LED_ONBOARD 2

	#define RE PWM1
	#define GR PWM2
	#define BL PWM3

#else

	// BTS7960 Motor_cua(EN_M1, PWML_M1, PWMR_M1);
	// BTS7960 Motor_bangtai(EN_M2, PWML_M2, PWMR_M2);
	// BTS7960 Motor_xoaychai(EN_M3, PWML_M3, PWMR_M3);
	// BTS7960 Motor_nang_ha(EN_M4, PWML_M4, PWMR_M4);

	BTS7960 Motor_cua(EN_M1, PWML_M1, PWMR_M1);
	BTS7960 Motor_bangtai(EN_M2, PWML_M2, PWMR_M2);
	BTS7960 Motor_xoaychai(EN_M4, PWML_M4, PWMR_M4);
	BTS7960 Motor_nang_ha(EN_M3, PWML_M3, PWMR_M3);


	#define cambien_kimloai1 PIN_A1
	#define cambien_kimloai2 PIN_A2
	#define cambien_cua1 PIN_A3
	#define cambien_cua2 PIN_A4
	#define cambien_cua3 PIN_A5
	#define cambien_cua4 PIN_A6
	#define cambien_vat1 PIN_A7
	#define cambien_vat2 PIN_A8
	#define cambien_nangha PIN_B1
	#define cambien_cuamay PIN_B2
	#define cambien_cuathungrac PIN_B3
	#define cambien_thungracchai PIN_B4
	#define cambien_thungraclon PIN_B5
	#define cambien_thungrackhac PIN_B6
	#define	nutnhan 0

	#define LED_NHANDIEN PWM4
	#define motor_nghien_thuan RL2
	#define motor_nghien_nghich RL3
	#define motor_ep_thuan RL4
	#define motor_ep_nghich RL5

	#define LED_ONBOARD 2

	#define RE PWM1
	#define GR PWM2
	#define BL PWM3

#endif