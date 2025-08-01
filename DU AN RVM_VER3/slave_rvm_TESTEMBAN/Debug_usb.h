

HWCDC Serial_Debug;

static void usbEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  if (event_base == ARDUINO_HW_CDC_EVENTS) {
    switch (event_id) {
      case ARDUINO_HW_CDC_RX_EVENT:
        //Serial0.println("\nCDC EVENT:: ARDUINO_HW_CDC_RX_EVENT");
        // sends all bytes read from USB Hardware Serial to UART0
        while (Serial_Debug.available()) {
        //   Serial_Debug.write(Serial_Debug.read());
          String input = Serial_Debug.readString();
		if(input=="chaythuan")
        {
        test_runmotor_thuan();
        }

        if(input=="chaynghich")
        {
        test_runmotor_nghich();
        }

        if(input=="dung")
        {
        stop_motor_cua();
        }
        
            }
            break;
        default: break;
        }
  }
}