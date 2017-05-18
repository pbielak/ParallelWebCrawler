#include "WebsiteDownloader.h"

size_t WebsiteDownloader::write_function(void* ptr, size_t size, size_t nmemb, void* data) {
    std::string* str = (std::string*) data;
    char* sptr = (char*) ptr;

    for(int i = 0; i < size * nmemb; ++i) {
        (*str) += sptr[i];
    }

    return size * nmemb;
}

std::string WebsiteDownloader::get_website(std::string url) {
    std::string website_data;

    CURL* curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_function);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &website_data);

    CURLcode response = curl_easy_perform(curl_handle);

    if(response != CURLE_OK) {
        exit(-1);
    }

    curl_easy_cleanup(curl_handle);

    return website_data;
}