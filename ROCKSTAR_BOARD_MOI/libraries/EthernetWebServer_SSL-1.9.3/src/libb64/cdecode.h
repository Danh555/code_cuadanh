/****************************************************************************************************************************
  cdecoder.h - c source to a base64 decoding algorithm implementation

  EthernetWebServer_SSL is a library for the Ethernet shields to run WebServer and Client with/without SSL

  Use SSLClient Library code from https://github.com/OPEnSLab-OSU/SSLClient

  Built by Khoi Hoang https://github.com/khoih-prog/EthernetWebServer_SSL

  Version: 1.9.3

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.1.0   K Hoang      14/11/2020 Initial coding for SAMD21/SAMD51, nRF52, SAM DUE to support Ethernet shields using SSL.
                                  Supporting W5x00 using Ethernetx, ENC28J60 using EthernetENC and UIPEthernet libraries
  ...
  1.8.0   K Hoang      05/04/2022 Use Ethernet_Generic library as default. Support SPI2 for ESP32
  1.8.1   K Hoang      09/04/2022 Add support to SPI1 for RP2040 using arduino-pico core
  1.8.2   K Hoang      27/04/2022 Change from `arduino.cc` to `arduino.tips` in examples
  1.9.0   K Hoang      05/05/2022 Add support to custom SPI for Teensy, Mbed RP2040, Portenta_H7, etc.
  1.9.1   K Hoang      25/08/2022 Auto-select SPI SS/CS pin according to board package
  1.9.2   K Hoang      07/09/2022 Slow SPI clock for old W5100 shield or SAMD Zero. Improve support for SAMD21
  1.9.3   K Hoang      26/10/2022 Add support to Seeed XIAO_NRF52840 and XIAO_NRF52840_SENSE using `mbed` or `nRF52` core
 *****************************************************************************************************************************/

#pragma once

// Reintroduce to prevent duplication compile error if other lib/core already has LIB64
// pragma once can't prevent that
#ifndef BASE64_CDECODE_H
#define BASE64_CDECODE_H

#define base64_decode_expected_len(n) ((n * 3) / 4)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  step_a, step_b, step_c, step_d
} base64_decodestep;

typedef struct
{
  base64_decodestep step;
  char plainchar;
} base64_decodestate;

void base64_init_decodestate(base64_decodestate* state_in);

int base64_decode_value(int value_in);

int base64_decode_block(const char* code_in, const int length_in, char* plaintext_out, base64_decodestate* state_in);

int base64_decode_chars(const char* code_in, const int length_in, char* plaintext_out);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* BASE64_CDECODE_H */

