#include "TextProcessor.h"
#include <sstream>

std::vector<std::string> TextProcessor::splitWords(const std::string& text) {
    std::istringstream iss(text);
    std::vector<std::string> words((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());
    return words;
}

TextIndex TextProcessor::createTextIndex(const std::string& text, int pageSize) {
    TextIndex index;
    int charCount = 0;
    int page = 1;
    int position = 0;
    std::string currentWord;

    for (size_t i = 0; i < text.size(); ++i) {
        if (std::isalpha(text[i]) || std::isdigit(text[i])){
            currentWord += text[i];
            charCount++;
            position++;
        } else {
            if (!currentWord.empty()) {
                index.addWord(currentWord, TextIndex::Location(page, position - currentWord.length() + 1));
                currentWord = "";
            }

        }
        if(charCount >= pageSize){
            page++;
            charCount = 0;
            position = 0;
        }
    }

    if (!currentWord.empty()) {
        index.addWord(currentWord, TextIndex::Location(page, position - currentWord.length() + 1));
    }
    return index;
}
