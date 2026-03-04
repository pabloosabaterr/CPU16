#include "cpu.h"

void init(CPU *restrict cpu){
    for(int i = 0; i < CPU_REGS; ++i){
        cpu->regs[i] = 0;
    }
    cpu->PC = USER_START;
    cpu->SP = SP_START;
    cpu->flags = 0;
    cpu->EPC = 0;
    cpu->cause = 0;
    cpu->status = MUSER;
}

void loadFirm(uint8_t *restrict mem){
    /*
        Exception handler at 0x0100

        IO BASE
        0x0100 : LUI R4, 0XFF
        0x0102 : ORI R4, 0xF0

        SYSCALL OPTIONS
        0x0104 : CMPI R2, 0         ; EXIT
        0x0106 : JEQ +9             ; 0x011A
        0x0108 : CMPI R2, 1         ; PRINT_INT
        0x010A : JEQ +8             ; 0x011C
        0x010C : CMPI R2, 2         ; PRINT_CHAR
        0x010E : JEQ +8             ; 0x0120
        0x0110 : CMPI R2, 3         ; READ_INT
        0x0112 : JEQ +8             ; 0x0124
        0x0114 : CMPI R2, 4         ; READ_CHAR
        0x0116 : JEQ +8             ; 0x0128

        UNKNOWN SYSCALL
        0x0118 : ERET               ; unknown syscall

        EXIT
        0x011A : HALT

        PRINT_INT
        0x011C : STORE R4, R4, 4    ;
        0x011E : ERET

        PRINT_CHAR
        0x0120 : STORE R4, R3, 0    ;
        0x0122 : ERET

        READ_INT
        0x0124 : LOAD R1, R4, 4     ;
        0x0126 : ERET

        READ_CHAR
        0x0128 : LOAD R1, R4, 0     ;
        0x012A : ERET
    */

    static const uint8_t firm[] = {
        0xFF, 0x8C,  // LUI  R4, 0xFF
        0xF0, 0x94,  // ORI  R4, 0xF0
        0x00, 0xEA,  // CMPI R2, 0
        0x09, 0xB0,  // JEQ  +9
        0x01, 0xEA,  // CMPI R2, 1
        0x08, 0xB0,  // JEQ  +8
        0x02, 0xEA,  // CMPI R2, 2
        0x08, 0xB0,  // JEQ  +8
        0x03, 0xEA,  // CMPI R2, 3
        0x08, 0xB0,  // JEQ  +8
        0x04, 0xEA,  // CMPI R2, 4
        0x08, 0xB0,  // JEQ  +8
        0x01, 0xF0,  // ERET
        0x00, 0xF8,  // HALT
        0x64, 0x64,  // STORE R4, R3, 4
        0x01, 0xF0,  // ERET
        0x60, 0x64,  // STORE R4, R3, 0
        0x01, 0xF0,  // ERET
        0x84, 0x59,  // LOAD  R1, R4, 4
        0x01, 0xF0,  // ERET
        0x80, 0x59,  // LOAD  R1, R4, 0
        0x01, 0xF0,  // ERET
    };
    memcpy(mem + HANDLER_ADDR, firm, sizeof(firm));
}

static uint16_t IOread(uint16_t addr){
    switch(addr){
        case IO_UART_DATA: {
            int c = getchar();
            return (c == EOF) ? 0 : (uint16_t)c;
        }
        case IO_NUM_DATA: {
            int32_t num;
            if(scanf("%d", &num) != 1){
                num = 0;
            }
            return (uint16_t)num;
        }
        default: {
            return 0;
        }
    }
}

static void IOwrite(uint16_t addr, uint16_t val){
    switch(addr){
        case IO_UART_DATA: {
            putchar(val & 0xFF);
            fflush(stdout);
            break;
        }
        case IO_NUM_DATA: {
            printf("%d", (int16_t)val);
            fflush(stdout);
            break;
        }
    }
}

int32_t step(CPU *restrict cpu, uint8_t *restrict mem){
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
    // 8 bits immediate value (for IL instructions)
    uint8_t imm8 = inst;
    // 11 bits immediate value (for conditional branches)
    uint16_t imm11 = inst & 0x7FF;
    // 2 option bits 
    uint8_t option = inst & 0x03;
    
    switch(opcode){
        case 0x00: {
            // ADD ra, rb, rc || ra = rb + rc
            uint16_t b = readReg(cpu, rb);
            uint16_t c = readReg(cpu, rc);
            uint32_t fullRes = (uint32_t)b + c;
            uint16_t res = (uint16_t)fullRes;
            writeReg(cpu, ra, b + c);
            setFlags(cpu, res, fullRes, ADD_OVERFLOW(b, c, res));
            break;
        }
        case 0x01: {
            // SUB ra, rb, rc || ra = rb - rc
            uint16_t b = readReg(cpu, rb);
            uint16_t c = readReg(cpu, rc);
            uint32_t fullRes = (uint32_t)b - c;
            uint16_t res = (uint16_t)fullRes;
            writeReg(cpu, ra, readReg(cpu, rb) - readReg(cpu, rc));
            setFlags(cpu, res, fullRes, SUB_OVERFLOW(b, c, res));
            break;
        }
        case 0x02: {
            // AND ra, rb, rc || ra = rb & rc
            uint16_t res = readReg(cpu, rb) & readReg(cpu, rc);
            writeReg(cpu, ra, res);
            setZeroSign(cpu, res);
            break;
        }
        case 0x03: {
            // OR ra, rb, rc || ra = rb | rc
            uint16_t res = readReg(cpu, rb) | readReg(cpu, rc);
            writeReg(cpu, ra, res);
            setZeroSign(cpu, res);
            break;
        }
        case 0x04: {
            // XOR ra, rb, rc || ra = rb ^ rc
            uint16_t res = readReg(cpu, rb) ^ readReg(cpu, rc);
            writeReg(cpu, ra, res);
            setZeroSign(cpu, res);
            break;
        }
        case 0x05: {
            // SHL ra, rb, rc || ra = rb << (rc & 0xF)
            uint16_t b = readReg(cpu, rb);
            uint16_t shift = readReg(cpu, rc) & 0x0F;
            uint16_t res;
            if(option & 0x01){
                // arithmetic shift
                res = (int16_t)b << shift;
            } else {
                // logical shift
                res = b << shift;
            }
            writeReg(cpu, ra, res);
            setZeroSign(cpu, res);
            if (shift > 0 && ((b >> (16 - shift)) & 1)) cpu->flags |= F_C;
            break;
        }
        case 0x06: {
            // SHR ra, rb, rc || ra = rb >> (rc & 0x0F)
            uint16_t b = readReg(cpu, rb);
            uint8_t shift = readReg(cpu, rc) & 0x0F;
            uint16_t res;
            if(option & 0x01){
                // arithmetic shift
                res = (int16_t)b >> shift;
            } else {
                // logical shift
                res = b >> shift;
            }
            writeReg(cpu, ra, res);
            setZeroSign(cpu, res);
            if(shift > 0 && (b >> (shift - 1)) & 1) cpu->flags |= F_C;
            break;
        }
        case 0x07: {
            // MUL ra, rb, rc || ra = low 16 bits, (ra + 1) = high 16 bit
            uint16_t b = readReg(cpu, rb);
            uint16_t c = readReg(cpu, rc);
            uint32_t fullRes = (uint32_t)b * c;
            uint16_t hiBits = (uint16_t)(fullRes >> 16);
            writeReg(cpu, ra, (uint16_t)fullRes);
            if(ra != R7){
                writeReg(cpu, (ra + 1) & 0x07, hiBits);
            }
            if(fullRes == 0) cpu->flags |= F_Z;
            if(hiBits & 0x8000) cpu->flags |= F_N;
            break;
        }
        case 0x08: {
            // CMP
            uint16_t a = readReg(cpu, ra);
            uint16_t b = readReg(cpu, rb);
            uint32_t fullRes = (uint32_t)a - b;
            uint16_t res = (uint16_t)fullRes;
            setFlags(cpu, res, fullRes, SUB_OVERFLOW(a, b, res));
            break;
        }
        case 0x09: {
            // NOT
            uint16_t res = ~readReg(cpu, rb);
            writeReg(cpu, ra, res);
            setZeroSign(cpu, res);
            break;
        }
        case 0x0A: {
            // NEG
            uint16_t b = readReg(cpu, rb);
            uint16_t res = -b;
            writeReg(cpu, ra, res);
            setZeroSign(cpu, res);
            if(b != 0) cpu->flags |= F_C;
            if(b == 0x8000) cpu->flags |= F_O;
            break;
        }
        case 0x0B: {
            // LOAD
            uint16_t addr = readReg(cpu, rb) + sext5(imm5);
            if(addr >= IO_UART_DATA){
                if(cpu->status & MUSER){
                    trap(cpu, CMEM_FAULT);
                }else {
                    writeReg(cpu, ra, IOread(addr));
                }
            }else {
                uint16_t val = mem[addr] | (mem[addr + 1] << 8);
                writeReg(cpu, ra, val);
            }
            break;
        }
        case 0x0C: {
            // STORE
            uint16_t addr = readReg(cpu, ra) + sext5(imm5);
            uint16_t val = readReg(cpu, rb);
            if(addr >= IO_UART_DATA){
                if(cpu->status & MUSER){
                    trap(cpu, CMEM_FAULT);
                }else {
                    IOwrite(addr, val);
                }
            }else {
                mem[addr] = val;
                mem[addr + 1] = val >> 8;
            }
            break;
        }
        case 0x0D: {
            // PUSH
            cpu->SP -= 2;
            uint16_t val = readReg(cpu, ra);
            // no need to mask, u16 -> u8 takes 8 lower bits
            mem[cpu->SP] = val;
            mem[cpu->SP + 1] = val >> 8;
            break;
        }
        case 0x0E: {
            // POP
            uint16_t val = mem[cpu->SP] | (mem[cpu->SP + 1] << 8);
            writeReg(cpu, ra, val);
            cpu->SP += 2;
            break;
        }
        case 0x0F: {
            // ADDI
            uint16_t b = readReg(cpu, rb);
            uint32_t fullRes = (uint32_t)b + sext5(imm5);
            uint16_t res = (uint16_t)fullRes;
            writeReg(cpu, ra, res);
            setFlags(cpu, res, fullRes, ADD_OVERFLOW(b, sext5(imm5), res));
            break;
        }
        case 0x10: {
            // SUBI
            uint16_t b = readReg(cpu, rb);
            uint32_t fullRes = (uint32_t)b - sext5(imm5);
            uint16_t res = (uint16_t)fullRes;
            writeReg(cpu, ra, res);
            setFlags(cpu, res, fullRes, SUB_OVERFLOW(b, sext5(imm5), res));
            break;
        }
        case 0x11: {
            // LUI
            uint16_t res = imm8 << 8;
            writeReg(cpu, ra, res);
            break;
        }
        case 0x12: {
            // ORI
            uint16_t a = readReg(cpu, ra);
            uint16_t res = a | imm8;
            writeReg(cpu, ra, res);
            break;
        }
        case 0X13: {
            // CALL
            cpu->SP -= 2;
            mem[cpu->SP] = cpu->PC;
            mem[cpu->SP + 1] = cpu->PC >> 8;
            cpu->PC = readReg(cpu, ra);
            break;
        }
        case 0x14: {
            // RET
            cpu->PC = mem[cpu->SP] | (mem[cpu->SP + 1] << 8);
            cpu->SP += 2;
            break;
        }
        case 0x15: {
            // JUMP
            cpu->PC = readReg(cpu, ra);
            break;
        }
        case 0x16: {
            // JEQ
            if(cpu->flags & F_Z){
                cpu->PC = cpu->PC + (sext11(imm11) << 1);
            }
            break;
        }
        case 0x17: {
            // JNE
            if(!(cpu->flags & F_Z)){
                cpu->PC = cpu->PC + (sext11(imm11) << 1);
            }
            break;
        }
        case 0x18: {
            //JLT
            if(cpu->flags & F_N){
                cpu->PC = cpu->PC + (sext11(imm11) << 1);
            }   
            break;
        }
        case 0x19: {
            // JGT
            if(!(cpu->flags & F_N) && !(cpu->flags & F_Z)){
                cpu->PC = cpu->PC + (sext11(imm11) << 1);
            }
            break;
        }
        case 0x1A: {
            // DIV
            uint16_t b = readReg(cpu, rb);
            uint16_t c = readReg(cpu, rc);
            if(c == 0){
                trap(cpu, CDIV_ZERO);
                return -1;
            }
            uint16_t res, rem;
            if(option & 0x01){
                res = (int16_t)b / (int16_t)c;
                rem = (int16_t)b % (int16_t)c;
            } else {
                res = b / c;
                rem = b % c;
            }
            writeReg(cpu, ra, res);
            if(ra != R7){
                writeReg(cpu, (ra + 1) & 0x07, rem);
            }
            uint16_t flagTarget = (option & 0x02) && (ra != R7) ? rem : res;
            setZeroSign(cpu, flagTarget);
            break;
        }
        case 0x1B : {
            // ANDI
            uint16_t a = readReg(cpu, ra);
            uint16_t res = a & imm8;
            writeReg(cpu, ra, res);
            setZeroSign(cpu, res);
            break;
        }
        case 0x1C: {
            // TST
            uint16_t a = readReg(cpu, ra);
            uint16_t b = readReg(cpu, rb);
            uint16_t res = a & b;
            setZeroSign(cpu, res);
            break;
        }
        case 0x1D: {
            // CMPI
            uint16_t a = readReg(cpu, ra);
            uint32_t fullRes = (uint32_t)a - sext8(imm8);
            uint16_t res = (uint16_t)fullRes;
            setFlags(cpu, res, fullRes, SUB_OVERFLOW(a, sext8(imm8), res));
            break;
        }
        case 0x1E: {
            // SYSCALL
            if(inst & 0x01){
                // ERET
                if(!(cpu->status & MUSER)){
                    cpu->PC = cpu->EPC;
                    cpu->status |= MUSER; 
                }
            } else {
                trap(cpu, CSYSCALL);
            }
            break;
        }
        case 0x1F: {
            // HALT
            return 0;
        }
        default: {
            trap(cpu, CILL_OP);
            return -1;
        }
    }
    return 1;
}