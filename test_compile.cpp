// Simple compilation test for polygon mesh library
#include <iostream>
#include "include/polygon_mesh/polygon_mesh.hpp"

int main() {
    std::cout << "=== Compilation Test ===" << std::endl;
    
    try {
        // Test Vector3 creation and operations
        polygon_mesh::math::Vector3f v1(1.0f, 2.0f, 3.0f);
        polygon_mesh::math::Vector3f v2(4.0f, 5.0f, 6.0f);
        auto v3 = v1 + v2;
        
        std::cout << "Vector operations work: " << v3.x << ", " << v3.y << ", " << v3.z << std::endl;
        
        // Test Mesh creation
        polygon_mesh::core::Meshf mesh;
        
        // Add simple triangle
        auto vid1 = mesh.add_vertex(polygon_mesh::math::Vector3f(0.0f, 0.0f, 0.0f));
        auto vid2 = mesh.add_vertex(polygon_mesh::math::Vector3f(1.0f, 0.0f, 0.0f));  
        auto vid3 = mesh.add_vertex(polygon_mesh::math::Vector3f(0.5f, 1.0f, 0.0f));
        
        mesh.add_triangle(vid1, vid2, vid3);
        
        std::cout << "Mesh creation works: " << mesh.vertex_count() << " vertices, " 
                  << mesh.face_count() << " faces" << std::endl;
        
        // Compute normals
        mesh.compute_normals();
        
        std::cout << "Normal computation works!" << std::endl;
        
        // Test bounding box
        auto bbox = mesh.bounding_box();
        std::cout << "Bounding box: min(" << bbox.min_point.x << "," << bbox.min_point.y << "," << bbox.min_point.z 
                  << ") max(" << bbox.max_point.x << "," << bbox.max_point.y << "," << bbox.max_point.z << ")" << std::endl;
        
        std::cout << "=== All tests passed! ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}