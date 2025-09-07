cmake_minimum_required(VERSION 3.21)
project(cprior)

add_compile_options(-O3 -fopenmp)

add_executable(cprior
        main.cpp
        util/Gamma.cpp
        util/ConstantSumRange.cpp
        encoder/Entry.cpp
        encoder/Entry.h
        encoder/Tuple.cpp
        encoder/DataSet.cpp
        encoder/DataSet.h
        multinomial/Evaluator.cpp
        multinomial/Evaluator.h
        encoder/DataGenerator.cpp
        encoder/DataGenerator.h
        ../test/tools/DecisionTreePredictor.cpp
        ../test/tools/DecisionTreePredictor.h
        multinomial/CentralPredictive.cpp
        multinomial/CentralPredictive.h
        multinomial/Deterministic.h
        multinomial/Conditional.cpp
        multinomial/Conditional.h
)

target_compile_definitions(cprior PUBLIC NDEBUG)

target_link_libraries(cprior mlpack)