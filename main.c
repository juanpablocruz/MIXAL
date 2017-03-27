#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
label_vector computedLabels;


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

void push_label(label l) {
    if(computedLabels.used == computedLabels.size) {
        computedLabels.size*=2;
        label* newBuf = realloc(computedLabels.buffer, computedLabels.size);
        if(!newBuf) {
            abort();
        }
        computedLabels.buffer = newBuf;
    }
    computedLabels.buffer[computedLabels.used++] = l;
}


Modifiers getModifiers(char *cmd,char *args) {
    Modifiers mods;
    char *address = strtok(args,"(");
    char *delim = "";
    int size = 0;
    int to, from;
    to = 5;
    from = 0;
    if((delim = strtok(NULL,"("))) {
        delim = strtok(delim,")");
        from = atoi(strtok(delim, ":"));
        to = atoi(strtok(NULL,":"));
    }
    int addr = atoi(address);
    mods.address = addr;

    int FiveBitMask = 0xffffff;
    int fromMask = (int)(FiveBitMask >> (from * sizeof(int)));
    int toMask = (int)(FiveBitMask << ((5-to) * sizeof(int))) & FiveBitMask;
    mods.mask = (fromMask & toMask)& FiveBitMask;
    return mods;
}

void storeRegister(memory_register *R, int addrs) {
    memory_register *ptr = &memory;
    ptr += addrs;
    *ptr = *R;
}

uint32_t getMasked(uint32_t mask, uint32_t value) {
    uint32_t result = 0;
    int hex = mask;
    int val = value;
    int decimal = 0;
    int h, v;
    for(int i=1; i < 6; i++) {
        h = hex%16;
        v = val%10;
        hex/=16;
        val/=10;
        result += (v&h)*(int)pow(10,decimal);
        decimal++;
    }
    if(hex%16) result *= -1;
    return result;
}

int run() {
    int result = 0;
    ptrAddr = &memory;
    ptrAddr += J.INDEX;
    memory_register *tmp = &memory;
    int instr = 0;
    int v,v2;
    while(instr++ < 10) {
        tmp = &memory;
        switch(ptrAddr->COD) {
        case LDA:
            tmp += ptrAddr->ADDR;
            v = getMasked(ptrAddr->mask, tmp->DATA);
            if(v < 0) {
                v = abs(v);
                X.Sign = -1;
            }
            A.DATA = v;
            break;
        case LDX:
            tmp += ptrAddr->ADDR;
            v = getMasked(ptrAddr->mask, tmp->DATA);
            if(v < 0) {
                v = abs(v);
                X.Sign = -1;
            }
            X.DATA = v;
            break;
        case STA:
            storeRegister(&A, ptrAddr->ADDR);
            break;
        case STX:
            storeRegister(&X, ptrAddr->ADDR);
            break;
        case ADD:
            tmp += ptrAddr->ADDR;
            v = getMasked(ptrAddr->mask, tmp->DATA);
            if(v < 0) {
                v = abs(v);
                tmp->Sign = -1;
            }

            v2 = (A.DATA * A.Sign) + (v * tmp->Sign);
            A.Sign = (v2 > 0)?POSITIVE:NEGATIVE;
            A.DATA = abs(v2);
            break;
        case SUB:
            tmp += ptrAddr->ADDR;
            v = getMasked(ptrAddr->mask, tmp->DATA);
            if(v < 0) {
                v = abs(v);
                tmp->Sign = -1;
            }

            v2 = (A.DATA * A.Sign) + (v * tmp->Sign * -1);
            A.Sign = (v2 > 0)?POSITIVE:NEGATIVE;
            A.DATA = abs(v2);
            break;
        }
        ptrAddr++;
    }

    return result;
}

void compile(char *cmd, char *args, label l) {
    Modifiers mods = getModifiers(cmd,args);
    if(strcmp(l.label,"") != 0) {
        l.address = &ptrAddr;
        push_label(l);
    }
    if(strcmp(cmd,"LDA") == 0) {
        ptrAddr->COD = LDA;
        ptrAddr->ADDR = mods.address;
        ptrAddr->mask = mods.mask;
        ptrAddr++;
    } else if(strcmp(cmd,"LDX") == 0) {
        ptrAddr->COD = LDX;
        ptrAddr->ADDR = mods.address;
        ptrAddr->mask = mods.mask;
        ptrAddr++;
    } else if((strlen(cmd) == 3) && cmd[0] == 'L' && cmd[1] == 'D') {
        switch(cmd[2]) {
        case '1':
            ptrAddr->COD = LD1;
            ptrAddr->ADDR = mods.address;
            ptrAddr->mask = mods.mask;
            ptrAddr++;
            break;
        case '2':
            ptrAddr->COD = LD2;
            ptrAddr->ADDR = mods.address;
            ptrAddr->mask = mods.mask;
            ptrAddr++;
            break;
        case '3':
            ptrAddr->COD = LD3;
            ptrAddr->ADDR = mods.address;
            ptrAddr->mask = mods.mask;
            ptrAddr++;
            break;
        case '4':
            ptrAddr->COD = LD4;
            ptrAddr->ADDR = mods.address;
            ptrAddr->mask = mods.mask;
            ptrAddr++;
            break;
        case '5':
            ptrAddr->COD = LD5;
            ptrAddr->ADDR = mods.address;
            ptrAddr->mask = mods.mask;
            ptrAddr++;
            break;
        case '6':
            ptrAddr->COD = LD6;
            ptrAddr->ADDR = mods.address;
            ptrAddr->mask = mods.mask;
            ptrAddr++;
            break;
        }
    } else if(strcmp(cmd,"STA") == 0) {
        ptrAddr->COD = STA;
        ptrAddr->ADDR = mods.address;
        ptrAddr->mask = mods.mask;
        ptrAddr++;
    } else if(strcmp(cmd,"STX") == 0) {
        ptrAddr->COD = STX;
        ptrAddr->ADDR = mods.address;
        ptrAddr->mask = mods.mask;
        ptrAddr++;
    } else if(strcmp(cmd,"ADD") == 0) {
       ptrAddr->COD = ADD;
       ptrAddr->ADDR = mods.address;
       ptrAddr->mask = mods.mask;
       ptrAddr++;
    } else if(strcmp(cmd,"SUB") == 0) {
        ptrAddr->COD = SUB;
        ptrAddr->ADDR = mods.address;
        ptrAddr->mask = mods.mask;
        ptrAddr++;
     } else if(strcmp(cmd,"ORIG") == 0) {
       J.INDEX = mods.address;
       ptrAddr->mask = mods.mask;
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
        printf("%u %u:%c%u (0x%6x)",
               ((((int)&(*it)) - (int)&memory))/sizeof(memory_register),
               it->COD,
               (it->Sign>0)?'+':'-',
               it->ADDR,
               it->mask);
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
        int total_args = 0;
        char *corig = line;
        while((*corig++)) {if(*corig==' ') total_args++;}
        char *end_token;
        char *cmd;
        char *labels;
        label l;
        l.label = "";
        if(total_args > 1) {
            labels = strtok_s(line, " ",&end_token);
            cmd = strtok_s(NULL, " ",&end_token);
            l.label = labels;
            printf("%s ",labels);
        } else {
            cmd = strtok_s(line, " ",&end_token);
        }

        char *args = strtok_s(NULL," ",&end_token);
        printf("%s %s\n",cmd,args);
        compile(cmd,args, l);
        line = strtok_s(NULL, "\n", &end_str);
    }
}

void printLabels() {
    if(computedLabels.used > 0) {
        printf("\nLabels: \n");
        for(int i = 0; i < computedLabels.used; i++) {
            printf("%s 0x%x\n", computedLabels.buffer[i].label, computedLabels.buffer[i].address);
        }
        printf("\n");
    }

}

char *source = "ORIG 2015\n\
LD1 2000(1:2)\n\
LD6 2000(0:2)\n\
LDX 1999(2:5)\n\
STORE STX 2001\n\
LDA 2000\n\
ADD 1999(3:4)\n\
STA 2002\n\
LDA 2016\n";

int main(int argc, char *argv[])
{
    memory = calloc( MAX_MEMORY, MAX_MEMORY * sizeof(memory_register));
    computedLabels.size = 2;
    computedLabels.used = 0;
    computedLabels.buffer = (label*)malloc(sizeof(label*) * computedLabels.size);
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
        printLabels();

    return 0;
}
