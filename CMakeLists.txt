cmake_minimum_required (VERSION 3.14)

project("ral" 
        VERSION 0.1
        DESCRIPTION "Roger Allen's Lisp"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

add_subdirectory("src")
