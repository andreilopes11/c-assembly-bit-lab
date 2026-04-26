#ifndef BITOPS_H
#define BITOPS_H

#include <stddef.h>
#include <stdint.h>

/*
 * Assembly functions exposed to C.
 *
 * The implementations live in src/bitops.S and follow the Linux x86-64
 * System V ABI, so C can call them as if they were normal C functions.
 */

/*
 * XOR every byte in data[0..len-1] and return the final 8-bit checksum.
 *
 * Example:
 *   bytes: 6d 69 6e
 *   xor:   6d ^ 69 ^ 6e
 */
uint8_t xor_checksum(const uint8_t *data, size_t len);

/*
 * Rotate a 64-bit value to the left by shift bits.
 *
 * This is not encryption. It is one mixing step used by the educational
 * digest in main.c to make bit changes spread through the 64-bit state.
 */
uint64_t rotl64(uint64_t value, unsigned int shift);

#endif
