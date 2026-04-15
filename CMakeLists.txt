cmake_minimum_required(VERSION 3.20)

project(sutta-ranger VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(sutta-ranger src/main.cpp)
