#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#define CPU_REGS 8
#define CPU_MEM_SIZE 65536
#define PC_START 0x0000
#define SP_START 0xFFFE

#define F_Z (1 << 3)
#define F_C (1 << 2)
#define F_N  (1 << 1)
#define F_O (1 << 0)

#define R0  0  // hardwired zero
#define R1  1  // return value
#define R2  2  // argument 0
#define R3  3  // argument 1
#define R4  4  // temporary 0
#define R5  5  // temporary 1
#define R6  6  // saved 0
#define R7  7  // saved 1

typedef struct CPU {
    uint16_t regs[CPU_REGS];
    uint16_t PC;
    uint16_t SP;
    uint8_t flags;
} CPU;

void init(CPU *cpu);

int32_t step(CPU *cpu, uint8_t *mem);

static inline uint16_t readReg(CPU *cpu, uint8_t reg){
    return cpu->regs[reg & 0x07];
}

static inline void writeReg(CPU *cpu, uint8_t reg, uint16_t value){
    if(reg != R0){
        cpu->regs[reg & 0x07] = value;
    }
    // silently ignore writes to R0
}

static inline int16_t sext5(uint8_t imm5){
    return (imm5 & 0x10) ? (int16_t)(imm5 | 0xFFE0) : (int16_t)imm5;
}

#endif // CPU_H