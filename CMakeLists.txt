cmake_minimum_required(VERSION 3.25)
project(spmt)

set(CMAKE_CXX_STANDARD 17)

add_executable(main.cpp Package.hpp Package.cpp Version.hpp Version.cpp Args.hpp Args.cpp Data.hpp Data.cpp)
