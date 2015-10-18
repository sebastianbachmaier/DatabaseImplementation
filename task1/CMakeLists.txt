cmake_minimum_required(VERSION 2.6)
project(task1)

add_executable(task1 Types.cpp main.cpp)

install(TARGETS task1 RUNTIME DESTINATION bin)
