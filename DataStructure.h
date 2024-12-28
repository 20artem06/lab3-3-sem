#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H

#include <iostream>


// Forward declarations
template<typename Tkey, typename Telement> class IDictionary;
template<typename T> class ISet;
template<typename T> class ISortedSequence;



template<typename Tkey, typename Telement>
struct BTreeNode {
    bool isLeaf;
    int keyCount;
    Tkey* keys;
    Telement* elements;
    BTreeNode<Tkey, Telement>** children;

    BTreeNode(int t, bool leaf) : isLeaf(leaf), keyCount(0) {
        keys = new Tkey[2 * t - 1];
        elements = new Telement[2 * t - 1];
        children = new BTreeNode<Tkey, Telement>*[2 * t];
        for (int i = 0; i < 2 * t; ++i) {
            children[i] = nullptr;
        }
    }
    ~BTreeNode() {
        delete[] keys;
        delete[] elements;
        delete[] children;
    }
};

template<typename Tkey, typename Telement>
class IDictionary {
public:
    virtual int GetCount() = 0;
    virtual int GetCapacity() = 0;
    virtual Telement Get(const Tkey& key) = 0;
    virtual bool ContainsKey(const Tkey& key) = 0;
    virtual void Add(const Tkey& key, const Telement& element) = 0;
    virtual void Remove(const Tkey& key) = 0;
    virtual ~IDictionary() {}
};

template<typename Tkey, typename Telement>
class BTreeDictionary : public IDictionary<Tkey, Telement> {
private:
    BTreeNode<Tkey, Telement>* root;
    int t;
    int count;

    void splitChild(BTreeNode<Tkey, Telement>* parent, int index, BTreeNode<Tkey, Telement>* child) {
        BTreeNode<Tkey, Telement>* newChild = new BTreeNode<Tkey, Telement>(t, child->isLeaf);
        newChild->keyCount = t - 1;

        for (int j = 0; j < t-1; ++j){
            newChild->keys[j] = child->keys[j + t];
            newChild->elements[j] = child->elements[j+t];
        }

        if(!child->isLeaf){
            for (int j = 0; j < t; ++j) {
                newChild->children[j] = child->children[j + t];
                child->children[j + t] = nullptr;
            }
        }
        child->keyCount = t - 1;

        for (int j = parent->keyCount; j >= index +1; j--) {
            parent->children[j+1] = parent->children[j];
        }
        parent->children[index + 1] = newChild;

        for(int j = parent->keyCount - 1; j >= index; j--)
        {
            parent->keys[j + 1] = parent->keys[j];
            parent->elements[j+1] = parent->elements[j];
        }
        parent->keys[index] = child->keys[t-1];
        parent->elements[index] = child->elements[t-1];
        parent->keyCount++;
    }

    void insertNonFull(BTreeNode<Tkey, Telement>* node, const Tkey& key, const Telement& element) {
        int i = node->keyCount - 1;
        if (node->isLeaf) {
            while (i >= 0 && key < node->keys[i]) {
                node->keys[i + 1] = node->keys[i];
                node->elements[i + 1] = node->elements[i];
                i--;
            }
            node->keys[i + 1] = key;
            node->elements[i + 1] = element;
            node->keyCount++;

        } else {
            while(i >= 0 && key < node->keys[i]){
                i--;
            }
            if (node->children[i + 1]->keyCount == 2 * t - 1) {
                splitChild(node, i+1, node->children[i + 1]);
                if(key > node->keys[i+1])
                    i++;
            }
            insertNonFull(node->children[i + 1], key, element);
        }
    }

    bool containsKeyRecursive(BTreeNode<Tkey, Telement>* node, const Tkey& key) {
        if (node == nullptr) {
            return false;
        }

        int i = 0;
        while(i < node->keyCount && key > node->keys[i]){
            i++;
        }
        if (i < node->keyCount && key == node->keys[i]) {
            return true;
        }
        if (node->isLeaf) {
            return false;
        }
        return containsKeyRecursive(node->children[i], key);
    }

    void removeRecursive(BTreeNode<Tkey, Telement>* node, const Tkey& key) {
        if (!node)
            return;
        int idx = 0;
        while(idx < node->keyCount && key > node->keys[idx])
            idx++;
        if (idx < node->keyCount && node->keys[idx] == key)
        {
            if (node->isLeaf)
            {
                for(int i = idx; i < node->keyCount - 1; i++){
                    node->keys[i] = node->keys[i+1];
                    node->elements[i] = node->elements[i+1];
                }
                node->keyCount--;

            }
            else
            {

                if(node->children[idx]->keyCount >= t){
                    BTreeNode<Tkey, Telement>* temp = node->children[idx];
                    while (!temp->isLeaf){
                        temp = temp->children[temp->keyCount];
                    }
                    int j = temp->keyCount-1;
                    node->keys[idx] = temp->keys[j];
                    node->elements[idx] = temp->elements[j];
                    removeRecursive(node->children[idx], node->keys[idx]);
                }
                else if (node->children[idx + 1]->keyCount >= t) {
                    BTreeNode<Tkey, Telement>* temp = node->children[idx+1];
                    while (!temp->isLeaf){
                        temp = temp->children[0];
                    }
                    node->keys[idx] = temp->keys[0];
                    node->elements[idx] = temp->elements[0];
                    removeRecursive(node->children[idx + 1], node->keys[idx]);
                }
                else {
                    merge(node, idx);
                    removeRecursive(node->children[idx], key);
                }
            }
            return;
        } else {
            if(node->isLeaf){
                return;
            }
            if (node->children[idx]->keyCount < t)
            {
                if (idx < node->keyCount && node->children[idx + 1]->keyCount >= t)
                {
                    fill(node,idx);
                }
                else if (idx > 0 && node->children[idx-1]->keyCount >= t)
                {
                    fill(node,idx-1);
                }
                else
                {
                    if(idx < node->keyCount){
                        merge(node, idx);
                    }
                    else {
                        merge(node, idx-1);
                        idx--;
                    }
                }
            }
            removeRecursive(node->children[idx], key);
        }
    }
    void fill(BTreeNode<Tkey, Telement>* node, int idx){

        if (idx < node->keyCount  && node->children[idx+1]->keyCount >= t)
        {
            BTreeNode<Tkey, Telement>* child = node->children[idx];
            BTreeNode<Tkey, Telement>* sibling = node->children[idx+1];
            for(int i = child->keyCount - 1; i >= 0; i--) {
                child->keys[i+1] = child->keys[i];
                child->elements[i+1] = child->elements[i];
            }
            if (!child->isLeaf) {
                for(int i = child->keyCount; i >= 0; i--)
                    child->children[i+1] = child->children[i];
                child->children[0] = sibling->children[0];
                for (int i = 0; i < sibling->keyCount - 1; i++)
                    sibling->children[i] = sibling->children[i + 1];
            }
            child->keys[0] = node->keys[idx];
            child->elements[0] = node->elements[idx];
            node->keys[idx] = sibling->keys[0];
            node->elements[idx] = sibling->elements[0];
            for(int i = 0; i < sibling->keyCount - 1; i++)
            {
                sibling->keys[i] = sibling->keys[i+1];
                sibling->elements[i] = sibling->elements[i+1];
            }
            child->keyCount++;
            sibling->keyCount--;
        }
        else {
            BTreeNode<Tkey, Telement>* child = node->children[idx];
            BTreeNode<Tkey, Telement>* sibling = node->children[idx-1];
            child->keys[child->keyCount] = node->keys[idx-1];
            child->elements[child->keyCount] = node->elements[idx-1];
            if (!child->isLeaf)
                child->children[child->keyCount + 1] = sibling->children[sibling->keyCount];
            node->keys[idx - 1] = sibling->keys[sibling->keyCount - 1];
            node->elements[idx-1] = sibling->elements[sibling->keyCount-1];
            child->keyCount++;
            sibling->keyCount--;
        }
    }
    void merge(BTreeNode<Tkey, Telement>* node, int idx){
        BTreeNode<Tkey, Telement>* child = node->children[idx];
        BTreeNode<Tkey, Telement>* sibling = node->children[idx+1];
        child->keys[t - 1] = node->keys[idx];
        child->elements[t-1] = node->elements[idx];
        for(int i = 0; i < sibling->keyCount; i++){
            child->keys[i+t] = sibling->keys[i];
            child->elements[i + t] = sibling->elements[i];
        }
        if (!child->isLeaf) {
            for(int i = 0; i <= sibling->keyCount; i++)
                child->children[i+t] = sibling->children[i];
        }

        for (int i = idx; i < node->keyCount - 1; i++) {
            node->keys[i] = node->keys[i + 1];
            node->elements[i] = node->elements[i+1];
            node->children[i + 1] = node->children[i + 2];
        }
        child->keyCount += sibling->keyCount+1;
        node->keyCount--;
        delete sibling;
    }

    void freeTree(BTreeNode<Tkey, Telement>* node){
        if (node == nullptr) return;
        if (!node->isLeaf){
            for (int i = 0; i <= node->keyCount; ++i) {
                freeTree(node->children[i]);
            }
        }
        delete node;
    }

    Telement findElementRecursive(BTreeNode<Tkey, Telement>* node, const Tkey& key){
        if (node == nullptr) {
            throw std::runtime_error("Key not found");
        }

        int i = 0;
        while(i < node->keyCount && key > node->keys[i]){
            i++;
        }
        if (i < node->keyCount && key == node->keys[i]) {
            return node->elements[i];
        }
        if (node->isLeaf) {
            throw std::runtime_error("Key not found");
        }
        return findElementRecursive(node->children[i], key);
    }

public:
    BTreeDictionary(int degree = 3) : root(nullptr), t(degree), count(0) {}

    ~BTreeDictionary() {
        freeTree(root);
    }

    int GetCount() override {
        return count;
    }

    int GetCapacity() override {
        return -1;
    }

    Telement Get(const Tkey& key) override {
        return findElementRecursive(root, key);
    }

    bool ContainsKey(const Tkey& key) override {
        return containsKeyRecursive(root, key);
    }

    void Add(const Tkey& key, const Telement& element) override {
        if (ContainsKey(key)){
            throw std::runtime_error("Key already exists");
        }
        if (root == nullptr) {
            root = new BTreeNode<Tkey, Telement>(t, true);
        }
        if (root->keyCount == 2 * t - 1) {
            BTreeNode<Tkey, Telement>* newRoot = new BTreeNode<Tkey, Telement>(t, false);
            newRoot->children[0] = root;
            splitChild(newRoot, 0, root);
            insertNonFull(newRoot, key, element);
            root = newRoot;
        } else {
            insertNonFull(root, key, element);
        }
        count++;
    }

    void Remove(const Tkey& key) override {
        if (root == nullptr || !ContainsKey(key)) {
            return;
        }
        removeRecursive(root, key);
        if (root->keyCount == 0){
            if(!root->isLeaf){
                BTreeNode<Tkey, Telement>* temp = root;
                root = root->children[0];
                delete temp;
            }
            else {
                BTreeNode<Tkey, Telement>* temp = root;
                root = nullptr;
                delete temp;
            }
        }
        count--;
    }
};

template <typename T>
struct BPlusTreeNode {
    bool isLeaf;
    int keyCount;
    T* keys;
    BPlusTreeNode<T>** children;
    BPlusTreeNode<T>* next;

    BPlusTreeNode(int t, bool leaf) : isLeaf(leaf), keyCount(0), next(nullptr) {
        keys = new T[2 * t];
        children = new BPlusTreeNode<T>*[2*t+1];
        for(int i = 0; i <= 2*t; ++i){
            children[i] = nullptr;
        }
    }
    ~BPlusTreeNode(){
        delete[] keys;
        delete[] children;
    }
};



template<typename T>
class ISet {
public:
    virtual bool Contains(const T& element) = 0;
    virtual void Add(const T& element) = 0;
    virtual void Remove(const T& element) = 0;
    virtual int GetCount() = 0;
    virtual ~ISet() {}
};


template<typename T>
class BPlusTreeSet : public ISet<T> {
private:
    BPlusTreeNode<T>* root;
    int t;
    int count;

    void splitChild(BPlusTreeNode<T>* parent, int index, BPlusTreeNode<T>* child)
    {
        BPlusTreeNode<T>* newChild = new BPlusTreeNode<T>(t, child->isLeaf);
        newChild->keyCount = t;

        for (int j = 0; j < t; ++j) {
            newChild->keys[j] = child->keys[j+t];
        }
        if(!child->isLeaf){
            for(int j = 0; j<= t; ++j){
                newChild->children[j] = child->children[j + t];
                child->children[j+t] = nullptr;
            }
        }

        child->keyCount = t;

        for (int j = parent->keyCount; j >= index +1; j--) {
            parent->children[j+1] = parent->children[j];
        }
        parent->children[index + 1] = newChild;
        for(int j = parent->keyCount; j >= index+1; j--)
        {
            parent->keys[j] = parent->keys[j - 1];
        }
        parent->keys[index] = newChild->keys[0];
        parent->keyCount++;
    }


    void insertNonFull(BPlusTreeNode<T>* node, const T& key){
        int i = node->keyCount - 1;
        if(node->isLeaf){
            while(i>=0 && key < node->keys[i]) {
                node->keys[i+1] = node->keys[i];
                i--;
            }
            node->keys[i + 1] = key;
            node->keyCount++;
        }
        else{
            while (i >=0 && key < node->keys[i])
                i--;

            if(node->children[i+1]->keyCount == 2 * t) {
                splitChild(node, i+1, node->children[i + 1]);
                if(key > node->keys[i+1])
                    i++;
            }
            insertNonFull(node->children[i + 1], key);
        }
    }

    bool containsRecursive(BPlusTreeNode<T>* node, const T& key){
        if (node == nullptr) {
            return false;
        }
        int i = 0;
        while (i < node->keyCount && key > node->keys[i]){
            i++;
        }
        if(i < node->keyCount && key == node->keys[i])
            return true;
        if (node->isLeaf) {
            return false;
        }
        return containsRecursive(node->children[i], key);
    }
    void removeRecursive(BPlusTreeNode<T>* node, const T& key)
    {
        if (!node)
            return;
        int idx = 0;
        while(idx < node->keyCount && key > node->keys[idx])
            idx++;
        if(idx < node->keyCount && node->keys[idx] == key){
            if (node->isLeaf) {
                for(int i = idx; i < node->keyCount - 1; i++)
                    node->keys[i] = node->keys[i+1];
                node->keyCount--;
            }
            else {
                if (node->children[idx]->keyCount >= t) {
                    BPlusTreeNode<T>* temp = node->children[idx];
                    while (!temp->isLeaf) {
                        temp = temp->children[temp->keyCount];
                    }
                    int j = temp->keyCount - 1;
                    node->keys[idx] = temp->keys[j];
                    removeRecursive(node->children[idx], node->keys[idx]);
                } else if (node->children[idx+1]->keyCount >= t) {
                    BPlusTreeNode<T>* temp = node->children[idx+1];
                    while (!temp->isLeaf)
                        temp = temp->children[0];
                    node->keys[idx] = temp->keys[0];
                    removeRecursive(node->children[idx + 1], node->keys[idx]);
                } else {
                    merge(node, idx);
                    removeRecursive(node->children[idx], key);
                }
            }
            return;
        } else
        {
            if(node->isLeaf){
                return;
            }
            if (node->children[idx]->keyCount < t) {
                if (idx < node->keyCount && node->children[idx + 1]->keyCount >= t)
                {
                    fill(node,idx);
                }
                else if (idx > 0 && node->children[idx - 1]->keyCount >= t) {
                    fill(node, idx-1);
                }
                else{
                    if(idx < node->keyCount)
                        merge(node, idx);
                    else {
                        merge(node, idx-1);
                        idx--;
                    }
                }
            }
            removeRecursive(node->children[idx], key);
        }
    }

    void fill(BPlusTreeNode<T>* node, int idx){
        if (idx < node->keyCount  && node->children[idx+1]->keyCount >= t){
            BPlusTreeNode<T>* child = node->children[idx];
            BPlusTreeNode<T>* sibling = node->children[idx+1];
            for(int i = child->keyCount - 1; i >= 0; i--)
                child->keys[i+1] = child->keys[i];
            if (!child->isLeaf){
                for(int i = child->keyCount; i >=0; --i)
                    child->children[i+1] = child->children[i];
                child->children[0] = sibling->children[0];
                for (int i = 0; i < sibling->keyCount-1; i++)
                    sibling->children[i] = sibling->children[i+1];
            }
            child->keys[0] = node->keys[idx];
            node->keys[idx] = sibling->keys[0];
            for(int i = 0; i < sibling->keyCount-1; ++i)
                sibling->keys[i] = sibling->keys[i+1];
            child->keyCount++;
            sibling->keyCount--;
        }
        else {
            BPlusTreeNode<T>* child = node->children[idx];
            BPlusTreeNode<T>* sibling = node->children[idx - 1];
            child->keys[child->keyCount] = node->keys[idx - 1];
            if (!child->isLeaf)
                child->children[child->keyCount + 1] = sibling->children[sibling->keyCount];
            node->keys[idx - 1] = sibling->keys[sibling->keyCount - 1];
            child->keyCount++;
            sibling->keyCount--;
        }
    }
    void merge(BPlusTreeNode<T>* node, int idx){
        BPlusTreeNode<T>* child = node->children[idx];
        BPlusTreeNode<T>* sibling = node->children[idx+1];
        child->keys[t] = node->keys[idx];
        for(int i = 0; i < sibling->keyCount; i++)
            child->keys[i+t+1] = sibling->keys[i];
        if (!child->isLeaf)
            for(int i = 0; i <= sibling->keyCount; i++)
                child->children[i+t+1] = sibling->children[i];
        for(int i = idx; i < node->keyCount - 1; i++){
            node->keys[i] = node->keys[i+1];
            node->children[i+1] = node->children[i+2];
        }
        child->keyCount += sibling->keyCount + 1;
        node->keyCount--;
        delete sibling;
    }


    void freeTree(BPlusTreeNode<T>* node) {
        if (node == nullptr) return;
        if (!node->isLeaf) {
            for(int i = 0; i <= node->keyCount; ++i)
                freeTree(node->children[i]);
        }
        delete node;
    }

public:
    BPlusTreeSet(int degree = 3) : root(nullptr), t(degree), count(0) {}
    ~BPlusTreeSet(){
        freeTree(root);
    }
    bool Contains(const T& element) override {
        return containsRecursive(root, element);
    }

    void Add(const T& element) override {
        if(Contains(element))
            return;
        if (root == nullptr) {
            root = new BPlusTreeNode<T>(t, true);
        }
        if(root->keyCount == 2 * t) {
            BPlusTreeNode<T>* newRoot = new BPlusTreeNode<T>(t, false);
            newRoot->children[0] = root;
            splitChild(newRoot, 0, root);
            insertNonFull(newRoot, element);
            root = newRoot;
        }
        else {
            insertNonFull(root, element);
        }
        count++;
    }

    void Remove(const T& element) override {
        if (root == nullptr || !Contains(element)) {
            return;
        }
        removeRecursive(root, element);
        if(root->keyCount == 0){
            if(!root->isLeaf){
                BPlusTreeNode<T>* temp = root;
                root = root->children[0];
                delete temp;
            }
            else{
                BPlusTreeNode<T>* temp = root;
                root = nullptr;
                delete temp;
            }
        }
        count--;
    }


    int GetCount() override {
        return count;
    }
};


template<typename T>
class ISortedSequence {
public:
    virtual void Add(const T& element) = 0;
    virtual void Remove(const T& element) = 0;
    virtual int GetCount() = 0;
    virtual T Get(int index) = 0;
    virtual ~ISortedSequence() {}
};

template <typename T>
class BTreeSortedSequence : public ISortedSequence<T> {
private:
    BTreeNode<T, T>* root;
    int t;
    int count;

    void splitChild(BTreeNode<T, T>* parent, int index, BTreeNode<T, T>* child) {
        BTreeNode<T, T>* newChild = new BTreeNode<T, T>(t, child->isLeaf);
        newChild->keyCount = t - 1;

        for (int j = 0; j < t-1; ++j){
            newChild->keys[j] = child->keys[j + t];
            newChild->elements[j] = child->elements[j+t];
        }

        if(!child->isLeaf){
            for (int j = 0; j < t; ++j) {
                newChild->children[j] = child->children[j + t];
                child->children[j + t] = nullptr;
            }
        }
        child->keyCount = t - 1;

        for (int j = parent->keyCount; j >= index +1; j--) {
            parent->children[j+1] = parent->children[j];
        }
        parent->children[index + 1] = newChild;

        for(int j = parent->keyCount - 1; j >= index; j--)
        {
            parent->keys[j + 1] = parent->keys[j];
            parent->elements[j+1] = parent->elements[j];
        }
        parent->keys[index] = child->keys[t-1];
        parent->elements[index] = child->elements[t-1];
        parent->keyCount++;
    }

    void insertNonFull(BTreeNode<T, T>* node, const T& element) {
        int i = node->keyCount - 1;
        if (node->isLeaf) {
            while (i >= 0 && element < node->keys[i]) {
                node->keys[i + 1] = node->keys[i];
                node->elements[i + 1] = node->elements[i];
                i--;
            }
            node->keys[i + 1] = element;
            node->elements[i + 1] = element;
            node->keyCount++;

        } else {
            while(i >= 0 && element < node->keys[i]){
                i--;
            }
            if (node->children[i + 1]->keyCount == 2 * t - 1) {
                splitChild(node, i+1, node->children[i + 1]);
                if(element > node->keys[i+1])
                    i++;
            }
            insertNonFull(node->children[i + 1], element);
        }
    }

    void removeRecursive(BTreeNode<T, T>* node, const T& element) {
        if (!node)
            return;
        int idx = 0;
        while(idx < node->keyCount && element > node->keys[idx])
            idx++;
        if (idx < node->keyCount && node->keys[idx] == element)
        {
            if (node->isLeaf)
            {
                for(int i = idx; i < node->keyCount - 1; i++){
                    node->keys[i] = node->keys[i+1];
                    node->elements[i] = node->elements[i+1];
                }
                node->keyCount--;

            }
            else
            {

                if(node->children[idx]->keyCount >= t){
                    BTreeNode<T, T>* temp = node->children[idx];
                    while (!temp->isLeaf){
                        temp = temp->children[temp->keyCount];
                    }
                    int j = temp->keyCount-1;
                    node->keys[idx] = temp->keys[j];
                    node->elements[idx] = temp->elements[j];
                    removeRecursive(node->children[idx], node->keys[idx]);
                }
                else if (node->children[idx + 1]->keyCount >= t) {
                    BTreeNode<T, T>* temp = node->children[idx+1];
                    while (!temp->isLeaf){
                        temp = temp->children[0];
                    }
                    node->keys[idx] = temp->keys[0];
                    node->elements[idx] = temp->elements[0];
                    removeRecursive(node->children[idx + 1], node->keys[idx]);
                }
                else {
                    merge(node, idx);
                    removeRecursive(node->children[idx], element);
                }
            }
            return;
        } else {
            if(node->isLeaf){
                return;
            }
            if (node->children[idx]->keyCount < t)
            {
                if (idx < node->keyCount && node->children[idx + 1]->keyCount >= t)
                {
                    fill(node,idx);
                }
                else if (idx > 0 && node->children[idx-1]->keyCount >= t)
                {
                    fill(node,idx-1);
                }
                else
                {
                    if(idx < node->keyCount){
                        merge(node, idx);
                    }
                    else {
                        merge(node, idx-1);
                        idx--;
                    }
                }
            }
            removeRecursive(node->children[idx], element);
        }
    }
    void fill(BTreeNode<T, T>* node, int idx){

        if (idx < node->keyCount  && node->children[idx+1]->keyCount >= t)
        {
            BTreeNode<T, T>* child = node->children[idx];
            BTreeNode<T, T>* sibling = node->children[idx+1];
            for(int i = child->keyCount - 1; i >= 0; i--) {
                child->keys[i+1] = child->keys[i];
                child->elements[i+1] = child->elements[i];
            }
            if (!child->isLeaf) {
                for(int i = child->keyCount; i >= 0; i--)
                    child->children[i+1] = child->children[i];
                child->children[0] = sibling->children[0];
                for (int i = 0; i < sibling->keyCount - 1; i++)
                    sibling->children[i] = sibling->children[i + 1];
            }
            child->keys[0] = node->keys[idx];
            child->elements[0] = node->elements[idx];
            node->keys[idx] = sibling->keys[0];
            node->elements[idx] = sibling->elements[0];
            for(int i = 0; i < sibling->keyCount - 1; i++)
            {
                sibling->keys[i] = sibling->keys[i+1];
                sibling->elements[i] = sibling->elements[i+1];
            }
            child->keyCount++;
            sibling->keyCount--;
        }
        else {
            BTreeNode<T, T>* child = node->children[idx];
            BTreeNode<T, T>* sibling = node->children[idx-1];
            child->keys[child->keyCount] = node->keys[idx-1];
            child->elements[child->keyCount] = node->elements[idx-1];
            if (!child->isLeaf)
                child->children[child->keyCount + 1] = sibling->children[sibling->keyCount];
            node->keys[idx - 1] = sibling->keys[sibling->keyCount - 1];
            node->elements[idx-1] = sibling->elements[sibling->keyCount-1];
            child->keyCount++;
            sibling->keyCount--;
        }
    }
    void merge(BTreeNode<T, T>* node, int idx){
        BTreeNode<T, T>* child = node->children[idx];
        BTreeNode<T, T>* sibling = node->children[idx+1];
        child->keys[t - 1] = node->keys[idx];
        child->elements[t-1] = node->elements[idx];
        for(int i = 0; i < sibling->keyCount; i++){
            child->keys[i+t] = sibling->keys[i];
            child->elements[i + t] = sibling->elements[i];
        }
        if (!child->isLeaf) {
            for(int i = 0; i <= sibling->keyCount; i++)
                child->children[i+t] = sibling->children[i];
        }

        for (int i = idx; i < node->keyCount - 1; i++) {
            node->keys[i] = node->keys[i + 1];
            node->elements[i] = node->elements[i+1];
            node->children[i + 1] = node->children[i + 2];
        }
        child->keyCount += sibling->keyCount+1;
        node->keyCount--;
        delete sibling;
    }

    void freeTree(BTreeNode<T, T>* node){
        if (node == nullptr) return;
        if (!node->isLeaf){
            for (int i = 0; i <= node->keyCount; ++i) {
                freeTree(node->children[i]);
            }
        }
        delete node;
    }

    T findElementByIndex(BTreeNode<T, T>* node, int index){
        if (node == nullptr) {
            throw std::out_of_range("Index out of range");
        }
        if (node->isLeaf)
        {
            if(index >=0 && index < node->keyCount)
                return node->keys[index];
            else {
                throw std::out_of_range("Index out of range");
            }
        }
        int count = 0;
        for (int i = 0; i <= node->keyCount; ++i)
        {
            int childSize = countNodes(node->children[i]);
            if(count + childSize > index){
                return findElementByIndex(node->children[i], index - count);
            }
            count += childSize;
        }
        throw std::out_of_range("Index out of range");
    }

    int countNodes(BTreeNode<T, T>* node){
        if (node == nullptr) return 0;
        if (node->isLeaf) return node->keyCount;
        int count = 0;
        for(int i = 0; i <= node->keyCount; ++i){
            count += countNodes(node->children[i]);
        }
        return count;
    }
public:
    BTreeSortedSequence(int degree = 3) : root(nullptr), t(degree), count(0) {}

    ~BTreeSortedSequence() {
        freeTree(root);
    }

    void Add(const T& element) override {
        if (root == nullptr) {
            root = new BTreeNode<T, T>(t, true);
        }
        if (root->keyCount == 2 * t - 1) {
            BTreeNode<T, T>* newRoot = new BTreeNode<T, T>(t, false);
            newRoot->children[0] = root;
            splitChild(newRoot, 0, root);
            insertNonFull(newRoot, element);
            root = newRoot;
        } else {
            insertNonFull(root, element);
        }
        count++;
    }

    void Remove(const T& element) override {
        if (root == nullptr)
            return;
        removeRecursive(root, element);
        if (root->keyCount == 0){
            if(!root->isLeaf){
                BTreeNode<T, T>* temp = root;
                root = root->children[0];
                delete temp;
            }
            else {
                BTreeNode<T, T>* temp = root;
                root = nullptr;
                delete temp;
            }
        }
        count--;
    }

    int GetCount() override {
        return count;
    }

    T Get(int index) override {
        return findElementByIndex(root, index);
    }
};

#endif // BTREESORTEDSEQUENCE_H
