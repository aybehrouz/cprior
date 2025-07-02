cmake_minimum_required(VERSION 3.21)
project(cprior)

add_compile_options(-Wall -Wno-sign-compare -D_GLIBCXX_DEBUG -fopenmp)

enable_testing()

file(COPY "data/" DESTINATION "data/")

# adding the Google_Tests_run target
add_executable(full_test
        inf_engine_test.cpp
        ../src/util/Gamma.cpp
        ../src/util/ConstantSumRange.cpp
        model_node_test.cpp
        ../src/encoder/Entry.cpp
        ../src/encoder/Tuple.cpp
        tuple_test.cpp
        ../src/encoder/DataSet.cpp
        dataset_test.cpp
        ../src/multinomial/Evaluator.cpp
        ../src/encoder/DataGenerator.cpp
        tools/DecisionTreePredictor.cpp
        ../src/encoder/DataGenerator.h
)

# linking Google_Tests_run with libraries
target_link_libraries(
        full_test
        GTest::gmock_main
        GTest::gtest_main
        mlpack
)
