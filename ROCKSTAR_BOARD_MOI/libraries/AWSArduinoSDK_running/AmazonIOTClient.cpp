#include "AmazonIOTClient.h"
#include "AWSFoundationalTypes.h"
#include <stdlib.h>
#include "Utils.h"

AmazonIOTClient::AmazonIOTClient() : AWSClient4() {
    this->awsService = "s3";
    this->contentType = "application/json"; /* text/plain multipart/form-data*/
    this->signedHeaders = "content-length;content-type;host;x-amz-content-sha256;x-amz-date";
    this->uri = "/";
    this->queryString = "";
    this->httpS = true;
}

const char* AmazonIOTClient::update_shadow(MinimalString shadow, ActionError& actionError) {
    actionError = NONE_ACTIONERROR;

    this->method = "PUT";
    char* request = createRequest(shadow);

    return request;
    const char* response = sendData(request);

    return response;
}
const char* AmazonIOTClient::get_timeshadow() {
    
    const char* request = mrCongRequestTime();
    return request;
}

const char* AmazonIOTClient::get_shadow(MinimalString shadow, ActionError& actionError) {
    actionError = NONE_ACTIONERROR;

    this->method = "GET";
	shadow = "";
    char* request = createRequest(shadow);
    // return request;
    const char* response = sendData(request);
    return response;
}

