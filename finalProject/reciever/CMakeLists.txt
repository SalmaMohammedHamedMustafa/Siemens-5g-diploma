cmake_minimum_required(VERSION 3.10)

# Set the project name
project(ClientProject)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# Add the socket handler source files
file(GLOB SOCKET_HANDLER_SRC socket_handler/*.cpp)
file(GLOB ORAN_SRC ORAN/*.cpp)
file(GLOB PARSER_SRC parser/*.cpp)
file(GLOB CRC_SRC CRC/*.cpp)

# Include directories 
include_directories(socket_handler ORAN parser)

# Create an executable and include all source files
add_executable(ClientApp main.cpp ${SOCKET_HANDLER_SRC} ${ORAN_SRC} ${PARSER_SRC} ${CRC_SRC})
