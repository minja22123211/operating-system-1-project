#ifndef PROJECT_BASE_SEMAPHORE_HPP
#define PROJECT_BASE_SEMAPHORE_HPP

#include "../lib/hw.h"
#include "list.hpp"

class Riscv;
class K_Console;
class K_Buffer;

class K_Semaphore{
private:
    K_Semaphore(size_t);
    static K_Semaphore* createSemaphore(int);
    static int deleteSemaphore(K_Semaphore*);
    static int deleteAllSemaphores();

    List<K_Thread> blockedQueue;
    int value;
    bool closedSemaphore;
    static List<K_Semaphore> allSemaphores;


    int wait();
    int signal();
    void block();
    void unblock();

    int close();
    void* operator new(size_t n);
    void operator delete(void* p);

    friend class Riscv;
    friend class K_Thread;
    friend class K_Console;
    friend class K_Buffer;
};

#endif //PROJECT_BASE_SEMAPHORE_HPP