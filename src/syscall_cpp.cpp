//
// Created by os on 8/21/23.
//


#include "../h/syscall_cpp.hpp"
#include "../h/thread.hpp"

using size_t = decltype(sizeof(0));

void *operator new(size_t n)
{
    return mem_alloc(n);
}

void *operator new[](size_t n)
{
    return mem_alloc(n);
}

void operator delete(void *p)
{
    mem_free(p);
}

void operator delete[](void *p)
{
    mem_free(p);
}

Thread::Thread(void (*body)(void *), void *arg) {
    thread_create(&myHandle,body,arg);
}

Thread::~Thread() {
    thread_delete(myHandle);
}

int Thread::start() {
    if(arg==this)return thread_create(&myHandle,body,arg);
    return -1;
}

void Thread::join() {
    sem_wait(myHandle->getSemaphore());
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t time) {
    return time_sleep(time);
}

Thread::Thread() {
    body = WrapperThread::wrapper;
    arg = this;
}

void WrapperThread::wrapper(void *arg) {
    WrapperThread* t = (WrapperThread*)arg;
    t->run();
}

Semaphore::Semaphore(unsigned int init) {
    sem_open(&myHandle,init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

void PeriodicThread::terminate() {
    delete this;
}

PeriodicThread::PeriodicThread(time_t period) {
    this->period = period;
}

void PeriodicThread::run() {
    while(1){
        periodicActivation();
        sleep(period);
    }
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}