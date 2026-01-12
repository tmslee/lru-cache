#ifndef LRU_CACHE_HPP
#define LRU_CACHE_HPP

#include <cstddef>
#include <list>
#include <unordered_map>
#include <optional>
#include <stdexcept>

namespace lru {

template<typename Key, typename Value>
class LRUCache {
public:
    explicit LRUCache(std::size_t capacity);

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
    using LIstIterator = typename ListType::iterator;
    using MapType = std::unodered_map<Key, ListIterator>;

    void move_to_front(ListIterator it);

    std::size_t capacity_;
    ListType items_;
    MapType lookup_;
};
    
}
#endif