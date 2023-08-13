cmake_minimum_required(VERSION 3.16)
project(suika)

set(CMAKE_CXX_STANDARD 20)

add_executable(suika main.cpp
        logger.h
        util.h
        util.h
        EtherDevice.h
        EtherDevice.cpp
        logger.cpp
        util.cpp)
