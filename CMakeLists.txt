cmake_minimum_required(VERSION 3.11)
set(EXECUTABLE_NAME softengine)
project(${EXECUTABLE_NAME})

# Use the correct system wide version of g++ on Linux
if (UNIX)
    set(CMAKE_CXX_COMPILER /usr/bin/g++)
endif (UNIX)

# Compiler configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-Ofast")

include_directories(Demo)
include_directories(Library)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-modules)

# Find SDL2 dependencies
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

find_package(SDL2TTF REQUIRED)
include_directories(${SDL2TTF_INCLUDE_DIR})

find_package(SDL2image REQUIRED)
include_directories(${SDL2_IMAGE_INCLUDE_DIR})

find_package(SDL2mixer REQUIRED)
include_directories(${SDL2_MIXER_INCLUDE_DIRS})

# Find OpenAL dependency
find_package(OpenAL REQUIRED)
include_directories(${OPENAL_INCLUDE_DIR})

set(SOURCE_FILES
    # -- Demo files --
    Demo/main.cpp
    Demo/Levels/Default.cpp
    Demo/Levels/Default.h
    Demo/Levels/StressTest.cpp
    Demo/Levels/StressTest.h
    Demo/Levels/LightTest.cpp
    Demo/Levels/LightTest.h
    Demo/Levels/TextureTest.cpp
    Demo/Levels/TextureTest.h
    Demo/Levels/Garden.cpp
    Demo/Levels/Garden.h
    Demo/Levels/Beach.cpp
    Demo/Levels/Beach.h

    # -- Header files --
    Library/Graphics/Color.h
    Library/Graphics/ColorBuffer.h
    Library/Graphics/RasterFilter.h
    Library/Graphics/Rasterizer.h
    Library/Graphics/TextureBuffer.h
    Library/Graphics/TriangleBuffer.h
    Library/Loaders/Loader.h
    Library/Loaders/ObjLoader.h
    Library/Sound/AudioEngine.h
    Library/Sound/Sound.h
    Library/System/Camera.h
    Library/System/DebugStats.h
    Library/System/Flags.h
    Library/System/Geometry.h
    Library/System/Level.h
    Library/System/Math.h
    Library/System/Objects.h
    Library/System/ParticleSystem.h
    Library/System/Positionable.h
    Library/System/Quaternion.h
    Library/UI/Alert.h
    Library/UI/UI.h
    Library/UI/UIObjects.h
    Library/Engine.h
    Library/Helpers.h

    # -- CPP Files --
    Source/Graphics/Color.cpp
    Source/Graphics/ColorBuffer.cpp
    Source/Graphics/RasterFilter.cpp
    Source/Graphics/Rasterizer.cpp
    Source/Graphics/TextureBuffer.cpp
    Source/Graphics/TriangleBuffer.cpp
    Source/Loaders/Loader.cpp
    Source/Loaders/ObjLoader.cpp
    Source/Sound/AudioEngine.cpp
    Source/Sound/Sound.cpp
    Source/System/Camera.cpp
    Source/System/DebugStats.cpp
    Source/System/Geometry.cpp
    Source/System/Level.cpp
    Source/System/Math.cpp
    Source/System/Objects.cpp
    Source/System/ParticleSystem.cpp
    Source/System/Positionable.cpp
    Source/System/Quaternion.cpp
    Source/UI/Alert.cpp
    Source/UI/UI.cpp
    Source/UI/UIObjects.cpp
    Source/Engine.cpp
    Source/Helpers.cpp
)

add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

# Link SDL libraries
target_link_libraries(${EXECUTABLE_NAME} ${SDL2_LIBRARY})
target_link_libraries(${EXECUTABLE_NAME} ${SDL2TTF_LIBRARY})
target_link_libraries(${EXECUTABLE_NAME} ${SDL2_IMAGE_LIBRARY})
target_link_libraries(${EXECUTABLE_NAME} ${SDL2_MIXER_LIBRARIES})

# Link OpenAL
target_link_libraries(${EXECUTABLE_NAME} ${OPENAL_LIBRARY})