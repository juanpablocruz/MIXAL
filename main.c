#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MEMORY 4000

#include "datatypes.h"

struct Memory memoria;
Register A;
Register X;
Index indices[6];
Index J;
char *CMD;
char *line;


void copyArrayMod(char *a, const char *b, Modifiers mod) {
    int i = (mod.from)-1;
    int l = (mod.to)-1;
    while((i <= l) && (*a++ = *(b+i++)));
}
void copyArray(char *a, const char *b) {
    while((*a++ = *b++));
}

Register getRegisterFromAddress(int addr) {
    Register result;
    if(addr < MAX_MEMORY && addr >= 0) {
        result = memoria.data[addr];
    }
    return result;
}


char getSign(enum signs sign) {
    char s = ' ';
    switch (sign) {
    case POSITIVE:
        s = '+';
        break;
    case NEGATIVE:
        s = '-';
        break;
    default:
        break;
    }
    return s;
}

void printRegister(Register *r) {
    printf("0X%x: ",&(*r));
    printf("%c",getSign(r->sign));
    for(int i = 0; i < sizeof(r->data); i++) {
        printf("%c",r->data[i]);
    }
    printf("\n");
}

void printIndex(int i) {
    Index *in = &(indices[i]);
    printf("0X%x: ",&in + i);
    printf("%c",getSign(in->sign));
    for(int i = 0; i < sizeof(in->data); i++) {
        printf("%c",in->data[i]);
    }
    printf("\n");
}

void setRegister(enum signs s, const char *value, Register *r) {
    r->sign = s;
    copyArray(&r->data, value);
}

void setIndex(enum signs s, const char *value, int index) {
    int in = index-48;
    Index *i = indices + in;
    i->sign = s;
    copyArray(&i->data, value);
    indices[in] = *i;
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

void loadRegister(Register *des, char *cmd,char *args) {
    enum signs s;

    Modifiers mods = getModifiers(cmd,args);
    Register d = getRegisterFromAddress(mods.address);

    if(mods.from == 0) {
        mods.from = 1;
        mods.to = 6;
        s = d.sign;
    }
    char *reg2 = "";
    copyArrayMod(&reg2,&d.data,mods);
    setRegister(s, &reg2,des);
}

void storeRegister(Register *src, char *cmd, char *args) {
    Modifiers mods = getModifiers(cmd, args);

    if(mods.address < MAX_MEMORY && mods.address >= 0) {
        memoria.data[mods.address] = *src;
    }
}

void loadIndex(int i, char *cmd,char *args) {
    enum signs s;

    Modifiers mods = getModifiers(cmd,args);
    Register d = getRegisterFromAddress(mods.address);

    if(mods.from == 0) {
        mods.from = 4;
        mods.to = 6;
        s = d.sign;
    }
    char *reg2 = "";
    copyArrayMod(&reg2,&d.data,mods);
    setIndex(s, &reg2, i);
}

void add(char *cmd, char *args) {
    Modifiers mods = getModifiers(cmd,args);
    Register d = getRegisterFromAddress(mods.address);

    int rA = atoi(A.data);
    int rR = atoi(d.data);

    switch (A.sign) {
    case NEGATIVE:
        rA *= -1;
        break;
    }
    switch (d.sign) {
    case NEGATIVE:
        rR *= -1;
        break;
    }

    rA += rR;
    if(rA >= 0) {
        A.sign = POSITIVE;
    } else {
        A.sign = NEGATIVE;
    }
    sprintf(A.data, "%d", abs(rA));
}

void process(char *cmd, char *args) {
    if(strcmp(cmd,"LDA") == 0) {
        loadRegister(&A,cmd, args);
    } else if(strcmp(cmd,"LDX") == 0) {
        loadRegister(&X,cmd, args);
    } else if((strlen(cmd) == 3) && cmd[0] == 'L' && cmd[1] == 'D') {
        loadIndex(cmd[2]-1,cmd, args);
    } else if(strcmp(cmd,"STA") == 0) {
        storeRegister(&A, cmd, args);
    } else if(strcmp(cmd,"STX") == 0) {
       storeRegister(&X, cmd, args);
    } else if(strcmp(cmd,"ADD") == 0) {
       add(cmd, args);
    }
}

void printMemory() {
    int j = 0;
    for(int i = 0; i < (sizeof(memoria.data)/sizeof(Register)); i++) {
        //printRegister(memoria.data+i);
        printf("0x%x: ",&memoria.data[i]);
        printf("%c",getSign(memoria.data[i].sign));
        for(int k = 0; k < sizeof(memoria.data[i].data); k++) {
            printf("%c",memoria.data[i].data[k]);
        }
        printf("  ");
        if(j++ % 5 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

void printSpecialRegisters() {
    printf("Register A: ");
    printRegister(&A);
    printf("Register X: ");
    printRegister(&X);

    for(int i = 0; i < sizeof(indices)/sizeof(Index); i++) {
        printf("Index%d: ",(i+1));
        printIndex(i);
    }

    printf("Index J: ");
    printf("0X%x: ",&J);
    printf("%c",getSign(J.sign));
    for(int i = 0; i < sizeof(J.data); i++) {
        printf("%c",J.data[i]);
    }
    printf("\n");
}

void parseCode(const char* src) {
    char *end_str;
    char *line = strtok_s(src,"\n",&end_str);

    while(line != NULL) {
        char *end_token;
        printf("%s\n",line);
        char *cmd = strtok_s(line, " ",&end_token);
        char *args = strtok_s(NULL," ",&end_token);
        process(cmd,args);
        line = strtok_s(NULL, "\n", &end_str);
    }
}

char *source = "LD1 2000(0:2)\n\
               LD6 2000(0:2)\n\
               LDX 2000\n\
               STX 2001\n\
               LDA 3999\n\
               ADD 2000\n";

int main(int argc, char *argv[])
{

    setRegister(NEGATIVE,"80354",&memoria.data[2000]);
    setRegister(POSITIVE,"11111",&memoria.data[3999]);

    parseCode(source);
    printMemory();
    printSpecialRegisters();
   // printIndex(1);
   // printRegister(&X);
    printf("memory size: %d\n", sizeof(memoria));
    return 0;
}
