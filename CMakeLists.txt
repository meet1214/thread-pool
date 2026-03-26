cmake_minimum_required(VERSION 3.14)
project(ThreadPool CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)
add_executable(tests tests/ThreadPoolTest.cpp)
target_include_directories(tests PRIVATE include)  
target_link_libraries(tests PRIVATE GTest::gtest_main)
target_compile_options(tests PRIVATE -Wall -Wextra)
add_executable(benchmark benchmark/benchmark.cpp)
target_include_directories(benchmark PRIVATE include)
target_compile_options(benchmark PRIVATE -Wall -Wextra)