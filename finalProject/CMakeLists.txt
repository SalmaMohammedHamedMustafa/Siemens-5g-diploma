cmake_minimum_required(VERSION 3.10)

# Set the project name
project(EthernetProject)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the Ethernet source files
file(GLOB ETH_SRC Ethernet/*.cpp)

# Add the CRC source files
file(GLOB CRC_SRC CRC/*.cpp)

# Add the socket handler source files
file(GLOB SOCKET_HANDLER_SRC socket_handler/*.cpp)

# Add the ECPRI source files
file(GLOB ECPRI_SRC ECPRI/*.cpp)

# Add the ORAN source files
file(GLOB ORAN_SRC ORAN/*.cpp)

# Include directories for Ethernet, CRC, socket_handler, ECPRI, and ORAN headers
include_directories(Ethernet CRC socket_handler ECPRI ORAN)

# Create an executable and include all source files
add_executable(EthernetApp main.cpp ${ETH_SRC} ${CRC_SRC} ${SOCKET_HANDLER_SRC} ${ECPRI_SRC} ${ORAN_SRC})
