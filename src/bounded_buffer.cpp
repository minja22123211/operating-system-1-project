//
// Created by os on 8/21/23.
//

#include "../h/bounded_buffer.hpp"
#include "../h/memory_allocator.hpp"
#include "../h/semaphore.hpp"


void K_Buffer::putChar(char c) {
    buff[tail] = c;
    tail = (tail + 1) % BUFF_SIZE;
    size++;
}

char K_Buffer::getChar() {
    char c = buff[head];
    head = (head + 1) % BUFF_SIZE;
    size--;
    return c;
}

void *K_Buffer::operator new(size_t n) {
    return K_MemoryAllocator::mem_alloc(sizeof(K_Buffer),true);
}

void K_Buffer::operator delete(void *p) {
    K_MemoryAllocator::mem_free(p);
}

K_Buffer::K_Buffer() {
    head=0;tail=0;size=0;
    itemAvaliable = K_Semaphore::createSemaphore(0);
}