cmake_minimum_required(VERSION 3.10)

# Set the project name
project(EthernetProject)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the Ethernet source files
file(GLOB ETH_SRC Ethernet/*.cpp)

# Add the CRC source files (Assuming the CRC code is located in CRC/ directory)
file(GLOB CRC_SRC CRC/*.cpp)

# Include directories for Ethernet and CRC headers
include_directories(Ethernet CRC)

# Create an executable
add_executable(EthernetApp main.cpp ${ETH_SRC} ${CRC_SRC})
