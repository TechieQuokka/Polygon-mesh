# Polygon Mesh Library

A high-performance C++ library for polygon mesh processing, designed for 3D graphics applications, game engines, and CAD systems.

## Features

- **Modern C++17/20**: Template-based design with strong type safety
- **Header-Only**: Easy integration with no compilation dependencies
- **High Performance**: Optimized data structures and algorithms
- **Extensible**: Modular architecture for custom algorithms
- **Cross-Platform**: Windows, Linux, and macOS support

## Core Components

### Data Structures
- **Vertex**: Position, normal, UV coordinates
- **Edge**: Half-edge data structure with adjacency information  
- **Face**: Polygonal faces with normal vectors
- **Mesh**: Container with topology management

### Math Utilities
- **Vector2/Vector3**: Full vector algebra operations
- **Bounding Box**: Spatial bounds computation
- **Math Utils**: Constants, interpolation, and utility functions

## Quick Start

### Requirements
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.16+ (for building examples and tests)

### Basic Usage

```cpp
#include <polygon_mesh/polygon_mesh.hpp>
using namespace polygon_mesh;

// Create a mesh
core::Meshf mesh;

// Add vertices
auto v1 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
auto v2 = mesh.add_vertex({1.0f, 0.0f, 0.0f});
auto v3 = mesh.add_vertex({0.5f, 1.0f, 0.0f});

// Add triangle face
mesh.add_triangle(v1, v2, v3);

// Compute normals
mesh.compute_normals();

// Get statistics
std::cout << "Vertices: " << mesh.vertex_count() << std::endl;
std::cout << "Surface area: " << mesh.surface_area() << std::endl;
```

### Building Examples

```bash
mkdir build && cd build
cmake ..
make
./examples/basic_triangle
./examples/cube_mesh
```

## Project Structure

```
polygon-mesh/
├── include/polygon_mesh/    # Header files
│   ├── core/               # Core data structures
│   ├── math/               # Math utilities
│   ├── algorithms/         # Processing algorithms
│   └── io/                 # File I/O (planned)
├── examples/               # Usage examples
├── tests/                  # Unit tests (planned)
└── docs/                   # Documentation
```

## Architecture Overview

The library follows a layered architecture:

1. **Foundation Layer**: Memory management, threading support
2. **Core Layer**: Basic data structures (Mesh, Vertex, Face, Edge)
3. **Algorithm Layer**: Mesh processing and geometric algorithms
4. **Application Layer**: High-level interfaces and utilities

### Design Principles

- **RAII**: Resource management through constructors/destructors
- **Template-Based**: Generic programming for type flexibility
- **STL Integration**: Leverages standard library containers and algorithms
- **Performance First**: Optimized for large mesh processing

## Mesh Operations

### Basic Operations
- Add/remove vertices, edges, faces
- Topology validation and manifold checking
- Bounding box computation
- Surface area and volume calculation

### Geometric Computations
- Face and vertex normal computation
- Mesh validation and topology analysis
- Spatial queries and intersection testing

## Future Features

### Planned Algorithms
- Mesh decimation (edge collapse, vertex clustering)
- Mesh smoothing (Laplacian, Taubin)
- Subdivision surfaces (Loop, Catmull-Clark)
- Delaunay triangulation
- Collision detection (octree, BVH)

### I/O Support
- OBJ file format
- PLY file format
- STL file format

### Advanced Features
- GPU acceleration (CUDA/OpenCL)
- Parallel processing (OpenMP/TBB)
- Mesh repair algorithms
- Shape analysis tools

## Contributing

We welcome contributions! Please see our [Architecture Documentation](docs/architecture.md) for detailed design information.

### Development Setup
1. Clone the repository
2. Install dependencies (CMake, C++17 compiler)
3. Build and run tests
4. Follow coding standards and add tests for new features

## License

[License information to be added]

## Acknowledgments

Built with modern C++ best practices and inspired by leading mesh processing libraries like CGAL, OpenMesh, and libigl.