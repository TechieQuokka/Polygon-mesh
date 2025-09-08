#include <polygon_mesh/polygon_mesh.hpp>
#include <iostream>
#include <iomanip>

using namespace polygon_mesh;

void demonstrate_basic_mesh_operations() {
    std::cout << "\n=== Basic Mesh Operations Demo ===\n";
    
    // Create a simple mesh
    core::Mesh<float> mesh;
    
    // Add vertices for a pyramid
    auto v0 = mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(0.0f, 0.0f, 0.0f)));    // Base center
    auto v1 = mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(1.0f, 0.0f, 1.0f)));    // Base corner 1
    auto v2 = mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(-1.0f, 0.0f, 1.0f)));   // Base corner 2
    auto v3 = mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(-1.0f, 0.0f, -1.0f)));  // Base corner 3
    auto v4 = mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(1.0f, 0.0f, -1.0f)));   // Base corner 4
    auto v5 = mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(0.0f, 2.0f, 0.0f)));    // Apex
    
    std::cout << "Added " << mesh.vertex_count() << " vertices\n";
    
    // Add base triangles (fan triangulation)
    mesh.add_triangle(v0, v1, v2);
    mesh.add_triangle(v0, v2, v3);
    mesh.add_triangle(v0, v3, v4);
    mesh.add_triangle(v0, v4, v1);
    
    // Add side triangles
    mesh.add_triangle(v1, v5, v2);
    mesh.add_triangle(v2, v5, v3);
    mesh.add_triangle(v3, v5, v4);
    mesh.add_triangle(v4, v5, v1);
    
    std::cout << "Added " << mesh.face_count() << " faces\n";
    
    // Compute bounding box
    auto bbox = mesh.bounding_box();
    std::cout << "Bounding box: min(" << bbox.min_point.x << ", " 
              << bbox.min_point.y << ", " << bbox.min_point.z << ") "
              << "max(" << bbox.max_point.x << ", " 
              << bbox.max_point.y << ", " << bbox.max_point.z << ")\n";
}

void demonstrate_math_operations() {
    std::cout << "\n=== Math Operations Demo ===\n";
    
    // Vector operations
    math::Vector3<float> v1(1.0f, 2.0f, 3.0f);
    math::Vector3<float> v2(4.0f, 5.0f, 6.0f);
    
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Vector v1: (" << v1.x << ", " << v1.y << ", " << v1.z << ")\n";
    std::cout << "Vector v2: (" << v2.x << ", " << v2.y << ", " << v2.z << ")\n";
    
    auto v3 = v1 + v2;
    std::cout << "v1 + v2: (" << v3.x << ", " << v3.y << ", " << v3.z << ")\n";
    
    auto dot = v1.dot(v2);
    std::cout << "v1 · v2: " << dot << "\n";
    
    auto cross = v1.cross(v2);
    std::cout << "v1 × v2: (" << cross.x << ", " << cross.y << ", " << cross.z << ")\n";
    
    std::cout << "Length of v1: " << v1.length() << "\n";
    
    auto normalized = v1.normalize();
    std::cout << "Normalized v1: (" << normalized.x << ", " << normalized.y << ", " << normalized.z << ")\n";
    
    // Matrix operations
    std::cout << "\n--- Matrix Operations ---\n";
    
    // Create transformation matrices
    auto translation = math::Matrix4<float>::translation(math::Vector3<float>(2.0f, 3.0f, 4.0f));
    auto rotation = math::Matrix4<float>::rotation_y(utils::math::degrees_to_radians(45.0f));
    auto scaling = math::Matrix4<float>::scaling(math::Vector3<float>(2.0f, 1.0f, 0.5f));
    
    // Combine transformations
    auto transform = translation * rotation * scaling;
    
    // Transform a point
    math::Vector3<float> point(1.0f, 0.0f, 0.0f);
    auto transformed_point = transform.transform_point(point);
    
    std::cout << "Original point: (" << point.x << ", " << point.y << ", " << point.z << ")\n";
    std::cout << "Transformed point: (" << transformed_point.x << ", " 
              << transformed_point.y << ", " << transformed_point.z << ")\n";
}

void demonstrate_utilities() {
    std::cout << "\n=== Utilities Demo ===\n";
    
    // String utilities
    std::string test_str = "  Hello, World!  ";
    std::cout << "Original: '" << test_str << "'\n";
    std::cout << "Trimmed: '" << utils::string::trim(test_str) << "'\n";
    std::cout << "Uppercase: '" << utils::string::to_upper(test_str) << "'\n";
    
    // Math utilities
    std::cout << "\n--- Math Utilities ---\n";
    std::cout << "Pi: " << utils::math::pi<float>() << "\n";
    std::cout << "45 degrees to radians: " << utils::math::degrees_to_radians(45.0f) << "\n";
    std::cout << "π/2 radians to degrees: " << utils::math::radians_to_degrees(utils::math::pi<float>() / 2.0f) << "\n";
    
    float a = 10.0f, b = 20.0f, t = 0.3f;
    std::cout << "Linear interpolation between " << a << " and " << b << " at t=" << t << ": " 
              << utils::math::lerp(a, b, t) << "\n";
    
    // Random utilities
    std::cout << "\n--- Random Utilities ---\n";
    utils::Random& rng = utils::global_random();
    rng.set_seed(42); // For reproducible results
    
    std::cout << "Random integer [1, 10]: " << rng.random_int(1, 10) << "\n";
    std::cout << "Random float [0, 1]: " << rng.random_float() << "\n";
    std::cout << "Random bool (50% chance): " << (rng.random_bool() ? "true" : "false") << "\n";
    
    // Filesystem utilities
    std::cout << "\n--- Filesystem Utilities ---\n";
    std::string filepath = "/path/to/file.obj";
    std::cout << "Full path: " << filepath << "\n";
    std::cout << "Extension: " << utils::filesystem::get_extension(filepath) << "\n";
    std::cout << "Filename: " << utils::filesystem::get_filename(filepath) << "\n";
    std::cout << "Directory: " << utils::filesystem::get_directory(filepath) << "\n";
    std::cout << "Stem: " << utils::filesystem::get_stem(filepath) << "\n";
}

void demonstrate_profiling() {
    std::cout << "\n=== Profiling Demo ===\n";
    
    // Enable profiler
    utils::Profiler::instance().enable();
    
    {
        PROFILE_SCOPE("mesh_creation");
        
        // Create and populate a mesh
        core::Mesh<float> mesh;
        for (int i = 0; i < 100; ++i) { // Reduced from 1000 for stability
            mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(
                static_cast<float>(i),
                static_cast<float>(i * i),
                static_cast<float>(i * 0.5f)
            )));
        }
        
        std::cout << "Created mesh with " << mesh.vertex_count() << " vertices\n";
    }
    
    {
        PROFILE_SCOPE("math_operations");
        
        math::Vector3<float> result;
        for (int i = 0; i < 1000; ++i) { // Reduced from 10000 for stability
            math::Vector3<float> v1(static_cast<float>(i), static_cast<float>(i + 1), static_cast<float>(i + 2));
            math::Vector3<float> v2(static_cast<float>(i * 2), static_cast<float>(i * 3), static_cast<float>(i * 4));
            result = v1.cross(v2);
        }
        
        std::cout << "Performed 1000 cross products, final result: (" 
                  << result.x << ", " << result.y << ", " << result.z << ")\n";
    }
    
    // Print profiling report
    utils::Profiler::instance().print_report();
}

void demonstrate_features() {
    std::cout << "\n=== Library Features ===\n";
    std::cout << "Version: " << polygon_mesh::version_string() << "\n";
    std::cout << "Matrix math support: " << (features::has_matrix_math() ? "Yes" : "No") << "\n";
    std::cout << "Algorithm support: " << (features::has_algorithms() ? "Yes" : "No") << "\n";
    std::cout << "File I/O support: " << (features::has_file_io() ? "Yes" : "No") << "\n";
    std::cout << "OBJ format support: " << (features::has_obj_support() ? "Yes" : "No") << "\n";
    std::cout << "PLY format support: " << (features::has_ply_support() ? "Yes" : "No") << "\n";
    std::cout << "Threading utilities: " << (features::has_threading_utils() ? "Yes" : "No") << "\n";
    std::cout << "Memory pool: " << (features::has_memory_pool() ? "Yes" : "No") << "\n";
    std::cout << "Profiling: " << (features::has_profiling() ? "Yes" : "No") << "\n";
    std::cout << "STL format support: " << (features::has_stl_support() ? "Yes" : "No (Future)") << "\n";
    std::cout << "GPU acceleration: " << (features::has_gpu_acceleration() ? "Yes" : "No (Future)") << "\n";
}

int main() {
    std::cout << "=== Polygon Mesh Library Stable Demo ===\n";
    
    try {
        demonstrate_features();
        demonstrate_basic_mesh_operations();
        demonstrate_math_operations();
        demonstrate_utilities();
        demonstrate_profiling();
        
        std::cout << "\n=== Demo completed successfully! ===\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}