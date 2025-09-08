# Tests CMakeLists.txt

# Basic test executable
add_executable(basic_test basic_test.cpp)
target_link_libraries(basic_test polygon_mesh::polygon_mesh)
target_compile_features(basic_test PRIVATE cxx_std_17)

# Comprehensive mesh test
add_executable(mesh_test mesh_test.cpp)
target_link_libraries(mesh_test polygon_mesh::polygon_mesh)
target_compile_features(mesh_test PRIVATE cxx_std_17)

# Performance test
add_executable(performance_test performance_test.cpp)
target_link_libraries(performance_test polygon_mesh::polygon_mesh)
target_compile_features(performance_test PRIVATE cxx_std_17)

# Add tests
add_test(NAME basic_test COMMAND basic_test)
add_test(NAME mesh_test COMMAND mesh_test)
add_test(NAME performance_test COMMAND performance_test)