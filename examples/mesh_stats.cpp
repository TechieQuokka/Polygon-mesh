#include <iostream>
#include <polygon_mesh/polygon_mesh.hpp>

using namespace polygon_mesh;

int main() {
    std::cout << "=== Mesh Statistics Example ===" << std::endl;
    
    try {
        // Create a simple cube mesh
        core::Meshf mesh;
        
        // Add vertices for a cube
        std::vector<uint32_t> vertices;
        vertices.push_back(mesh.add_vertex(math::Vector3f(-1.0f, -1.0f, -1.0f))); // 0
        vertices.push_back(mesh.add_vertex(math::Vector3f( 1.0f, -1.0f, -1.0f))); // 1
        vertices.push_back(mesh.add_vertex(math::Vector3f( 1.0f,  1.0f, -1.0f))); // 2
        vertices.push_back(mesh.add_vertex(math::Vector3f(-1.0f,  1.0f, -1.0f))); // 3
        vertices.push_back(mesh.add_vertex(math::Vector3f(-1.0f, -1.0f,  1.0f))); // 4
        vertices.push_back(mesh.add_vertex(math::Vector3f( 1.0f, -1.0f,  1.0f))); // 5
        vertices.push_back(mesh.add_vertex(math::Vector3f( 1.0f,  1.0f,  1.0f))); // 6
        vertices.push_back(mesh.add_vertex(math::Vector3f(-1.0f,  1.0f,  1.0f))); // 7
        
        // Add cube faces (12 triangles)
        // Front face
        mesh.add_triangle(vertices[0], vertices[1], vertices[2]);
        mesh.add_triangle(vertices[0], vertices[2], vertices[3]);
        // Back face  
        mesh.add_triangle(vertices[4], vertices[6], vertices[5]);
        mesh.add_triangle(vertices[4], vertices[7], vertices[6]);
        // Left face
        mesh.add_triangle(vertices[0], vertices[3], vertices[7]);
        mesh.add_triangle(vertices[0], vertices[7], vertices[4]);
        // Right face
        mesh.add_triangle(vertices[1], vertices[5], vertices[6]);
        mesh.add_triangle(vertices[1], vertices[6], vertices[2]);
        // Top face
        mesh.add_triangle(vertices[3], vertices[2], vertices[6]);
        mesh.add_triangle(vertices[3], vertices[6], vertices[7]);
        // Bottom face
        mesh.add_triangle(vertices[0], vertices[4], vertices[5]);
        mesh.add_triangle(vertices[0], vertices[5], vertices[1]);
        
        // Compute statistics
        std::cout << "\nMesh Statistics:" << std::endl;
        std::cout << "  Vertices: " << mesh.vertex_count() << std::endl;
        std::cout << "  Edges: " << mesh.edge_count() << std::endl;
        std::cout << "  Faces: " << mesh.face_count() << std::endl;
        std::cout << "  Surface area: " << mesh.surface_area() << std::endl;
        
        // Get bounding box
        const auto& bbox = mesh.bounding_box();
        std::cout << "\nBounding Box:" << std::endl;
        std::cout << "  Min: " << bbox.min_point << std::endl;
        std::cout << "  Max: " << bbox.max_point << std::endl;
        std::cout << "  Center: " << bbox.center() << std::endl;
        std::cout << "  Size: " << bbox.size() << std::endl;
        
        // Topology validation
        bool is_valid = mesh.validate_topology();
        std::cout << "\nTopology validation: " << (is_valid ? "PASSED" : "FAILED") << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\n=== Example completed successfully! ===" << std::endl;
    return 0;
}