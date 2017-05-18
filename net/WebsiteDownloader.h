#pragma once

#include <cstdlib>
#include <string>
#include <curl/curl.h>

class WebsiteDownloader {
public:
    static size_t write_function(void* ptr, size_t size, size_t nmemb, std::string* s/*void* data*/);
    static std::string get_website(std::string url);
};