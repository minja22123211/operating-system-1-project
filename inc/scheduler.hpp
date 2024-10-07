//
// Created by os on 8/6/23.
//
#ifndef PROJECT_BASE_SCHEDULER_HPP
#define PROJECT_BASE_SCHEDULER_HPP

#include "list.hpp"

class K_Thread;
class K_Semaphore;
class Riscv;
class ThreadSleepingList;

class K_Scheduler{
private:
    K_Scheduler() = delete;

    static void put(K_Thread*);
    static K_Thread* get();

    static List<K_Thread> readyQueue;

    friend class K_Thread;
    friend class K_Semaphore;
    friend class Riscv;
    friend class ThreadSleepingList;
};

#endif //PROJECT_BASE_SCHEDULER_HPP