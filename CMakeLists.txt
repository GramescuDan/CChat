cmake_minimum_required(VERSION 3.21)
project(CChat C)

set(CMAKE_C_STANDARD 99)

add_executable(CChat Client.c server.c)
