#include "AmazonIOTClient.h"
#include "AWSFoundationalTypes.h"
#include <stdlib.h>
#include "Utils.h"

AmazonIOTClient::AmazonIOTClient() : AWSClient4() {
    this->awsService = "s3";
    this->contentType = "application/json";
    // this->contentType = "binary/octet-stream";
    this->signedHeaders = "content-length;content-type;host;x-amz-content-sha256;x-amz-date";
    this->uri = "/";
    this->queryString = "";
    this->httpS = true;
}

const char* AmazonIOTClient::update_shadow(MinimalString shadow, ActionError& actionError) {
    actionError = NONE_ACTIONERROR;

    this->method = "PUT";
    char* request = createRequest(shadow);
    const char* response = sendData(request);

    return response;
}
const char* AmazonIOTClient::update_shadow_kichthuoc(MinimalString shadow, ActionError& actionError, int kichthuoc) {
    actionError = NONE_ACTIONERROR;

    this->method = "PUT";
    char* request = createRequest_kichthuoc(shadow, kichthuoc);
    /* thiếu payload và \n\r\n\r*/
    const char* response = sendData_kichthuoc(request, request, kichthuoc);

    return response;
}

const char* AmazonIOTClient::get_shadow(ActionError& actionError) {
    actionError = NONE_ACTIONERROR;

    this->method = "GET";
	MinimalString shadow = "";
    char* request = createRequest(shadow);
    // return request;
    const char* response = sendData(request);
    return response;
}
