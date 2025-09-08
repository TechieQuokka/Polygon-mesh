# Examples CMakeLists.txt

# Basic triangle example
add_executable(basic_triangle basic_triangle.cpp)
target_link_libraries(basic_triangle polygon_mesh::polygon_mesh)
target_compile_features(basic_triangle PRIVATE cxx_std_17)

# Cube example
add_executable(cube_mesh cube_mesh.cpp)
target_link_libraries(cube_mesh polygon_mesh::polygon_mesh)
target_compile_features(cube_mesh PRIVATE cxx_std_17)

# Mesh statistics example
add_executable(mesh_stats mesh_stats.cpp)
target_link_libraries(mesh_stats polygon_mesh::polygon_mesh)
target_compile_features(mesh_stats PRIVATE cxx_std_17)

# Advanced mesh generation example
add_executable(advanced_mesh advanced_mesh.cpp)
target_link_libraries(advanced_mesh polygon_mesh::polygon_mesh)
target_compile_features(advanced_mesh PRIVATE cxx_std_17)