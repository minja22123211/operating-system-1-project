//
// Created by os on 8/21/23.
//

//
// Created by os on 8/4/23.
//

#include "../h/syscall_c.h"
#include "../h/memory_allocator.hpp"


uint64 setArguments(uint64 code,uint64 arg1, uint64 arg2, uint64 arg3,uint64 arg4){
    uint64 retValue;
    __asm__ volatile("ecall");
    __asm__ volatile("mv %0,a0" : "=r" (retValue));
    return retValue;
}

void* mem_alloc (size_t size){
    size +=sizeof(K_MemoryAllocator::UsedFragment);
    if(size%MEM_BLOCK_SIZE!=0)size = size/MEM_BLOCK_SIZE + 1;
    else size = size/MEM_BLOCK_SIZE;

    return (void*)setArguments(0x01,size,0,0,0);

}

int mem_free (void* mem){
    return (int)setArguments(0x02,(uint64)mem,0,0,0);
}

int thread_create (thread_t* handle,void(*start_routine)(void*), void* arg){
    uint64 * stack = new uint64[DEFAULT_STACK_SIZE];
    return (int)setArguments(0x11,(uint64)handle,uint64(start_routine),(uint64)arg,(uint64)stack);
}

void thread_dispatch (){
    setArguments(0x13,0,0,0,0);
}

int thread_exit(){
    return (int)setArguments(0x12,0,0,0,0);
}


int sem_open (sem_t* handle,unsigned init){
    return (int)setArguments(0x21,(uint64)handle,(uint64)init,0,0);
}
int sem_close (sem_t handle){
    return (int)setArguments(0x22,(uint64)handle,0,0,0);
}

int sem_wait (sem_t id){
    return (int)setArguments(0x23,(uint64)id,0,0,0);
}
int sem_signal (sem_t id){
    return (int)setArguments(0x24,(uint64)id,0,0,0);
}

void thread_join (thread_t handle){
    setArguments(0x14,(uint64)handle,0,0,0);
}


int time_sleep (time_t time){
    return (int)setArguments(0x31,(uint64)time,0,0,0);
}


char getc (){
    return (char) setArguments(0x41,0,0,0,0);
}

void putc (char character){
    setArguments(0x42,(uint64)character,0,0,0);
}

void swapMode(){
    setArguments(0x51,0,0,0,0);
}

void clean_space(){
    setArguments(0x52,0,0,0,0);
}

int thread_delete(thread_t handle){
    return setArguments(0x53,(uint64)handle,0,0,0);
}
