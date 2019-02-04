cmake_minimum_required(VERSION 3.8)
set(EXECUTABLE_NAME softengine)
project(${EXECUTABLE_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Ofast")
# -Wall -Wextra

include_directories(Demo)
include_directories(Library)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-modules)

# Find SDL2 and SDL2_ttf
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

find_package(SDL2TTF REQUIRED)
include_directories(${SDL2TTF_INCLUDE_DIR})

set(SOURCE_FILES
    # -- Demo files --
    Demo/main.cpp
    Demo/Levels/Default.cpp
    Demo/Levels/Default.h
    Demo/Levels/StressTest.cpp
    Demo/Levels/StressTest.h

    # -- Header files --

    # UI
    Library/UI/UI.h
    Library/UI/UIObjects.h

    # Engine (Headers)
    Library/Helpers.h
    Library/Objects.h
    Library/Level.h
    Library/Types.h
    Library/Engine.h
    Library/Quaternion.h
    Library/Loaders/Loader.h
    Library/Loaders/ObjLoader.h
    Library/Graphics/Rasterizer.h
    Library/Graphics/RasterQueue.h

    # -- CPP Files --

    # UI (CPP Files)
    Source/UI/UI.cpp
    Source/UI/UIObjects.cpp

    # Engine (CPP Files)
    Source/Objects.cpp
    Source/Level.cpp
    Source/Types.cpp
    Source/Engine.cpp
    Source/Quaternion.cpp
    Source/Loaders/Loader.cpp
    Source/Loaders/ObjLoader.cpp
    Source/Graphics/Rasterizer.cpp
    Source/Graphics/RasterQueue.cpp
)

add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

# Link SDL libraries
target_link_libraries(${EXECUTABLE_NAME} ${SDL2_LIBRARY})
target_link_libraries(${EXECUTABLE_NAME} ${SDL2TTF_LIBRARY})