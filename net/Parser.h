#pragma once

#include <set>
#include <regex>

class Parser {
public:
    static std::set<std::string> parse_links(std::string website_data);
};