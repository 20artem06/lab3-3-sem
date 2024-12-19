#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H

#include <set>
#include <map>

template<typename KeyType, typename ValueType>
class IDictionary {
public:
    virtual void insert(const KeyType& key, const ValueType& value) = 0;
    virtual ValueType find(const KeyType& key) = 0;
    virtual ~IDictionary() {}
};

template<typename KeyType, typename ValueType>
class SimpleDictionary : public IDictionary<KeyType, ValueType> {
private:
    std::map<KeyType, ValueType> data;
public:
    void insert(const KeyType& key, const ValueType& value) override {
        data[key] = value;
    }

    ValueType find(const KeyType& key) override {
        return data[key]; // Можно добавить обработку, если ключ не найден
    }
};

#endif // DATASTRUCTURE_H
