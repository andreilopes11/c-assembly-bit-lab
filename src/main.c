#include "bitops.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/*
 * Print each byte as two hexadecimal digits.
 *
 * For the ASCII message "hello blockchain", the byte 'h' is 0x68,
 * 'e' is 0x65, and so on. This makes the string visible as raw bytes.
 */
static void print_bytes(const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
        if (i + 1 < len) {
            putchar(' ');
        }
    }
}

/*
 * Build a small educational 64-bit digest.
 *
 * This is inspired by hash-style mixing:
 *   1. XOR the next byte into the state.
 *   2. Multiply by a large odd constant.
 *   3. Rotate the state left using the Assembly rotl64 function.
 *
 * It is useful for learning bytes and bit operations, but it is not a
 * cryptographic hash and must not be used for security.
 */
static uint64_t toy_digest(const uint8_t *data, size_t len) {
    uint64_t state = 0xcbf29ce484222325ULL;

    for (size_t i = 0; i < len; i++) {
        state ^= data[i];
        state *= 0x100000001b3ULL;
        state = rotl64(state, 7);
    }

    return state;
}

int main(int argc, char **argv) {
    /*
     * If the user passes an argument, use it as the message.
     * Otherwise, use a default string so the program always has input.
     */
    const char *message = argc > 1 ? argv[1] : "blockchain bytes";

    /*
     * C strings are bytes ending with a hidden '\0'. strlen counts only the
     * visible bytes before that terminator.
     */
    const uint8_t *data = (const uint8_t *)message;
    size_t len = strlen(message);

    /* Implemented in Assembly: XOR all bytes into one 8-bit checksum. */
    uint8_t checksum = xor_checksum(data, len);

    /* Implemented in C, but it calls the Assembly rotl64 mixing function. */
    uint64_t digest = toy_digest(data, len);

    printf("C + Assembly Bit Lab\n");

    /* Human-readable input exactly as received from the terminal. */
    printf("message: %s\n", message);

    /* Number of bytes, not necessarily the number of human characters. */
    printf("length:  %zu bytes\n", len);

    /* Raw hexadecimal representation of every input byte. */
    printf("bytes:   ");
    print_bytes(data, len);
    putchar('\n');

    /* One-byte XOR checksum returned by src/bitops.S. */
    printf("xor:     0x%02x\n", checksum);

    /* 64-bit educational digest printed as 16 hexadecimal digits. */
    printf("digest:  0x%016" PRIx64 "\n", digest);
    printf("\n");
    printf("Note: this digest is educational, not cryptographic.\n");

    return 0;
}
