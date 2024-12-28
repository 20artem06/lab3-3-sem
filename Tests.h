#ifndef TESTS_H
#define TESTS_H

#include <sstream>

class Tests {
public:
    void runAllTests(std::ostringstream& resultStream);
private:
    void testTextProcessor(std::ostringstream& resultStream);
    void testBTreeDictionary(std::ostringstream& resultStream);
    void testBPlusTreeSet(std::ostringstream& resultStream);
    void testBTreeSortedSequence(std::ostringstream& resultStream);
    void testLinearSearch(std::ostringstream& resultStream);
};

#endif // TESTS_H
