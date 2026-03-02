#include "cpu.h"

void init(CPU *cpu){
    for(int i = 0; i < CPU_REGS; ++i){
        cpu->regs[i] = 0;
    }
    cpu->PC = PC_START;
    cpu->SP = SP_START;
    cpu->flags = 0;
}

int32_t step(CPU *cpu, uint8_t *mem){
    uint16_t inst = mem[cpu->PC] | (mem[cpu->PC + 1] << 8);
    cpu->PC += 2;

    // 5 bytes opcode
    uint8_t opcode = (inst >> 11) & 0x1F;

    uint8_t ra = (inst >> 8) & 0x07;
    uint8_t rb = (inst >> 5) & 0x07;
    uint8_t rc = (inst >> 2) & 0x07;
    uint8_t imm5 = inst & 0x1F;
    
    switch(opcode){
        case 0x00: {
            writeReg(cpu, ra, readReg(cpu, rb) + readReg(cpu, rc));
            break;
        }
        case 0x01: {
            // SUB
            break;
        }
        case 0x02: {
            // AND
            break;
        }
        case 0x03: {
            // OR
            break;
        }
        case 0x04: {
            // XOR
            break;
        }
        case 0x1F: {
            // HALT
            return 0;
        }
        default: {
            printf("Unknown opcode: 0x%02X\n at PC = 0x%04X\n", opcode, cpu->PC - 2);
            return -1;
        }
    }
    // no halt at the end 
    return 1;
}