# Build Instructions

## Requirements

### Windows
- **Visual Studio 2017** or later (with C++17 support)
- **CMake 3.16** or later  
- **Git** (for cloning)

Alternative:
- **MinGW-w64** with GCC 7+
- **MSYS2** environment

### Linux/macOS
- **GCC 7+** or **Clang 5+**
- **CMake 3.16+**
- **Make** or **Ninja**

## Installation

### Option 1: Visual Studio (Windows)

1. Install Visual Studio 2019/2022 with C++ workload
2. Install CMake from [cmake.org](https://cmake.org/download/)
3. Open Developer Command Prompt

```cmd
git clone <repository-url>
cd polygon-mesh
mkdir build && cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release
```

### Option 2: MinGW/MSYS2 (Windows)

1. Install MSYS2 from [msys2.org](https://www.msys2.org/)
2. Update package database:
```bash
pacman -Syu
```

3. Install development tools:
```bash
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-cmake  
pacman -S mingw-w64-x86_64-make
```

4. Build project:
```bash
cd /c/path/to/polygon-mesh
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

### Option 3: Linux/macOS

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential cmake

# CentOS/RHEL
sudo yum groupinstall "Development Tools"
sudo yum install cmake

# macOS (with Homebrew)
brew install cmake

# Build
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Running Examples

After successful build:

```bash
# Windows
.\examples\basic_triangle.exe
.\examples\cube_mesh.exe

# Linux/macOS  
./examples/basic_triangle
./examples/cube_mesh
```

## Testing Compilation (Header-Only)

Since this is a header-only library, you can test compilation with a simple file:

```cpp
#include <polygon_mesh/polygon_mesh.hpp>
using namespace polygon_mesh;

int main() {
    core::Meshf mesh;
    auto v1 = mesh.add_vertex({0,0,0});
    auto v2 = mesh.add_vertex({1,0,0});  
    auto v3 = mesh.add_vertex({0,1,0});
    mesh.add_triangle(v1, v2, v3);
    return 0;
}
```

Compile with:
```bash
# GCC/Clang
g++ -std=c++17 -I./include test.cpp -o test

# MSVC  
cl /std:c++17 /I.\include test.cpp
```

## Troubleshooting

### Common Issues

1. **C++17 not supported**: Update compiler
2. **CMake not found**: Add to PATH or install
3. **Header not found**: Check include paths
4. **Link errors**: This is header-only, no linking required

### Verification

Check your setup:
```bash
# Check compiler
gcc --version
clang --version

# Check CMake
cmake --version

# Check C++17 support
echo '#if __cplusplus < 201703L
#error "C++17 required"
#endif
int main(){}' > test.cpp && g++ -std=c++17 test.cpp
```

## IDE Setup

### Visual Studio Code
1. Install C/C++ extension
2. Configure `c_cpp_properties.json`:
```json
{
    "configurations": [{
        "name": "Win32",
        "includePath": ["${workspaceFolder}/include"],
        "cppStandard": "c++17"
    }]
}
```

### CLion
1. Open project folder
2. CMake will be detected automatically
3. Build configurations will be available

### Visual Studio
1. Open folder or use CMake integration
2. Configure include paths in project settings