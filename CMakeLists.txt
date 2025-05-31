cmake_minimum_required(VERSION 3.31)
project(cprior)

set(CMAKE_CXX_STANDARD 23)

add_compile_options(-Wall -Wno-sign-compare -D_GLIBCXX_DEBUG)

add_executable(cprior main.cpp
        src/math/HpFloat.h
        src/math/Gamma.cpp
        src/math/Gamma.h
        src/multinomial/Variable.cpp
        src/multinomial/Variable.h
        src/multinomial/InferenceNode.cpp
        src/multinomial/InferenceNode.h
        src/multinomial/AttributeTuple.cpp
        src/multinomial/AttributeTuple.h)
