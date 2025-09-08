#include <iostream>
#include <vector>

// Simple test without our library
int main() {
    std::cout << "Hello, C++17!" << std::endl;
    
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    
    for (const auto& num : numbers) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
    
    // Test C++17 feature: structured binding
    auto pair = std::make_pair(42, 3.14);
    auto [first, second] = pair;
    std::cout << "Structured binding: " << first << ", " << second << std::endl;
    
    return 0;
}