cmake_minimum_required(VERSION 3.15)
project(second_task)

set(CMAKE_CXX_STANDARD 14)

find_package( OpenCV REQUIRED )

add_executable(second_task main.cpp)

target_link_libraries( second_task ${OpenCV_LIBS} )