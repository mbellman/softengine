# softengine
A software 3D rendering engine, written as an educational exercise.

## Development

### Windows
1. Install [Visual Studio Code](https://code.visualstudio.com/)
2. Add `CMake Tools` and `C/C++` extensions to VS Code
3. Create a folder @ `C:/mingw-dev/SDL2/`
4. Download `SDL2-devel-2.X.X-mingw.tar.gz` from https://www.libsdl.org/download-2.0.php
5. Extract contents
6. Open `[Extracted folder]` -> `SDL2-2.X.X` -> `i686-w64-mingw32`
7. Copy `bin`, `include`, and `lib` folders into `C:/mingw-dev/SDL2/`
8. Open local repo folder in VS Code
9. Open `CMake Tools` panel
10. Configure project
11. Build project
12. Copy `SDL2.dll` into `.build/`
13. In `CMake Tools`, right click on configured project target -> `Run with debugger`

### Linux
1. It just works!

### OS X
1. R.I.P. Steve Jobs.