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
    uint32_t mask;
    union{
        struct{
            uint16_t COD;
            uint64_t ADDR;
        };
        struct{
            uint16_t _PAD1;
            uint64_t DATA;
        };
        struct{
            uint64_t _PAD2;
            uint16_t INDEX;
        };
        struct{
            uint8_t byte1;
            uint8_t byte2;
            uint8_t byte3;
            uint8_t byte4;
            uint8_t byte5;
            uint8_t _PAD3;
            uint32_t _PAD4;
        };

    };
} memory_register;

typedef struct {
    char *label;
    uint64_t address;
}label;

typedef struct {
    size_t size;
    size_t used;
    label *buffer;
}label_vector;

typedef struct {
    uint32_t mask;
    uint64_t address;
    char *label;
}Modifiers;

#endif // DATATYPES_H
