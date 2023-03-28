#include "memory_manager.hpp"

namespace lc3vm {

uint16_t MemoryManager::read(address_t addr)
{
    // TODO: check for overflow
    return m_ram[addr];
}

void MemoryManager::write(address_t addr, word_t value)
{
    // TODO: check for overflow
    m_ram[addr] = value;
}


}