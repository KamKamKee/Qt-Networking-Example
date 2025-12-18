cmake_minimum_required(VERSION 3.16)

project(TcpNetworking LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)

add_library(TcpNetworking SHARED
  TcpNetworking_global.h
  nettype.h
  netmessage.h netmessage.cpp
  netsocket.h netsocket.cpp
  netserver.h netserver.cpp
  nettype.h
)

target_include_directories(TcpNetworking INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(TcpNetworking PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)

target_compile_definitions(TcpNetworking PRIVATE TCPNETWORKING_LIBRARY)
