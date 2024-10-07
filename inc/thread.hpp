//
// Created by os on 8/5/23.
//

#ifndef PROJECT_BASE_THREAD_HPP
#define PROJECT_BASE_THREAD_HPP

#include "../lib/hw.h"
#include "semaphore.hpp"
#include "thread_sleeping_list.hpp"
#include "list.hpp"

class K_Scheduler;
class Riscv;
class K_Console;
class ThreadSleepingList;

class K_Thread{
public:
    K_Semaphore* getSemaphore()const{return sem;}
private:
    using Body = void (*)(void*);
    struct Context{
        uint64 ra,sp;
    };
    enum State{SUSPENDED,READY,RUNNING,TERMINATING};
    enum Mode{USER,SYSTEM};

    K_Thread(Body,uint64*,void*,time_t);
    ~K_Thread();
    static K_Thread* createThread(Body,uint64*,void*,time_t);
    static int deleteThread(K_Thread*);

    static K_Thread* idle;
    static time_t timerCounter;
    static K_Thread* running;
    static List<K_Thread> allThreads;

    Body body;
    void* threadArg;
    uint64* stack;
    Context context;
    time_t timeSlice;
    State state;
    K_Semaphore* sem;
    Mode mode;

    static int deleteAllThreads();
    static int exit();
    static void threadWrapper();
    static void dispatch();
    static void yield(Context *oldContext, Context *runningContext);

    void* operator new(size_t n);
    void operator delete(void* p);

    friend class K_Scheduler;
    friend class Riscv;
    friend class K_Semaphore;
    friend class K_Console;
    friend class ThreadSleepingList;
};

#endif //PROJECT_BASE_THREAD_HPP