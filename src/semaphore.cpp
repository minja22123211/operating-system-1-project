//
// Created by os on 8/21/23.
//

//
// Created by os on 8/11/23.
//

#include "../h/semaphore.hpp"
#include "../h/memory_allocator.hpp"
#include "../h/thread.hpp"
#include "../h/scheduler.hpp"

List<K_Semaphore> K_Semaphore::allSemaphores;

K_Semaphore::K_Semaphore(size_t value) {
    this->value = value;
    closedSemaphore = false;
    allSemaphores.add(this);
}

void *K_Semaphore::operator new(size_t n) {
    return K_MemoryAllocator::mem_alloc(sizeof(K_Semaphore), true);
}

void K_Semaphore::operator delete(void *p) {
    K_MemoryAllocator::mem_free(p);
}

int K_Semaphore::wait() {
    if(closedSemaphore)return -1;
    value--;
    if(value<0)block();
    if(closedSemaphore)return -1;
    return 0;
}

int K_Semaphore::signal() {
    if(closedSemaphore)return -1;
    value++;
    if(value<=0)unblock();
    return 0;
}

K_Semaphore *K_Semaphore::createSemaphore(int value) {
    return (K_Semaphore*)new K_Semaphore(value);
}

void K_Semaphore::block() {
    blockedQueue.add(K_Thread::running);
    if(K_Thread::running->state == K_Thread::RUNNING) {
        K_Thread::running->state = K_Thread::SUSPENDED;
    }
    K_Thread::dispatch();
}

void K_Semaphore::unblock() {
    K_Thread* thread = blockedQueue.remove();
    if(thread){
        thread->state = K_Thread::READY;
        K_Scheduler::put(thread);
    }
}

int K_Semaphore::deleteAllSemaphores() {
    while(allSemaphores.head){
        K_Semaphore* sem = allSemaphores.remove();
        if(sem->closedSemaphore)delete sem;
        else return -1;
    }
    return 0;
}

int K_Semaphore::close() {
    if(closedSemaphore)return -1;
    while(blockedQueue.head){
        unblock();
    }
    closedSemaphore = true;
    return 0;
}
