#include "SearchAlgorithms.h"

int SearchAlgorithms::linearSearch(const std::vector<std::string>& data, const std::string& target) {
    for (size_t i = 0; i < data.size(); ++i) {
        if (data[i] == target) {
            return i;
        }
    }
    return -1;
}
