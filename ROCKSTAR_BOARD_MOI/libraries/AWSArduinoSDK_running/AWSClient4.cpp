/*
 * AWSClient4.cpp
 *
 *  See AWSClient4.h for description.
 *  See http://docs.aws.amazon.com/AmazonS3/latest/API/sig-v4-header-based-auth.html
 *
 */

#include "AWSClient4.h"
#include "Utils.h"
#include "DeviceIndependentInterfaces.h"
#include "AWSFoundationalTypes.h"
#include "sha256.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <arduino.h>

AWSClient4::AWSClient4()
{
    /* Null until set in init method or derived class function. */
    awsRegion = 0;
    awsEndpoint = 0;
    awsSecKey = 0;
    awsKeyID = 0;
    httpClient = 0;
    dateTimeProvider = 0;
    method = 0;
    uri = 0;
    payload = "";
}

void
AWSClient4::setAWSRegion(const char * awsRegion)
{
    int len = strlen(awsRegion) + 1;
    this->awsRegion = new char[len]();
    strcpy(this->awsRegion, awsRegion);
}

void
AWSClient4::setAWSEndpoint(const char * awsEndpoint)
{
    int len = strlen(awsEndpoint) + 1;
    this->awsEndpoint = new char[len]();
    strcpy(this->awsEndpoint, awsEndpoint);
}

void
AWSClient4::setAWSDomain(const char * awsDomain)
{
    int len = strlen(awsDomain) + 1;
    this->awsDomain = new char[len]();
    strcpy(this->awsDomain, awsDomain);
}

void
AWSClient4::setAWSPath(const char * awsPath)
{
    int len = strlen(awsPath) + 1;
    this->awsPath = new char[len]();
    strcpy(this->awsPath, awsPath);
}

void
AWSClient4::setAWSSecretKey(const char * awsSecKey)
{
    int len = strlen(awsSecKey) + 1;
    this->awsSecKey = new char[len]();
    strcpy(this->awsSecKey, awsSecKey);
}

void
AWSClient4::setAWSKeyID(const char * awsKeyID)
{
    int len = strlen(awsKeyID) + 1;
    this->awsKeyID = new char[len]();
    strcpy(this->awsKeyID, awsKeyID);
}

void
AWSClient4::setHttpClient(IHttpClient* httpClient)
{
    this->httpClient = httpClient;
}

void
AWSClient4::setDateTimeProvider(IDateTimeProvider* dateTimeProvider)
{
    this->dateTimeProvider = dateTimeProvider;
}

AWSClient4::~AWSClient4()
{
    if (awsRegion != 0)
        delete[] awsRegion;
    if (awsEndpoint != 0)
        delete[] awsEndpoint;
    if (awsSecKey != 0)
        delete[] awsSecKey;
    if (awsKeyID != 0)
        delete[] awsKeyID;
    if (awsDomain != 0)
        delete[] awsDomain;
    if (awsPath != 0)
        delete[] awsPath;
}


void
AWSClient4::createCanonicalHeaders()
{
  // headers, alphabetically sorted, lowercase, eg: key:value
  // content-length:l
  // content-type:x
  // host:host
  // x-amz-content-sha256:hash
  // x-amz-date:date
  canonical_headers[0] = 0;
  sprintf(canonical_headers, "%scontent-length:%d\n", canonical_headers,
          strlen(payload.getCStr()));
  sprintf(canonical_headers, "%scontent-type:%s\n", canonical_headers,
          contentType);
  sprintf(canonical_headers, "%shost:%s\n", canonical_headers, awsDomain);
  // sprintf(canonical_headers, "%srange:bytes=0-9\n", canonical_headers); // s3
  sprintf(canonical_headers, "%sx-amz-content-sha256:%s\n",
          canonical_headers, payloadHash);
  sprintf(canonical_headers, "%sx-amz-date:%sT%sZ\n\n", canonical_headers,
          awsDate, awsTime);
  return;
}

void
AWSClient4::createRequestHeaders(char* signature)
{
  headers[0] = 0;
  sprintf(headers, "%sContent-Type: %s\r\n", headers, contentType);
  sprintf(headers, "%sContent-Length: %d\r\n", headers,
          strlen(payload.getCStr()));
  sprintf(headers, "%sHost: %s\r\n", headers, awsDomain);
  sprintf(headers, "%sX-Amz-Content-Sha256: %s\r\n", headers, payloadHash);
  sprintf(headers, "%sX-Amz-Date: %sT%sZ\r\n", headers, awsDate, awsTime);
  sprintf(headers, "%sAuthorization: AWS4-HMAC-SHA256 Credential=%s/%s/%s/%s/"
                   "aws4_request, SignedHeaders=%s, Signature=%s\r\n", headers,
          awsKeyID, awsDate, awsRegion, awsService, signedHeaders, signature);
  return;
}

void
AWSClient4::createStringToSign(char* canonical_request)
{
  SHA256* sha256 = new SHA256();
  char* hashed = (*sha256)(canonical_request, strlen(canonical_request));
  delete sha256;

  string_to_sign[0] = 0;
  sprintf(string_to_sign, "%sAWS4-HMAC-SHA256\n", string_to_sign);
  sprintf(string_to_sign, "%s%sT%sZ\n", string_to_sign, awsDate, awsTime);
  sprintf(string_to_sign, "%s%s/%s/%s/aws4_request\n", string_to_sign,
          awsDate, awsRegion, awsService);

  sprintf(string_to_sign, "%s%s", string_to_sign, hashed);

  return;
}

void
AWSClient4::createCanonicalRequest()
{
  canonical_request[0] = 0;
  sprintf(canonical_request, "%s%s\n", canonical_request, method); // VERB
  sprintf(canonical_request, "%s%s\n", canonical_request, awsPath); // URI
  sprintf(canonical_request, "%s%s\n", canonical_request,
                                              queryString); // queryString

  createCanonicalHeaders();

  sprintf(canonical_request, "%s%s", canonical_request, canonical_headers);
                                                      // headers
  sprintf(canonical_request, "%s%s\n", canonical_request, signedHeaders);
                                                      // signed_headers
  sprintf(canonical_request, "%s%s", canonical_request, payloadHash);
                                                      // payload

  return;
}


char*
AWSClient4::createSignature(const char* toSign)
{
    /* Allocate memory for the signature */
    char* signature = new char[HASH_HEX_LEN4 + 1]();

    /* Create the signature key */
    /* + 4 for "AWS4" */
    int keyLen = strlen(awsSecKey) + 4;
    char* key = new char[keyLen + 1]();
    sprintf(key, "AWS4%s", awsSecKey);

    /* repeatedly apply hmac with the appropriate values. See
     * http://docs.aws.amazon.com/general/latest/gr/sigv4-calculate-signature.html
     * for algorithm. */
    char* k1 = hmacSha256(key, keyLen, awsDate, strlen(awsDate));
    delete[] key;
    char* k2 = hmacSha256(k1, SHA256_DEC_HASH_LEN, awsRegion,
            strlen(awsRegion));
    delete[] k1;
    char* k3 = hmacSha256(k2, SHA256_DEC_HASH_LEN, awsService,
            strlen(awsService));
    delete[] k2;
    char* k4 = hmacSha256(k3, SHA256_DEC_HASH_LEN, "aws4_request", 12);
    delete[] k3;
    char* k5 = hmacSha256(k4, SHA256_DEC_HASH_LEN, toSign, strlen(toSign));
    delete[] k4;

    /* Convert the chars in hash to hex for signature. */
    for (int i = 0; i < SHA256_DEC_HASH_LEN; ++i) {
        sprintf(signature + 2 * i, "%02lx", 0xff & (unsigned long) k5[i]);
    }
    delete[] k5;
    return signature;
}


char*
AWSClient4::createRequest(MinimalString &reqPayload)
{    
    Serial.println(F("createRequest"));
    
    /* Check that all values have been initialized. */
    if (awsRegion == 0 || awsEndpoint == 0 || awsSecKey == 0 || awsKeyID == 0
            || httpClient == 0 || dateTimeProvider == 0)
        return 0;

    // set date and time
    // @TODO: find out why sprintf doesn't work
    const char* dateTime = dateTimeProvider->getDateTime();

    Serial.print(F(" dateTime = "));
    Serial.println(dateTime);

    strncpy(awsDate, dateTime, 8);
    awsDate[AWS_DATE_LEN4] = '\0';
    strncpy(awsTime, dateTime + 8, 6);
    awsTime[AWS_TIME_LEN4] = '\0';

    SHA256* sha256 = new SHA256();
    payloadHash = (*sha256)(reqPayload.getCStr(), reqPayload.length());
    delete sha256;
    
    Serial.print(F(" 248"));
    
    payload = reqPayload;
    Serial.print(F(" 251"));
    // create the canonical request
    createCanonicalRequest();
    Serial.print(F(" 254"));
    // create the signing string
    createStringToSign(canonical_request);
    Serial.print(F(" 257"));
    // create the signature
    char *signature = createSignature(string_to_sign);

    // create the headers
    createRequestHeaders(signature);
    delete signature;
    Serial.println(F(" 264"));
    // get the host/domain
    // char *host = createHost();

    // create the request with all the vars
    uint16_t kichthuoc = strlen(method) +
                             strlen(awsPath) + strlen(headers) +
                             strlen(reqPayload.getCStr()) + 20;

    Serial.print(F(" kichthuoc = "));
    Serial.println(kichthuoc);
    char* request = new char[kichthuoc]();
/*
    char* request = new char[strlen(method) + strlen(awsDomain) +
                             strlen(awsPath) + strlen(headers) +
                             strlen(reqPayload.getCStr()) + 16]();//16
  */                           
    sprintf(request, "%s %s HTTP/1.1\r\n%s\r\n%s\r\n\r\n", method, awsPath,
            headers, reqPayload.getCStr());

    Serial.println(F("end createRequest"));
    return request;
}

const char*
AWSClient4::mrCongRequestTime()
{    
    Serial.println(F("RequestTime"));
    
    /* Check that all values have been initialized. */
    if (awsRegion == 0 || awsEndpoint == 0 || awsSecKey == 0 || awsKeyID == 0
            || httpClient == 0 || dateTimeProvider == 0)
        return 0;

    // set date and time
    // @TODO: find out why sprintf doesn't work
    const char* dateTime = dateTimeProvider->getDateTime();

    Serial.print(F(" dateTime = "));
    Serial.println(dateTime);
    Serial.println(F("end createRequest"));
    return dateTime;
}
const char*
AWSClient4::sendData(const char* data)
{
    Serial.println(F("sendData"));
    // char* server = createHost();
    int port = httpS ? 443 : 80;
    
    // Serial.println("path");
    // int port = 443;
    const char* response = httpClient->send(data, awsDomain, port);
    // printf("line 331");
    // delete[] server;
    Serial.println(F("end sendData"));
    return response;
}
