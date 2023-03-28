#ifndef CPU_H
#define CPU_H


#include <array>
#include <cstdint>
#include <functional>
#include "memory_manager.hpp"

namespace lc3vm {

enum Reg{
    R_R0=0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC, // program counter
    R_PSR, // processor status register
    R_COUNT
};

enum Flag {
    POS = 1 << 0,
    ZER = 1 << 1,
    NEG = 1 << 2,
    PRVL = 1 << 15
};

enum opcode {
    BR   = 0x0,
    ADD  = 0x1,
    LD   = 0x2,
    ST   = 0x3,
    JSR  = 0x4,
    AND  = 0x5,
    LDR  = 0x6,
    STR  = 0x7,
    RTI  = 0x8,
    NOT  = 0x9,
    LDI  = 0xA,
    STI  = 0xB,
    JMP  = 0xC,
    LEA  = 0xE,
    TRAP = 0xF
};

enum trapvec {
    GETC    = 0x20,
    OUT     = 0x21,
    PUTS    = 0x22,
    IN      = 0x23,
    PUTSP   = 0x24,
    HALT    = 0x25
};

class CPU
{
public:
    CPU(MemoryManager& mm) : m_memory(mm) {}
    void reset(address_t prog_addr);
    void step();
    void setHaltCallback(std::function<void(void)> callback);

private:
    std::function<void(void)> m_haltCallback;
    void handleTrap(word_t instr);
    void updateFlags(Reg dr);
    word_t sext(uint16_t val, uint16_t bitsize);
    std::array<word_t, R_COUNT> m_registers;
    MemoryManager& m_memory;
};

}

#endif