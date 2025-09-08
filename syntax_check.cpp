// Syntax and compilation check for polygon mesh library
// This file tests basic inclusion and instantiation without execution

#define COMPILE_CHECK_ONLY
#include "include/polygon_mesh/polygon_mesh.hpp"

// Template instantiation tests
template class polygon_mesh::math::Vector3<float>;
template class polygon_mesh::math::Vector3<double>;
template class polygon_mesh::core::Vertex<float>;
template class polygon_mesh::core::Face<float>;
template class polygon_mesh::core::Mesh<float>;

int main() {
    using namespace polygon_mesh;
    
    // Test basic construction (should compile without runtime)
    math::Vector3f vec1;
    math::Vector3f vec2(1.0f, 2.0f, 3.0f);
    core::Meshf mesh;
    
    // Test basic operations
    auto vec3 = vec1 + vec2;
    auto vertex_id = mesh.add_vertex({0.0f, 0.0f, 0.0f});
    
    // Test type aliases
    static_assert(std::is_same_v<core::Meshf, core::Mesh<float>>);
    static_assert(std::is_same_v<math::Vector3f, math::Vector3<float>>);
    
    return 0;
}