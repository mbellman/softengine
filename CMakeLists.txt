cmake_minimum_required(VERSION 3.8)
set(EXECUTABLE_NAME softengine)
project(${EXECUTABLE_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Ofast")
# -Wall -Wextra

include_directories(Library)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-modules)

# Find SDL2 and SDL2_ttf
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

find_package(SDL2TTF REQUIRED)
include_directories(${SDL2TTF_INCLUDE_DIR})

set(SOURCE_FILES
    Source/main.cpp
    Library/Helpers.h

    # UI
    Library/UI/UI.h         Source/UI/UI.cpp
    Library/UI/UIObjects.h  Source/UI/UIObjects.cpp

    # Engine
    Library/Objects.h       Source/Objects.cpp
    Library/Types.h         Source/Types.cpp
    Library/Rasterizer.h    Source/Rasterizer.cpp
    Library/Engine.h        Source/Engine.cpp
    Library/Quaternion.h    Source/Quaternion.cpp
)

add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

# Link SDL libraries
target_link_libraries(${EXECUTABLE_NAME} ${SDL2_LIBRARY})
target_link_libraries(${EXECUTABLE_NAME} ${SDL2TTF_LIBRARY})