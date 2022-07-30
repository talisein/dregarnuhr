#include <sstream>
#include "curl.h"

void CURLEscapeDeleter::operator()(char* str) const
{
    curl_free(str);
}

void CURLEasyDeleter::operator()(CURL* curl) const
{
    curl_easy_cleanup(curl);
}

void CURLShareDeleter::operator()(CURLSH* share) const
{
    CURLSHcode code = curl_share_cleanup(share);
    if (code != CURLSHE_OK) [[unlikely]] {
        std::clog << "Failed to clean curl share: %s"
                  << curl_share_strerror(code) << '\n';
    }
}

void CURLSListDeleter::operator()(struct curl_slist* list) const
{
    curl_slist_free_all(list);
}

CURLcode curl::perform()
{
    CURLcode res = curl_easy_perform(_p.get());
    if (CURLE_OK != res) [[unlikely]] {
        const char *estr = curl_easy_strerror(res);
        std::clog << "Error performing http request ";

        long curl_errno {0};
        get_info(CURLINFO_OS_ERRNO, &curl_errno);
        char *url {nullptr};
        get_info(CURLINFO_EFFECTIVE_URL, &url);
        long response_code {0};
        get_info(CURLINFO_RESPONSE_CODE, &response_code);
        char *redirect_url {nullptr};
        get_info(CURLINFO_REDIRECT_URL, &redirect_url);
        curl_off_t uploaded {0};
        get_info(CURLINFO_SIZE_UPLOAD_T, &uploaded);
        curl_off_t downloaded {0};
        get_info(CURLINFO_SIZE_DOWNLOAD_T, &downloaded);
        long ssl_verify_result {0};
        get_info(CURLINFO_SSL_VERIFYRESULT, &ssl_verify_result);
        long num_connects {0};
        get_info(CURLINFO_NUM_CONNECTS, &num_connects);
        char *ip {nullptr};
        get_info(CURLINFO_PRIMARY_IP, &ip);
        long port {0};
        get_info(CURLINFO_PRIMARY_PORT, &port);
        double total_time {0.0};
        get_info(CURLINFO_TOTAL_TIME, &total_time);

        std::clog << "(" << response_code << "): " << estr << '\n';
    } else {
        char *url {nullptr};
        get_info(CURLINFO_EFFECTIVE_URL, &url);
        long response_code {0};
        get_info(CURLINFO_RESPONSE_CODE, &response_code);
        char *redirect_url {nullptr};
        get_info(CURLINFO_REDIRECT_URL, &redirect_url);
        curl_off_t uploaded {0};
        get_info(CURLINFO_SIZE_UPLOAD_T, &uploaded);
        curl_off_t downloaded {0};
        get_info(CURLINFO_SIZE_DOWNLOAD_T, &downloaded);
        long ssl_verify_result {0};
        get_info(CURLINFO_SSL_VERIFYRESULT, &ssl_verify_result);
        long num_connects {0};
        get_info(CURLINFO_NUM_CONNECTS, &num_connects);
        char *ip {nullptr};
        get_info(CURLINFO_PRIMARY_IP, &ip);
        long port {0};
        get_info(CURLINFO_PRIMARY_PORT, &port);
        double total_time {0.0};
        get_info(CURLINFO_TOTAL_TIME, &total_time);
        std::clog << "\nCurl success. " << response_code << " " << url << " "
           << downloaded << "B down " << uploaded << "B up in "
           << total_time << "s\n";
    }

    return res;
}
