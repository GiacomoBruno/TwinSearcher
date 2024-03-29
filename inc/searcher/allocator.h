#pragma once
#include <memory>

template <class T, std::size_t growSize = 1024>
class MemoryPool
{
    struct Block
    {
        Block *next;
    };

    class Buffer
    {
        static const std::size_t blockSize = sizeof(T) > sizeof(Block) ? sizeof(T) : sizeof(Block);
        uint8_t                  data[blockSize * growSize];

       public:
        Buffer *const next;

        Buffer(Buffer *next) : next(next)
        {
        }

        T *getBlock(std::size_t index)
        {
            return reinterpret_cast<T *>(&data[blockSize * index]);
        }
    };

    Block      *firstFreeBlock = nullptr;
    Buffer     *firstBuffer    = nullptr;
    std::size_t bufferedBlocks = growSize;

   public:
    MemoryPool()                                       = default;
    MemoryPool(MemoryPool &&memoryPool)                = delete;
    MemoryPool(const MemoryPool &memoryPool)           = delete;
    MemoryPool operator=(MemoryPool &&memoryPool)      = delete;
    MemoryPool operator=(const MemoryPool &memoryPool) = delete;

    ~MemoryPool()
    {
        while (firstBuffer)
        {
            Buffer *buffer = firstBuffer;
            firstBuffer    = buffer->next;
            delete buffer;
        }
    }

    T *allocate()
    {
        if (firstFreeBlock)
        {
            Block *block   = firstFreeBlock;
            firstFreeBlock = block->next;
            return reinterpret_cast<T *>(block);
        }

        if (bufferedBlocks >= growSize)
        {
            firstBuffer    = new Buffer(firstBuffer);
            bufferedBlocks = 0;
        }

        return firstBuffer->getBlock(bufferedBlocks++);
    }

    void deallocate(T *pointer)
    {
        Block *block   = reinterpret_cast<Block *>(pointer);
        block->next    = firstFreeBlock;
        firstFreeBlock = block;
    }
};

template <class T, std::size_t growSize = 1024>
class Allocator : private MemoryPool<T, growSize>
{
   public:
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using const_pointer = T const*;
    using reference = T&;
    using const_reference = T const&;
    using value_type = T;

    template <class U>
    struct rebind
    {
        using other = Allocator<U, growSize>;
    };

    pointer allocate(size_type n, const void *hint = 0)
    {
        if (n != 1 || hint)
            throw std::bad_alloc();

        return MemoryPool<T, growSize>::allocate();
    }

    void deallocate(pointer p, size_type n)
    {
        MemoryPool<T, growSize>::deallocate(p);
    }

    void construct(pointer p, const_reference val)
    {
        new (p) T(val);
    }

    void destroy(pointer p)
    {
        p->~T();
    }
};