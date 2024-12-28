#include "Tests.h"
#include "TextProcessor.h"
#include "DataStructure.h"
#include "SearchAlgorithms.h"
#include <sstream>


void Tests::runAllTests(std::ostringstream& resultStream) {
    testTextProcessor(resultStream);
    testBTreeDictionary(resultStream);
    testBPlusTreeSet(resultStream);
    testBTreeSortedSequence(resultStream);
    testLinearSearch(resultStream);
}

void Tests::testTextProcessor(std::ostringstream& resultStream) {
    TextProcessor processor;

    std::string testInput = "слово1 слово2 слово1 слово3 слово4 слово1";
    int pageSize = 20;


    TextIndex expectedIndex;
    expectedIndex.addWord("слово1", TextIndex::Location(1,1));
    expectedIndex.addWord("слово2", TextIndex::Location(1, 8));
    expectedIndex.addWord("слово1", TextIndex::Location(1,15));
    expectedIndex.addWord("слово3", TextIndex::Location(1,22));
    expectedIndex.addWord("слово4", TextIndex::Location(1,29));
    expectedIndex.addWord("слово1", TextIndex::Location(2,1));


    auto index = processor.createTextIndex(testInput, pageSize);

    if(index == expectedIndex){
        resultStream << "TestTextProcessor passed!\n";
    }
    else {
        resultStream << "TestTextProcessor failed!\n";
    }
}

void Tests::testBTreeDictionary(std::ostringstream& resultStream) {
    BTreeDictionary<std::string, int> dict;

    dict.Add("key1", 1);
    dict.Add("key2", 2);

    // Проверка поиска
    if (dict.Get("key1") == 1 && dict.Get("key2") == 2 && dict.ContainsKey("key1") && !dict.ContainsKey("key3")) {
        resultStream << "TestBTreeDictionary passed!\n";
    } else {
        resultStream << "TestBTreeDictionary failed!\n";
    }
    dict.Remove("key1");
    if (dict.ContainsKey("key1")) {
        resultStream << "TestBTreeDictionary failed remove!\n";
    }

    if(dict.GetCount() != 1){
        resultStream << "TestBTreeDictionary failed count!\n";
    }

    try {
        dict.Get("key1");
        resultStream << "TestBTreeDictionary failed exception!\n";
    }
    catch(...){
    }
}


void Tests::testBPlusTreeSet(std::ostringstream& resultStream) {
    BPlusTreeSet<int> set;
    set.Add(1);
    set.Add(2);
    set.Add(1);

    if (set.Contains(1) && set.Contains(2) && !set.Contains(3) && set.GetCount() == 2) {
        resultStream << "TestBPlusTreeSet passed!\n";
    } else {
        resultStream << "TestBPlusTreeSet failed!\n";
    }

    set.Remove(1);
    if (set.Contains(1) || set.GetCount() != 1) {
        resultStream << "TestBPlusTreeSet failed remove!\n";
    }
}

void Tests::testBTreeSortedSequence(std::ostringstream& resultStream) {
    BTreeSortedSequence<int> sequence;
    sequence.Add(3);
    sequence.Add(1);
    sequence.Add(2);

    if (sequence.Get(0) == 1 && sequence.Get(1) == 2 && sequence.Get(2) == 3 && sequence.GetCount() == 3) {
        resultStream << "TestBTreeSortedSequence passed!\n";
    } else {
        resultStream << "TestBTreeSortedSequence failed!\n";
    }

    sequence.Remove(2);
    if (sequence.Get(0) == 1 && sequence.Get(1) == 3 && sequence.GetCount() == 2) {

    } else {
        resultStream << "TestBTreeSortedSequence failed remove!\n";
    }
    try{
        sequence.Get(2);
        resultStream << "TestBTreeSortedSequence failed exception!\n";
    } catch (...){

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
