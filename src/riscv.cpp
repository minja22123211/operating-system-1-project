//
// Created by os on 8/21/23.
//

//
// Created by os on 8/4/23.
//

#include "../h/riscv.hpp"
#include "../h/syscall_c.h"
#include "../test/printing.hpp"
#include "../h/memory_allocator.hpp"
#include "../h/thread.hpp"
#include "../h/scheduler.hpp"
#include "../h/console.hpp"

extern "C" void supervisorTrap();
extern void wrapperUserMain(void* ptr);

static volatile bool userMode = false;
static volatile bool errorPrinted = false;


extern void idleThread(void* ptr);

void Riscv::initConsole() {
    K_Console::outBuffer = new K_Buffer();
    K_Console::inBuffer = new K_Buffer();

    K_Console::consoleReady = K_Semaphore::createSemaphore(0);
    K_Console::spaceAvaliable = K_Semaphore::createSemaphore(BUFF_SIZE);

}


void Riscv::initKernel() {

    uint64 trapAdr = (uint64)(&supervisorTrap) | (uint64)0x01UL;
    __asm__ volatile("csrw stvec, %0" : :  "r" (trapAdr));

    thread_t threadKernelMain;
    thread_t threadPutConsumer;
    thread_t threadUserMain;

    initConsole();

    thread_create(&threadKernelMain,nullptr,nullptr);
    thread_create(&threadPutConsumer,K_Console::putcConsumer,nullptr);
    thread_create(&K_Thread::idle,idleThread, nullptr);
    swapMode();
    thread_create(&threadUserMain,wrapperUserMain,nullptr);

    thread_join(threadUserMain);
    swapMode();
    clean_space();
}

void Riscv::popSppSpie()
{
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void Riscv::handleSyscallTrap() {
    uint64 scause = r_scause();
    uint64 volatile operationCode;
    uint64 volatile arg1;
    uint64 volatile arg2;
    uint64 volatile arg3;
    uint64 volatile arg4;
    __asm__ volatile("mv %0,a4" : "=r" (arg4));
    __asm__ volatile("mv %0,a3" : "=r" (arg3));
    __asm__ volatile("mv %0,a2" : "=r" (arg2));
    __asm__ volatile("mv %0,a1" : "=r" (arg1));
    __asm__ volatile("mv %0,a0" : "=r" (operationCode));

    uint64 volatile sepc = r_sepc() + 4;
    uint64 volatile sstatus = r_sstatus();

    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL)
    {
        uint64 ret=0;
        switch(operationCode){
            case 0x01:
                ret= (uint64)K_MemoryAllocator::mem_alloc(arg1,false);
                __asm__ volatile("mv a0,%0" : : "r" (ret) );
                break;
            case 0x02:
                ret = (uint64)K_MemoryAllocator::mem_free((void*)arg1);
                __asm__ volatile("mv a0,%0" : : "r" (ret) );
                break;
            case 0x11:
                ret = (uint64)K_Thread::createThread((K_Thread::Body)arg2,(uint64*)arg4,(void*)arg3,DEFAULT_TIME_SLICE);
                *((K_Thread**)arg1) = (K_Thread*)ret;
                if(ret!=0) __asm__ volatile("li a0,0x00");
                else __asm__ volatile("li a0,0xFFFFFFFFFFFFFFFF"); // return fault code
                break;
            case 0x12:
                K_Thread::exit();
                __asm__ volatile("mv a0,%0" : : "r" (ret) ); // return fault code
                break;
            case 0x13:
                K_Thread::dispatch();
                break;
            case 0x14:
                if(K_Thread::running != (K_Thread*)arg1)    ((K_Thread*)arg1)->sem->wait();
                break;
            case 0x21:
                ret = (uint64)K_Semaphore::createSemaphore((unsigned)arg2);
                *((K_Thread**)arg1) = (K_Thread*)ret;
                if(ret!=0) __asm__ volatile("li a0,0x00");
                else __asm__ volatile("li a0,0xFFFFFFFFFFFFFFFF");
                break;
            case 0x22:
                ret = (uint64)( ((K_Semaphore*)arg1)->close());
                if(ret==0) __asm__ volatile("li a0,0x00");
                else __asm__ volatile("li a0,0xFFFFFFFFFFFFFFFF");
                break;
            case 0x23:
                ret = (uint64)( ((K_Semaphore*)arg1)->wait() );
                if(ret==0) __asm__ volatile("li a0,0x00");
                else __asm__ volatile("li a0,0xFFFFFFFFFFFFFFFF");
                break;
            case 0x24:
                ret = (uint64)((K_Semaphore*)arg1)->signal();
                if(ret==0) __asm__ volatile("li a0,0x00");
                else __asm__ volatile("li a0,0xFFFFFFFFFFFFFFFF");
                break;
            case 0x31:
                if((time_t)arg1<=0){
                    __asm__ volatile("li a0,0xFFFFFFFFFFFFFFFF");
                    break;
                }
                K_Thread::running->state = K_Thread::SUSPENDED;
                ret = (uint64)ThreadSleepingList::addSleepingNode(K_Thread::running,(time_t)arg1);
                K_Thread::dispatch();
                if(ret==0) __asm__ volatile("li a0,0x00");
                else __asm__ volatile("li a0,0xFFFFFFFFFFFFFFFF");
                break;
            case 0x41:
                ret = (char)K_Console::getcConsumer();
                __asm__ volatile("mv a0,%0" : : "r" (ret) );
                break;
            case 0x42:
                K_Console::putcProducer((char)arg1);
                break;
            case 0x51:
                if(!userMode){sstatus = sstatus & (~(1<<8));userMode = true;}
                else {sstatus = sstatus | (1<<8);userMode = false;}
                break;
            case 0x52:
                cleanSpace();
                break;
            case 0x53:
                ret = K_Thread::deleteThread((K_Thread*)arg1);
                if(ret==0) __asm__ volatile("li a0,0x00");
                else __asm__ volatile("li a0,0xFFFFFFFFFFFFFFFF");
                break;
            default:
                break;
        }
        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    else{
        if(!errorPrinted) {

            printString("Error : \n");
            printInt(scause);
            K_Console::putcProducer('\n');
            printInt(r_sepc());
            K_Console::putcProducer('\n');
            errorPrinted = true;
        }
    }

}

void Riscv::handleTimerTrap() {
    mc_sip(SIP_SSIP);
    ThreadSleepingList::updateList();
    K_Thread::timerCounter++;
    if(K_Thread::timerCounter >= K_Thread::running->timeSlice){
        uint64 volatile sepc = r_sepc();
        uint64 volatile sstatus = r_sstatus();
        K_Thread::dispatch();
        w_sstatus(sstatus);
        w_sepc(sepc);
    }
}

void Riscv::handleConsoleTrap() {
    static bool first = true;
    mc_sip(SIP_SEIP);
    int ireq = plic_claim();
    if(ireq==0x0a){
        if((*((char*)CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT) && !first)K_Console::consoleReady->unblock();
        if(first)first = false;
        while((*((char*)CONSOLE_STATUS) & CONSOLE_RX_STATUS_BIT) && K_Console::inBuffer->size<BUFF_SIZE){
            K_Console::inBuffer->putChar(*((char*)CONSOLE_RX_DATA));
            if(K_Console::inBuffer->size==1)K_Console::inBuffer->itemAvaliable->unblock();
        }
        plic_complete(ireq);
    }
}

void Riscv::cleanSpace() {
    delete K_Console::outBuffer;
    delete K_Console::inBuffer;

    K_Scheduler::readyQueue.deleteList();
    ThreadSleepingList::deleteList();
    K_Semaphore::deleteAllSemaphores();
    //if(res!=0){printString("FATAL ERROR!");return;}
    K_Thread::deleteAllThreads();
    //if(res!=0){printString("FATAL ERROR!");return;}
}