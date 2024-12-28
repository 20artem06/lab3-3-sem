#ifndef TEXTPROCESSOR_H
#define TEXTPROCESSOR_H

#include <string>
#include <map>
#include <vector>
#include "TextIndex.h"

class TextProcessor {
public:
    TextIndex createTextIndex(const std::string& text, int pageSize);
private:
    std::vector<std::string> splitWords(const std::string& text);
};

#endif // TEXTPROCESSOR_H
