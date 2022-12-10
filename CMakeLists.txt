cmake_minimum_required(VERSION 3.0.0)
project(TrueCore VERSION 0.1.0)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_library(TrueCore ${SRC_FILES})
target_include_directories(TrueCore PUBLIC include/)