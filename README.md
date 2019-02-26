# softengine
A software 3D rendering engine, written as an educational exercise.

## To do

### Features

* Conical light sources
* Model: dynamic levels of detail
* Background color gradient/image skybox
* KB+M input manager
* Sectors/area zones
* Real-time level settings editing
* Level layouts stored as files
* Level editor
* Precalculated static shadows

**Pending optimizations:**

* Disable fresnel calculations for entities which don't need it
* Screen projection optimizations
* Texture sampling logic cleanup
* Staggered perspective-correct/linear UV interpolation

### Refactoring/organization

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