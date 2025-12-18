cmake_minimum_required(VERSION 3.16)

project(Networking-Example LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Client)
add_subdirectory(Server)
add_subdirectory(TcpNetworking)
