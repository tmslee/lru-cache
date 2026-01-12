#include <gtest/gtest.h>
#include "lru_cache/lru_cache.hpp"

#include <string>

TEST(LRUCache, ConstructWithCapacity) {
    lru::LRUCache<int,int> cache(5);
    EXPECT_EQ(cache.capacity(), 5);
    EXPECT_EQ(cache.size(), 0);
    EXPECT_TRUE(cache.empty());
}
TEST(LRUCache, ZeroCapacityThrows) {
    EXPECT_THROW((lru::LRUCache<int,int>(0)), std::invalid_argument);
}
TEST(LRUCache, PutAndGet) {
    lru::LRUCache<int, std::string> cache(3);
    cache.put(1, "one");
    cache.put(2, "two");
    auto val = cache.get(1);
    ASSERT_TRUE(val);
    EXPECT_EQ(val.value(), "one");
}
TEST(LRUCache, GetMissing) {
    lru::LRUCache<int,int> cache(3);
    auto val = cache.get(999);
    EXPECT_FALSE(val);
}
TEST(LRUCache, UpdateExisting) {
    lru::LRUCache<int, std::string> cache(3);
    cache.put(1,"one");
    cache.put(1,"ONE");
    EXPECT_EQ(cache.size(), 1);
    EXPECT_EQ(cache.get(1).value(), "ONE");
}
TEST(LRUCache, EvictsLRU) {
    lru::LRUCache<int, std::string> cache(3);
    cache.put(1, "one");
    cache.put(2, "two");
    cache.put(3, "three");
    cache.put(4, "four");
    EXPECT_FALSE(cache.get(1));
    EXPECT_TRUE(cache.get(2));
    EXPECT_TRUE(cache.get(3));
    EXPECT_TRUE(cache.get(4));
}
TEST(LRUCache, GetUpdatesRecency) {
    lru::LRUCache<int, std::string> cache(3);
    cache.put(1, "one");
    cache.put(2, "two");
    cache.put(3, "three");

    cache.get(1); //1 is not most recent
    cache.put(4, "four"); //evicts 2, not 1

    EXPECT_TRUE(cache.get(1));
    EXPECT_FALSE(cache.get(2));
}
TEST(LRUCache, PutUpdatesRecency) {
    lru::LRUCache<int, std::string> cache(3);
    cache.put(1, "one");
    cache.put(2, "two");
    cache.put(3, "three");

    cache.put(1, "ONE"); //1 is not most recent
    cache.put(4, "four"); //evecits 2, not 1

    EXPECT_TRUE(cache.get(1));
    EXPECT_FALSE(cache.get(2));
}
TEST(LRUCache, Contains) {
    lru::LRUCache<int, int> cache(3);
    cache.put(1,100);
    EXPECT_TRUE(cache.contains(1));
    EXPECT_FALSE(cache.contains(2));
}
TEST(LRUCache, Erase) {
    lru::LRUCache<int, int> cache(3);
    cache.put(1,100);
    cache.put(2,200);
    EXPECT_TRUE(cache.erase(1));
    EXPECT_FALSE(cache.contains(1));
    EXPECT_EQ(cache.size(), 1);
    EXPECT_FALSE(cache.erase(999));
}
TEST(LRUCache, Clear) {
    lru::LRUCache<int,int> cache(3);
    cache.put(1,100);
    cache.put(2,200);
    cache.put(3,300);
    cache.clear();
    EXPECT_TRUE(cache.empty());
    EXPECT_EQ(cache.size(), 0);
    EXPECT_FALSE(cache.contains(1));
}
TEST(LRUCache, CapacityOne) {
    lru::LRUCache<int,int> cache(1);
    cache.put(1,100);
    EXPECT_EQ(cache.get(1).value(), 100);
    cache.put(2, 200);
    EXPECT_FALSE(cache.get(1));
    EXPECT_EQ(cache.get(2).value(), 200);    
}
TEST(LRUCache, StringKeys) {
    lru::LRUCache<std::string, int> cache(3);
    cache.put("apple", 1);
    cache.put("banana", 2);
    EXPECT_EQ(cache.get("apple").value(), 1);
    EXPECT_FALSE(cache.get("cherry"));
}