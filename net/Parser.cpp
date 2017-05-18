#include "Parser.h"

std::set<std::string> Parser::parse_links(std::string website_data) {
    std::regex link_regex("<a href=\"(http[s]{0,1}://.*?)\"", std::regex_constants::icase);

    return {std::sregex_token_iterator(website_data.begin(), website_data.end(), link_regex, 1),
            std::sregex_token_iterator{}};
}
