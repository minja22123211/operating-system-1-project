#ifndef PROJECT_BASE_BUFF_HPP
#define PROJECT_BASE_BUFF_HPP

const int BUFF_SIZE = 256;

#include "../lib/hw.h"

class K_Console;
class Riscv;
class K_Semaphore;

class K_Buffer{
private:

    K_Buffer();

    char buff[BUFF_SIZE];
    int head,tail,size;
    K_Semaphore* itemAvaliable;

    void putChar(char c);
    char getChar();

    void* operator new(size_t n);
    void operator delete(void*);




    friend class Riscv;
    friend class K_Console;
};

#endif //PROJECT_BASE_BUFF_HPP