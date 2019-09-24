cmake_minimum_required(VERSION 3.14)
project(first_task)
set(CMAKE_CXX_STANDARD 14)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_package( OpenCV REQUIRED )
add_executable(first_task main.cpp binarize.h binarize.cpp PencilFinder.cpp PencilFinder.h NoisesRemover.cpp NoisesRemover.h)
target_link_libraries( first_task ${OpenCV_LIBS} )