/*
    Tuan Nguyen (http://www.iotsharing.com) or (nha.tuan84@gmail.com)
*/

#ifndef UDHTTP_H
#define UDHTTP_H

#include <functional>
#include <WiFi.h>
#include <SSLClient.h>

#define HEADER_SIZE	500
#define CHUNK_SIZE	100
#define HOST_LEN    200
//callback that will be invoked whenever data is available
typedef int (*DataCb)(uint8_t *buffer, int len);
typedef void (*ProgressCb)(int percent);

class UDHttp
{
    private:
	void sendChunk(SSLClient& client, uint8_t *buf, int len);
        int simpleUrlParser(char *url, char *host, int &port);
    public: 
        UDHttp();
        ~UDHttp();
        int upload(SSLClient& client, char *uploadUrlHandler, char *fileName, int sizeOfFile, DataCb dataCb, ProgressCb progressCb, DataCb responseCb);
        int download(SSLClient& client,char *downloadUrl, char *downloadFile, DataCb dataCb, ProgressCb progressCb);
};

#endif
