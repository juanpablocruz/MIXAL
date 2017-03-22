#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MEMORY 4000

#include "operations.h"
#include "datatypes.h"

struct Memory memoria;
void *memory;
memory_register A;
memory_register X;
Index indices[6];
memory_register J;
char *CMD;
char *line;
memory_register *ptrAddr;

memory_register* getRegisterFromAddress(int addr) {
    memory_register* R;
    R = &memory;
    R += addr;
    return R;
}


void setRegister(memory_register *r, uint32_t value) {
    memory_register *ptr = &memory;
    ptr += r->ADDR;
    ptr->DATA = value;
    ptr->Sign = r->Sign;
}


Modifiers getModifiers(char *cmd,char *args) {
    Modifiers mods;
    char *address = strtok(args,"(");
    char *delim = "";
    int size = 0;
    mods.to = 6;
    mods.from = 0;
    if((delim = strtok(NULL,"("))) {
        delim = strtok(delim,")");
        mods.from = atoi(strtok(delim, ":"));
        mods.to = atoi(strtok(NULL,":"));
    }
    int addr = atoi(address);
    mods.address = addr;
    return mods;
}

void storeRegister(memory_register *R, int addrs) {
    memory_register *ptr = &memory;
    ptr += addrs;
    *ptr = *R;
}

int run() {
    int result = 0;
    ptrAddr = &memory;
    ptrAddr += J.INDEX;
    memory_register *tmp = &memory;
    int instr = 0;
    while(instr++ < 10) {
        tmp = &memory;
        switch(ptrAddr->COD) {
        case LDA:
            tmp += ptrAddr->ADDR;
            A.DATA = tmp->DATA;
            break;
        case LDX:
            tmp += ptrAddr->ADDR;
            X.DATA = tmp->DATA;
            break;
        case STA:
            storeRegister(&A, ptrAddr->ADDR);
            break;
        case STX:
            storeRegister(&X, ptrAddr->ADDR);
            break;
        case ADD:
            tmp += ptrAddr->ADDR;
            int v = (A.DATA * A.Sign) + (tmp->DATA * tmp->Sign);
            A.Sign = (v > 0)?POSITIVE:NEGATIVE;
            A.DATA = abs(v);
            break;
        }
        ptrAddr++;
    }

    return result;
}

void compile(char *cmd, char *args) {
    Modifiers mods = getModifiers(cmd,args);

    if(strcmp(cmd,"LDA") == 0) {
        ptrAddr->COD = LDA;
        ptrAddr->ADDR = mods.address;
        ptrAddr++;
    } else if(strcmp(cmd,"LDX") == 0) {
        ptrAddr->COD = LDX;
        ptrAddr->ADDR = mods.address;
        ptrAddr++;
    } else if((strlen(cmd) == 3) && cmd[0] == 'L' && cmd[1] == 'D') {
        switch(cmd[2]) {
        case '1':
            ptrAddr->COD = LD1;
            ptrAddr->ADDR = mods.address;
            ptrAddr++;
            break;
        case '2':
            ptrAddr->COD = LD2;
            ptrAddr->ADDR = mods.address;
            ptrAddr++;
            break;
        case '3':
            ptrAddr->COD = LD3;
            ptrAddr->ADDR = mods.address;
            ptrAddr++;
            break;
        case '4':
            ptrAddr->COD = LD4;
            ptrAddr->ADDR = mods.address;
            ptrAddr++;
            break;
        case '5':
            ptrAddr->COD = LD5;
            ptrAddr->ADDR = mods.address;
            ptrAddr++;
            break;
        case '6':
            ptrAddr->COD = LD6;
            ptrAddr->ADDR = mods.address;
            ptrAddr++;
            break;
        }
    } else if(strcmp(cmd,"STA") == 0) {
        ptrAddr->COD = STA;
        ptrAddr->ADDR = mods.address;
        ptrAddr++;
    } else if(strcmp(cmd,"STX") == 0) {
        ptrAddr->COD = STX;
        ptrAddr->ADDR = mods.address;
        ptrAddr++;
    } else if(strcmp(cmd,"ADD") == 0) {
       ptrAddr->COD = ADD;
       ptrAddr->ADDR = mods.address;
       ptrAddr++;
    } else if(strcmp(cmd,"ORIG") == 0) {
       Modifiers mods = getModifiers(cmd,args);
       J.INDEX = mods.address;
       ptrAddr += J.INDEX;
    }
}

void printInnerMemory() {
    int j = 1;
    memory_register *it = &memory;
    memory_register *value;
    printf("Sizeof: %u\n",sizeof(memory_register));
    while(it != ptrAddr) {
        value = &memory;
        value += it->ADDR;
        printf("%u %u:%c%u",((((int)&(*it)) - (int)&memory))/sizeof(memory_register),it->COD,(it->Sign>0)?'+':'-',it->ADDR);
        if(&(*it) > value) {
            printf(" %u",value->COD);
        }
        printf("\t");
        //if(!(j++ % 7))
            printf("\n");
        it++;
    }

    printf("\n");
}

void printSpecialRegisters() {
    printf("\n\n\t Special Registers \n");

    printf("Register A: ");
    printf("0X%x: ",&A);
    printf("%c%u",(A.Sign > 0)?'+':'-',A.DATA);
    printf("\n");

    printf("Register X: ");
    printf("0X%x: ",&X);
    printf("%c%u",(X.Sign > 0)?'+':'-',X.DATA);
    printf("\n");

    printf("Index J: ");
    printf("0X%x: ",&J);
    printf("%u",J.INDEX);
    printf("\n");
}

void parseCode(const char* src) {
    char *end_str;
    char *line = strtok_s(src,"\n",&end_str);

    while(line != NULL) {
        char *end_token;
        char *cmd = strtok_s(line, " ",&end_token);
        char *args = strtok_s(NULL," ",&end_token);
        compile(cmd,args);
        line = strtok_s(NULL, "\n", &end_str);
    }
}

char *source = "ORIG 2015\n\
               LD1 2000(0:2)\n\
               LD6 2000(0:2)\n\
               LDX 2000\n\
               STX 2001\n\
               LDA 2000\n\
               ADD 1999\n\
               STA 2002\n\
               LDA 2016\n";

int main(int argc, char *argv[])
{
    memory = malloc( MAX_MEMORY * sizeof(memory_register));
    ptrAddr = &memory;
    J.DATA = 0;
    A.Sign = POSITIVE;
    X.Sign = POSITIVE;

    memory_register r1;
    r1.ADDR = 2000;
    r1.Sign = POSITIVE;
    memory_register r2;
    r2.ADDR = 1999;
    r2.Sign = NEGATIVE;

    setRegister(&r1, 80354);
    setRegister(&r2, 11111);

    parseCode(source);

    run();
    printInnerMemory();
    printSpecialRegisters();

    return 0;
}
