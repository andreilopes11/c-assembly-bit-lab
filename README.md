# C + Assembly Bit Lab

Educational project for learning how C can call functions written in x86-64 Assembly.

The goal is to make the bridge between these ideas visible:

- bytes;
- registers;
- bitwise operations;
- bit rotation;
- a simple checksum;
- compilation;
- linking;
- terminal execution.

This project does not implement real cryptography. It uses an educational digest to show how hash-like functions can mix bytes with XOR, multiplication, and rotation.

## Run

Prerequisites:

```bash
gcc --version
make --version
```

On Debian, Ubuntu, or WSL Ubuntu, install them with:

```bash
sudo apt update
sudo apt install build-essential
```

With `make`:

```bash
make
make run
./build/bitlab "my transaction"
```

Without `make`, using `gcc` directly:

```bash
mkdir -p build
gcc -Iinclude -std=c11 -Wall -Wextra -O2 -g src/main.c src/bitops.S -o build/bitlab
./build/bitlab "my transaction"
```

## Files

```text
include/bitops.h   declarations for the Assembly functions
src/bitops.S       Assembly functions in AT&T syntax for Linux x86-64
src/main.c         C program that calls the Assembly functions
Makefile           project build file
```

## What to Observe

- `xor_checksum` receives a pointer and a length using the System V AMD64 convention.
- The first argument arrives in `RDI`.
- The second argument arrives in `RSI`.
- The 8-bit return value comes back in `AL`.
- `rotl64` receives the value in `RDI`, the shift in `ESI`, and returns through `RAX`.

## Understanding the Output

Example:

```text
message: my transaction
length:  14 bytes
bytes:   6d 79 20 74 72 61 6e 73 61 63 74 69 6f 6e
xor:     0x65
digest:  0x9f7574346fb6ec2c
```

### `message`

This is the string received by the program on the command line.

```bash
./build/bitlab "my transaction"
```

In `main.c`, it comes from `argv[1]`:

```c
const char *message = argc > 1 ? argv[1] : "blockchain bytes";
```

If no argument is passed, the program uses `"blockchain bytes"` as the default value.

### `length`

This is the message length in bytes, calculated with `strlen(message)`.

```text
my transaction = 14 bytes
```

Important: this counts bytes, not necessarily human characters. In UTF-8, accented characters can use more than one byte, so two visually similar strings can have different byte lengths.

### `bytes`

This is the hexadecimal representation of every message byte.

```text
6d 79 20 74 72 61 6e 73 61 63 74 69 6f 6e
```

Each hexadecimal value maps to an ASCII character:

```text
6d = m
79 = y
20 = space
74 = t
72 = r
61 = a
6e = n
73 = s
61 = a
63 = c
74 = t
69 = i
6f = o
6e = n
```

Putting the bytes together gives:

```text
my transaction
```

### `xor`

This is a simple 1-byte checksum calculated in Assembly by `xor_checksum`.

The algorithm XORs every byte:

```text
0x6d ^ 0x79 ^ 0x20 ^ 0x74 ^ 0x72 ^ 0x61 ^ 0x6e
^ 0x73 ^ 0x61 ^ 0x63 ^ 0x74 ^ 0x69 ^ 0x6f ^ 0x6e
= 0x65
```

In Assembly, the accumulator lives in `AL`, the lowest byte of `RAX`:

```asm
xorb (%rdi), %al
```

This is useful for learning bitwise operations, but it is not security. Different inputs can easily produce the same XOR value.

### `digest`

This is an educational 64-bit digest calculated in `main.c` by `toy_digest`.

For every byte, the program:

```text
1. mixes the byte into the state with XOR;
2. multiplies by a large constant;
3. rotates bits to the left by calling rotl64 in Assembly.
```

The result looks hash-like, but it is not cryptographic. It exists to show how bytes, XOR, multiplication, and rotation can mix bits. Real blockchain code should use mature libraries and algorithms such as Keccak-256, SHA-256, Ed25519, or secp256k1 as appropriate for the ecosystem.

## Where Each Item Is Produced

```text
message  -> src/main.c, message variable
length   -> src/main.c, strlen(message)
bytes    -> src/main.c, print_bytes function
xor      -> src/bitops.S, xor_checksum function
digest   -> src/main.c, toy_digest function, calling rotl64 in src/bitops.S
```

## Useful Commands

```bash
objdump -d build/bitlab
nm build/bitlab
make clean
```

## Creating Another C + Assembly Project From Scratch

Use this checklist when you want to create a new lab beyond `c-assembly-bit-lab`.

### New Scenario: `message-metrics-lab`

Goal: create a C program that receives a terminal message and calls Assembly functions to calculate simple metrics:

- `length`: calculated in C with `strlen`;
- `bytes`: printed in C as hexadecimal;
- `sum`: calculated in Assembly by adding every byte;
- `spaces`: calculated in Assembly by counting ASCII spaces `0x20`.

Example input:

```bash
./build/metricslab "abc 123"
```

Expected output:

```text
Message Metrics Lab
message: abc 123
length:  7 bytes
bytes:   61 62 63 20 31 32 33
sum:     476
spaces:  1
```

This example is useful because it shows C reading a terminal argument, turning a string into bytes, calling Assembly, reading memory byte by byte in Assembly, returning values to C, and producing a reusable pattern for future labs.

### 1. Create the Structure

From the repository root:

```bash
mkdir -p Development/message-metrics-lab/include
mkdir -p Development/message-metrics-lab/src
cd Development/message-metrics-lab
```

Expected structure:

```text
message-metrics-lab/
  include/
    metrics.h
  src/
    main.c
    metrics.S
  Makefile
```

### 2. Create the C Header

The header is the contract between C and Assembly. It tells the C compiler that these functions exist even though the implementation is in a `.S` file.

```c
#ifndef METRICS_H
#define METRICS_H

#include <stddef.h>
#include <stdint.h>

uint64_t asm_sum_bytes(const uint8_t *data, size_t len);
uint64_t asm_count_spaces(const uint8_t *data, size_t len);

#endif
```

### 3. Implement the Assembly Functions

`asm_sum_bytes` should:

```text
1. start RAX at zero;
2. read one byte from memory;
3. add it to RAX;
4. advance the pointer;
5. decrease the length;
6. repeat until the length reaches zero.
```

`asm_count_spaces` should:

```text
1. start RAX at zero;
2. compare the current byte with 0x20;
3. increment RAX when the byte is a space;
4. advance the pointer;
5. decrease the length;
6. repeat until the length reaches zero.
```

Watch these registers:

- `RDI` starts by pointing to the first message byte;
- `RSI` starts with the message length;
- `RAX` stores the value returned to C;
- `movzbq (%rdi), %rcx` reads one byte from memory and expands it to 64 bits;
- `cmpb $0x20, (%rdi)` compares the current byte with an ASCII space.

### 4. Create the C Program

The role of C:

- read the terminal argument;
- calculate the string length;
- print bytes;
- call Assembly functions;
- format the output.

The role of Assembly:

- walk through bytes in memory;
- calculate the sum;
- count spaces;
- return numbers to C.

### 5. Create the Makefile

Keep warnings enabled and debug symbols available:

```makefile
CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -O2 -g
CPPFLAGS ?= -Iinclude
BUILD_DIR := build
TARGET := $(BUILD_DIR)/metricslab
SOURCES := src/main.c src/metrics.S

.PHONY: all run clean inspect symbols

all: $(TARGET)

$(TARGET): $(SOURCES) include/metrics.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET) "abc 123"

inspect: $(TARGET)
	objdump -d $(TARGET)

symbols: $(TARGET)
	nm $(TARGET)

clean:
	rm -rf $(BUILD_DIR)
```

Important: command lines inside Makefile rules must start with a tab, not spaces.

### 6. Compile

```bash
make
```

### 7. Run

```bash
make run
./build/metricslab "hello blockchain"
```

### 8. Inspect the Binary

List symbols:

```bash
nm build/metricslab
```

You should see symbols similar to:

```text
00000000000012a0 T asm_sum_bytes
00000000000012c0 T asm_count_spaces
```

The important point is to see `T asm_sum_bytes` and `T asm_count_spaces`, which means the functions were linked into the executable.

View the linked Assembly:

```bash
objdump -d build/metricslab
```

### 9. Debug with GDB

```bash
gdb ./build/metricslab
```

Inside GDB:

```gdb
break asm_sum_bytes
run "abc 123"
info registers rdi rsi rax
x/7xb $rdi
stepi
```

Watch how:

- `rdi` points to the first byte of the message;
- `rsi` starts with `7`, the length of `"abc 123"`;
- `rax` starts at `0` and accumulates the sum.

### 10. Checklist for Any New C + Assembly Project

Use this sequence:

1. Define a small function for Assembly.
2. Write its signature in the `.h` header.
3. Implement the function in the `.S` file.
4. Include the `.S` file in the build.
5. Call the function from C.
6. Print the result.
7. Run with simple input.
8. Calculate the expected result manually.
9. Compare real output with expected output.
10. Use `nm` to confirm symbols.
11. Use `objdump` to inspect the binary.
12. Use `gdb` to inspect registers and memory.
13. Document what each register does.

### 11. Common Errors

`undefined reference to asm_sum_bytes`:

The name in the header does not match the Assembly label, or `src/metrics.S` was not included in the build.

`No such file or directory: ./build/metricslab`:

The `build` directory does not exist yet, or the project was not compiled before running.

`missing separator`:

One of the Makefile command lines starts with spaces instead of a tab.

`Error: junk at end of line`:

Likely an Assembly syntax error. This guide uses AT&T syntax, the default GNU assembler syntax on Linux.

Different result than expected:

Check whether the message has an accent, an extra space, or different quote characters.
