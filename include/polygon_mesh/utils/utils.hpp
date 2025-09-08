#pragma once

// Main utils header file - includes all utility modules

#include <polygon_mesh/utils/memory.hpp>
#include <polygon_mesh/utils/threading.hpp>
#include <polygon_mesh/utils/profiling.hpp>

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <random>
#include <type_traits>

namespace polygon_mesh {
namespace utils {

// String utilities
namespace string {
    
    inline std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> result;
        std::stringstream ss(str);
        std::string item;
        
        while (std::getline(ss, item, delimiter)) {
            result.push_back(item);
        }
        
        return result;
    }
    
    inline std::string trim(const std::string& str) {
        auto start = str.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) return "";
        
        auto end = str.find_last_not_of(" \t\n\r");
        return str.substr(start, end - start + 1);
    }
    
    inline std::string to_lower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    inline std::string to_upper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }
    
    inline bool starts_with(const std::string& str, const std::string& prefix) {
        return str.length() >= prefix.length() && 
               str.substr(0, prefix.length()) == prefix;
    }
    
    inline bool ends_with(const std::string& str, const std::string& suffix) {
        return str.length() >= suffix.length() && 
               str.substr(str.length() - suffix.length()) == suffix;
    }
    
    inline bool contains(const std::string& str, const std::string& substring) {
        return str.find(substring) != std::string::npos;
    }
    
    inline std::string replace_all(const std::string& str, const std::string& from, const std::string& to) {
        std::string result = str;
        std::size_t pos = 0;
        
        while ((pos = result.find(from, pos)) != std::string::npos) {
            result.replace(pos, from.length(), to);
            pos += to.length();
        }
        
        return result;
    }
    
} // namespace string

// Math utilities
namespace math {
    
    template<typename T>
    constexpr T pi() {
        return static_cast<T>(3.141592653589793238462643383279502884);
    }
    
    template<typename T>
    constexpr T degrees_to_radians(T degrees) {
        return degrees * pi<T>() / static_cast<T>(180);
    }
    
    template<typename T>
    constexpr T radians_to_degrees(T radians) {
        return radians * static_cast<T>(180) / pi<T>();
    }
    
    template<typename T>
    constexpr T clamp(T value, T min_val, T max_val) {
        return value < min_val ? min_val : (value > max_val ? max_val : value);
    }
    
    template<typename T>
    constexpr T lerp(T a, T b, T t) {
        return a + t * (b - a);
    }
    
    template<typename T>
    constexpr T smoothstep(T edge0, T edge1, T x) {
        T t = clamp((x - edge0) / (edge1 - edge0), T(0), T(1));
        return t * t * (T(3) - T(2) * t);
    }
    
    template<typename T>
    constexpr bool approximately_equal(T a, T b, T epsilon = std::numeric_limits<T>::epsilon()) {
        return std::abs(a - b) <= epsilon;
    }
    
    template<typename T>
    constexpr T sign(T value) {
        return (T(0) < value) - (value < T(0));
    }
    
    template<typename T>
    constexpr T square(T value) {
        return value * value;
    }
    
} // namespace math

// Random number generation utilities
class Random {
private:
    mutable std::random_device rd_;
    mutable std::mt19937 gen_;
    
public:
    Random() : gen_(rd_()) {}
    explicit Random(std::uint32_t seed) : gen_(seed) {}
    
    // Integer random values
    int random_int(int min_val, int max_val) const {
        std::uniform_int_distribution<int> dis(min_val, max_val);
        return dis(gen_);
    }
    
    std::size_t random_size_t(std::size_t min_val, std::size_t max_val) const {
        std::uniform_int_distribution<std::size_t> dis(min_val, max_val);
        return dis(gen_);
    }
    
    // Floating point random values
    float random_float(float min_val = 0.0f, float max_val = 1.0f) const {
        std::uniform_real_distribution<float> dis(min_val, max_val);
        return dis(gen_);
    }
    
    double random_double(double min_val = 0.0, double max_val = 1.0) const {
        std::uniform_real_distribution<double> dis(min_val, max_val);
        return dis(gen_);
    }
    
    // Boolean random values
    bool random_bool(double probability = 0.5) const {
        std::bernoulli_distribution dis(probability);
        return dis(gen_);
    }
    
    // Normal distribution
    template<typename T>
    T random_normal(T mean = T(0), T stddev = T(1)) const {
        std::normal_distribution<T> dis(mean, stddev);
        return dis(gen_);
    }
    
    // Choose random element from container
    template<typename Container>
    auto random_element(const Container& container) const -> decltype(*container.begin()) {
        if (container.empty()) {
            throw std::runtime_error("Cannot choose from empty container");
        }
        
        auto it = container.begin();
        std::advance(it, random_size_t(0, container.size() - 1));
        return *it;
    }
    
    // Shuffle container
    template<typename Container>
    void shuffle(Container& container) const {
        std::shuffle(container.begin(), container.end(), gen_);
    }
    
    // Sample without replacement
    template<typename Container>
    std::vector<typename Container::value_type> sample(const Container& container, std::size_t count) const {
        if (count > container.size()) {
            throw std::runtime_error("Cannot sample more elements than available");
        }
        
        std::vector<typename Container::value_type> elements(container.begin(), container.end());
        shuffle(elements);
        elements.resize(count);
        return elements;
    }
    
    void set_seed(std::uint32_t seed) {
        gen_.seed(seed);
    }
};

// Global random instance
inline Random& global_random() {
    static Random instance;
    return instance;
}

// File system utilities
namespace filesystem {
    
    inline std::string get_extension(const std::string& filepath) {
        auto pos = filepath.find_last_of('.');
        if (pos == std::string::npos) return "";
        return filepath.substr(pos + 1);
    }
    
    inline std::string get_filename(const std::string& filepath) {
        auto pos = filepath.find_last_of("/\\");
        if (pos == std::string::npos) return filepath;
        return filepath.substr(pos + 1);
    }
    
    inline std::string get_directory(const std::string& filepath) {
        auto pos = filepath.find_last_of("/\\");
        if (pos == std::string::npos) return "";
        return filepath.substr(0, pos);
    }
    
    inline std::string get_stem(const std::string& filepath) {
        std::string filename = get_filename(filepath);
        auto pos = filename.find_last_of('.');
        if (pos == std::string::npos) return filename;
        return filename.substr(0, pos);
    }
    
    inline std::string join_path(const std::string& dir, const std::string& filename) {
        if (dir.empty()) return filename;
        if (filename.empty()) return dir;
        
        char last_char = dir.back();
        if (last_char == '/' || last_char == '\\') {
            return dir + filename;
        }
        
        #ifdef _WIN32
        return dir + "\\" + filename;
        #else
        return dir + "/" + filename;
        #endif
    }
    
} // namespace filesystem

// Type utilities
namespace type_traits {
    
    // Check if type is floating point
    template<typename T>
    constexpr bool is_floating_point_v = std::is_floating_point<T>::value;
    
    // Check if type is arithmetic
    template<typename T>
    constexpr bool is_arithmetic_v = std::is_arithmetic<T>::value;
    
    // Get underlying type for enums
    template<typename T>
    using underlying_type_t = typename std::underlying_type<T>::type;
    
    // Size of type in bytes
    template<typename T>
    constexpr std::size_t sizeof_v = sizeof(T);
    
} // namespace type_traits

// Container utilities
namespace container {
    
    template<typename Container, typename Predicate>
    void erase_if(Container& container, Predicate pred) {
        auto it = std::remove_if(container.begin(), container.end(), pred);
        container.erase(it, container.end());
    }
    
    template<typename Container, typename T>
    bool contains(const Container& container, const T& value) {
        return std::find(container.begin(), container.end(), value) != container.end();
    }
    
    template<typename Container>
    void reverse(Container& container) {
        std::reverse(container.begin(), container.end());
    }
    
    template<typename Container>
    void sort(Container& container) {
        std::sort(container.begin(), container.end());
    }
    
    template<typename Container, typename Compare>
    void sort(Container& container, Compare comp) {
        std::sort(container.begin(), container.end(), comp);
    }
    
    template<typename Container>
    auto min_element(const Container& container) -> decltype(*container.begin()) {
        return *std::min_element(container.begin(), container.end());
    }
    
    template<typename Container>
    auto max_element(const Container& container) -> decltype(*container.begin()) {
        return *std::max_element(container.begin(), container.end());
    }
    
} // namespace container

} // namespace utils
} // namespace polygon_mesh