# Fibonacci example

## Code:

```
ADDI R2, R0, 3      03 7A
SYSCALL             00 F0
ADDI R2, R0, 0      00 7A
ADDI R3, R0, 1      01 7B
ADD  R4, R0, R1     04 04
ADD  R5, R2, R3     4C 05
ADD  R2, R3, R0     60 02
ADD  R3, R5, R0     A0 03
SUBI R4, R4, 1      81 84
CMP  R4, R0         00 44
JNE  -6             FA BF
ADD  R1, R2, R0     40 01
HALT                00 F8
```

## Running the example

```bash
make
./output examples/fibonacci/fib.bin
```

## Results

For fibonacci(10):
```
(0x0000) R0 = 0
(0x0037) R1 = 55
(0x0037) R2 = 55
(0x0059) R3 = 89
(0x0000) R4 = 0
(0x0059) R5 = 89
(0x0000) R6 = 0
(0x0000) R7 = 0
PC = 0x0014
SP = 0xFFFE
flags = 0x0
```