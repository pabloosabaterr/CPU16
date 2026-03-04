#include <stdlib.h>
#include <string.h>
#include "cpu/cpu.h"


int main(int argc, char *argv[]) {
    if(argc < 2){
        fprintf(stderr, "Usage: %s <binary>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");
    if(!f){
        fprintf(stderr, "Error: could not open '%s'\n", argv[1]);
        return 1;
    }

    CPU cpu;
    uint8_t mem[CPU_MEM_SIZE] = {0};
    init(&cpu);

    size_t n = fread(mem, 1, CPU_MEM_SIZE, f);
    fclose(f);

    if(n == 0){
        fprintf(stderr, "Error: file is empty\n");
        return 1;
    }

    while(step(&cpu, mem) > 0);

    for(int i = 0; i < CPU_REGS; ++i){
        printf("(0x%04X) R%d = %d\n", cpu.regs[i], i, (int16_t)cpu.regs[i]);
    }
    printf("PC = 0x%04X\n", cpu.PC);
    printf("SP = 0x%04X\n", cpu.SP);
    printf("flags = 0x%01X\n", cpu.flags);

    return 0;
}