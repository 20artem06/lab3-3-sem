#ifndef TEXTPROCESSOR_H
#define TEXTPROCESSOR_H

#include <string>
#include <map>
#include <vector>

class TextProcessor {
public:
    std::map<std::string, int> createAlphabeticalIndex(const std::string& text, int pageSize);
private:
    std::vector<std::string> splitWords(const std::string& text);
    void paginateWords(const std::vector<std::string>& words, int pageSize, std::map<std::string, int>& index);
};

#endif // TEXTPROCESSOR_H
