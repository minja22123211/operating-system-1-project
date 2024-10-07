//
// Created by os on 8/1/23.
//


#include "../h/syscall_c.h"
#include "../test/printing.hpp"
#include "../h/riscv.hpp"

extern "C" void supervisorTrap();

extern void userMain();

void wrapperUserMain(void* ptr){
    userMain();
}

void main(){
    Riscv::initKernel();
}

