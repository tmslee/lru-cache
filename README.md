# LRU Cache
A header-only C++20 LRU (Least Recently Used) cache implementation with O(1) operations

## Features
- O(1) get and put operations
- Template-based for any key/value types
- Automatic eviction of least recently used items
- C++20 concepts for compile-time type safety
- header-only: just include and use

## Requirements
- C++20 compiler (GCC 10+, Clang 11+)
- CMake 3.20+ (for building tests)

## Installation
Header-only. copy `include/lru_cache_lru_cache.hpp` to your project and include it

## Usage
```cpp
#include <lru_cache/lru_cache.hpp>
#include <iostream>
#include <string>
int main() {
    lru::LRUCache cache(3);
    cache.put(1, "one");
    cache.put(2, "two");
    cache.put(3, "three");
    
    //access key 1 (becomes most recent)
    if (auto val = cache.get(1)) {
        std::cout<<val.value()<<std::endl;
    }

    //adding key 4 evicts key 2 (least recent)
    cache.put(4, "four");
    cache.contains(2); //false;
    cache.contains(1); //true
}
```

## API

### `LRUCache(std::siz_t capacity)`
construct cache with given capacity. throws `std::invalid_argument` if capacity is 0
### `std::optional<Value> get(const Key& key)`
get value for key. returns `std::nullopt` if not found. updates recency
### `void put(const Key& key, Value&& value)`
insert or update key-value pair. evicts LRU item if at capacity, updates recency. perfect forwards the value during this operation
### `bool contains(const Key& key) const`
check if key exists, does not update recency
### `bool erase(const Key& key)`
remove key & returns true if key existed
### `void clear()`
remove all entries
### `std::size_t size() const noexcept`
current number of entries 
### `std::size_t capacity() const noexcept`
max number of entries
### `bool empty() const noexcept`
true if cache is empty

## Design Notes
### value type requirements
values must be copy constructible. this is enforced at compile time using C++20 concepts:
```cpp
template<typename Key, std::copy_constructible Value>
class LRUCache {...}
```
the `get()` method return `std::optional<Value>`, which requires copying 
The `get()` method returns `std::optional<Value>`, which requires copying. Alternative approaches like returning a pointer or `std::reference_wrapper` could support move-only types but introduce dangling reference risks. We opted for safety over flexibility.

### key type considerations
keys are taken by `const&` rather than perfect forwarding because:
- keys are typically cheap (int, small strings with SSO)
- the key is needed twice; once for list and once for the map
- moving key into list would prevent using it for the map

### memeory management
the class follows the rule of 0, all members are self-manaing, so compiler generated copy/move constructors and destructor are correct

## how it works
uses `std::unordered_map` for O(1) look up and `std::list` for O(1) recency updates.
```
Map: key -> list iterator
List: [MRU] <-> [...] <-> [LRU]
```
- `get()`: find in map, move not to front via `splice()`
- `put()`: insert at front, evict from back if full
- `std::list::splice()` relinks poitners without copying or allocating

## Building Tests
```bash
cmake -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## with sanitizers
```bash
cmake -B build -DENABLE_SANITIZERS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## License
MIT