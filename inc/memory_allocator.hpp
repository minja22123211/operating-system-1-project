//
// Created by os on 8/1/23.
//

#ifndef PROJECT_BASE_MEMORY_ALLOCATOR_HPP
#define PROJECT_BASE_MEMORY_ALLOCATOR_HPP
#include "../lib/hw.h"

class Riscv;
class K_Thread;
class K_Semaphore;
class ThreadSleepingList;
class K_Buffer;
template <typename T> class List;

class K_MemoryAllocator {
public:
    struct UsedFragment {
        size_t size;
        UsedFragment *next;
    };
private:
    K_MemoryAllocator() = delete;

    static void *mem_alloc(size_t size,bool reduce);
    static int mem_free(void *node);
    static size_t reduceSize(size_t size);

    static UsedFragment *head;
    static UsedFragment *last;

    friend class Riscv;
    friend class K_Thread;
    friend class K_Semaphore;
    friend class ThreadSleepingList;
    friend class K_Buffer;
    friend class List <K_Thread>;
    friend class List<K_Semaphore>;
};


#endif //PROJECT_BASE_MEMORY_ALLOCATOR_HPP