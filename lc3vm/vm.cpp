#include "vm.hpp"
#include <fstream>
#include <iterator>
#include <vector>

#include <stdio.h>
#include <stdint.h>
#include <signal.h>
/* unix only */
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>

namespace lc3vm {

struct termios original_tio;


void VM::disable_input_buffering()
{
    tcgetattr(STDIN_FILENO, &original_tio);
    struct termios new_tio = original_tio;
    new_tio.c_lflag &= ~ICANON & ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void VM::restore_input_buffering()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}

void VM::loadProgram(const std::string& filename)
{
    std::ifstream f(filename.c_str(), std::ios::binary);
    word_t word;
    std::vector<unsigned char> buff(std::istreambuf_iterator<char>(f), {});
    
    word = buff[0];
    word <<= 8;
    word |= buff[1];
    address_t addr = word;
    m_cpu.reset(addr);
    for(size_t i = 2; i < buff.size() ; i+=2)
    {
        word = buff[i];
        word <<= 8;
        // what if size is odd?
        word |= buff[i+1];
        m_memoryManager.write(addr, word);
        addr++;
    }
    m_memoryManager.write(addr, 0xF025);
}

void VM::run()
{
    disable_input_buffering();
    bool running = true;
    m_cpu.setHaltCallback([&running](){running = false;});
    while(running)
    {
        m_cpu.step();

        if (check_key())
        {
            auto key = getchar();
            //std::cout << key << std::endl;
            m_memoryManager.write(MR_KBSR, (1 << 15));
            m_memoryManager.write(MR_KBDR, key);
            //std::cout << m_memoryManager.read(MR_KBDR) << std::endl;
        }
        else
        {
            m_memoryManager.write(MR_KBSR, 0);
        }
    // check keyboard io
    // update memory mapped registers
    // check memory mapped registers for data to print ?
    // clear memory mapped registers for data to print ?
    // mcr - when bit [15] is set  machine should stop
    }
    restore_input_buffering();
}

uint16_t VM::check_key()
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return select(1, &readfds, NULL, NULL, &timeout) != 0;
}

}