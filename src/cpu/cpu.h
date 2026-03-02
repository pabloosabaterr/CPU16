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

#define R_ZERO  0
#define R_RV    1
#define R_A0    2
#define R_A1    3
#define R_T0    4
#define R_T1    5
#define R_S0    6
#define R_S1    7

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
    if(reg != R_ZERO){
        cpu->regs[reg & 0x07] = value;
    }
    // silently ignore writes to R_ZERO
}

#endif // CPU_H