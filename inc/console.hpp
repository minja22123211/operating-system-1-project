#ifndef PROJECT_BASE_CONSOLE_HPP
#define PROJECT_BASE_CONSOLE_HPP

#include "../lib/hw.h"
#include "bounded_buffer.hpp"
#include "thread.hpp"


class Riscv;
class K_Semaphore;
class K_Thread;

class K_Console{
private:
    K_Console() = delete;

    static K_Buffer* outBuffer;
    static K_Buffer* inBuffer;

    static K_Semaphore* spaceAvaliable;
    static K_Semaphore* consoleReady;


    static void putcProducer(char c);
    static void putcConsumer(void* ptr);

    static char getcConsumer();

    friend class Riscv;
    friend class K_Thread;
};



#endif //PROJECT_BASE_CONSOLE_HPP