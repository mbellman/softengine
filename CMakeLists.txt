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
    Demo/Scenes/Default.cpp
    Demo/Scenes/Default.h
    Demo/Scenes/StressTest.cpp
    Demo/Scenes/StressTest.h
    Demo/Scenes/LightTest.cpp
    Demo/Scenes/LightTest.h
    Demo/Scenes/TextureTest.cpp
    Demo/Scenes/TextureTest.h
    Demo/Scenes/Garden.cpp
    Demo/Scenes/Garden.h
    Demo/Scenes/Beach.cpp
    Demo/Scenes/Beach.h
    Demo/Scenes/SectorTest.cpp
    Demo/Scenes/SectorTest.h
    Demo/Scenes/AnimationTest.cpp
    Demo/Scenes/AnimationTest.h

    # -- Header files --
    Library/Graphics/Color.h
    Library/Graphics/ColorBuffer.h
    Library/Graphics/Illuminator.h
    Library/Graphics/RasterFilter.h
    Library/Graphics/Rasterizer.h
    Library/Graphics/TextureBuffer.h
    Library/Graphics/TriangleBuffer.h
    Library/Loaders/Loader.h
    Library/Loaders/ObjLoader.h
    Library/Sound/AudioEngine.h
    Library/Sound/Sound.h
    Library/System/Camera.h
    Library/System/CommandLine.h
    Library/System/Controller.h
    Library/System/DebugStats.h
    Library/System/Flags.h
    Library/System/Geometry.h
    Library/System/InputManager.h
    Library/System/Math.h
    Library/System/Objects.h
    Library/System/ParticleSystem.h
    Library/System/Positionable.h
    Library/System/Quaternion.h
    Library/System/Scene.h
    Library/UI/Alert.h
    Library/UI/UI.h
    Library/UI/UIObjects.h
    Library/Constants.h
    Library/Engine.h
    Library/Helpers.h

    # -- CPP Files --
    Source/Graphics/Color.cpp
    Source/Graphics/ColorBuffer.cpp
    Source/Graphics/Illuminator.cpp
    Source/Graphics/RasterFilter.cpp
    Source/Graphics/Rasterizer.cpp
    Source/Graphics/TextureBuffer.cpp
    Source/Graphics/TriangleBuffer.cpp
    Source/Loaders/Loader.cpp
    Source/Loaders/ObjLoader.cpp
    Source/Sound/AudioEngine.cpp
    Source/Sound/Sound.cpp
    Source/System/Camera.cpp
    Source/System/CommandLine.cpp
    Source/System/Controller.cpp
    Source/System/DebugStats.cpp
    Source/System/Geometry.cpp
    Source/System/InputManager.cpp
    Source/System/Math.cpp
    Source/System/Objects.cpp
    Source/System/ParticleSystem.cpp
    Source/System/Positionable.cpp
    Source/System/Quaternion.cpp
    Source/System/Scene.cpp
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