//
// Created by os on 8/21/23.
//

//
// Created by os on 8/14/23.
//
#include "../h/console.hpp"
#include "../h/semaphore.hpp"


K_Buffer* K_Console::outBuffer;
K_Buffer* K_Console::inBuffer;

K_Semaphore* K_Console::consoleReady;
K_Semaphore* K_Console::spaceAvaliable;




void K_Console::putcProducer(char c) {
    volatile char tmp = c;
    if(outBuffer->size==BUFF_SIZE)spaceAvaliable->block();
    outBuffer->putChar(tmp);
    if(outBuffer->size==1)outBuffer->itemAvaliable->unblock();
}

void K_Console::putcConsumer(void* ptr) {
    while(1){
        if(outBuffer->size==0)outBuffer->itemAvaliable->block();
        if( !(*((char*)CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT))consoleReady->block();
        *((char*)CONSOLE_TX_DATA) = outBuffer->getChar();
        if(outBuffer->size==BUFF_SIZE-1)spaceAvaliable->unblock();
    }
}

char K_Console::getcConsumer() {
    if(inBuffer->size==0)inBuffer->itemAvaliable->block();
    char c = inBuffer->getChar();
    return c;
}