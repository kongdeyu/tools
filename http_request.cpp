 
#include "http_request.h"

namespace tool
{
bool HttpRequest::_s_has_init = false;

HttpRequest::HttpRequest() : _curl(NULL)
{
    if(!_s_has_init)
    {
        curl_global_init(CURL_GLOBAL_ALL);
        _s_has_init = true;
    }
    _linger.l_onoff = 1;
    _inger.l_linger = 0;
    _resp.reset();
    _error[0] = 0;
}

HttpRequest::~HttpRequest()
{
    if(NULL != _curl)
    {
        curl_easy_cleanup(_curl);
        _curl = NULL;
    }
    if(_s_has_init)
    {
        curl_global_cleanup();
        _s_has_init = false;
    }
}

bool HttpRequest::request(const std::string &url,
        HTTPMethodTYPE method,
        const std::string &data,
        int32_t timeout)
{
    if(NULL != _curl)
    {
        curl_easy_cleanup(_curl);
        _curl = NULL;
    }

    // init
    _curl = curl_easy_init();
    if(NULL == _curl)
    {
        return false;
    }

    _resp.reset();
    _error[0] = 0;
    if(CURLE_OK != curl_easy_setopt(_curl, CURLOPT_TIMEOUT_MS, timeout)
            || CURLE_OK != curl_easy_setopt(_curl, CURLOPT_NOSIGNAL, 1)
            || CURLE_OK != curl_easy_setopt(_curl, CURLOPT_FORBID_REUSE, 1)
            || CURLE_OK != curl_easy_setopt(_curl, CURLOPT_ERRORBUFFER, _error)
            || CURLE_OK != curl_easy_setopt(_curl, CURLOPT_WRITEDATA, (void *)(&_resp))
            || CURLE_OK != curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, write_callback)
            || CURLE_OK != curl_easy_setopt(_curl, CURLOPT_SOCKOPTDATA, (void *)(&_linger))
            || CURLE_OK != curl_easy_setopt(_curl, CURLOPT_SOCKOPTFUNCTION, socket_callback))
    {
        return false;
    }

    // request
    if(CURLE_OK != curl_easy_setopt(_curl, CURLOPT_URL, url.c_str()))
    {
        return false;
    }

    if(HTTP_GET == method)
    {
        if(CURLE_OK != curl_easy_setopt(_curl, CURLOPT_HTTPGET, 1))
        {
            return false;
        }
    }
    if(HTTP_POST == method)
    {
        if(CURLE_OK != curl_easy_setopt(_curl, CURLOPT_POST, 1)
                || CURLE_OK != curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, data.c_str()))
        {
            return false;
        }
    }
    else  // invalid request type
    {
        return false
    }

    long retcode;
    if(CURLE_OK != curl_easy_perform(_curl)
            || CURLE_OK != curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &retcode)
            || 200 != retcode)
    {
        return false;
    }
    return true;
}

std::string HttpRequest::response()
{
    return std::string(_resp.buf, _resp.length);
}

std::string HttpRequest::error()
{
    return std::string(_error);
}

size_t HttpRequest::write_callback(char *ptr,
        size_t size,
        size_t nmemb,
        void *userdata)
{
    size_t bytes_n = size * nmemb;
    HttpWriteData *p_resp = (HttpWriteData*)userdata;
    if(NULL == p_resp || p_resp->length + bytes_n > _S_HTTP_MAX_RESPONSE_BYTES)
    {
        p_resp->error = 1;
        return 0;
    }
    memcpy(p_resp->buf + p_resp->length, ptr, bytes_n);
    p_resp->length += bytes_n;
    return bytes_n;
}

int32_t HttpRequest::socket_callback(void *clientp,
        curl_socket_t curlfd,
        curlsocktype)
{
    setsockopt(curlfd, SOL_SOCKET, SO_LINGER, clientp, sizeof(linger));
    return CURLE_OK;
}
}
