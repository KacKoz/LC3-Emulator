#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <cstdint>
#include <array>

#define MEMORY_SIZE 0xFFFF

namespace lc3vm {

enum mmio {
    MR_KBSR = 0xFE00, /* keyboard status */
    MR_KBDR = 0xFE02  /* keyboard data */
};

using address_t = uint16_t;
using word_t = uint16_t;

class MemoryManager
{
public:
    uint16_t read(address_t addr);
    void write(address_t addr, word_t value);

private:
    std::array<word_t, MEMORY_SIZE> m_ram;
};


};

#endif