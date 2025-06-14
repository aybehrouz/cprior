cmake_minimum_required(VERSION 3.21)
project(cprior)

add_compile_options(-O3)

add_executable(cprior
        main.cpp
        util/Gamma.cpp
        util/ConstantSumRange.cpp
        encoder/Entry.cpp
        encoder/Entry.h
        encoder/Tuple.cpp
        encoder/DataSet.cpp
        encoder/DataSet.h
)

target_compile_definitions(cprior PUBLIC NDEBUG)