#ifndef DATATYPES_H
#define DATATYPES_H

#include <stdint.h>

enum signs {EMPTY, POSITIVE=1, NEGATIVE=-1};

typedef struct  {
    enum sign sign;
    char data[5];
}Register;

typedef struct {
    enum sign sign;
    char data[2];
} Index;

struct Memory {
    Register data[MAX_MEMORY];
};

typedef struct{
    int8_t Sign;
    union{
        struct{
            uint16_t COD;
            uint32_t ADDR;
        };
        struct{
            uint16_t _PAD1;
            uint32_t DATA;
        };
        struct{
            uint32_t _PAD2;
            uint16_t INDEX;
        };
    };
} memory_register;

typedef struct {
    int from;
    int to;
    int address;
}Modifiers;

#endif // DATATYPES_H
