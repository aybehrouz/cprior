cmake_minimum_required(VERSION 3.31)
project(cprior)

set(CMAKE_CXX_STANDARD 23)

add_compile_options(-Wall -Wno-sign-compare -D_GLIBCXX_DEBUG)

add_executable(cprior main.cpp
        src/util/Gamma.cpp
        src/encoder/AttributeTuple.cpp
        src/multinomial/MockOutcome.h
        src/util/ConstantSumRange.cpp
        src/util/ConstantSumRange.h)
