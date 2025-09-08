#include <iostream>
#include <polygon_mesh/polygon_mesh.hpp>

using namespace polygon_mesh;

int main() {
    std::cout << "=== Polygon Mesh Library - Basic Triangle Example ===" << std::endl;
    
    try {
        // Create a mesh
        core::Meshf mesh;
        
        // Add vertices for a simple triangle
        auto v1 = mesh.add_vertex(math::Vector3f(0.0f, 0.0f, 0.0f));
        auto v2 = mesh.add_vertex(math::Vector3f(1.0f, 0.0f, 0.0f));
        auto v3 = mesh.add_vertex(math::Vector3f(0.5f, 1.0f, 0.0f));
        
        std::cout << "Added 3 vertices:" << std::endl;
        std::cout << "  v1: " << mesh.get_vertex(v1).position << std::endl;
        std::cout << "  v2: " << mesh.get_vertex(v2).position << std::endl;
        std::cout << "  v3: " << mesh.get_vertex(v3).position << std::endl;
        
        // Add a triangle face
        auto face_id = mesh.add_triangle(v1, v2, v3);
        
        std::cout << "\nAdded triangle face with ID: " << face_id << std::endl;
        
        // Compute normals
        mesh.compute_normals();
        
        std::cout << "\nComputed normals:" << std::endl;
        const auto& face = mesh.get_face(face_id);
        std::cout << "  Face normal: " << face.normal << std::endl;
        
        for (size_t i = 0; i < 3; ++i) {
            const auto& vertex = mesh.get_vertex(i);
            std::cout << "  Vertex " << i << " normal: " << vertex.normal << std::endl;
        }
        
        // Get mesh statistics
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
        
        // Validate topology
        bool is_valid = mesh.validate_topology();
        std::cout << "\nTopology validation: " << (is_valid ? "PASSED" : "FAILED") << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\n=== Example completed successfully! ===" << std::endl;
    return 0;
}