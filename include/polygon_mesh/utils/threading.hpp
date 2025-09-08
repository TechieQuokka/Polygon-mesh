#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <queue>
#include <functional>
#include <future>
#include <memory>

namespace polygon_mesh {
namespace utils {

// Simple thread pool for parallel processing
class ThreadPool {
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    std::atomic<bool> stop_;
    
public:
    explicit ThreadPool(std::size_t threads = std::thread::hardware_concurrency())
        : stop_(false) {
        for (std::size_t i = 0; i < threads; ++i) {
            workers_.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex_);
                        condition_.wait(lock, [this] { return stop_.load() || !tasks_.empty(); });
                        
                        if (stop_.load() && tasks_.empty()) {
                            return;
                        }
                        
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    
                    task();
                }
            });
        }
    }
    
    ~ThreadPool() {
        stop_.store(true);
        condition_.notify_all();
        
        for (auto& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
    
    // Non-copyable, non-moveable
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;
    
    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
        using return_type = typename std::result_of<F(Args...)>::type;
        
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        std::future<return_type> result = task->get_future();
        
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            
            if (stop_.load()) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }
            
            tasks_.emplace([task]() { (*task)(); });
        }
        
        condition_.notify_one();
        return result;
    }
    
    std::size_t size() const {
        return workers_.size();
    }
    
    std::size_t pending_tasks() const {
        std::unique_lock<std::mutex> lock(const_cast<std::mutex&>(queue_mutex_));
        return tasks_.size();
    }
};

// Parallel for loop implementation
template<typename Iterator, typename Function>
void parallel_for(Iterator first, Iterator last, Function&& func, 
                  std::size_t num_threads = std::thread::hardware_concurrency()) {
    const std::size_t length = std::distance(first, last);
    if (length == 0) return;
    
    if (num_threads == 1 || length < num_threads) {
        // Serial execution
        std::for_each(first, last, std::forward<Function>(func));
        return;
    }
    
    const std::size_t chunk_size = length / num_threads;
    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    
    Iterator chunk_start = first;
    for (std::size_t i = 0; i < num_threads - 1; ++i) {
        Iterator chunk_end = chunk_start;
        std::advance(chunk_end, chunk_size);
        
        threads.emplace_back([chunk_start, chunk_end, &func]() {
            std::for_each(chunk_start, chunk_end, func);
        });
        
        chunk_start = chunk_end;
    }
    
    // Process remaining elements in main thread
    std::for_each(chunk_start, last, func);
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
}

// Parallel for with index
template<typename Function>
void parallel_for_index(std::size_t start, std::size_t end, Function&& func,
                       std::size_t num_threads = std::thread::hardware_concurrency()) {
    const std::size_t length = end - start;
    if (length == 0) return;
    
    if (num_threads == 1 || length < num_threads) {
        // Serial execution
        for (std::size_t i = start; i < end; ++i) {
            func(i);
        }
        return;
    }
    
    const std::size_t chunk_size = length / num_threads;
    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    
    std::size_t chunk_start = start;
    for (std::size_t i = 0; i < num_threads - 1; ++i) {
        std::size_t chunk_end = chunk_start + chunk_size;
        
        threads.emplace_back([chunk_start, chunk_end, &func]() {
            for (std::size_t j = chunk_start; j < chunk_end; ++j) {
                func(j);
            }
        });
        
        chunk_start = chunk_end;
    }
    
    // Process remaining elements in main thread
    for (std::size_t i = chunk_start; i < end; ++i) {
        func(i);
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
}

// Atomic counter for thread-safe counting
class AtomicCounter {
private:
    std::atomic<std::size_t> count_;
    
public:
    explicit AtomicCounter(std::size_t initial = 0) : count_(initial) {}
    
    std::size_t increment() {
        return count_.fetch_add(1) + 1;
    }
    
    std::size_t decrement() {
        return count_.fetch_sub(1) - 1;
    }
    
    std::size_t get() const {
        return count_.load();
    }
    
    void set(std::size_t value) {
        count_.store(value);
    }
    
    void reset() {
        count_.store(0);
    }
};

// Thread-safe progress tracking
class ProgressTracker {
private:
    std::atomic<std::size_t> completed_;
    std::atomic<std::size_t> total_;
    std::mutex callback_mutex_;
    std::function<void(double)> progress_callback_;
    
public:
    explicit ProgressTracker(std::size_t total = 0) 
        : completed_(0), total_(total) {}
    
    void set_total(std::size_t total) {
        total_.store(total);
    }
    
    void set_progress_callback(std::function<void(double)> callback) {
        std::lock_guard<std::mutex> lock(callback_mutex_);
        progress_callback_ = std::move(callback);
    }
    
    void increment(std::size_t amount = 1) {
        std::size_t new_completed = completed_.fetch_add(amount) + amount;
        std::size_t current_total = total_.load();
        
        if (current_total > 0) {
            double progress = static_cast<double>(new_completed) / current_total;
            
            std::lock_guard<std::mutex> lock(callback_mutex_);
            if (progress_callback_) {
                progress_callback_(std::min(progress, 1.0));
            }
        }
    }
    
    double get_progress() const {
        std::size_t current_completed = completed_.load();
        std::size_t current_total = total_.load();
        
        if (current_total == 0) return 0.0;
        return std::min(static_cast<double>(current_completed) / current_total, 1.0);
    }
    
    std::size_t get_completed() const {
        return completed_.load();
    }
    
    std::size_t get_total() const {
        return total_.load();
    }
    
    void reset() {
        completed_.store(0);
        total_.store(0);
    }
    
    bool is_complete() const {
        return completed_.load() >= total_.load();
    }
};

// Spin lock for low-latency synchronization
class SpinLock {
private:
    std::atomic_flag flag_;
    
public:
    SpinLock() {
        flag_.clear();
    }
    
    void lock() {
        while (flag_.test_and_set(std::memory_order_acquire)) {
            // Busy wait with occasional yield
            std::this_thread::yield();
        }
    }
    
    bool try_lock() {
        return !flag_.test_and_set(std::memory_order_acquire);
    }
    
    void unlock() {
        flag_.clear(std::memory_order_release);
    }
};

// RAII wrapper for spin lock
class SpinLockGuard {
private:
    SpinLock& lock_;
    
public:
    explicit SpinLockGuard(SpinLock& lock) : lock_(lock) {
        lock_.lock();
    }
    
    ~SpinLockGuard() {
        lock_.unlock();
    }
    
    // Non-copyable, non-moveable
    SpinLockGuard(const SpinLockGuard&) = delete;
    SpinLockGuard& operator=(const SpinLockGuard&) = delete;
    SpinLockGuard(SpinLockGuard&&) = delete;
    SpinLockGuard& operator=(SpinLockGuard&&) = delete;
};

// Thread-local storage helper
template<typename T>
class ThreadLocalStorage {
private:
    thread_local static std::unique_ptr<T> instance_;
    
public:
    static T& get() {
        if (!instance_) {
            instance_ = std::make_unique<T>();
        }
        return *instance_;
    }
    
    template<typename... Args>
    static T& get_or_create(Args&&... args) {
        if (!instance_) {
            instance_ = std::make_unique<T>(std::forward<Args>(args)...);
        }
        return *instance_;
    }
    
    static void reset() {
        instance_.reset();
    }
    
    static bool exists() {
        return instance_ != nullptr;
    }
};

template<typename T>
thread_local std::unique_ptr<T> ThreadLocalStorage<T>::instance_;

} // namespace utils
} // namespace polygon_mesh