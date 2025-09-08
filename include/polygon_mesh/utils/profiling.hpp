#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <thread>
#include <limits>

namespace polygon_mesh {
namespace utils {

// High-resolution timer
class Timer {
private:
    std::chrono::high_resolution_clock::time_point start_time_;
    
public:
    Timer() : start_time_(std::chrono::high_resolution_clock::now()) {}
    
    void reset() {
        start_time_ = std::chrono::high_resolution_clock::now();
    }
    
    double elapsed_seconds() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_);
        return duration.count() / 1000000.0;
    }
    
    double elapsed_milliseconds() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_);
        return duration.count() / 1000.0;
    }
    
    std::int64_t elapsed_microseconds() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_);
        return duration.count();
    }
    
    std::int64_t elapsed_nanoseconds() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time_);
        return duration.count();
    }
};

// RAII timer for automatic profiling
class ScopedTimer {
private:
    Timer timer_;
    std::string name_;
    bool* completed_flag_;
    double* result_storage_;
    
public:
    explicit ScopedTimer(const std::string& name) 
        : name_(name), completed_flag_(nullptr), result_storage_(nullptr) {}
    
    ScopedTimer(const std::string& name, bool* completed_flag)
        : name_(name), completed_flag_(completed_flag), result_storage_(nullptr) {}
    
    ScopedTimer(const std::string& name, double* result_storage)
        : name_(name), completed_flag_(nullptr), result_storage_(result_storage) {}
    
    ~ScopedTimer() {
        double elapsed = timer_.elapsed_seconds();
        
        if (result_storage_) {
            *result_storage_ = elapsed;
        }
        
        if (completed_flag_) {
            *completed_flag_ = true;
        }
        
        if (!name_.empty()) {
            std::cout << "[TIMER] " << name_ << ": " << elapsed << " seconds\n";
        }
    }
};

// Performance statistics collector
struct PerformanceStats {
    std::string name;
    std::size_t call_count = 0;
    double total_time = 0.0;
    double min_time = std::numeric_limits<double>::max();
    double max_time = 0.0;
    std::thread::id thread_id;
    
    void add_sample(double time) {
        ++call_count;
        total_time += time;
        min_time = std::min(min_time, time);
        max_time = std::max(max_time, time);
    }
    
    double average_time() const {
        return call_count > 0 ? total_time / call_count : 0.0;
    }
    
    void reset() {
        call_count = 0;
        total_time = 0.0;
        min_time = std::numeric_limits<double>::max();
        max_time = 0.0;
    }
};

// Global profiler
class Profiler {
private:
    std::unordered_map<std::string, PerformanceStats> stats_;
    std::mutex mutex_;
    bool enabled_;
    
    Profiler() : enabled_(true) {}
    
public:
    static Profiler& instance() {
        static Profiler instance;
        return instance;
    }
    
    void enable() { enabled_ = true; }
    void disable() { enabled_ = false; }
    bool is_enabled() const { return enabled_; }
    
    void begin_sample(const std::string& name) {
        if (!enabled_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        (void)name; // Suppress unused parameter warning
        // Store start time in thread-local storage or similar
    }
    
    void end_sample(const std::string& name, double elapsed_time) {
        if (!enabled_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        auto& stats = stats_[name];
        if (stats.name.empty()) {
            stats.name = name;
            stats.thread_id = std::this_thread::get_id();
        }
        stats.add_sample(elapsed_time);
    }
    
    void add_sample(const std::string& name, double elapsed_time) {
        if (!enabled_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        auto& stats = stats_[name];
        if (stats.name.empty()) {
            stats.name = name;
            stats.thread_id = std::this_thread::get_id();
        }
        stats.add_sample(elapsed_time);
    }
    
    PerformanceStats get_stats(const std::string& name) const {
        std::unique_lock<std::mutex> lock(const_cast<std::mutex&>(mutex_));
        auto it = stats_.find(name);
        return it != stats_.end() ? it->second : PerformanceStats{};
    }
    
    std::vector<PerformanceStats> get_all_stats() const {
        std::unique_lock<std::mutex> lock(const_cast<std::mutex&>(mutex_));
        std::vector<PerformanceStats> result;
        result.reserve(stats_.size());
        for (const auto& pair : stats_) {
            result.push_back(pair.second);
        }
        return result;
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        stats_.clear();
    }
    
    void reset_stats(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = stats_.find(name);
        if (it != stats_.end()) {
            it->second.reset();
        }
    }
    
    void print_report(std::ostream& os = std::cout) const {
        std::unique_lock<std::mutex> lock(const_cast<std::mutex&>(mutex_));
        
        os << "\n=== Performance Profiling Report ===\n";
        os << "Function Name                    | Calls  | Total Time | Avg Time   | Min Time   | Max Time  \n";
        os << "--------------------------------|--------|------------|------------|------------|----------\n";
        
        for (const auto& pair : stats_) {
            const auto& stats = pair.second;
            os << std::left << std::setw(32) << stats.name << "| "
               << std::right << std::setw(6) << stats.call_count << " | "
               << std::setw(10) << std::fixed << std::setprecision(6) << stats.total_time << " | "
               << std::setw(10) << std::fixed << std::setprecision(6) << stats.average_time() << " | "
               << std::setw(10) << std::fixed << std::setprecision(6) << stats.min_time << " | "
               << std::setw(10) << std::fixed << std::setprecision(6) << stats.max_time << "\n";
        }
        os << "\n";
    }
    
    void save_report(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (file.is_open()) {
            print_report(file);
        }
    }
};

// Profile scope helper
class ProfileScope {
private:
    std::string name_;
    Timer timer_;
    
public:
    explicit ProfileScope(const std::string& name) : name_(name) {}
    
    ~ProfileScope() {
        double elapsed = timer_.elapsed_seconds();
        Profiler::instance().add_sample(name_, elapsed);
    }
};

// Macros for easy profiling
#define PROFILE_SCOPE(name) polygon_mesh::utils::ProfileScope _prof_scope(name)
#define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCTION__)

// Memory usage profiling
class MemoryProfiler {
private:
    struct AllocationRecord {
        std::size_t size;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string tag;
    };
    
    std::unordered_map<void*, AllocationRecord> allocations_;
    std::mutex mutex_;
    std::size_t total_allocated_;
    std::size_t peak_allocated_;
    std::size_t current_allocated_;
    
    MemoryProfiler() : total_allocated_(0), peak_allocated_(0), current_allocated_(0) {}
    
public:
    static MemoryProfiler& instance() {
        static MemoryProfiler instance;
        return instance;
    }
    
    void record_allocation(void* ptr, std::size_t size, const std::string& tag = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_[ptr] = {size, std::chrono::high_resolution_clock::now(), tag};
        total_allocated_ += size;
        current_allocated_ += size;
        peak_allocated_ = std::max(peak_allocated_, current_allocated_);
    }
    
    void record_deallocation(void* ptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = allocations_.find(ptr);
        if (it != allocations_.end()) {
            current_allocated_ -= it->second.size;
            allocations_.erase(it);
        }
    }
    
    std::size_t get_total_allocated() const {
        std::unique_lock<std::mutex> lock(const_cast<std::mutex&>(mutex_));
        return total_allocated_;
    }
    
    std::size_t get_peak_allocated() const {
        std::unique_lock<std::mutex> lock(const_cast<std::mutex&>(mutex_));
        return peak_allocated_;
    }
    
    std::size_t get_current_allocated() const {
        std::unique_lock<std::mutex> lock(const_cast<std::mutex&>(mutex_));
        return current_allocated_;
    }
    
    std::size_t get_allocation_count() const {
        std::unique_lock<std::mutex> lock(const_cast<std::mutex&>(mutex_));
        return allocations_.size();
    }
    
    void print_memory_report(std::ostream& os = std::cout) const {
        std::unique_lock<std::mutex> lock(const_cast<std::mutex&>(mutex_));
        
        os << "\n=== Memory Profiling Report ===\n";
        os << "Total Allocated: " << total_allocated_ << " bytes\n";
        os << "Peak Allocated: " << peak_allocated_ << " bytes\n";
        os << "Currently Allocated: " << current_allocated_ << " bytes\n";
        os << "Active Allocations: " << allocations_.size() << "\n";
        
        if (!allocations_.empty()) {
            os << "\nActive Allocations:\n";
            os << "Address          | Size       | Tag\n";
            os << "-----------------|------------|------------------\n";
            
            for (const auto& pair : allocations_) {
                os << std::hex << pair.first << std::dec << " | "
                   << std::setw(10) << pair.second.size << " | "
                   << pair.second.tag << "\n";
            }
        }
        os << "\n";
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_.clear();
        total_allocated_ = 0;
        peak_allocated_ = 0;
        current_allocated_ = 0;
    }
};

} // namespace utils
} // namespace polygon_mesh