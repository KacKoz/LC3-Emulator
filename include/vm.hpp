#ifndef VM_H
#define VM_H

#include <string>
#include <iostream>
#include "cpu.hpp"
#include "memory_manager.hpp"

namespace lc3vm {

class VM
{
public:
    VM(): m_cpu(m_memoryManager) {}
    void loadProgram(const std::string& filename);
    void run();

private:
    void disable_input_buffering();
    void restore_input_buffering();
    uint16_t check_key();
    MemoryManager m_memoryManager;
    CPU m_cpu;
};

}

#endif