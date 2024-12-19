#include "TextProcessor.h"
#include <sstream>

std::vector<std::string> TextProcessor::splitWords(const std::string& text) {
    std::istringstream iss(text);
    std::vector<std::string> words((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());
    return words;
}

void TextProcessor::paginateWords(const std::vector<std::string>& words, int pageSize, std::map<std::string, int>& index) {
    int wordCount = 0;
    int page = 1;

    for (const auto& word : words) {
        if (wordCount < pageSize / 2 && page == 1) {
            index[word] = page;
            ++wordCount;
        } else if (wordCount < (page * (pageSize * 0.75))) {
            index[word] = page;
            ++wordCount;
        } else {
            ++page;
            wordCount = 1;
            index[word] = page;
        }
    }
}

std::map<std::string, int> TextProcessor::createAlphabeticalIndex(const std::string& text, int pageSize) {
    std::map<std::string, int> index;
    std::vector<std::string> words = splitWords(text);
    paginateWords(words, pageSize, index);
    return index;
}
