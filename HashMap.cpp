#include <iostream>
#include <vector>
#include <list>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <algorithm>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType> > class HashMap {
public:
    HashMap(Hash hasher_ = Hash()):hasher(hasher_) {
        setSize(HashMapSize);
    }
    template<class Iter>
    HashMap(Iter begin, Iter end, Hash hasher_ = Hash()):HashMap(hasher_) {
        setSize(HashMapSize);
        for (auto it = begin; it != end; ++it) {
            data_.push_back(*it);
        }
        setIterators();
    }
    HashMap(std::initializer_list<std::pair<const KeyType, ValueType>> init_list, Hash hasher_ = Hash()) :HashMap(init_list.begin(), init_list.end(), hasher_) {}

    size_t size() const {
        return data_.size();
    }
    bool empty() const {
        return data_.empty();
    }

    Hash hash_function() const {
        return hasher;
    }

    void insert(const std::pair<const KeyType, ValueType>& add) {
        if (find(add.first) == end()) {
            data_.push_front(add);
            remainers_[getPos(add.first)].push_back(data_.begin());
        }
    }
    void erase(const KeyType& del) {
        size_t pos = getPos(del);
        for (auto it = remainers_[pos].begin(); it != remainers_[pos].end(); ++it) {
            if ((*it)->first == del) {
                data_.erase(*it);
                remainers_[pos].erase(it);
                break;
            }
        }
    }

    typedef typename std::list<std::pair<const KeyType, ValueType>>::iterator iterator;
    typedef typename std::list<std::pair<const KeyType, ValueType>>::const_iterator const_iterator;

    iterator begin() {
        return data_.begin();
    }
    iterator end() {
        return data_.end();
    }
    const_iterator begin() const {
        return data_.begin();
    }
    const_iterator end() const {
        return data_.end();
    }

    iterator find(const KeyType& key) {
        size_t pos = getPos(key);
        for (auto it = remainers_[pos].begin(); it != remainers_[pos].end(); ++it) {
            if ((*it)->first == key) {
                return *it;
            }
        }
        return end();
    }

    const_iterator find(const KeyType& key) const {
        size_t pos = getPos(key);
        for (auto it = remainers_[pos].begin(); it != remainers_[pos].end(); ++it) {
            if ((*it)->first == key) {
                return *it;
            }
        }
        return end();
    }

    ValueType& operator[](const KeyType& key) {
        iterator f = find(key);
        if (f == end()) {
            data_.push_front({ key, ValueType() });
            size_t pos = getPos(key);
            remainers_[pos].push_back(data_.begin());
            return data_.begin()->second;
        }
        return f->second;
    }
    const ValueType& at(const KeyType& key) const {
        const_iterator f = find(key);
        if (f == end()) {
            throw std::out_of_range("out of range");
        }
        return f->second;
    }

    void clear() {
        for (auto& el : data_) {
            remainers_[getPos(el.first)].clear();
        }
        data_.clear();
    }

    HashMap& operator =(const HashMap& other) {
        if (this != &other) {
            data_.clear();
            for (auto elem : other.data_) {
                data_.push_back(elem);
            }
            setSize(HashMapSize);
            setIterators();
        }
        return *this;
    }

    void print() const {
        for (auto it = data_.begin(); it != data_.end(); ++it) {
            std::cout << "(" << it->first << ", " << it->second << ") ";
        }
        std::cout << std::endl;
    }

private:
    const int HashMapSize = 100000;
    std::list<std::pair<const KeyType, ValueType>> data_;
    std::vector<std::list<iterator>> remainers_;
    Hash hasher;
    void setSize(size_t size) {
        remainers_ = std::vector<std::list<iterator>>(size, std::list<iterator>());
    }
    size_t getPos(const KeyType& elem) const {
        return hasher(elem) % HashMapSize;
    }
    void setIterators() {
        for (auto it = data_.begin(); it != data_.end(); ++it) {
            remainers_[getPos(it->first)].push_back(it);
        }
    }
};
