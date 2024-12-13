cmake_minimum_required(VERSION 3.0)
project(EvFTP)

set(EXECUTABLE_OUTPUT_PATH_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 设置调试信息 以及 启动C++11语言标准
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 ")

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)
