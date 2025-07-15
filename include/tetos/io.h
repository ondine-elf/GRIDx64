#ifndef TETOS_IO_H
#define TETOS_IO_H

#include <stdint.h>

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile (
        "inb %1, %0;"
        : "=a" (ret)
        : "Nd" (port)
    );
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile (
        "outb %0, %1;"
        :
        : "a" (val), "Nd" (port)
    );
}

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    asm volatile (
        "inw %1, %0;"
        : "=a" (ret)
        : "Nd" (port)
    );
    return ret;
}

static inline void outw(uint16_t port, uint16_t val) {
    asm volatile (
        "outw %0, %1;"
        :
        : "a" (val), "Nd" (port)
    );
}

static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    asm volatile (
        "inl %1, %0;"
        : "=a" (ret)
        : "Nd" (port)
    );
    return ret;
}

static inline void outl(uint16_t port, uint32_t val) {
    asm volatile (
        "outl %0, %1;"
        :
        : "a" (val), "Nd" (port)
    );
}

static inline void io_wait() {
    outb(0x80, 0x00);
}

static inline uint64_t rdmsr(uint32_t msr) {
    uint32_t low, high;
    asm volatile (
        "rdmsr;"
        : "=a" (low), "=d" (high)
        : "c" (msr)
    );
    return ((uint64_t)high << 32) | low;
}

static inline void wrmsr(uint32_t msr, uint64_t value) {
    uint32_t low = (uint32_t)(value & 0xFFFFFFFF);
    uint32_t high = (uint32_t)(value >> 32);
    asm volatile (
        "wrmsr;"
        :
        : "c" (msr), "a" (low), "d" (high)
    );
}

#endif