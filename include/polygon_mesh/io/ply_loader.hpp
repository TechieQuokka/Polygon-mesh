#pragma once

#include <polygon_mesh/core/mesh.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace polygon_mesh {
namespace io {

// PLY file format loader and saver
class PLYLoader {
public:
    enum class Format {
        ASCII,
        BINARY_LITTLE_ENDIAN,
        BINARY_BIG_ENDIAN
    };

    struct Property {
        std::string name;
        std::string type;
        bool is_list = false;
        std::string count_type;
    };

    struct Element {
        std::string name;
        std::size_t count;
        std::vector<Property> properties;
    };

    // Load mesh from PLY file
    template<typename T>
    static core::Mesh<T> load(const std::string& filepath) {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open PLY file: " + filepath);
        }

        // Parse header
        Format format;
        std::vector<Element> elements;
        parse_header(file, format, elements);

        core::Mesh<T> mesh;

        // Find vertex and face elements
        const Element* vertex_element = nullptr;
        const Element* face_element = nullptr;

        for (const auto& element : elements) {
            if (element.name == "vertex") {
                vertex_element = &element;
            } else if (element.name == "face") {
                face_element = &element;
            }
        }

        if (!vertex_element) {
            throw std::runtime_error("PLY file does not contain vertex data");
        }

        // Read vertices
        std::vector<math::Vector3<T>> positions;
        std::vector<math::Vector3<T>> normals;
        std::vector<math::Vector2<T>> uvs;

        read_vertices(file, *vertex_element, format, positions, normals, uvs);

        // Add vertices to mesh
        for (std::size_t i = 0; i < positions.size(); ++i) {
            math::Vector3<T> normal = i < normals.size() ? normals[i] : math::Vector3<T>(0);
            math::Vector2<T> uv = i < uvs.size() ? uvs[i] : math::Vector2<T>(0);
            mesh.add_vertex(core::Vertex<T>(positions[i], normal, uv));
        }

        // Read faces if available
        if (face_element) {
            read_faces(file, *face_element, format, mesh);
        }

        return mesh;
    }

    // Save mesh to PLY file
    template<typename T>
    static bool save(const std::string& filepath, const core::Mesh<T>& mesh, 
                     Format format = Format::ASCII) {
        try {
            std::ofstream file(filepath, format == Format::ASCII ? 
                              std::ios::out : std::ios::binary);
            if (!file.is_open()) {
                return false;
            }

            // Write header
            write_header(file, mesh, format);

            // Write data
            if (format == Format::ASCII) {
                write_ascii_data(file, mesh);
            } else {
                write_binary_data(file, mesh, format);
            }

            return true;
        } catch (const std::exception&) {
            return false;
        }
    }

private:
    static void parse_header(std::ifstream& file, Format& format, 
                           std::vector<Element>& elements) {
        std::string line;
        
        // Check magic number
        if (!std::getline(file, line)) {
            throw std::runtime_error("Failed to read PLY file");
        }
        
        // Trim whitespace from line
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line != "ply") {
            throw std::runtime_error("Invalid PLY file format - expected 'ply', got: '" + line + "'");
        }

        while (std::getline(file, line)) {
            if (line == "end_header") {
                break;
            }

            std::istringstream iss(line);
            std::string command;
            iss >> command;

            if (command == "format") {
                std::string format_str;
                iss >> format_str;
                
                if (format_str == "ascii") {
                    format = Format::ASCII;
                } else if (format_str == "binary_little_endian") {
                    format = Format::BINARY_LITTLE_ENDIAN;
                } else if (format_str == "binary_big_endian") {
                    format = Format::BINARY_BIG_ENDIAN;
                }
            }
            else if (command == "element") {
                std::string name;
                std::size_t count;
                iss >> name >> count;
                elements.push_back({name, count, {}});
            }
            else if (command == "property") {
                if (elements.empty()) continue;
                
                std::string type_or_list;
                iss >> type_or_list;
                
                if (type_or_list == "list") {
                    std::string count_type, data_type, name;
                    iss >> count_type >> data_type >> name;
                    elements.back().properties.push_back({name, data_type, true, count_type});
                } else {
                    std::string name;
                    iss >> name;
                    elements.back().properties.push_back({name, type_or_list, false, ""});
                }
            }
        }
    }

    template<typename T>
    static void read_vertices(std::ifstream& file, const Element& vertex_element, 
                             Format format, std::vector<math::Vector3<T>>& positions,
                             std::vector<math::Vector3<T>>& normals,
                             std::vector<math::Vector2<T>>& uvs) {
        positions.reserve(vertex_element.count);
        
        // Find property indices
        int x_idx = -1, y_idx = -1, z_idx = -1;
        int nx_idx = -1, ny_idx = -1, nz_idx = -1;
        int u_idx = -1, v_idx = -1;

        for (std::size_t i = 0; i < vertex_element.properties.size(); ++i) {
            const auto& prop = vertex_element.properties[i];
            if (prop.name == "x") x_idx = static_cast<int>(i);
            else if (prop.name == "y") y_idx = static_cast<int>(i);
            else if (prop.name == "z") z_idx = static_cast<int>(i);
            else if (prop.name == "nx") nx_idx = static_cast<int>(i);
            else if (prop.name == "ny") ny_idx = static_cast<int>(i);
            else if (prop.name == "nz") nz_idx = static_cast<int>(i);
            else if (prop.name == "u" || prop.name == "s") u_idx = static_cast<int>(i);
            else if (prop.name == "v" || prop.name == "t") v_idx = static_cast<int>(i);
        }

        for (std::size_t i = 0; i < vertex_element.count; ++i) {
            std::vector<float> values(vertex_element.properties.size());
            
            if (format == Format::ASCII) {
                std::string line;
                if (!std::getline(file, line)) {
                    break; // End of file
                }
                
                std::istringstream iss(line);
                std::size_t j = 0;
                for (auto& value : values) {
                    if (!(iss >> value)) {
                        break; // Invalid value
                    }
                    ++j;
                }
                
                if (j != values.size()) {
                    continue; // Incomplete vertex data
                }
            } else {
                // Binary reading would go here
                // Simplified for this implementation
            }

            // Extract position
            if (x_idx >= 0 && y_idx >= 0 && z_idx >= 0) {
                positions.emplace_back(static_cast<T>(values[x_idx]),
                                     static_cast<T>(values[y_idx]),
                                     static_cast<T>(values[z_idx]));
            }

            // Extract normal if available
            if (nx_idx >= 0 && ny_idx >= 0 && nz_idx >= 0) {
                if (normals.size() < positions.size()) {
                    normals.resize(positions.size(), math::Vector3<T>(0));
                }
                normals.back() = math::Vector3<T>(static_cast<T>(values[nx_idx]),
                                                static_cast<T>(values[ny_idx]),
                                                static_cast<T>(values[nz_idx]));
            }

            // Extract UV if available
            if (u_idx >= 0 && v_idx >= 0) {
                if (uvs.size() < positions.size()) {
                    uvs.resize(positions.size(), math::Vector2<T>(0));
                }
                uvs.back() = math::Vector2<T>(static_cast<T>(values[u_idx]),
                                            static_cast<T>(values[v_idx]));
            }
        }
    }

    template<typename T>
    static void read_faces(std::ifstream& file, const Element& face_element,
                          Format format, core::Mesh<T>& mesh) {
        for (std::size_t i = 0; i < face_element.count; ++i) {
            if (format == Format::ASCII) {
                std::string line;
                if (!std::getline(file, line)) {
                    break; // End of file
                }
                
                std::istringstream iss(line);
                int count;
                if (!(iss >> count) || count < 3) {
                    continue; // Invalid face
                }
                
                std::vector<core::VertexId> vertices;
                vertices.reserve(count);
                
                for (int j = 0; j < count; ++j) {
                    int vertex_id;
                    if (!(iss >> vertex_id) || vertex_id < 0) {
                        break; // Invalid vertex id
                    }
                    vertices.push_back(static_cast<core::VertexId>(vertex_id));
                }
                
                if (vertices.size() == static_cast<std::size_t>(count) && vertices.size() >= 3) {
                    if (vertices.size() == 3) {
                        mesh.add_triangle(vertices[0], vertices[1], vertices[2]);
                    } else {
                        mesh.add_face(vertices);
                    }
                }
            } else {
                // Binary reading would go here
                // Simplified for this implementation
            }
        }
    }

    template<typename T>
    static void write_header(std::ofstream& file, const core::Mesh<T>& mesh, Format format) {
        file << "ply\n";
        
        switch (format) {
            case Format::ASCII:
                file << "format ascii 1.0\n";
                break;
            case Format::BINARY_LITTLE_ENDIAN:
                file << "format binary_little_endian 1.0\n";
                break;
            case Format::BINARY_BIG_ENDIAN:
                file << "format binary_big_endian 1.0\n";
                break;
        }

        file << "comment Generated by Polygon Mesh Library\n";
        file << "element vertex " << mesh.vertex_count() << "\n";
        file << "property float x\n";
        file << "property float y\n";
        file << "property float z\n";

        // Check if we have normals (check all vertices to be safe)
        bool has_normals = false;
        const auto& vertices = mesh.vertices();
        for (std::size_t i = 0; i < vertices.size(); ++i) {
            if (vertices[i].has_normal()) {
                has_normals = true;
                break;
            }
        }
        
        if (has_normals) {
            file << "property float nx\n";
            file << "property float ny\n";
            file << "property float nz\n";
        }

        // Check if we have uvs (check all vertices to be safe)
        bool has_uvs = false;
        for (std::size_t i = 0; i < vertices.size(); ++i) {
            if (vertices[i].has_uv()) {
                has_uvs = true;
                break;
            }
        }
        
        if (has_uvs) {
            file << "property float u\n";
            file << "property float v\n";
        }

        file << "element face " << mesh.face_count() << "\n";
        file << "property list uchar int vertex_indices\n";
        file << "end_header\n";
    }

    template<typename T>
    static void write_ascii_data(std::ofstream& file, const core::Mesh<T>& mesh) {
        // Write vertices
        const auto& vertices = mesh.vertices();
        
        // Determine what properties to write by checking all vertices
        bool write_normals = false;
        bool write_uvs = false;
        for (std::size_t i = 0; i < vertices.size(); ++i) {
            if (vertices[i].has_normal()) write_normals = true;
            if (vertices[i].has_uv()) write_uvs = true;
        }
        
        for (std::size_t i = 0; i < vertices.size(); ++i) {
            const auto& vertex = vertices[i];
            file << vertex.position.x << " " << vertex.position.y << " " << vertex.position.z;
            
            if (write_normals) {
                if (vertex.has_normal()) {
                    file << " " << vertex.normal.x << " " << vertex.normal.y << " " << vertex.normal.z;
                } else {
                    file << " 0.0 0.0 0.0";
                }
            }
            
            if (write_uvs) {
                if (vertex.has_uv()) {
                    file << " " << vertex.uv.x << " " << vertex.uv.y;
                } else {
                    file << " 0.0 0.0";
                }
            }
            
            file << "\n";
        }

        // Write faces
        const auto& faces = mesh.faces();
        for (std::size_t i = 0; i < faces.size(); ++i) {
            const auto& face = faces[i];
            file << face.vertex_count();
            for (std::size_t j = 0; j < face.vertices.size(); ++j) {
                file << " " << face.vertices[j];
            }
            file << "\n";
        }
    }

    template<typename T>
    static void write_binary_data(std::ofstream& file, const core::Mesh<T>& mesh, Format format) {
        (void)format; // Suppress unused parameter warning
        // Binary writing implementation would go here
        // For simplicity, fall back to ASCII for now
        write_ascii_data(file, mesh);
    }
};

// Convenience functions
template<typename T>
core::Mesh<T> load_ply(const std::string& filepath) {
    return PLYLoader::load<T>(filepath);
}

template<typename T>
bool save_ply(const std::string& filepath, const core::Mesh<T>& mesh, 
              PLYLoader::Format format = PLYLoader::Format::ASCII) {
    return PLYLoader::save(filepath, mesh, format);
}

} // namespace io
} // namespace polygon_mesh