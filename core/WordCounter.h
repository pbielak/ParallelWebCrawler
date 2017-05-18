#pragma once

#include <map>
#include <sstream>
#include "CrawlerState.h"

class WordCounter {
private:
    static std::map<std::string, int> count_words(std::string website_data);

public:
    static void update_crawler_state(CrawlerState* state, std::string website_data);
};