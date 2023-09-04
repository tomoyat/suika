cmake_minimum_required(VERSION 3.16)
project(suika)

set(CMAKE_CXX_STANDARD 20)

add_executable(suika main.cpp
        Logger.h
        Util.h
        Util.h
        EtherDevice.h
        EtherDevice.cpp
        Logger.cpp
        Util.cpp
        Interrupt.h
        Protocol.h
        Arp.h
        Arp.cpp
        NetworkInterface.h
        IpNetworkInterface.h
        IpNetworkInterface.cpp
        ArpData.h)
