#include <stdlib.h>
#include <string.h>
#include "cpu/cpu.h"


int main() {
    CPU cpu;
    uint8_t mem[CPU_MEM_SIZE] = {0};

    init(&cpu);

    /*
        MOV R2, 10 -> ADDI R2, R0, 10 - 01111 010 000 01010
        MOV R3, 5  -> ADDI R3, R0, 5  - 01111 011 000 00101
        ADD R1, R2, R3                - 00000 001 010 011 00
        MOV R2, 15 -> ADDI R2, R0, 15 - 01111 010 000 01111
        ADDI R2, R2, 5                - 01111 010 010 00101
        SUB R1, R1, R2                - 00001 001 001 010 00
        HALT                          - 11111 000 000 000 00
    */

    uint8_t program[] = {0x0A,0x7A, 0x05,0x7B, 0x4C,0x01, 0x0F,0x7A, 0x45,0x7A, 0x28,0x09, 0x00,0xF8};
    memcpy(mem, program, sizeof(program));

    while(step(&cpu, mem) > 0);
    printf("R1 = %d\n", (int16_t)cpu.regs[R1]);
    printf("R2 = %d\n", (int16_t)cpu.regs[R2]);
    printf("R3 = %d\n", (int16_t)cpu.regs[R3]);

    return 0;
}