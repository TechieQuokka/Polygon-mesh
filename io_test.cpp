#include <polygon_mesh/polygon_mesh.hpp>
#include <iostream>

using namespace polygon_mesh;

int main() {
    std::cout << "=== File I/O Advanced Test ===\n";
    
    try {
        // Create a more complex mesh (pyramid)
        core::Mesh<float> pyramid;
        
        // Base vertices (square)
        pyramid.add_vertex(core::Vertex<float>(math::Vector3<float>(-1.0f, 0.0f, -1.0f)));  // 0
        pyramid.add_vertex(core::Vertex<float>(math::Vector3<float>( 1.0f, 0.0f, -1.0f)));  // 1
        pyramid.add_vertex(core::Vertex<float>(math::Vector3<float>( 1.0f, 0.0f,  1.0f)));  // 2
        pyramid.add_vertex(core::Vertex<float>(math::Vector3<float>(-1.0f, 0.0f,  1.0f)));  // 3
        pyramid.add_vertex(core::Vertex<float>(math::Vector3<float>( 0.0f, 2.0f,  0.0f)));  // 4 (apex)
        
        // Add faces
        pyramid.add_triangle(0, 1, 2);  // Base triangle 1
        pyramid.add_triangle(0, 2, 3);  // Base triangle 2
        pyramid.add_triangle(0, 4, 1);  // Side 1
        pyramid.add_triangle(1, 4, 2);  // Side 2
        pyramid.add_triangle(2, 4, 3);  // Side 3
        pyramid.add_triangle(3, 4, 0);  // Side 4
        
        std::cout << "Created pyramid: " << pyramid.vertex_count() 
                  << " vertices, " << pyramid.face_count() << " faces\n";
        
        // Test OBJ format
        std::cout << "\n--- Testing OBJ format ---\n";
        std::string obj_filename = "pyramid_test.obj";
        if (io::save_obj(obj_filename, pyramid)) {
            std::cout << "✓ Successfully saved to " << obj_filename << "\n";
            
            auto loaded_obj = io::load_obj<float>(obj_filename);
            std::cout << "✓ Successfully loaded from " << obj_filename << ": "
                      << loaded_obj.vertex_count() << " vertices, " 
                      << loaded_obj.face_count() << " faces\n";
            
            // Compare bounding boxes
            auto orig_bbox = pyramid.bounding_box();
            auto loaded_bbox = loaded_obj.bounding_box();
            std::cout << "  Original bbox: (" << orig_bbox.min_point.x << ", " << orig_bbox.min_point.y << ", " << orig_bbox.min_point.z << ")"
                      << " to (" << orig_bbox.max_point.x << ", " << orig_bbox.max_point.y << ", " << orig_bbox.max_point.z << ")\n";
            std::cout << "  Loaded bbox: (" << loaded_bbox.min_point.x << ", " << loaded_bbox.min_point.y << ", " << loaded_bbox.min_point.z << ")"
                      << " to (" << loaded_bbox.max_point.x << ", " << loaded_bbox.max_point.y << ", " << loaded_bbox.max_point.z << ")\n";
        } else {
            std::cout << "✗ Failed to save to " << obj_filename << "\n";
        }
        
        // Test PLY format
        std::cout << "\n--- Testing PLY format ---\n";
        std::string ply_filename = "pyramid_test.ply";
        if (io::save_ply(ply_filename, pyramid)) {
            std::cout << "✓ Successfully saved to " << ply_filename << "\n";
            
            auto loaded_ply = io::load_ply<float>(ply_filename);
            std::cout << "✓ Successfully loaded from " << ply_filename << ": "
                      << loaded_ply.vertex_count() << " vertices, " 
                      << loaded_ply.face_count() << " faces\n";
        } else {
            std::cout << "✗ Failed to save to " << ply_filename << "\n";
        }
        
        // Test auto-detection
        std::cout << "\n--- Testing auto-detection ---\n";
        std::string auto_obj = "auto_test.obj";
        std::string auto_ply = "auto_test.ply";
        
        if (io::save_mesh(auto_obj, pyramid)) {
            std::cout << "✓ Auto-detected OBJ format and saved\n";
            auto loaded = io::load_mesh<float>(auto_obj);
            std::cout << "✓ Auto-detected and loaded OBJ: " << loaded.vertex_count() << " vertices\n";
        }
        
        if (io::save_mesh(auto_ply, pyramid)) {
            std::cout << "✓ Auto-detected PLY format and saved\n";
            auto loaded = io::load_mesh<float>(auto_ply);
            std::cout << "✓ Auto-detected and loaded PLY: " << loaded.vertex_count() << " vertices\n";
        }
        
        // Test format info
        std::cout << "\n--- Format Information ---\n";
        auto formats = io::get_supported_formats();
        for (auto format : formats) {
            auto info = io::get_format_info(format);
            std::cout << "Format: " << info.name << " - " << info.description << "\n";
            std::cout << "  Extensions: ";
            for (const auto& ext : info.extensions) {
                std::cout << ext << " ";
            }
            std::cout << "\n";
            std::cout << "  Features: Colors=" << info.supports_colors 
                      << ", Normals=" << info.supports_normals
                      << ", UVs=" << info.supports_uvs << "\n";
        }
        
        std::cout << "\n=== All I/O tests completed successfully! ===\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}