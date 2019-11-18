cmake_minimum_required(VERSION 3.15)
project(second_task)

set(CMAKE_CXX_STANDARD 14)

find_package( OpenCV REQUIRED )

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(second_task main.cpp)

target_link_libraries( second_task ${OpenCV_LIBS} )