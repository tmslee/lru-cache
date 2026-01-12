#include "lru_cache/lru_cache.hpp"
#include <iostream>
#include <string>

int main() {
    lru::LRUCache<int, std::string> cache(3);
    cache.put(1, "one");
    cache.put(2, "two");
    cache.put(3, "three");
    std::cout<<"Size: "<<cache.size()<<std::endl;
    if(auto val = cache.get(1)) {
        std::cout<<"Key 1: "<<val.value()<<std::endl;
    }
    cache.put(4, "four");
    std::cout<<"Contains 2? "<<(cache.contains(2) ? "yes" : "no")<<std::endl;
    std::cout<<"Contains 4? "<<(cache.contains(4) ? "yes" : "no")<<std::endl;
    return 0;
}