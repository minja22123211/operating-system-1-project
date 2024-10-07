#ifndef PROJECT_BASE_THREAD_SLEEPING_LIST_HPP
#define PROJECT_BASE_THREAD_SLEEPING_LIST_HPP

#include "../lib/hw.h"

class K_Thread;
class Riscv;
class ThreadSleepingList{
public:
    struct SleepingNode{
        K_Thread* data;
        time_t time;
        SleepingNode* next;
    };
private:
    ThreadSleepingList() = delete;

    static SleepingNode* head;
    static SleepingNode* last;
    static int addSleepingNode(K_Thread *, time_t);
    static K_Thread *removeSleepingNode();
    static void updateList();
    static void deleteList();
    static void removeGiven(K_Thread* data);

    friend class  K_Thread;
    friend class Riscv;
};



#endif //PROJECT_BASE_THREAD_SLEEPING_LIST_HPP