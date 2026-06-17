cmake_minimum_required(VERSION 3.22)

project(PixieToolbox VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(glaze_ENABLE_REFLECTION26 ON)

add_subdirectory(Dependencies)
add_subdirectory(Source)
