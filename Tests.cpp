#include "Tests.h"
#include "TextProcessor.h"
#include "DataStructure.h"
#include "SearchAlgorithms.h"

void Tests::runAllTests(std::ostringstream& resultStream) {
    testTextProcessor(resultStream);
    testSimpleDictionary(resultStream);
    testLinearSearch(resultStream);
}

void Tests::testTextProcessor(std::ostringstream& resultStream) {
    TextProcessor processor;

    std::string testInput = "слово1 слово2 слово3 слово4 слово5";
    int pageSize = 3;

    // Ожидаемое значение: слово1 на странице 1, слово2 на странице 1, слово3 на странице 1, слово4 на странице 2, слово5 на странице 2
    std::map<std::string, int> expectedIndex = {
        {"слово1", 1},
        {"слово2", 1},
        {"слово3", 1},
        {"слово4", 2},
        {"слово5", 2}
    };

    auto index = processor.createAlphabeticalIndex(testInput, pageSize);

    // Проверка результатов
    if (index == expectedIndex) {
        resultStream << "TestTextProcessor passed!\n";
    } else {
        resultStream << "TestTextProcessor failed!\n";
    }
}

void Tests::testSimpleDictionary(std::ostringstream& resultStream) {
    SimpleDictionary<std::string, int> dict;

    dict.insert("key1", 1);
    dict.insert("key2", 2);

    // Проверка поиска
    if (dict.find("key1") == 1 && dict.find("key2") == 2) {
        resultStream << "TestSimpleDictionary passed!\n";
    } else {
        resultStream << "TestSimpleDictionary failed!\n";
    }
}

void Tests::testLinearSearch(std::ostringstream& resultStream) {
    std::vector<std::string> data = {"apple", "banana", "cherry", "date"};

    if (SearchAlgorithms::linearSearch(data, "banana") == 1 &&
        SearchAlgorithms::linearSearch(data, "fig") == -1) {
        resultStream << "TestLinearSearch passed!\n";
    } else {
        resultStream << "TestLinearSearch failed!\n";
    }
}
