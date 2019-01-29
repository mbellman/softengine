cmake_minimum_required(VERSION 3.8)
set(EXECUTABLE_NAME softengine)
project(${EXECUTABLE_NAME})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Ofast")
# -Wall -Wextra

include_directories(Library)

# Find SDL2
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-modules)

if (WIN32)
    set(SDL2_PATH "C:/mingw-dev/SDL2")
endif(WIN32)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

set(SOURCE_FILES
    Source/main.cpp
    Library/Helpers.h
    Library/Objects.h       Source/Objects.cpp
    Library/Types.h         Source/Types.cpp
    Library/Rasterizer.h    Source/Rasterizer.cpp
    Library/Engine.h        Source/Engine.cpp
    Library/Quaternion.h    Source/Quaternion.cpp
)

add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})
target_link_libraries(${EXECUTABLE_NAME} ${SDL2_LIBRARY})