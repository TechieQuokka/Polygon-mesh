#pragma once

// Main IO header file - includes all file format loaders

#include <polygon_mesh/io/obj_loader.hpp>
#include <polygon_mesh/io/ply_loader.hpp>
#include <string>
#include <algorithm>
#include <stdexcept>

namespace polygon_mesh {
namespace io {

// Supported file formats
enum class FileFormat {
    UNKNOWN,
    OBJ,
    PLY,
    STL,  // Future implementation
    OFF   // Future implementation
};

// Detect file format from extension
inline FileFormat detect_format(const std::string& filepath) {
    std::string extension = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "obj") return FileFormat::OBJ;
    if (extension == "ply") return FileFormat::PLY;
    if (extension == "stl") return FileFormat::STL;
    if (extension == "off") return FileFormat::OFF;
    
    return FileFormat::UNKNOWN;
}

// Generic load function that auto-detects format
template<typename T>
core::Mesh<T> load_mesh(const std::string& filepath) {
    FileFormat format = detect_format(filepath);
    
    switch (format) {
        case FileFormat::OBJ:
            return load_obj<T>(filepath);
        case FileFormat::PLY:
            return load_ply<T>(filepath);
        case FileFormat::STL:
            throw std::runtime_error("STL format not yet implemented");
        case FileFormat::OFF:
            throw std::runtime_error("OFF format not yet implemented");
        default:
            throw std::runtime_error("Unsupported file format: " + filepath);
    }
}

// Generic save function that auto-detects format
template<typename T>
bool save_mesh(const std::string& filepath, const core::Mesh<T>& mesh) {
    FileFormat format = detect_format(filepath);
    
    switch (format) {
        case FileFormat::OBJ:
            return save_obj(filepath, mesh);
        case FileFormat::PLY:
            return save_ply(filepath, mesh);
        case FileFormat::STL:
            throw std::runtime_error("STL format not yet implemented");
        case FileFormat::OFF:
            throw std::runtime_error("OFF format not yet implemented");
        default:
            throw std::runtime_error("Unsupported file format: " + filepath);
    }
}

// File format information
struct FormatInfo {
    std::string name;
    std::string description;
    std::vector<std::string> extensions;
    bool supports_colors;
    bool supports_normals;
    bool supports_uvs;
    bool supports_materials;
    bool ascii_format;
    bool binary_format;
};

// Get format information
inline FormatInfo get_format_info(FileFormat format) {
    switch (format) {
        case FileFormat::OBJ:
            return {
                "Wavefront OBJ",
                "Widely supported 3D mesh format with material support",
                {"obj"},
                false, true, true, true,
                true, false
            };
        case FileFormat::PLY:
            return {
                "Stanford PLY",
                "Polygon File Format with flexible property support",
                {"ply"},
                true, true, true, false,
                true, true
            };
        case FileFormat::STL:
            return {
                "STL",
                "Stereolithography format for 3D printing",
                {"stl"},
                false, true, false, false,
                true, true
            };
        case FileFormat::OFF:
            return {
                "Object File Format",
                "Simple mesh format with vertex and face data",
                {"off"},
                false, false, false, false,
                true, false
            };
        default:
            return {"Unknown", "Unknown format", {}, false, false, false, false, false, false};
    }
}

// Get list of supported formats
inline std::vector<FileFormat> get_supported_formats() {
    return {FileFormat::OBJ, FileFormat::PLY};
}

// Get list of all known formats (including unimplemented ones)
inline std::vector<FileFormat> get_all_formats() {
    return {FileFormat::OBJ, FileFormat::PLY, FileFormat::STL, FileFormat::OFF};
}

// Utility functions
inline std::string get_format_filter_string() {
    std::string filter = "All Supported (";
    bool first = true;
    
    for (auto format : get_supported_formats()) {
        auto info = get_format_info(format);
        for (const auto& ext : info.extensions) {
            if (!first) filter += ";";
            filter += "*." + ext;
            first = false;
        }
    }
    
    filter += ")|";
    first = true;
    
    for (auto format : get_supported_formats()) {
        auto info = get_format_info(format);
        for (const auto& ext : info.extensions) {
            if (!first) filter += ";";
            filter += "*." + ext;
            first = false;
        }
    }
    
    // Add individual format filters
    for (auto format : get_supported_formats()) {
        auto info = get_format_info(format);
        filter += "|" + info.name + " (";
        bool ext_first = true;
        for (const auto& ext : info.extensions) {
            if (!ext_first) filter += ";";
            filter += "*." + ext;
            ext_first = false;
        }
        filter += ")|";
        ext_first = true;
        for (const auto& ext : info.extensions) {
            if (!ext_first) filter += ";";
            filter += "*." + ext;
            ext_first = false;
        }
    }
    
    return filter;
}

} // namespace io
} // namespace polygon_mesh