#ifndef LRU_CACHE_HPP
#define LRU_CACHE_HPP

#include <cstddef>
#include <list>
#include <unordered_map>
#include <optional>
#include <stdexcept>

namespace lru {
/*
    some note on copy/moveability of Value:
    - when we fetch items from our map we use the get() method that can return one of the three:
    - std::optional<Value>
    - ptr to Value &Value (or nullptr)
    - std::optional<std::reference_wrapper<Value>>
    
    - std::optional<Value> requires the value to be copyable and will not be able to handle move only objs like std::unique_ptr
    - the latter two can handle move only objs but are not safe as if consumer copies returning pointer that can lead to dangling pointers when things get deref'ed
    - in our exercise we will opt to prioritize safety while improve clarity by introducing concepts (c++20) to enforce that our values are copyable.
*/

template<typename Key, std::copy_constructible Value>
class LRUCache {
public:
    explicit LRUCache(std::size_t capacity);
    /* 
        note: we dont manage raw resources here
        all members are copyable, moveable, and self destructring.
        we can let the compiler take care of move/copy ctors and dtor.
    */ 

    // returns value if found, moves to front (most recent)
    std::optional<Value> get(const Key& key);
    // insert or update, evicts LRU if at cap
    void put(const Key& key, Value&& value);
    /*
        note on why we dont perfect forward key
        - key are designed to be cheap; typically int or std::string (with SSO for short strings) or simple structs
        - we need the key twice; in the list and in the map. if we moved the key into the list, we can't use it for the map
    */

    // Query state
    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] std::size_t capacity() const noexcept;
    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] bool contains(const Key& key) const;

    // remove key
    bool erase(const Key& key);
    //clear entries
    void clear();

private:
    using ListType = std::list<std::pair<Key,Value>>;
    using ListIterator = typename ListType::iterator;
    using MapType = std::unordered_map<Key, ListIterator>;

    void move_to_front(ListIterator it);

    std::size_t capacity_;
    ListType items_;
    MapType lookup_;
};

template<typename Key, std::copy_constructible Value>
LRUCache<Key, Value>::LRUCache(std::size_t capacity) : capacity_(capacity) {
    if(capacity == 0) {
        throw std::invalid_argument("lru cache capacity must be greater than 0");
    }
}

template<typename Key, std::copy_constructible Value>
std::optional<Value> LRUCache<Key,Value>::get(const Key& key) {
    auto it = lookup_.find(key);
    if(it == lookup_.end()) {
        return std::nullopt;
    }
    move_to_front(it->second);
    return items_.front().second;
}

template<typename Key, std::copy_constructible Value>
void LRUCache<Key,Value>::put(const Key& key, Value&& value) {
    auto it = lookup_.find(key);
    
    if(it != lookup_.end()) {
        //key exists, update value & move to front
        it->second->second = std::forward<Value>(value);
        move_to_front(it->second);
        return;
    }

    // evict LRU if at cap
    if(items_.size() >= capacity_) {
        auto& lru = items_.back();
        lookup_.erase(lru.first);
        items_.pop_back();
    }

    //insert new item at front
    items_.emplace_front(key, std::forward<Value>(value));
    lookup_[key] = items_.begin();
}

template<typename Key, std::copy_constructible Value>
void LRUCache<Key,Value>::move_to_front(ListIterator it) {
    // splice moves a node within list without copying or allocating O(1)
    // this is alist specific operation that relinks pointers
    items_.splice(items_.begin(), items_, it);
}

template<typename Key, std::copy_constructible Value>
std::size_t LRUCache<Key,Value>::size() const noexcept {
    return items_.size();
}

template<typename Key, std::copy_constructible Value>
std::size_t LRUCache<Key,Value>::capacity() const noexcept {
    return capacity_;
}

template<typename Key, std::copy_constructible Value>
bool LRUCache<Key,Value>::empty() const noexcept {
    return items_.empty();
}

template<typename Key, std::copy_constructible Value>
bool LRUCache<Key,Value>::contains(const Key& key) const {
    return lookup_.find(key) != lookup_.end();
}

template<typename Key, std::copy_constructible Value>
bool LRUCache<Key,Value>::erase(const Key& key) {
    auto it = lookup_.find(key);
    if(it == lookup_.end()) {
        return false;
    }
    items_.erase(it->second);
    lookup_.erase(it);
    return true;
}

template<typename Key, std::copy_constructible Value>
void LRUCache<Key, Value>::clear() {
    items_.clear();
    lookup_.clear();
}

}
#endif