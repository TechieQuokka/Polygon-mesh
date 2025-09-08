#include <iostream>
#include <polygon_mesh/polygon_mesh.hpp>

using namespace polygon_mesh;

int main() {
    std::cout << "=== Polygon Mesh Library - Cube Example ===" << std::endl;
    
    try {
        // Create a mesh for a unit cube
        core::Meshf mesh;
        
        // Add vertices for a unit cube
        std::cout << "Creating unit cube vertices..." << std::endl;
        
        // Bottom face (z = 0)
        auto v0 = mesh.add_vertex(math::Vector3f(-0.5f, -0.5f, -0.5f));
        auto v1 = mesh.add_vertex(math::Vector3f( 0.5f, -0.5f, -0.5f));
        auto v2 = mesh.add_vertex(math::Vector3f( 0.5f,  0.5f, -0.5f));
        auto v3 = mesh.add_vertex(math::Vector3f(-0.5f,  0.5f, -0.5f));
        
        // Top face (z = 1)
        auto v4 = mesh.add_vertex(math::Vector3f(-0.5f, -0.5f,  0.5f));
        auto v5 = mesh.add_vertex(math::Vector3f( 0.5f, -0.5f,  0.5f));
        auto v6 = mesh.add_vertex(math::Vector3f( 0.5f,  0.5f,  0.5f));
        auto v7 = mesh.add_vertex(math::Vector3f(-0.5f,  0.5f,  0.5f));
        
        std::cout << "Added " << mesh.vertex_count() << " vertices" << std::endl;
        
        // Add faces (using quads)
        std::cout << "Adding cube faces..." << std::endl;
        
        // Bottom face
        mesh.add_quad(v0, v1, v2, v3);
        
        // Top face
        mesh.add_quad(v4, v7, v6, v5);
        
        // Front face
        mesh.add_quad(v0, v4, v5, v1);
        
        // Back face
        mesh.add_quad(v2, v6, v7, v3);
        
        // Left face
        mesh.add_quad(v0, v3, v7, v4);
        
        // Right face
        mesh.add_quad(v1, v5, v6, v2);
        
        std::cout << "Added " << mesh.face_count() << " faces" << std::endl;
        
        // Compute normals
        std::cout << "Computing normals..." << std::endl;
        mesh.compute_normals();
        
        // Display mesh statistics
        std::cout << "\nCube Mesh Statistics:" << std::endl;
        std::cout << "  Vertices: " << mesh.vertex_count() << std::endl;
        std::cout << "  Edges: " << mesh.edge_count() << std::endl;
        std::cout << "  Faces: " << mesh.face_count() << std::endl;
        std::cout << "  Surface area: " << mesh.surface_area() << std::endl;
        std::cout << "  Volume: " << mesh.volume() << std::endl;
        
        // Expected values for unit cube:
        // Surface area = 6 (6 faces of area 1 each)
        // Volume = 1
        
        // Get bounding box
        const auto& bbox = mesh.bounding_box();
        std::cout << "\nBounding Box:" << std::endl;
        std::cout << "  Min: " << bbox.min_point << std::endl;
        std::cout << "  Max: " << bbox.max_point << std::endl;
        std::cout << "  Center: " << bbox.center() << std::endl;
        std::cout << "  Size: " << bbox.size() << std::endl;
        std::cout << "  Volume: " << bbox.volume() << std::endl;
        
        // Validate topology
        bool is_valid = mesh.validate_topology();
        std::cout << "\nTopology validation: " << (is_valid ? "PASSED" : "FAILED") << std::endl;
        
        // Display some face information
        std::cout << "\nFace Information:" << std::endl;
        for (size_t i = 0; i < std::min(mesh.face_count(), size_t(3)); ++i) {
            const auto& face = mesh.get_face(i);
            std::cout << "  Face " << i << ": " << face.vertex_count() << " vertices, normal: " << face.normal << std::endl;
        }
        
        // Display some edge information
        std::cout << "\nEdge Information:" << std::endl;
        std::cout << "  Total edges: " << mesh.edge_count() << std::endl;
        
        // Count boundary edges (should be 0 for a closed cube)
        size_t boundary_edges = 0;
        for (size_t i = 0; i < mesh.edge_count(); ++i) {
            const auto& edge = mesh.get_edge(i);
            if (edge.is_boundary) {
                boundary_edges++;
            }
        }
        std::cout << "  Boundary edges: " << boundary_edges << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\n=== Cube example completed successfully! ===" << std::endl;
    return 0;
}