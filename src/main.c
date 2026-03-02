#include "cpu/cpu.h"

int main() {
    CPU cpu;
    uint8_t mem[CPU_MEM_SIZE] = {0};

    init(&cpu);

    return 0;
}