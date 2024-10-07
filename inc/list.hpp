//
// Created by marko on 20.4.22..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_LIST_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_LIST_HPP

#include "memory_allocator.hpp"

class K_Semaphore;
class K_Scheduler;
class K_Thread;
class Riscv;


template<typename T>
class List
{
private:
    struct Elem
    {
        T *data;
        Elem *next;

        Elem(T *data, Elem *next) : data(data), next(next) {}
    };

    Elem *head, *tail;

public:
    List() : head(0), tail(0) {}

    List(const List<T> &) = delete;

    List<T> &operator=(const List<T> &) = delete;

    void add(T *data)
    {
        Elem *elem = (Elem*)K_MemoryAllocator::mem_alloc(sizeof(Elem),true);
        elem->data = data;
        elem->next = nullptr;
        if (tail)
        {
            tail->next = elem;
            tail = elem;
        } else
        {
            head = tail = elem;
        }
    }

    T *remove()
    {
        if (!head) { return 0; }

        Elem *elem = head;
        head = head->next;
        if (!head) { tail = 0; }

        T *ret = elem->data;
        K_MemoryAllocator::mem_free(elem);
        return ret;
    }

    void removeGiven(T* data){
        Elem* curr = head, * prev = nullptr;
        while(curr){
            if(curr->data==data){
                Elem* old = curr;
                curr = curr->next;
                if(!prev)head = curr;
                else prev->next = curr;
                if(!curr)tail = prev;
                K_MemoryAllocator::mem_free(old);
            }
            prev = curr;
            curr = curr->next;
        }
    }

    void deleteList(){
        while(head){
            Elem* old = head;
            head = head->next;
            K_MemoryAllocator::mem_free(old);
        }
        head = tail = nullptr;
    }

    friend class  K_Semaphore;
    friend class K_Scheduler;
    friend class K_Thread;
    friend class Riscv;
};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_LIST_HPP
