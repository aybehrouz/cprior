cmake_minimum_required(VERSION 3.21)
project(cprior)

add_compile_options(-O3)

add_executable(cprior
        main.cpp
        util/Gamma.cpp
        encoder/AttributeTuple.cpp
        util/ConstantSumRange.cpp
)

target_compile_definitions(cprior PUBLIC NDEBUG)