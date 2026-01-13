#include "Globalvariables_DANH.h"
#include <WiFiClientSecure.h>


const char* test_root_ca= \
    // "-----BEGIN CERTIFICATE-----\n" \
    // "MIIEozCCBEmgAwIBAgIQTij3hrZsGjuULNLEDrdCpTAKBggqhkjOPQQDAjCBjzEL\n" \
    // "MAkGA1UEBhMCR0IxGzAZBgNVBAgTEkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4GA1UE\n" \
    // "BxMHU2FsZm9yZDEYMBYGA1UEChMPU2VjdGlnbyBMaW1pdGVkMTcwNQYDVQQDEy5T\n" \
    // "ZWN0aWdvIEVDQyBEb21haW4gVmFsaWRhdGlvbiBTZWN1cmUgU2VydmVyIENBMB4X\n" \
    // "DTI0MDMwNzAwMDAwMFoXDTI1MDMwNzIzNTk1OVowFTETMBEGA1UEAxMKZ2l0aHVi\n" \
    // "LmNvbTBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABARO/Ho9XdkY1qh9mAgjOUkW\n" \
    // "mXTb05jgRulKciMVBuKB3ZHexvCdyoiCRHEMBfFXoZhWkQVMogNLo/lW215X3pGj\n" \
    // "ggL+MIIC+jAfBgNVHSMEGDAWgBT2hQo7EYbhBH0Oqgss0u7MZHt7rjAdBgNVHQ4E\n" \
    // "FgQUO2g/NDr1RzTK76ZOPZq9Xm56zJ8wDgYDVR0PAQH/BAQDAgeAMAwGA1UdEwEB\n" \
    // "/wQCMAAwHQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMEkGA1UdIARCMEAw\n" \
    // "NAYLKwYBBAGyMQECAgcwJTAjBggrBgEFBQcCARYXaHR0cHM6Ly9zZWN0aWdvLmNv\n" \
    // "bS9DUFMwCAYGZ4EMAQIBMIGEBggrBgEFBQcBAQR4MHYwTwYIKwYBBQUHMAKGQ2h0\n" \
    // "dHA6Ly9jcnQuc2VjdGlnby5jb20vU2VjdGlnb0VDQ0RvbWFpblZhbGlkYXRpb25T\n" \
    // "ZWN1cmVTZXJ2ZXJDQS5jcnQwIwYIKwYBBQUHMAGGF2h0dHA6Ly9vY3NwLnNlY3Rp\n" \
    // "Z28uY29tMIIBgAYKKwYBBAHWeQIEAgSCAXAEggFsAWoAdwDPEVbu1S58r/OHW9lp\n" \
    // "LpvpGnFnSrAX7KwB0lt3zsw7CAAAAY4WOvAZAAAEAwBIMEYCIQD7oNz/2oO8VGaW\n" \
    // "WrqrsBQBzQH0hRhMLm11oeMpg1fNawIhAKWc0q7Z+mxDVYV/6ov7f/i0H/aAcHSC\n" \
    // "Ii/QJcECraOpAHYAouMK5EXvva2bfjjtR2d3U9eCW4SU1yteGyzEuVCkR+cAAAGO\n" \
    // "Fjrv+AAABAMARzBFAiEAyupEIVAMk0c8BVVpF0QbisfoEwy5xJQKQOe8EvMU4W8C\n" \
    // "IGAIIuzjxBFlHpkqcsa7UZy24y/B6xZnktUw/Ne5q5hCAHcATnWjJ1yaEMM4W2zU\n" \
    // "3z9S6x3w4I4bjWnAsfpksWKaOd8AAAGOFjrv9wAABAMASDBGAiEA+8OvQzpgRf31\n" \
    // "uLBsCE8ktCUfvsiRT7zWSqeXliA09TUCIQDcB7Xn97aEDMBKXIbdm5KZ9GjvRyoF\n" \
    // "9skD5/4GneoMWzAlBgNVHREEHjAcggpnaXRodWIuY29tgg53d3cuZ2l0aHViLmNv\n" \
    // "bTAKBggqhkjOPQQDAgNIADBFAiEAru2McPr0eNwcWNuDEY0a/rGzXRfRrm+6XfZe\n" \
    // "SzhYZewCIBq4TUEBCgapv7xvAtRKdVdi/b4m36Uyej1ggyJsiesA\n" \
    // "-----END CERTIFICATE-----\n";


    // "-----BEGIN CERTIFICATE-----\n" \
    // "MIICjzCCAhWgAwIBAgIQXIuZxVqUxdJxVt7NiYDMJjAKBggqhkjOPQQDAzCBiDEL\n" \
    // "MAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNl\n" \
    // "eSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMT\n" \
    // "JVVTRVJUcnVzdCBFQ0MgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAwMjAx\n" \
    // "MDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBiDELMAkGA1UEBhMCVVMxEzARBgNVBAgT\n" \
    // "Ck5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNleSBDaXR5MR4wHAYDVQQKExVUaGUg\n" \
    // "VVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMTJVVTRVJUcnVzdCBFQ0MgQ2VydGlm\n" \
    // "aWNhdGlvbiBBdXRob3JpdHkwdjAQBgcqhkjOPQIBBgUrgQQAIgNiAAQarFRaqflo\n" \
    // "I+d61SRvU8Za2EurxtW20eZzca7dnNYMYf3boIkDuAUU7FfO7l0/4iGzzvfUinng\n" \
    // "o4N+LZfQYcTxmdwlkWOrfzCjtHDix6EznPO/LlxTsV+zfTJ/ijTjeXmjQjBAMB0G\n" \
    // "A1UdDgQWBBQ64QmG1M8ZwpZ2dEl23OA1xmNjmjAOBgNVHQ8BAf8EBAMCAQYwDwYD\n" \
    // "VR0TAQH/BAUwAwEB/zAKBggqhkjOPQQDAwNoADBlAjA2Z6EWCNzklwBBHU6+4WMB\n" \
    // "zzuqQhFkoJ2UOQIReVx7Hfpkue4WQrO/isIJxOzksU0CMQDpKmFHjFJKS04YcPbW\n" \
    // "RNZu9YO6bVi9JNlWSOrvxKJGgYhqOkbRqZtNyWHa0V1Xahg=\n" \
    // "-----END CERTIFICATE-----\n";

"-----BEGIN CERTIFICATE-----\n" \
"MIID2zCCAsOgAwIBAgIQNsnNBwiIC0GIt72zzGAxGjANBgkqhkiG9w0BAQsFADCB\n" \
"gTE6MDgGA1UECwwxZ2VuZXJhdGVkIGJ5IEF2YXN0IEFudGl2aXJ1cyBmb3IgU1NM\n" \
"L1RMUyBzY2FubmluZzEeMBwGA1UECgwVQXZhc3QgV2ViL01haWwgU2hpZWxkMSMw\n" \
"IQYDVQQDDBpBdmFzdCBXZWIvTWFpbCBTaGllbGQgUm9vdDAeFw0yNjAxMDUxNTM0\n" \
"MjhaFw0yNjA0MDUxNTM0MjdaMCYxJDAiBgNVBAMTG3Ftcy1hcGkuZGV2LmFsdGFz\n" \
"b2Z0d2FyZS52bjCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMS2vVYe\n" \
"b4/8WRnvLf8BfSHTKk7ZUo1OkJs2gn1GzNN8gnbcbzZ9Sqq4qoIcyD4FKHf15GWQ\n" \
"XmvrMoT23V9UpI/F9qFsiLOB+8mieaOqtLNzNJ2zJiOpHBO4SaXUnP80wugkczTE\n" \
"C1xf8h34/IOAKRdwqXMeuqIsvQueFvzq3+qtmVB2C/yHaocLoAWIDJBJJJLG+akh\n" \
"UZn6227bjfNNjHVQZ+ICptd+ZEKu7NuYd6ogOpVWn2D9sUVXHXJjrIe8wORVh9Fh\n" \
"JkuepvhSscynkNWe2JF4Y1Whvq2upPJG7xNpKiFpQcxvGAfoZkj21W+dHWCCH0mv\n" \
"DrKsngaAwE7/upsCAwEAAaOBqDCBpTAOBgNVHQ8BAf8EBAMCBaAwHQYDVR0lBBYw\n" \
"FAYIKwYBBQUHAwEGCCsGAQUFBwMCMAwGA1UdEwEB/wQCMAAwHQYDVR0OBBYEFLyo\n" \
"11mLYUvW+xaGW6hMd7wN4ts0MB8GA1UdIwQYMBaAFMk1JesQy8Ns/t8+bL7DsIVz\n" \
"rLfWMCYGA1UdEQQfMB2CG3Ftcy1hcGkuZGV2LmFsdGFzb2Z0d2FyZS52bjANBgkq\n" \
"hkiG9w0BAQsFAAOCAQEAiwm5ZTMrDRO91tCav+1SsG8QN4xaNSxyNPju2hrYuz/E\n" \
"2P3UIYOtW5VpX4f7sYrFXhW6QLSBuAkOoIfzqA8TTzfxNiQ9WZdMSLkUBlcmqXgw\n" \
"lp5WYuWvpFszilKkp/x8cReRtaGmtYyHEpfrPoJVpCA/v5Z05gjP05aVstyQr0rR\n" \
"20cWXCVx3XdBY4gsjUd8fEMJj4VRedN3VNERcOc3o1yHNrfg/nMoCJwEb2v5Xh7A\n" \
"SZVz5AMfhQdRhDKOIejyUFGUjomPW4PGrA9disyla2TMcdxq6zfIuJ6/dWEc90UI\n" \
"uc74KujxY3hGSjXfAUUJbCNFfG3C1hWHTg9gXQNp8w==\n" \
"-----END CERTIFICATE-----\n";



// WiFiClientSecure sclient;

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.println("Connecting to WiFi...");
  WiFi.disconnect(true);
  delay(500);

  WiFi.begin(info_wifi.ssid, info_wifi.pass);

  unsigned long startAttemptTime = millis();
  const unsigned long timeout = 15000; // 15s timeout

  while (WiFi.status() != WL_CONNECTED &&
         millis() - startAttemptTime < timeout) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi connect FAILED");
  }
}

void reconnect()
{
	if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi lost. Reconnecting...");
    connectWiFi();
	}

	// code khác (HTTP, MQTT, ...)
	delay(1000);
}

void truycap_https(WiFiClientSecure &client, const char* test_root_ca, String host, int port, String bin)
{

    client.setCACert(test_root_ca);
    Serial.println("\nStarting connection to server...");

    if(!client.connect(host.c_str(), port)) {
        Serial.println("Connection failed!");
        return;
    }

    else 
    {
        Serial.println("CONNECT THANH CONG");
        Serial.println("Fetching Bin: " + String(bin)); 

        client.print(String("GET ") + bin + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Cache-Control: no-cache\r\n" +
                 "Connection: close\r\n\r\n");

        int contentLength=-1;
        String payload="";
        while (client.connected()) 
        {
            String line = client.readStringUntil('\n');
            if (line.startsWith(F("Content-Length: ")))
            {
                contentLength = line.substring(15).toInt();
                Serial.print("contentLength: ");
                Serial.println(contentLength);
            }
            if (line == "\r") 
            {
                Serial.println("headers received");
                break;
            } // than tru 
        }
    }
}

void CreateDeviceCode_simple(WiFiClientSecure &client, const char* host, int port)
{
  const char* path = "/api/Equipments/Auth/DeviceCode";

  client.setInsecure(); // TEST nhanh (prod thì dùng setCACert + syncTime)

  Serial.println("Connecting HTTPS...");
  if (!client.connect(host, port)) {
    Serial.println("HTTPS connect FAIL");
    return;
  }
  Serial.println("HTTPS connected");

  // 1) publicKey PEM (newline thật)
  String publicKey =
    "-----BEGIN PUBLIC KEY-----\n"
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAv...\n"
    "-----END PUBLIC KEY-----\n";

  // 2) Đổi newline thật -> \n trong JSON (GIỐNG curl)
  publicKey.replace("\n", "\\n");

 // JSON body KHÔNG publicKey
  String body =
    "{"
      "\"longitude\":106.700981,"
      "\"latitude\":10.77653,"
      "\"equipmentName\":\"Kiosk-FrontDesk-00\""
    "}";

  // 4) Gửi request
  client.print(String("POST ") + path + " HTTP/1.1\r\n" +
               "Host: " + String(host) + "\r\n" +
               "Content-Type: application/json\r\n" +
               "Content-Length: " + String(body.length()) + "\r\n" +
               "Connection: close\r\n\r\n" +
               body);

  // 5) In toàn bộ response (status + headers + body)
  while (client.connected() || client.available()) {
    if (client.available()) Serial.write(client.read());
  }

  client.stop();
  Serial.println("\nDONE");
}

