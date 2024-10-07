
#include "../h/thread.hpp"
#include "../h/memory_allocator.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.h"
#include "../h/scheduler.hpp"
#include "../test/printing.hpp"

#include "../h/console.hpp"

K_Thread* K_Thread::running = nullptr;
time_t K_Thread::timerCounter = 0;
K_Thread* K_Thread::idle;
List<K_Thread> K_Thread::allThreads;



void idleThread(void* ptr){
    while(1){}
}

K_Thread *K_Thread::createThread(Body body,uint64* stack,void* threadArg,time_t timeSlice) {
    K_Thread* thread = new K_Thread(body,stack,threadArg,timeSlice);
    return thread;
}

int K_Thread::deleteThread(K_Thread * thread) {
    if(!thread->sem->closedSemaphore)thread->sem->close();
    if(thread->state!=TERMINATING && thread!=running){
        //K_Scheduler::readyQueue.removeGiven(thread);
        //ThreadSleepingList::removeGiven(thread);
    }
    else if(thread==running)dispatch();
    return 0;
}


void *K_Thread::operator new(size_t n) {
    return K_MemoryAllocator::mem_alloc(sizeof(K_Thread),true);
}

void K_Thread::operator delete(void *p) {
    K_MemoryAllocator::mem_free(p);
}


void K_Thread::dispatch() {
    timerCounter = 0;
    K_Thread* oldRunning = running;
    if(oldRunning->state!=TERMINATING && oldRunning->state!=SUSPENDED && oldRunning->body!=idleThread){
        K_Scheduler::put(oldRunning);
        oldRunning->state = READY;
    }
    running = K_Scheduler::get();
    running->state = RUNNING;
    if(running==oldRunning)return;
    yield(&oldRunning->context,&running->context);
}

K_Thread::K_Thread(K_Thread::Body b,uint64* s,void* arg,time_t time){
    body = b;
    stack = s;
    threadArg = arg;
    context = {(uint64) &threadWrapper ,
               (uint64) &stack[DEFAULT_STACK_SIZE]};
    timeSlice = time;
    sem = K_Semaphore::createSemaphore(0);
    state = READY;
    if(!body || body==K_Console::putcConsumer)mode = SYSTEM;
    else mode = USER;
    if(!running){running = this;state = RUNNING;}
    else if(body!=idleThread){K_Scheduler::put(this);}
    allThreads.add(this);
}

K_Thread::~K_Thread() {
    K_MemoryAllocator::mem_free(stack);
}


void K_Thread::yield(K_Thread::Context *oldContext, K_Thread::Context *runningContext) {
    //_ZN8K_Thread5yieldEPNS_7ContextES1_:
    __asm__ volatile("sd ra,0(a0)");
    __asm__ volatile("sd sp,8(a0)");

    __asm__ volatile("ld ra,0(a1)");
    __asm__ volatile("ld sp,8(a1)");

}

void K_Thread::threadWrapper() {
    Riscv::popSppSpie();
    if(running->mode==SYSTEM)swapMode();
    running->body(running->threadArg);
    if(running->mode==SYSTEM)swapMode();
    thread_exit();
}

int K_Thread::exit(){
    running->state = TERMINATING;
    deleteThread(running);
    return 0;
}

int K_Thread::deleteAllThreads() {
    while(allThreads.head){
        K_Thread* thread= allThreads.remove();
        delete thread;
    }
    return 0;
}
