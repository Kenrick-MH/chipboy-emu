cmake_minimum_required(VERSION 3.15)
project(chipboy_core VERSION 1.0
                    LANGUAGES CXX C)

include(CTest)
find_package(Catch2 3 REQUIRED)

add_executable(pq_test "tests/test_utils/test_pq.cpp")
target_include_directories(pq_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(pq_test PRIVATE Catch2::Catch2WithMain)
include(Catch)
catch_discover_tests(pq_test)