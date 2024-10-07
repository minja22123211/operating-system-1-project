//
// Created by os on 8/21/23.
//

//
// Created by os on 8/1/23.
//
#include "../h/memory_allocator.hpp"

K_MemoryAllocator::UsedFragment* K_MemoryAllocator::head = nullptr;
K_MemoryAllocator::UsedFragment* K_MemoryAllocator::last = nullptr;

K_MemoryAllocator::UsedFragment *initilaizeNode(void *ptr, uint64 sz, K_MemoryAllocator::UsedFragment *nxt) {
    K_MemoryAllocator::UsedFragment *node = (K_MemoryAllocator::UsedFragment *) ptr;
    node->size = sz;
    node->next = nxt;
    return node;
}

void* K_MemoryAllocator::mem_alloc(size_t size,bool reduce) {
    // first fit
    if(reduce)size = reduceSize(size);
    size = size * MEM_BLOCK_SIZE;
    if (size > (size_t) HEAP_END_ADDR - (size_t) HEAP_START_ADDR)return nullptr;
    bool first = true;
    UsedFragment *curr = head, *prev = nullptr;
    size_t end = curr != nullptr ? (size_t) head : 0, start = (size_t) HEAP_START_ADDR, sz = 0;
    while (first || prev) {
        if (first)first = false;
        if (!head || (size <= end - (start + sz))) {
            UsedFragment *node = initilaizeNode((void *) ((size_t) start + sz), size, curr);
            prev != nullptr ? prev->next = node : head = node;
            if (!curr)last = node;
            return (void *) ((uint64) node + sizeof(UsedFragment));
        }
        prev = curr;
        curr = curr->next;
        start = end;
        curr != nullptr ? end = (size_t) curr : end = (size_t) HEAP_END_ADDR;
        sz = prev->size;
    }
    return nullptr;
}

int K_MemoryAllocator::mem_free(void *node) {
    node = (UsedFragment *) ((uint64) node - sizeof(UsedFragment));
    UsedFragment *curr = head;
    UsedFragment *prev = nullptr;
    while (curr && curr != node) {
        prev = curr;
        curr = curr->next;
    }
    if (!curr)return -1;
    prev != nullptr ? prev->next = curr->next : head = curr->next;
    if (last == curr)last = prev;
    return 0;
}

size_t K_MemoryAllocator::reduceSize(size_t size) {
    size += sizeof(UsedFragment);
    size_t prev_size = size;
    size = size/MEM_BLOCK_SIZE;
    if(prev_size%MEM_BLOCK_SIZE!=0)size++;
    return size;
}