#include <polygon_mesh/polygon_mesh.hpp>
#include <iostream>
#include <iomanip>

using namespace polygon_mesh;

void demonstrate_basic_matrix_operations() {
    std::cout << "=== Basic Matrix Operations ===\n";
    
    // Create identity matrix
    Matrix4 identity;
    identity.identity();
    std::cout << "Identity matrix created\n";
    
    // Create custom matrix
    Matrix4 custom(
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f
    );
    
    std::cout << "Custom matrix elements (0,0): " << custom(0,0) << ", (1,2): " << custom(1,2) << "\n";
    
    // Matrix addition
    auto sum = identity + custom;
    std::cout << "Matrix addition result (0,0): " << sum(0,0) << "\n";
    
    // Matrix multiplication
    auto product = identity * custom;
    std::cout << "Matrix multiplication result (0,0): " << product(0,0) << "\n";
    
    // Scalar multiplication
    auto scaled = custom * 2.0f;
    std::cout << "Scalar multiplication result (0,0): " << scaled(0,0) << "\n";
}

void demonstrate_transformations() {
    std::cout << "\n=== Transformation Matrices ===\n";
    
    // Translation matrix
    Vector3 translation_vec(5.0f, 3.0f, -2.0f);
    auto translation = Matrix4::translation(translation_vec);
    std::cout << "Translation matrix created for vector (" 
              << translation_vec.x << ", " << translation_vec.y << ", " << translation_vec.z << ")\n";
    
    // Scaling matrix
    Vector3 scale_vec(2.0f, 1.5f, 0.5f);
    auto scaling = Matrix4::scaling(scale_vec);
    std::cout << "Scaling matrix created for vector (" 
              << scale_vec.x << ", " << scale_vec.y << ", " << scale_vec.z << ")\n";
    
    // Uniform scaling
    auto uniform_scaling = Matrix4::scaling(2.0f);
    std::cout << "Uniform scaling matrix created with factor 2.0\n";
    
    // Rotation matrices
    float angle_degrees = 45.0f;
    float angle_radians = utils::math::degrees_to_radians(angle_degrees);
    
    auto rotation_x = Matrix4::rotation_x(angle_radians);
    auto rotation_y = Matrix4::rotation_y(angle_radians);
    auto rotation_z = Matrix4::rotation_z(angle_radians);
    
    std::cout << "Rotation matrices created for " << angle_degrees << " degrees around X, Y, Z axes\n";
    
    // Axis-angle rotation
    Vector3 axis(1.0f, 1.0f, 0.0f);
    axis = axis.normalize();
    auto axis_rotation = Matrix4::rotation_axis(axis, angle_radians);
    std::cout << "Axis-angle rotation matrix created around normalized axis (" 
              << axis.x << ", " << axis.y << ", " << axis.z << ")\n";
}

void demonstrate_view_projection() {
    std::cout << "\n=== View and Projection Matrices ===\n";
    
    // Look-at view matrix
    Vector3 eye(0.0f, 0.0f, 5.0f);
    Vector3 target(0.0f, 0.0f, 0.0f);
    Vector3 up(0.0f, 1.0f, 0.0f);
    
    auto view = Matrix4::look_at(eye, target, up);
    std::cout << "Look-at view matrix created\n";
    std::cout << "  Eye: (" << eye.x << ", " << eye.y << ", " << eye.z << ")\n";
    std::cout << "  Target: (" << target.x << ", " << target.y << ", " << target.z << ")\n";
    std::cout << "  Up: (" << up.x << ", " << up.y << ", " << up.z << ")\n";
    
    // Perspective projection
    float fov_degrees = 60.0f;
    float fov_radians = utils::math::degrees_to_radians(fov_degrees);
    float aspect_ratio = 16.0f / 9.0f;
    float near_plane = 0.1f;
    float far_plane = 100.0f;
    
    auto perspective = Matrix4::perspective(fov_radians, aspect_ratio, near_plane, far_plane);
    std::cout << "Perspective projection matrix created\n";
    std::cout << "  FOV: " << fov_degrees << " degrees\n";
    std::cout << "  Aspect ratio: " << aspect_ratio << "\n";
    std::cout << "  Near plane: " << near_plane << "\n";
    std::cout << "  Far plane: " << far_plane << "\n";
    
    // Orthographic projection
    float left = -10.0f, right = 10.0f;
    float bottom = -5.0f, top = 5.0f;
    float ortho_near = -1.0f, ortho_far = 1.0f;
    
    auto orthographic = Matrix4::orthographic(left, right, bottom, top, ortho_near, ortho_far);
    std::cout << "Orthographic projection matrix created\n";
    std::cout << "  Left: " << left << ", Right: " << right << "\n";
    std::cout << "  Bottom: " << bottom << ", Top: " << top << "\n";
    std::cout << "  Near: " << ortho_near << ", Far: " << ortho_far << "\n";
}

void demonstrate_transforming_points() {
    std::cout << "\n=== Transforming Points and Vectors ===\n";
    
    std::cout << std::fixed << std::setprecision(3);
    
    // Original point and vector
    Vector3 point(1.0f, 2.0f, 3.0f);
    Vector3 vector(1.0f, 0.0f, 0.0f);
    
    std::cout << "Original point: (" << point.x << ", " << point.y << ", " << point.z << ")\n";
    std::cout << "Original vector: (" << vector.x << ", " << vector.y << ", " << vector.z << ")\n";
    
    // Apply translation
    auto translation = Matrix4::translation(Vector3(5.0f, 0.0f, 0.0f));
    auto translated_point = translation.transform_point(point);
    auto translated_vector = translation.transform_vector(vector);
    
    std::cout << "\nAfter translation by (5, 0, 0):\n";
    std::cout << "  Point: (" << translated_point.x << ", " << translated_point.y << ", " << translated_point.z << ")\n";
    std::cout << "  Vector: (" << translated_vector.x << ", " << translated_vector.y << ", " << translated_vector.z << ")\n";
    
    // Apply rotation
    float angle = utils::math::degrees_to_radians(90.0f);
    auto rotation = Matrix4::rotation_z(angle);
    auto rotated_point = rotation.transform_point(point);
    auto rotated_vector = rotation.transform_vector(vector);
    
    std::cout << "\nAfter 90-degree Z rotation:\n";
    std::cout << "  Point: (" << rotated_point.x << ", " << rotated_point.y << ", " << rotated_point.z << ")\n";
    std::cout << "  Vector: (" << rotated_vector.x << ", " << rotated_vector.y << ", " << rotated_vector.z << ")\n";
    
    // Apply scaling
    auto scaling = Matrix4::scaling(Vector3(2.0f, 0.5f, 1.0f));
    auto scaled_point = scaling.transform_point(point);
    auto scaled_vector = scaling.transform_vector(vector);
    
    std::cout << "\nAfter scaling by (2, 0.5, 1):\n";
    std::cout << "  Point: (" << scaled_point.x << ", " << scaled_point.y << ", " << scaled_point.z << ")\n";
    std::cout << "  Vector: (" << scaled_vector.x << ", " << scaled_vector.y << ", " << scaled_vector.z << ")\n";
    
    // Combined transformation
    auto combined = translation * rotation * scaling;
    auto final_point = combined.transform_point(point);
    auto final_vector = combined.transform_vector(vector);
    
    std::cout << "\nAfter combined transformation (T * R * S):\n";
    std::cout << "  Point: (" << final_point.x << ", " << final_point.y << ", " << final_point.z << ")\n";
    std::cout << "  Vector: (" << final_vector.x << ", " << final_vector.y << ", " << final_vector.z << ")\n";
}

void demonstrate_advanced_operations() {
    std::cout << "\n=== Advanced Matrix Operations ===\n";
    
    // Create a test matrix
    Matrix4 test_matrix(
        2.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 3.0f, 0.0f, 2.0f,
        0.0f, 0.0f, 4.0f, 3.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    
    std::cout << std::fixed << std::setprecision(6);
    
    // Calculate determinant
    float det = test_matrix.determinant();
    std::cout << "Matrix determinant: " << det << "\n";
    
    // Transpose
    auto transposed = test_matrix.transpose();
    std::cout << "Matrix transposed (element [0,3]): " << transposed(0,3) << "\n";
    
    // Matrix comparison
    Matrix4 identity;
    identity.identity();
    
    bool is_identity = (test_matrix == identity);
    std::cout << "Test matrix equals identity: " << (is_identity ? "true" : "false") << "\n";
    
    bool not_identity = (test_matrix != identity);
    std::cout << "Test matrix not equals identity: " << (not_identity ? "true" : "false") << "\n";
}

void demonstrate_mesh_transformation() {
    std::cout << "\n=== Mesh Transformation Example ===\n";
    
    // Create a simple cube mesh
    Mesh cube;
    
    // Add cube vertices
    cube.add_vertex(Vector3(-1.0f, -1.0f, -1.0f)); // 0
    cube.add_vertex(Vector3( 1.0f, -1.0f, -1.0f)); // 1
    cube.add_vertex(Vector3( 1.0f,  1.0f, -1.0f)); // 2
    cube.add_vertex(Vector3(-1.0f,  1.0f, -1.0f)); // 3
    cube.add_vertex(Vector3(-1.0f, -1.0f,  1.0f)); // 4
    cube.add_vertex(Vector3( 1.0f, -1.0f,  1.0f)); // 5
    cube.add_vertex(Vector3( 1.0f,  1.0f,  1.0f)); // 6
    cube.add_vertex(Vector3(-1.0f,  1.0f,  1.0f)); // 7
    
    std::cout << "Created cube with " << cube.vertex_count() << " vertices\n";
    
    // Create transformation matrix (scale by 2, rotate 45 degrees around Y, translate by (3, 1, 0))
    auto scale = Matrix4::scaling(2.0f);
    auto rotation = Matrix4::rotation_y(utils::math::degrees_to_radians(45.0f));
    auto translation = Matrix4::translation(Vector3(3.0f, 1.0f, 0.0f));
    
    auto transform = translation * rotation * scale;
    
    // Apply transformation to all vertices
    auto vertices = cube.vertices(); // Get copy of vertices
    std::cout << "\nOriginal vertex 0: (" << vertices[0].position.x << ", " 
              << vertices[0].position.y << ", " << vertices[0].position.z << ")\n";
    
    Vector3 transformed_pos = transform.transform_point(vertices[0].position);
    std::cout << "Transformed vertex 0: (" << transformed_pos.x << ", " 
              << transformed_pos.y << ", " << transformed_pos.z << ")\n";
    
    // Calculate original and transformed bounding boxes
    auto original_bbox = cube.bounding_box();
    std::cout << "\nOriginal bounding box:\n";
    std::cout << "  Min: (" << original_bbox.min_point.x << ", " 
              << original_bbox.min_point.y << ", " << original_bbox.min_point.z << ")\n";
    std::cout << "  Max: (" << original_bbox.max_point.x << ", " 
              << original_bbox.max_point.y << ", " << original_bbox.max_point.z << ")\n";
    
    // For transformed bbox, we'd need to transform all vertices and recompute
    std::cout << "Transformation applied successfully\n";
}

int main() {
    std::cout << "=== Matrix4 Operations Demo ===\n\n";
    
    try {
        demonstrate_basic_matrix_operations();
        demonstrate_transformations();
        demonstrate_view_projection();
        demonstrate_transforming_points();
        demonstrate_advanced_operations();
        demonstrate_mesh_transformation();
        
        std::cout << "\n=== Matrix demo completed successfully! ===\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}