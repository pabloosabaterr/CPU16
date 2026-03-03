#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

#define CPU_REGS 8
#define CPU_MEM_SIZE 2 << 16 // 64KB
#define PC_START 0x0000
#define SP_START 0xFFFE

#define ADD_OVERFLOW(b, c, res) ((~(b ^ c) & (b ^ res)) & 0x8000)
#define SUB_OVERFLOW(b, c, res) (((b ^ c) & (b ^ res)) & 0x8000)

#define F_Z (1 << 3) // zero 
#define F_C (1 << 2) // carry
#define F_N  (1 << 1) // sign
#define F_O (1 << 0) // overflow 

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

void init(CPU *restrict cpu);

int32_t step(CPU *restrict cpu, uint8_t *restrict mem);

static inline uint16_t readReg(CPU *restrict cpu, uint8_t reg){
    return cpu->regs[reg & 0x07];
}

static inline void writeReg(CPU *restrict cpu, uint8_t reg, uint16_t value){
    if(reg != R0){
        cpu->regs[reg & 0x07] = value;
    }
    // silently ignore writes to R0
}

static inline uint16_t sext5(uint8_t imm5){
    return (imm5 & 0x10) ? (int16_t)(imm5 | 0xFFE0) : (int16_t)imm5;
}

static inline void setZeroSign(CPU *restrict cpu, uint16_t res){
    cpu->flags = 0;
    if(res == 0) cpu->flags |= F_Z;
    // check left bit 
    if(res & 0x8000) cpu->flags |= F_N;
}

static inline void setCarryOverflow(CPU *restrict cpu, uint32_t fullRes, bool overflow){
    // carry
    if(fullRes > 0xFFFF) cpu->flags |= F_C;
    // overflow
    if(overflow) cpu->flags |= F_O;
}

static inline void setFlags(CPU *restrict cpu, uint16_t res, uint32_t fullRes, bool overflow){
    setZeroSign(cpu, res);
    setCarryOverflow(cpu, fullRes, overflow);
}

#endif // CPU_H