#ifndef TEXTINDEX_H
#define TEXTINDEX_H

#include <string>
#include <vector>
#include <map>
#include <algorithm>

class TextIndex {
public:
    struct Location {
        int page;
        int position;
        Location(int p, int pos) : page(p), position(pos) {}
        Location() : page(0), position(0) {}

        bool operator==(const Location& other) const {
            return page == other.page && position == other.position;
        }
    };

    void addWord(const std::string& word, const Location& location);
    std::vector<Location> getWordLocations(const std::string& word) const;
    std::vector<std::string> getWords() const;

    bool operator==(const TextIndex& other) const {
        if (index.size() != other.index.size()) {
            return false;
        }
        for (const auto& pair : index) {
            auto otherPair = other.index.find(pair.first);
            if (otherPair == other.index.end() ) {
                return false;
            }
            if(otherPair->second.size() != pair.second.size()){
                return false;
            }
            for(size_t i = 0; i< pair.second.size(); ++i){
                if(!(pair.second[i] == otherPair->second[i])){
                    return false;
                }
            }
        }
        return true;
    }
private:
    std::map<std::string, std::vector<Location>> index;
};

#endif
