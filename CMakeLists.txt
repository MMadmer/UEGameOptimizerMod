cmake_minimum_required(VERSION 3.18)

set(TARGET Optimizer)
project(${TARGET})

add_library(${TARGET} SHARED
    "src/dllmain.cpp"
    "src/Replacer.hpp"
    "src/Replacer.cpp"
    "src/UFunctionUtils.hpp"
    "src/UFunctionUtils.cpp"
)

target_include_directories(${TARGET} PRIVATE .)

target_link_libraries(${TARGET} PRIVATE UE4SS)