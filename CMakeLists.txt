cmake_minimum_required (VERSION 2.8.11)
project (2017aut-quest-test)

add_subdirectory(gtest ${CMAKE_BINARY_DIR}/googletest)

add_executable(vector_test vector_test.cxx)

add_library(vector lab_vector.c)

target_link_libraries(vector_test vector gtest_main)
