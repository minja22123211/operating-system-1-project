//
// Created by os on 8/13/23.
//
#include "../h/thread_sleeping_list.hpp"
#include "../h/memory_allocator.hpp"
#include "../h/thread.hpp"
#include "../h/scheduler.hpp"

ThreadSleepingList::SleepingNode* ThreadSleepingList::head = nullptr;
ThreadSleepingList::SleepingNode* ThreadSleepingList::last = nullptr;

int ThreadSleepingList::addSleepingNode(K_Thread * thread,time_t time) {
    SleepingNode* node = (SleepingNode*)K_MemoryAllocator::mem_alloc(sizeof(SleepingNode),true);
    if(!node)return -1;
    node->data = thread;
    node->time = time;
    node->next = nullptr;
    if(!head)head = last = node;
    else{
        SleepingNode* curr = head, *prev = nullptr;
        time_t currTime = head->time, prevTime = 0;
        while(curr || prev){
            if(node->time < currTime || !curr){
                if(prev){node->time -= prevTime;prev->next = node;}
                else head = node;
                if(curr)curr->time = currTime - time;
                else last=node;
                node->next = curr;
                break;
            }
            prev = curr;prevTime = currTime;
            if(curr)curr = curr->next;
            if(curr)currTime +=curr->time;
        }
    }
    return 0;
}

K_Thread *ThreadSleepingList::removeSleepingNode() {
    if(!head)return nullptr;
    K_Thread* thread = head->data; SleepingNode* old = head;
    head = head->next;
    if(!head)last = nullptr;
    K_MemoryAllocator::mem_free(old);
    return thread;
}

void ThreadSleepingList::deleteList() {
    while(head){
        SleepingNode* old = head;
        head = head->next;
        K_MemoryAllocator::mem_free(old);
    }
    head = last = nullptr;
}

void ThreadSleepingList::updateList() {
    if(head)head->time--;
    else return;
    while(head){
        if(head->time==0){
            K_Thread* thread = removeSleepingNode();
            if(thread) {
                thread->state = K_Thread::READY;
                K_Scheduler::put(thread);
            }
        }
        else break;
    }
}

void ThreadSleepingList::removeGiven(K_Thread *data) {
    if(!head)return;
    SleepingNode* curr = head, * prev = nullptr;
    while(curr){
        if(curr->data==data){
            SleepingNode* old = curr;
            curr = curr->next;
            if(!prev)head = curr;
            else prev->next = curr;
            if(!curr)last = prev;
            K_MemoryAllocator::mem_free(old);
        }
        prev = curr;
        curr = curr->next;
    }
}

