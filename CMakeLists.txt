cmake_minimum_required(VERSION 3.20)
project(lru_cache VERSION 1.0.0 LANUGAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

option(ENABLE_SANITIZERS "Enable ASan and UBSan" OFF)
if(ENABLE_SANITIZERS)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

add_library(lru_cache INTERFACE)
target_include_directories(lru_cache INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(lru_cache_test tests/lru_cache_test.cpp)
target_link_libraries(lru_cache_test PRIVATE lru_cache GTest::gtest_main)
add_test(NAME lru_cache_test COMMAND lru_cache_test)

add_executable(basic_usage examples/basic_usage.cpp)
target_link_libraries(basic_usage PRIVATE lru_cache)