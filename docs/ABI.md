# ABI

## 1. Registers

| Register | Usage | Callee-saved |
|-|-|-|
| R0 | Hardwired zero | - |
| R1 | Return value | No |
| R2 | Argument 0 | No |
| R3 | Argument 1 | No |
| R4 | Temporary 0 | No |
| R5 | Temporary 1 | No |
| R6 | Saved 0 | Yes |
| R7 | Saved 1 | Yes |

## 2. Calling Convention

- Arguments are passed in R2 and R3. Additional arguments are pushed onto the stack right-to-left before the call.
- Return value is placed in R1.
- The caller saves R1–R5 if needed. The callee must preserve and restore R6–R7.
- `CALL` pushes the return address onto the stack. `RET` pops it.

## 3. Stack

- SP starts at `0xFFFE` and grows downward.

## 4. Data

- All values are 16-bit and little-endian.
- Memory accesses should be 2-byte aligned.