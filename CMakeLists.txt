cmake_minimum_required(VERSION 3.0.0)
project(VectrCore VERSION 0.1.0)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_library(VectrCore ${SRC_FILES})
target_include_directories(VectrCore PUBLIC include/)