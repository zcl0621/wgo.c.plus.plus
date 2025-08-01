cmake_minimum_required(VERSION 3.31)
project(consoleGo)

set(CMAKE_CXX_STANDARD 17)

add_executable(consoleGo main.cpp
        position.cpp
        color.h
        stone.cpp
        board.cpp
)
