# softengine
A software 3D rendering engine, written as an educational exercise.

## To do

### Features

* Improved hidden surface removal
* Light sources
* Texture mapping
* Parallelization

**Maybe:**

* OpenGL/hardware mode
* Dynamic shadows
* Ambient occlusion

### Refactoring/organization

* Move existing documentation into .h files, add more documentation
* Separate `Camera` into its own class with keyboard/mouse event handling

## Development

### Windows

#### Dependencies

- Download **SDL2**: https://www.libsdl.org/download-2.0.php -> `SDL2-devel-2.X.X-VC.zip`

#### Visual Studio
- Create a new Visual Studio project using the files in `/Source`.
- Set the following project properties:

```
Configuration Properties
  General
    VC++ Directories
      Include Directories: (Add a path to the '/Source' folder on your machine)
    C/C++
      General
        Additional Include Directories: (Add a path to your local 'C:/.../SDL2/include' folder)
      Optimization
        Optimization: /O2
      Code Generation
        Basic Runtime Checks: default
    Linker
      General
        Additional Library Directories: (Add a path to your local 'C:/.../SDL2/lib/x86' folder)
      Input
        Additional Dependencies: SDL2.lib, SDL2main.lib
      System
        SubSystem: Windows
```

- Copy `SDL2.dll` into the root folder.
- Make sure the project is using `x86` mode before trying to debug or build.

### Linux

#### VSCode:
- Ensure you have CMake, CMakeTools and the SDL2 libraries installed.
- Install the CMakeTools and C++ Intellisense extensions.
- Use the `Project Outline` to build and debug the executable.

### OS X
1. R.I.P. Steve Jobs.