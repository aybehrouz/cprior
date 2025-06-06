cmake_minimum_required(VERSION 3.21)
project(cprior)

add_compile_options(-Wall -Wno-sign-compare -D_GLIBCXX_DEBUG)

enable_testing()

# adding the Google_Tests_run target
add_executable(full_test
        inf_engine_test.cpp
        ../src/util/Gamma.cpp
        ../src/encoder/AttributeTuple.cpp
)

# linking Google_Tests_run with libraries
target_link_libraries(
        full_test
        GTest::gmock_main
        GTest::gtest_main
)
