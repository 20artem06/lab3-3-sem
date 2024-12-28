#include "TextIndex.h"

void TextIndex::addWord(const std::string& word, const Location& location) {
    index[word].push_back(location);
}

std::vector<TextIndex::Location> TextIndex::getWordLocations(const std::string& word) const {
    auto it = index.find(word);
    if (it != index.end()) {
        return it->second;
    }
    return {};
}

std::vector<std::string> TextIndex::getWords() const {
    std::vector<std::string> words;
    for (const auto& pair : index) {
        words.push_back(pair.first);
    }
    return words;
}
