    .text

    # uint8_t xor_checksum(const uint8_t *data, size_t len)
    #
    # System V AMD64 ABI:
    #   RDI = data pointer
    #   RSI = len
    #   AL  = 8-bit return value
    .globl xor_checksum
    .type xor_checksum, @function
xor_checksum:
    # EAX starts at zero. AL, the lowest byte of RAX, stores the checksum.
    xorl %eax, %eax

    # If len is zero, there are no bytes to read; return 0 immediately.
    testq %rsi, %rsi
    je .xor_done

.xor_loop:
    # XOR the current byte pointed to by RDI into AL.
    xorb (%rdi), %al

    # Move to the next byte and decrease the remaining length.
    incq %rdi
    decq %rsi

    # Continue until all bytes were consumed.
    jne .xor_loop

.xor_done:
    ret
    .size xor_checksum, .-xor_checksum

    # uint64_t rotl64(uint64_t value, unsigned int shift)
    #
    # System V AMD64 ABI:
    #   RDI = value
    #   ESI = shift
    #   RAX = 64-bit return value
    .globl rotl64
    .type rotl64, @function
rotl64:
    # RAX holds the return value, so copy the input value there first.
    movq %rdi, %rax

    # x86-64 rotate counts for 64-bit values use only the range 0..63.
    andl $63, %esi

    # The rolq instruction expects the variable shift count in CL.
    movl %esi, %ecx

    # Rotate the 64-bit value left by CL bits.
    rolq %cl, %rax
    ret
    .size rotl64, .-rotl64

    # Tell the linker this file does not need an executable stack.
    .section .note.GNU-stack,"",@progbits
