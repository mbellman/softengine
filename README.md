# softengine
A software 3D rendering engine, written as an educational exercise.

## To do

### Features

* Model animations
* Level editor + file save/load support
* Dynamic object loading/unloading

**Pending optimizations:**

### Refactoring/organization

* Constants file for easy mobility between modules
* Convert skybox.obj into a predefined Object subclass
* Correct triangle edge/clockwise/etc. checks to be with respect to raster space
* Engine::updateScreenProjection() -> Separate out near-plane clipping operations

## Development

### Windows

Pending updated description.

### Linux

#### VSCode:
- Ensure you have CMake, CMakeTools and the SDL2 libraries installed.
- Install the CMakeTools and C++ Intellisense extensions.
- Use the `Project Outline` to build and debug the executable.

### OS X
1. R.I.P. Steve Jobs.