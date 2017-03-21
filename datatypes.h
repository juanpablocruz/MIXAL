#ifndef DATATYPES_H
#define DATATYPES_H
enum signs {EMPTY, POSITIVE, NEGATIVE};

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

typedef struct {
    int from;
    int to;
    int address;
}Modifiers;

#endif // DATATYPES_H
