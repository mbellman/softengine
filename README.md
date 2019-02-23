# softengine
A software 3D rendering engine, written as an educational exercise.

## To do

### Features

* Positionable entities following others
* Mesh: custom static or real-time deformations
* Fresnel effect/grazing angle reflections/object reflectivity
* Model: dynamic levels of detail
* Precalculated static shadows

**Pending optimizations:**

* Texture sampling logic cleanup
* Staggered perspective-correct/linear UV interpolation

### Refactoring/organization

* Separate `Camera` into its own class with keyboard/mouse event handling
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