# Fibonacci example

## Code:

```
0:  ADDI R2, R0, 3      03 7A
1:  SYSCALL             00 F0
2:  ADDI R2, R0, 1      01 7A
3:  ADDI R3, R0, 0      00 7B
4:  ADD  R4, R0, R1     04 04
5:  JEQ  4              04 B0
6:  ADD  R3, R2, R3     4C 03
7:  SUB  R2, R3, R2     68 0A
8:  SUBI R4, R4, 1      81 84
9:  JNE  -4             FC BF
10: ADD  R1, R3, R0     60 01
11: HALT                00 F8
```

## Running the example

```bash
make
./output examples/fibonacci/fibo.bin
```

## Results

For fibonacci(10):
```
(0x0000) R0 = 0
(0x0037) R1 = 55
(0x0022) R2 = 34
(0x0037) R3 = 55
(0x0000) R4 = 0
(0x0116) R5 = 278
(0x0000) R6 = 0
(0x0000) R7 = 0
PC = 0x0218
SP = 0xFFF0
flags = 0x0
```