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
    // fetches 2 bytes and combines them into 16 bit instruction - little endian
    uint16_t inst = mem[cpu->PC] | (mem[cpu->PC + 1] << 8);
    cpu->PC += 2;

    // 5 bytes opcode
    uint8_t opcode = (inst >> 11) & 0x1F;

    // 3 bits for each register
    uint8_t ra = (inst >> 8) & 0x07;
    uint8_t rb = (inst >> 5) & 0x07;
    uint8_t rc = (inst >> 2) & 0x07;

    // 5 bits immediate value
    uint8_t imm5 = inst & 0x1F;
    
    switch(opcode){
        case 0x00: {
            // ADD ra, rb, rc
            writeReg(cpu, ra, readReg(cpu, rb) + readReg(cpu, rc));
            break;
        }
        case 0x01: {
            // SUB
            writeReg(cpu, ra, readReg(cpu, rb) - readReg(cpu, rc));
            break;
        }
        case 0x02: {
            // AND
            writeReg(cpu, ra, readReg(cpu, rb) & readReg(cpu, rc));
            break;
        }
        case 0x03: {
            // OR
            writeReg(cpu, ra, readReg(cpu, rb) | readReg(cpu, rc));
            break;
        }
        case 0x04: {
            // XOR
            writeReg(cpu, ra, readReg(cpu, rb) ^ readReg(cpu, rc));
            break;
        }
        case 0x05: {
            // SHL
            writeReg(cpu, ra, readReg(cpu, rb) << (readReg(cpu, rc) & 0x0F));
            break;
        }
        case 0x06: {
            // SHR
            writeReg(cpu, ra, readReg(cpu, rb) >> (readReg(cpu, rc) & 0x0F));
            break;
        }
        case 0x07: {
            // MUL
            writeReg(cpu, ra, readReg(cpu, rb) * readReg(cpu, rc));
            break;
        }
        case 0x08: {
            // CMP
            assert(0 && "CMP not implemented yet");
            break;
        }
        case 0x09: {
            // NOT
            writeReg(cpu, ra, ~readReg(cpu, rb));
            break;
        }
        case 0x0A: {
            // NEG
            writeReg(cpu, ra, -readReg(cpu, rb));
            break;
        }
        case 0x0B: {
            // LOAD
            assert(0 && "LOAD not implemented yet");
            break;
        }
        case 0x0C: {
            // STORE
            assert(0 && "STORE not implemented yet");
            break;
        }
        case 0x0D: {
            // PUSH
            assert(0 && "PUSH not implemented yet");
            break;
        }
        case 0x0E: {
            // POP
            assert(0 && "POP not implemented yet");
            break;
        }
        case 0x0F: {
            // ADDI
            writeReg(cpu, ra, readReg(cpu, rb) + sext5(imm5));
            break;
        }
        case 0x10: {
            // SUBI
            writeReg(cpu, ra, readReg(cpu, rb) - sext5(imm5));
            break;
        }
        case 0x1E: {
            // NOP
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
    return 1;
}