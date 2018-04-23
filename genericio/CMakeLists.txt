cmake_minimum_required(VERSION 3.3)

project(GenericIO)

set (CMAKE_CXX_STANDARD 11)

# Create directories
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/frontend)
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/mpi)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/frontend)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/frontend)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/mpi)