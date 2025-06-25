cmake_minimum_required(VERSION 3.21)
project(cprior)

add_compile_options(-Wall -Wno-sign-compare -D_GLIBCXX_DEBUG)

enable_testing()

file(COPY "data/" DESTINATION "data/")

# adding the Google_Tests_run target
add_executable(full_test
        inf_engine_test.cpp
        ../src/util/Gamma.cpp
        ../src/util/ConstantSumRange.cpp
        model_node_test.cpp
        mock/SmallOutcome.h
        ../src/encoder/Entry.cpp
        ../src/encoder/Entry.h
        ../src/encoder/Tuple.cpp
        tuple_test.cpp
        ../src/encoder/DataSet.cpp
        ../src/encoder/DataSet.h
        dataset_test.cpp
        ../src/multinomial/Evaluator.cpp
        ../src/multinomial/Evaluator.h
)

# linking Google_Tests_run with libraries
target_link_libraries(
        full_test
        GTest::gmock_main
        GTest::gtest_main
)
