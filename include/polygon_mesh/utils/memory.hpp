#pragma once

#include <memory>
#include <vector>
#include <cstddef>
#include <algorithm>

namespace polygon_mesh {
namespace utils {

// Memory pool for efficient allocation of small objects
template<typename T>
class MemoryPool {
private:
    struct Block {
        std::vector<T> objects;
        std::vector<bool> used;
        std::size_t next_free;
        
        explicit Block(std::size_t size) 
            : objects(size), used(size, false), next_free(0) {}
    };
    
    std::vector<std::unique_ptr<Block>> blocks_;
    std::size_t block_size_;
    std::size_t current_block_;
    
public:
    explicit MemoryPool(std::size_t block_size = 1024) 
        : block_size_(block_size), current_block_(0) {
        blocks_.push_back(std::make_unique<Block>(block_size_));
    }
    
    ~MemoryPool() = default;
    
    // Non-copyable, moveable
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    MemoryPool(MemoryPool&&) = default;
    MemoryPool& operator=(MemoryPool&&) = default;
    
    T* allocate() {
        // Try to find free slot in current block
        for (std::size_t block_idx = current_block_; block_idx < blocks_.size(); ++block_idx) {
            auto& block = *blocks_[block_idx];
            
            // Look for free slot starting from next_free
            for (std::size_t i = block.next_free; i < block_size_; ++i) {
                if (!block.used[i]) {
                    block.used[i] = true;
                    block.next_free = i + 1;
                    current_block_ = block_idx;
                    return &block.objects[i];
                }
            }
            
            // Reset next_free and try from beginning
            block.next_free = 0;
            for (std::size_t i = 0; i < block_size_; ++i) {
                if (!block.used[i]) {
                    block.used[i] = true;
                    block.next_free = i + 1;
                    current_block_ = block_idx;
                    return &block.objects[i];
                }
            }
        }
        
        // All blocks are full, allocate new block
        blocks_.push_back(std::make_unique<Block>(block_size_));
        current_block_ = blocks_.size() - 1;
        auto& new_block = *blocks_.back();
        new_block.used[0] = true;
        new_block.next_free = 1;
        return &new_block.objects[0];
    }
    
    void deallocate(T* ptr) {
        if (!ptr) return;
        
        // Find which block contains this pointer
        for (auto& block : blocks_) {
            T* block_start = block->objects.data();
            T* block_end = block_start + block_size_;
            
            if (ptr >= block_start && ptr < block_end) {
                std::size_t index = ptr - block_start;
                block->used[index] = false;
                block->next_free = std::min(block->next_free, index);
                return;
            }
        }
    }
    
    void clear() {
        for (auto& block : blocks_) {
            std::fill(block->used.begin(), block->used.end(), false);
            block->next_free = 0;
        }
        current_block_ = 0;
    }
    
    std::size_t allocated_count() const {
        std::size_t count = 0;
        for (const auto& block : blocks_) {
            count += std::count(block->used.begin(), block->used.end(), true);
        }
        return count;
    }
    
    std::size_t capacity() const {
        return blocks_.size() * block_size_;
    }
    
    double utilization() const {
        return static_cast<double>(allocated_count()) / capacity();
    }
};

// RAII wrapper for memory pool allocation
template<typename T>
class PoolAllocatedPtr {
private:
    T* ptr_;
    MemoryPool<T>* pool_;
    
public:
    PoolAllocatedPtr(T* ptr, MemoryPool<T>* pool) : ptr_(ptr), pool_(pool) {}
    
    ~PoolAllocatedPtr() {
        if (ptr_ && pool_) {
            pool_->deallocate(ptr_);
        }
    }
    
    // Non-copyable, moveable
    PoolAllocatedPtr(const PoolAllocatedPtr&) = delete;
    PoolAllocatedPtr& operator=(const PoolAllocatedPtr&) = delete;
    
    PoolAllocatedPtr(PoolAllocatedPtr&& other) noexcept 
        : ptr_(other.ptr_), pool_(other.pool_) {
        other.ptr_ = nullptr;
        other.pool_ = nullptr;
    }
    
    PoolAllocatedPtr& operator=(PoolAllocatedPtr&& other) noexcept {
        if (this != &other) {
            if (ptr_ && pool_) {
                pool_->deallocate(ptr_);
            }
            ptr_ = other.ptr_;
            pool_ = other.pool_;
            other.ptr_ = nullptr;
            other.pool_ = nullptr;
        }
        return *this;
    }
    
    T* get() const { return ptr_; }
    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_; }
    explicit operator bool() const { return ptr_ != nullptr; }
    
    T* release() {
        T* temp = ptr_;
        ptr_ = nullptr;
        pool_ = nullptr;
        return temp;
    }
};

// Aligned memory allocator
template<std::size_t Alignment>
class AlignedAllocator {
public:
    static void* allocate(std::size_t size) {
        #ifdef _WIN32
        return _aligned_malloc(size, Alignment);
        #else
        return std::aligned_alloc(Alignment, size);
        #endif
    }
    
    static void deallocate(void* ptr) {
        #ifdef _WIN32
        _aligned_free(ptr);
        #else
        std::free(ptr);
        #endif
    }
};

// Stack allocator for temporary allocations
class StackAllocator {
private:
    std::byte* buffer_;
    std::size_t size_;
    std::size_t offset_;
    bool owns_buffer_;
    
public:
    explicit StackAllocator(std::size_t size) 
        : buffer_(new std::byte[size]), size_(size), offset_(0), owns_buffer_(true) {}
    
    StackAllocator(void* buffer, std::size_t size)
        : buffer_(static_cast<std::byte*>(buffer)), size_(size), offset_(0), owns_buffer_(false) {}
    
    ~StackAllocator() {
        if (owns_buffer_) {
            delete[] buffer_;
        }
    }
    
    // Non-copyable, moveable
    StackAllocator(const StackAllocator&) = delete;
    StackAllocator& operator=(const StackAllocator&) = delete;
    StackAllocator(StackAllocator&&) = default;
    StackAllocator& operator=(StackAllocator&&) = default;
    
    template<typename T>
    T* allocate(std::size_t count = 1) {
        std::size_t required = sizeof(T) * count;
        std::size_t aligned_size = (required + alignof(T) - 1) & ~(alignof(T) - 1);
        
        if (offset_ + aligned_size > size_) {
            return nullptr; // Out of memory
        }
        
        T* result = reinterpret_cast<T*>(buffer_ + offset_);
        offset_ += aligned_size;
        return result;
    }
    
    void reset() {
        offset_ = 0;
    }
    
    std::size_t used() const { return offset_; }
    std::size_t remaining() const { return size_ - offset_; }
    double utilization() const { return static_cast<double>(offset_) / size_; }
    
    // Checkpoint system for nested allocations
    class Checkpoint {
    private:
        StackAllocator* allocator_;
        std::size_t saved_offset_;
        
    public:
        explicit Checkpoint(StackAllocator* allocator)
            : allocator_(allocator), saved_offset_(allocator->offset_) {}
        
        ~Checkpoint() {
            if (allocator_) {
                allocator_->offset_ = saved_offset_;
            }
        }
        
        // Non-copyable, moveable
        Checkpoint(const Checkpoint&) = delete;
        Checkpoint& operator=(const Checkpoint&) = delete;
        Checkpoint(Checkpoint&&) = default;
        Checkpoint& operator=(Checkpoint&&) = default;
        
        void release() {
            allocator_ = nullptr;
        }
    };
    
    Checkpoint create_checkpoint() {
        return Checkpoint(this);
    }
};

// Memory usage tracking
class MemoryTracker {
private:
    static std::size_t total_allocated_;
    static std::size_t peak_allocated_;
    static std::size_t allocation_count_;
    
public:
    static void record_allocation(std::size_t size) {
        total_allocated_ += size;
        peak_allocated_ = std::max(peak_allocated_, total_allocated_);
        ++allocation_count_;
    }
    
    static void record_deallocation(std::size_t size) {
        total_allocated_ -= size;
    }
    
    static std::size_t total_allocated() { return total_allocated_; }
    static std::size_t peak_allocated() { return peak_allocated_; }
    static std::size_t allocation_count() { return allocation_count_; }
    
    static void reset() {
        total_allocated_ = 0;
        peak_allocated_ = 0;
        allocation_count_ = 0;
    }
};

// Static member definitions (moved to source file in real implementation)
std::size_t MemoryTracker::total_allocated_ = 0;
std::size_t MemoryTracker::peak_allocated_ = 0;
std::size_t MemoryTracker::allocation_count_ = 0;

} // namespace utils
} // namespace polygon_mesh