cmake_minimum_required(VERSION 3.8)
set(EXECUTABLE_NAME softengine)
project(${EXECUTABLE_NAME})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-O3")
# -Wall -Wextra

include_directories(Source)

# Find SDL2
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-modules)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

set(SOURCE_FILES 
    Source/main.cpp 
    Source/Helpers.h
    Source/Objects.h Source/Objects.cpp
    Source/Types.h Source/Types.cpp
    Source/Rasterizer.cpp Source/Rasterizer.h
    Source/Engine.cpp Source/Engine.h
)

add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})
target_link_libraries(${EXECUTABLE_NAME} ${SDL2_LIBRARY})