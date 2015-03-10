
#pragma once

#include <string>
#include <curl/curl.h>

namespace tool
{
class HttpRequest
{
public:
    enum HTTPMethodTYPE
    {
        HTTP_GET,
        HTTP_POST
    };

    HttpRequest();
    ~HttpRequest();
    bool request(const std::string &url,
            HTTPMethodTYPE method = HTTP_GET,
            const std::string &data = "",
            int32_t timeout = 2000);
    std::string response();
    std::string error();

private:
    const static int32_t _S_HTTP_MAX_RESPONSE_BYTES = 10240;

    struct HttpWriteData
    {
        char buf[_S_HTTP_MAX_RESPONSE_BYTES];  // not end with '\0', use count
        uint32_t length;
        int32_t error;
        void reset()
        {
            length = 0;
            error = 0;
        }
    };

    static size_t write_callback(char *ptr,
            size_t size,
            size_t nmemb,
            void *userdata);
    static int32_t sockopt_callback(void *clientp,
            curl_socket_t curlfd,
            curlsocktype purpose);

    static bool _s_has_init;  // make sure curl_global_init() is only called once
    linger _linger;
    CURL *_curl;
    HttpWriteData _resp;
    char _error[CURL_ERROR_SIZE];
};
}
