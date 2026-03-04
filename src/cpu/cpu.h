#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define CPU_REGS 8
#define CPU_MEM_SIZE (1 << 16) // 64KB
#define SP_START 0xFFF0

// memory map
#define USER_START 0x0200
#define HANDLER_ADDR 0x0100

// MODES
#define MUSER 1
#define MKERNEL 0

// IO
#define IO_UART_DATA 0xFFF0 // & base
#define IO_NUM_DATA 0xFFF4

#define ADD_OVERFLOW(b, c, res) ((~(b ^ c) & (b ^ res)) & 0x8000)
#define SUB_OVERFLOW(b, c, res) (((b ^ c) & (b ^ res)) & 0x8000)

// CAUSES
#define CSYSCALL 0
#define CILL_OP 1
#define CDIV_ZERO 2
#define CMEM_FAULT 3

//FLAGS
#define F_Z (1 << 3) // zero 
#define F_C (1 << 2) // carry
#define F_N  (1 << 1) // sign
#define F_O (1 << 0) // overflow 

//REGISTERS
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
    uint16_t EPC; // saves PC on exception
    uint8_t cause;
    bool status; // 0 kernel, 1 user
} CPU;

void init(CPU *restrict cpu);
int32_t step(CPU *restrict cpu, uint8_t *restrict mem);
void loadFirm(uint8_t *restrict mem);

static inline void trap(CPU *restrict cpu, uint8_t *restrict mem, uint8_t cause){
    cpu->EPC = cpu->PC;
    cpu->cause = cause;
    cpu->status = 0;
    uint16_t addr = cause * 2;
    cpu->PC = mem[addr] | (mem[addr + 1] << 8);
}

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

static inline uint16_t sext8(uint8_t imm8){
    return (imm8 & 0x80) ? (int16_t)(imm8 | 0xFF00) : (int16_t)imm8;
}

static inline uint16_t sext11(uint16_t imm11){
    return (imm11 & 0x400) ? (int16_t)(imm11 | 0xF800) : (int16_t)imm11;
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