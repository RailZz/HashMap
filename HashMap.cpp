#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <list>
#include <stdexcept>
#include <vector>
#include <utility>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
public:
    HashMap(Hash _hasher = Hash{}) : hasher_(_hasher) {
        real_size_ = 0;
        setOptimalSize();
        resize(current_size_);
    }

    template<class Iter>
    HashMap(Iter begin, Iter end, Hash _hasher = Hash{}) : HashMap(_hasher) {
        real_size_ = 0;
        for (auto it = begin; it != end; ++it) {
            data_.push_back(*it);
            real_size_++;
        }
        setOptimalSize();
        resize(current_size_);
        setIterators();
    }

    HashMap(std::initializer_list<std::pair<const KeyType, ValueType>> init_list,
        Hash _hasher = Hash{})
        : HashMap(init_list.begin(), init_list.end(), _hasher) {}

    size_t size() const {
        return real_size_;
    }

    bool empty() const {
        return real_size_ == 0;
    }

    Hash hash_function() const {
        return hasher_;
    }

    void insert(const std::pair<const KeyType, ValueType>& add) {
        if (find(add.first) == end()) {
            real_size_++;
            data_.push_front(add);
            if (size() == current_size_ * 2) {
                current_size_ *= 2;
                resize(current_size_);
                setIterators();
            } else {
                buckets_[getPos(add.first)].push_back(data_.begin());
            }
        }
    }

    void erase(const KeyType& del) {
        size_t pos = getPos(del);
        for (auto it = buckets_[pos].begin(); it != buckets_[pos].end(); ++it) {
            if ((*it)->first == del) {
                real_size_--;
                data_.erase(*it);
                buckets_[pos].erase(it);
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
        for (auto it = buckets_[pos].begin(); it != buckets_[pos].end(); ++it) {
            if ((*it)->first == key) {
                return *it;
            }
        }
        return end();
    }

    const_iterator find(const KeyType& key) const {
        size_t pos = getPos(key);
        for (auto it = buckets_[pos].begin(); it != buckets_[pos].end(); ++it) {
            if ((*it)->first == key) {
                return *it;
            }
        }
        return end();
    }

    ValueType& operator[](const KeyType& key) {
        iterator f = find(key);
        if (f == end()) {
            insert({ key, ValueType{} });
            f = find(key);
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
            buckets_[getPos(el.first)].clear();
        }
        real_size_ = 0;
        data_.clear();
        setOptimalSize();
        resize(current_size_);
    }

    HashMap& operator =(const HashMap& other) {
        if (this != &other) {
            clear();
            real_size_ = 0;
            for (auto it = other.begin(); it != other.end(); ++it) {
                data_.push_back(*it);
                real_size_++;
            }
            setOptimalSize();
            resize(current_size_);
            setIterators();
        }
        return *this;
    }

private:
    size_t real_size_;
    size_t current_size_;
    std::list<std::pair<const KeyType, ValueType>> data_;
    std::vector<std::list<iterator>> buckets_;
    Hash hasher_;

    inline void resize(size_t size) {
        buckets_.assign(size, {});
    }

    inline void setOptimalSize() {
        current_size_ = 1;
        while (current_size_ <= real_size_) {
            current_size_ <<= 1;
        }
    }

    inline size_t getPos(const KeyType& elem) const {
        return hasher_(elem) % current_size_;
    }

    void setIterators() {
        for (auto it = data_.begin(); it != data_.end(); ++it) {
            buckets_[getPos(it->first)].push_back(it);
        }
    }
};
