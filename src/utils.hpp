#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include <vector>
#include "ttmath/ttmath.h"

using std::vector;

/* Registers */
enum
{
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC, /* program counter */
    R_COND,
    R_COUNT
};

/* Opcodes */
enum
{
    OP_BR = 0, /* branch */
    OP_ADD,    /* add  */
    OP_LD,     /* load */
    OP_ST,     /* store */
    OP_JSR,    /* jump register */
    OP_AND,    /* bitwise and */
    OP_LDR,    /* load register */
    OP_STR,    /* store register */
    OP_RTI,    /* unused */
    OP_NOT,    /* bitwise not */
    OP_LDI,    /* load indirect */
    OP_STI,    /* store indirect */
    OP_JMP,    /* jump */
    OP_RES,    /* reserved (unused) */
    OP_LEA,    /* load effective address */
    OP_TRAP    /* execute trap */
};

/* Condition Flags */
enum
{
    FL_POS = 1 << 0, /* P */
    FL_ZRO = 1 << 1, /* Z */
    FL_NEG = 1 << 2, /* N */
};

/* Memory Mapped Registers */
enum
{
    MR_KBSR = 98, /* keyboard status */
    MR_KBDR = 99  /* keyboard data */
};

/* TRAP Codes */
enum
{
    TRAP_GETC = 0x20,  /* get character from keyboard, not echoed onto the terminal */
    TRAP_OUT = 0x21,   /* output a character */
    TRAP_PUTS = 0x22,  /* output a word string */
    TRAP_IN = 0x23,    /* get character from keyboard, echoed onto the terminal */
    TRAP_PUTSP = 0x24, /* output a byte string */
    TRAP_HALT = 0x25   /* halt the program */
};


class trace {

public:
    int clk;
    uint16_t regs[R_COUNT];
    uint8_t opcode;
    uint8_t rs0;
    uint8_t rs1;
    uint8_t rs2;
    uint8_t immflag;
    uint8_t imm;
};

template <typename T>
vector<T> pad_vector(const vector<T>& vec, size_t size, T value = T()) {
    vector<T> new_vec = vec;
    new_vec.insert(new_vec.end(), size, value);
    return new_vec;
}

#endif