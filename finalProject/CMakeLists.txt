cmake_minimum_required(VERSION 3.10)

# Set the project name
project(EthernetProject)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the Ethernet source files
file(GLOB ETH_SRC Ethernet/*.cpp)

# Create an executable
add_executable(EthernetApp main.cpp ${ETH_SRC})
