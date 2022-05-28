#This is the build system of File URLs Doctor project
#Make sure you have cmake installed and use "make -j4" for maximum speed
#Xen <xen-dev@pm.me> xen-e.github.io

cmake_minimum_required(VERSION 3.8)

project(FUD LANGUAGES CXX)

#c++ 17 is a must, since this project uses <filesystem>
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#LIBCURL required as well, because the whole project revolves around it
find_package(CURL REQUIRED)

#include dirs...
include_directories(include ${CURL_INCLUDE_DIR})

#Read CMakeLists.txt in src folder for more
add_subdirectory(src)
