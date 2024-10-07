
#include "../h/scheduler.hpp"
#include "../h/thread.hpp"

List<K_Thread> K_Scheduler::readyQueue;

void K_Scheduler::put(K_Thread * thread) {
    readyQueue.add(thread);
}

K_Thread* K_Scheduler::get() {
    if(!readyQueue.head)return K_Thread::idle;
    else return readyQueue.remove();
}
