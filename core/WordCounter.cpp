#include <regex>
#include <ncurses.h>
#include "WordCounter.h"

std::map<std::string, int> WordCounter::count_words(std::string website_data) {
    std::map<std::string, int> words;
    std::regex word_regex("<[^<>]+>\\s*([A-Za-z]{1,10})\\s*</[^<>/]+>");

    for(std::sregex_iterator it(website_data.begin(), website_data.end(), word_regex), it_end; it != it_end; ++it) {
        words[(*it)[1].str()]++;
    }

    return words;
}

void WordCounter::update_crawler_state(CrawlerState* state, std::string website_data) {
    std::map<std::string, int> words = count_words(website_data);

    for(auto it = words.begin(); it != words.end(); ++it) {
        state->update_top_words(it->first, it->second);
    }
}



