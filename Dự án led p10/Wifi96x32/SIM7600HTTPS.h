/**
 * HTTPClient.h
 *
 * Created on: 02.11.2015
 *
 * Copyright (c) 2015 Markus Sattler. All rights reserved.
 * This file is part of the HTTPClient for Arduino.
 * Port to ESP32 by Evandro Luis Copercini (2017), 
 * changed fingerprints to CA verification. 	
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
 /**
 C:\Users\ALTA-RD\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.6\libraries\HTTPClient
 */

#ifndef SIM7600HTTPS__
#define SIM7600HTTPS__

/// HTTP codes see RFC7231
typedef enum {
    SIM7600_HTTP_CODE_CONTINUE = 100,
    SIM7600_HTTP_CODE_SWITCHING_PROTOCOLS = 101,
    SIM7600_HTTP_CODE_PROCESSING = 102,
    SIM7600_HTTP_CODE_OK = 200,
    SIM7600_HTTP_CODE_CREATED = 201,
    SIM7600_HTTP_CODE_ACCEPTED = 202,
    SIM7600_HTTP_CODE_NON_AUTHORITATIVE_INFORMATION = 203,
    SIM7600_HTTP_CODE_NO_CONTENT = 204,
    SIM7600_HTTP_CODE_RESET_CONTENT = 205,
    SIM7600_HTTP_CODE_PARTIAL_CONTENT = 206,
    SIM7600_HTTP_CODE_MULTI_STATUS = 207,
    SIM7600_HTTP_CODE_ALREADY_REPORTED = 208,
    SIM7600_HTTP_CODE_IM_USED = 226,
    SIM7600_HTTP_CODE_MULTIPLE_CHOICES = 300,
    SIM7600_HTTP_CODE_MOVED_PERMANENTLY = 301,
    SIM7600_HTTP_CODE_FOUND = 302,
    SIM7600_HTTP_CODE_SEE_OTHER = 303,
    SIM7600_HTTP_CODE_NOT_MODIFIED = 304,
    SIM7600_HTTP_CODE_USE_PROXY = 305,
    SIM7600_HTTP_CODE_TEMPORARY_REDIRECT = 307,
    SIM7600_HTTP_CODE_PERMANENT_REDIRECT = 308,
    SIM7600_HTTP_CODE_BAD_REQUEST = 400,
    SIM7600_HTTP_CODE_UNAUTHORIZED = 401,
    SIM7600_HTTP_CODE_PAYMENT_REQUIRED = 402,
    SIM7600_HTTP_CODE_FORBIDDEN = 403,
    SIM7600_HTTP_CODE_NOT_FOUND = 404,
    SIM7600_HTTP_CODE_METHOD_NOT_ALLOWED = 405,
    SIM7600_HTTP_CODE_NOT_ACCEPTABLE = 406,
    SIM7600_HTTP_CODE_PROXY_AUTHENTICATION_REQUIRED = 407,
    SIM7600_HTTP_CODE_REQUEST_TIMEOUT = 408,
    SIM7600_HTTP_CODE_CONFLICT = 409,
    SIM7600_HTTP_CODE_GONE = 410,
    SIM7600_HTTP_CODE_LENGTH_REQUIRED = 411,
    SIM7600_HTTP_CODE_PRECONDITION_FAILED = 412,
    SIM7600_HTTP_CODE_PAYLOAD_TOO_LARGE = 413,
    SIM7600_HTTP_CODE_URI_TOO_LONG = 414,
    SIM7600_HTTP_CODE_UNSUPPORTED_MEDIA_TYPE = 415,
    SIM7600_HTTP_CODE_RANGE_NOT_SATISFIABLE = 416,
    SIM7600_HTTP_CODE_EXPECTATION_FAILED = 417,
    SIM7600_HTTP_CODE_MISDIRECTED_REQUEST = 421,
    SIM7600_HTTP_CODE_UNPROCESSABLE_ENTITY = 422,
    SIM7600_HTTP_CODE_LOCKED = 423,
    SIM7600_HTTP_CODE_FAILED_DEPENDENCY = 424,
    SIM7600_HTTP_CODE_UPGRADE_REQUIRED = 426,
    SIM7600_HTTP_CODE_PRECONDITION_REQUIRED = 428,
    SIM7600_HTTP_CODE_TOO_MANY_REQUESTS = 429,
    SIM7600_HTTP_CODE_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
    SIM7600_HTTP_CODE_INTERNAL_SERVER_ERROR = 500,
    SIM7600_HTTP_CODE_NOT_IMPLEMENTED = 501,
    SIM7600_HTTP_CODE_BAD_GATEWAY = 502,
    SIM7600_HTTP_CODE_SERVICE_UNAVAILABLE = 503,
    SIM7600_HTTP_CODE_GATEWAY_TIMEOUT = 504,
    SIM7600_HTTP_CODE_SIM7600_HTTP_VERSION_NOT_SUPPORTED = 505,
    SIM7600_HTTP_CODE_VARIANT_ALSO_NEGOTIATES = 506,
    SIM7600_HTTP_CODE_INSUFFICIENT_STORAGE = 507,
    SIM7600_HTTP_CODE_LOOP_DETECTED = 508,
    SIM7600_HTTP_CODE_NOT_EXTENDED = 510,
    SIM7600_HTTP_CODE_NETWORK_AUTHENTICATION_REQUIRED = 511
} t_SIM7600_HTTP_codes;

 // char u8_response[200];
 char u8_response[200];
  int chieudai;

int8_t sendATcommandpro(String ATcommand, char* expected_answer, unsigned int timeout);
String sendgetcontent(String ATcommand, char* expected_answer, unsigned int timeout);

String test4(String url, int *kq)
{
	
	String http_str = "AT+HTTPPARA=\"URL\",\"" + url +"\"\r\n";
	
	
	// Serial.println("test2");
	String response1 = "";
	int httpcode=0;
	// String http_str = "AT+HTTPPARA=\"URL\",\"https://raw.githubusercontent.com/DaikCong/Hello-World/master/hello.txt\"\r\n";
	// String http_str = "AT+HTTPPARA=\"URL\",\"https://raw.githubusercontent.com/DaikCong/Hello-World/master/welcomeDongNai.txt\"\r\n";
	
	// String http_str = "AT+HTTPPARA=\"URL\",\"http://mttcs.thuathienhue.gov.vn/media/vms-mtc/SMediaContentS/gioithieu.txt?AWSAccessKeyId=POD_IDENTITY_S3112233&Expires=1645094451&Signature=9%2F7CUekCjv56ohCbQ14KS4It8yk%3D\"\r\n";
	/*
	
-> +HTTPREAD: DATA,155
-> <?xml version='1.0' encoding='UTF-8'?><Error><Code>AccessDenied</Code><Message>Request has expired</Message><RequestId>4442587FB7D0A2F9</RequestId></Error>
-> +HTTPREAD:0

	
	*/
	
	
	sendATcommandpro("AT+HTTPINIT\r\n", "OK", 5000);
	//OK

	sendATcommandpro(http_str, "OK",2000);
	// OK

	// sendATcommandpro("AT+HTTPACTION=0\r\n", "+HTTPACTION",5000);//mode =0: get, =1:post, =2 :head, =3: delete
	// OK	
	//+HTTPACTION: 0,200,81
	//xử lý chỗ này
	
	
	if(sendATcommandpro("AT+HTTPACTION=0\r\n", "+HTTPACTION",5000))
	{
		/*	
		AT+HTTPACTION=0


		OK

		+HTTPACTION: 0,200,114
		*/

		String inString = "";

		char *ps;
		ps = strstr(u8_response, "+HTTPACTION:");
		
		if(ps != NULL)
		{			
			int dauphay = 0;			
			// tìm dấu phẩy
			for ( uint8_t j = 0; j < sizeof(u8_response); j ++)
			{
			  if (ps[j] == ',')
			  {
				dauphay = j;
				break;
			  }
			}
			if (dauphay > 0)
			{
				//get code
					dauphay++;
					for ( ; dauphay < sizeof(u8_response); dauphay ++)
				  {            
					
					if (isDigit(ps[dauphay])) {
						inString += (char)ps[dauphay];					
					}
					else
					{
						break;
					}
				  }
				  
				  httpcode=inString.toInt();
				  *kq=httpcode;
				  Serial.print("httpcode=");
				  Serial.println(httpcode);
				  
				  
				  
				  //get len
				  inString="";
				  dauphay++;
				  for ( ; dauphay < sizeof(u8_response); dauphay ++)
				  {            
					
					if (isDigit(ps[dauphay])) {
						inString += (char)ps[dauphay];					
					}
					else
					{
						break;
					}
				  }
				  
				chieudai=inString.toInt();
				Serial.print("len=");
				Serial.println(chieudai);
				
			}

		} 

	}
	
	if(httpcode!=200)
	{
		Serial.println("Erro get file");
		sendATcommandpro("AT+HTTPTERM\r\n", "OK",3000);//đóng kết nối
		return "";
	}
	
	// sendATcommandpro("AT+HTTPHEAD\r\n","+HTTPHEAD", 20000);
	
	/*
	10:57:43.220 -> +HTTPHEAD: DATA,843
10:57:43.312 -> HTTP/1.1 200 OK
10:57:43.312 -> Connection: close
10:57:43.312 -> Content-Length: 51
10:57:43.312 -> Cache-Control: max-age=300
10:57:43.312 -> Content-Security-Policy: default-src 'none'; style-src 'unsafe-inline'; sandbox
10:57:43.312 -> Content-Type: text/plain; charset=utf-8
10:57:43.312 -> ETag: "ba06920d9f9059df846b791abcafa8ad25ce0540ac1ee1ab3b6da6b855cef0ad"
10:57:43.312 -> Strict-Transport-Security: max-age=31536000
10:57:43.312 -> X-Content-Type-Options: nosniff
10:57:43.312 -> X-Frame-Options: deny
10:57:43.312 -> X-XSS-Protection: 1; mode=block
10:57:43.312 -> X-GitHub-Request-Id: 2F1A:7584:598CD:C0AEF:62203440
10:57:43.312 -> Accept-Ranges: bytes
10:57:43.312 -> Date: Thu, 03 Mar 2022 03:57:20 GMT
10:57:43.312 -> Via: 1.1 varnish
10:57:43.312 -> X-Served-By: cache-hkg17929-HKG
10:57:43.312 -> X-Cache: HIT
10:57:43.312 -> X-Cache-Hits: 1
10:57:43.312 -> X-Timer: S1646279841.691163,VS0,VE1
10:57:43.312 -> Vary: Authorization,Accept-Encoding,Origin
10:57:43.312 -> Access-Control-Allow-Origin: *
10:57:43.312 -> X-Fastly-Request-ID: ded3f93e716195004056d6fdc5a854c1279fd537
10:57:43.312 -> Expires: Thu, 03 Mar 2022 04:02:20 GMT
10:57:43.312 -> Source-Age: 150
10:57:43.312 -> 
10:57:43.312 -> 
10:57:43.312 -> OK

*/
	
	/*
	if(sendATcommandpro("AT+HTTPREAD?\r\n","+HTTPREAD", 20000))
	{
		//lấy độ dài data
		//+HTTPREAD: LEN,51
		String inString = "";

		char *ps;
		Serial.println("get len");
		ps = strstr(u8_response, "LEN,");
		
		if(ps != NULL)
		{
			for ( uint8_t j = 0; j < 4; j ++)
          {            
            
			if (isDigit(ps[j+4])) {
				inString += (char)ps[j+4];					
			}
			else
			{
				break;
			}
          }
		} 
		  
		  chieudai=inString.toInt();
		  Serial.print("len=");
		  Serial.println(chieudai);
		  
	}
	*/
	
	//+HTTPREAD: LEN,51
	//OK
	if(chieudai)
	{
		Serial.println("Get new len mode");
		String s = "AT+HTTPREAD=" + String(chieudai) +"\r\n";
		response1=sendgetcontent(s,"+HTTPREAD", 20000);
	}
	else
	{
		Serial.println("Get default len mode");
		response1=sendgetcontent("AT+HTTPREAD=51\r\n","+HTTPREAD", 20000);
	}
	//Happy New Year. Chúc Mừng Năm Mới Xuân 2022
	//+HTTPREAD:0
		
	sendATcommandpro("AT+HTTPTERM\r\n", "OK",3000);
	//OK
	return response1;
}


int8_t sendATcommandpro(String ATcommand, char*  expected_answer, unsigned int timeout) // xu ly goi lenh
{

  //in debug
  Serial.print(F("Sent2 modem: "));
  Serial.println(ATcommand);
  uint8_t x = 0;
  bool answer = 0;
  uint8_t bien =0;
  unsigned long thoatngay;
  memset(u8_response, '\0', 200);

  delay(100);
  // while ( Serial2.available() > 0) Serial2.read();
  
  // if (ATcommand[0] != '\0') {
    Serial2.println(ATcommand);
  // }
  x = 0;
  long int time = millis();

  while ((bien==0) && (time + timeout) > millis())
  {
    while (Serial2.available())
    {
      u8_response[x] = Serial2.read();
	  // Serial.print(u8_response[x]);
      x++;
      if (strstr(u8_response, expected_answer) != NULL) {
        answer = 1;
      }
      if ( x > 198)
      {
        //// Serial.print(F(" out"));
        x = 198;
        // break;
      }
	  thoatngay=millis();
    }
	
	if(answer==1)
	{
		//cho time rec data
		if(millis() > thoatngay+500)
		{
			// Serial.print(F(" out"));
			bien=1;
		}			
	}
  }
  if(answer==0)
  {
	  Serial.print(F("Time out"));
  }

  Serial.println(u8_response);
  return answer;
}

String sendgetcontent(String ATcommand, char* expected_answer, unsigned int timeout) // xu ly goi lenh
{
  String response2 = "";
  //in debug
  Serial.print(F("Sent2 modem: "));
  Serial.println(ATcommand);
  uint8_t x = 0;
  bool answer = 0;
  uint8_t bien =0;
  unsigned long thoatngay;
  memset(u8_response, '\0', 200);

  delay(100);
  // while ( Serial2.available() > 0) Serial2.read();
  
  // if (ATcommand[0] != '\0') {
    Serial2.println(ATcommand);
  // }
  x = 0;
  long int time = millis();

  while ((bien==0) && (time + timeout) > millis())
  {
    while (Serial2.available())
    {
      char c = Serial2.read();
	  u8_response[x] = c;
	  // Serial.print(u8_response[x]);
      x++;


	  if(answer==1)
	  {
		  response2 += c;		  
	  }
	  else
	  {
		if (strstr(u8_response, expected_answer) != NULL) {
			answer = 1;
		}
	  }
	  
	   if ( x > 198)
      {
        //// Serial.print(F(" out"));
        x = 198;
        // break;
      }
	  thoatngay=millis();
    }
	
	if(answer==1)
	{
		//cho time rec data
		if(millis() > thoatngay+500)
		{
			// Serial.print(F(" out"));
			bien=1;
		}			
	}
  }
  if(answer==0)
  {
	  Serial.print(F("Time out"));
  }

  // Serial.print(F("Chuoi truoc:"));
  // Serial.println(response2);
  
  /*
  response2= : DATA,114
Chào Mừng Bạn Đến Sở Thông Tin Và Truyền Thông Tỉnh Đồng Nai
Chúc Bạn Ngày Mới Vui Vẻ

+HTTPREAD:0
*/
  
  int index = response2.indexOf('\n');
  
  String the_host;
  String content;
  if(index >= 0) {
		the_host = response2.substring(index, response2.length());        
		the_host.trim(); // remove \r
		index = the_host.indexOf("+HTTPREAD");
		content = the_host.substring(0, index);
		content.trim(); // remove \r
  }		
  return content;
}
#endif /* HTTPClient_H_ */
