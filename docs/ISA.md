# ISA

16 bit cpu
mapped byte to byte
16 bit buses

## Instructions

```
RIJ
R: 5 bit opcode, 3 bit reg a, 3 bit reg b, 3 bit reg c 2 padding
I: 5 bit opcode, 3 bit reg a, 3 bit reg b, 5 bit imm
J: 5 bit opcode, 11 bit imm
```
1. R instructions:
    - ADD, SUB, AND, OR, XOR, SHL, SHR, MUL, CMP, NOT, NEG
2. I instructions:
    - LOAD, STORE, PUSH, POP, ADDI, SUBI
3. J instructions:
    - NOP, HALT

## Op Codes

```
ADD    00000  0x00 : ra = rb + rc
SUB    00001  0x01 : ra = rb - rc
AND    00010  0x02 : ra = rb & rc
OR     00011  0x03 : ra = rb | rc
XOR    00100  0x04 : ra = rb ^ rc
SHL    00101  0x05 : ra = rb << (rc & 0xF)
SHR    00110  0x06 : ra = rb >> (rc & 0xF)
MUL    00111  0x07 : ra = rb * rc
CMP    01000  0x08 : not implemented
NOT    01001  0x09 : ra = ~rb
NEG    01010  0x0A : ra = -rb
LOAD   01011  0x0B : not implemented
STORE  01100  0x0C : not implemented
PUSH   01101  0x0D : not implemented
POP    01110  0x0E : not implemented
ADDI   01111  0x0F : ra = rb + sext(imm5)
SUBI   10000  0x10 : ra = rb - sext(imm5)
NOP    11110  0x1E : nothing
HALT   11111  0x1F : stop
```
