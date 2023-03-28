#include "cpu.hpp"
#include <iostream>

#define INSTR_ARG(instr, first, last) ((instr) >> (first)) & ((1 << ((last+1) - (first)))-1)

namespace lc3vm
{

    void CPU::reset(address_t address)
    {
        m_registers[R_PC] = address;
        m_registers[R_PSR] &= ~(0x7); // zero condition flags
    }

    void CPU::updateFlags(Reg dr)
    {
        m_registers[R_PSR] &= ~(0x7); // zero condition flags
        if(m_registers[dr] == 0)
        {
            m_registers[R_PSR] |= Flag::ZER;
        }
        else if(m_registers[dr] >> 15)
        {
            m_registers[R_PSR] |= Flag::NEG;
        }
        else
        {
            m_registers[R_PSR] |= Flag::POS;
        }
    }

    void CPU::step()
    {
        word_t instr = m_memory.read(m_registers[R_PC]++);
        switch (instr >> 12)
        {
        case BR:
        {
            bool n = INSTR_ARG(instr, 11, 11);
            bool z = INSTR_ARG(instr, 10, 10);
            bool p = INSTR_ARG(instr, 9, 9);
            if( (n && m_registers[R_PSR] & Flag::NEG) ||
                (z && m_registers[R_PSR] & Flag::ZER) ||
                (p && m_registers[R_PSR] & Flag::POS))
            {
                m_registers[R_PC] += sext(INSTR_ARG(instr, 0, 8), 9);
            }
            break;
        }
        case ADD:
        {
        // TODO: przeniesc moze dr na gore i sprawdzac flagi na koncu?
            Reg dr = static_cast<Reg>(INSTR_ARG(instr, 9, 11));
            word_t v1 = m_registers[INSTR_ARG(instr, 6, 8)];
            word_t v2 = INSTR_ARG(instr, 5, 5) ? sext(INSTR_ARG(instr, 0, 4), 5) : m_registers[INSTR_ARG(instr, 0, 2)];
            m_registers[dr] = v1 + v2;
            updateFlags(dr);
            break;
        }
        case LD:
        {
            Reg dr = static_cast<Reg>(INSTR_ARG(instr, 9, 11));
            address_t addr = static_cast<address_t>(m_registers[R_PC] + sext(INSTR_ARG(instr, 0, 8), 9));
            m_registers[dr] = m_memory.read(addr);
            updateFlags(dr);
            break;
        }
        case ST:
        {
            Reg sr = static_cast<Reg>(INSTR_ARG(instr, 9, 11));
            address_t addr = static_cast<address_t>(m_registers[R_PC] + INSTR_ARG(instr, 0, 8));
            m_memory.write(addr, m_registers[sr]);
            break;
        }
        case JSR:
        {
            // save next instruction
            m_registers[R_R7] = m_registers[R_PC];
            if(INSTR_ARG(instr, 11, 11))
            {
                // add offset
                m_registers[R_PC] += INSTR_ARG(instr, 0, 10);
            }
            else
            {
                // jump to address from base register
                m_registers[R_PC] = INSTR_ARG(instr, 6, 8);
            }
            break;
        }
        case AND:
        {
            Reg dr = static_cast<Reg>(INSTR_ARG(instr, 9, 11));
            word_t v1 = m_registers[INSTR_ARG(instr, 6, 8)];
            word_t v2 = INSTR_ARG(instr, 5, 5) ? sext(INSTR_ARG(instr, 0, 4), 5) : m_registers[INSTR_ARG(instr, 0, 2)];
            m_registers[dr] = v1 & v2;
            updateFlags(dr);
            break;
        }
        case LDR:
        {
            Reg dr = static_cast<Reg>(INSTR_ARG(instr, 9, 11));
            m_registers[dr] = m_memory.read(m_registers[INSTR_ARG(instr, 6, 8)] + sext(INSTR_ARG(instr, 0, 5), 6));
            updateFlags(dr);
            break;
        }
        case STR:
        {
            Reg sr = static_cast<Reg>(INSTR_ARG(instr, 9, 11));
            m_memory.write(m_registers[INSTR_ARG(instr, 6, 8)] + sext(INSTR_ARG(instr, 0, 5), 6), m_registers[sr]);
            break;
        }
        case RTI:
        {
            if(!(m_registers[R_PSR] & PRVL))
            {
                m_registers[R_PC]   = m_memory.read(  m_registers[R_R6]);
                m_registers[R_PSR]  = m_memory.read(++m_registers[R_R6]);
            }
            else
            {
                // TODO: privilage mode exception
                throw std::logic_error("Unimplemented: privilage mode exception");
            }
            break;
        }
        case NOT:
        {
            Reg dr = static_cast<Reg>(INSTR_ARG(instr, 9, 11));
            word_t v1 = m_registers[INSTR_ARG(instr, 6, 8)];
            m_registers[dr] = ~v1;
            updateFlags(dr);
            break;
        }
        case LDI:
        {
            Reg dr = static_cast<Reg>(INSTR_ARG(instr, 9, 11));
            address_t addr = m_memory.read(m_registers[R_PC] + sext(INSTR_ARG(instr, 0, 8), 9));
            m_registers[dr] = m_memory.read(addr);
            updateFlags(dr);
            break;
        }
        case STI:
        {
            Reg sr = static_cast<Reg>(INSTR_ARG(instr, 9, 11));
            address_t addr = m_memory.read(m_registers[R_PC] + sext(INSTR_ARG(instr, 0, 8), 9));
            m_memory.write(addr, m_registers[sr]);
            break;
        }
        case JMP:
        {
            m_registers[R_PC] = m_registers[INSTR_ARG(instr, 6, 8)];
            break;
        }
        case LEA:
        {
            Reg dr = static_cast<Reg>(INSTR_ARG(instr, 9, 11));
            m_registers[dr] = m_registers[R_PC] + sext(INSTR_ARG(instr, 0, 8), 9);
            updateFlags(dr);
            break;
        }
        case TRAP:
        {
            // TODO: handle TRAP from os asm code
            // m_registers[R_R7] = m_registers[R_PC];
            // m_registers[R_PC] = m_memory.read(INSTR_ARG(instr, 0, 7));
            handleTrap(instr);
            break;
        }
        default:
            throw std::logic_error("Unimplemented: Illegal opcode");
            break;
        }
    }

    void CPU::setHaltCallback(std::function<void(void)> callback)
    {
        m_haltCallback = callback;
    }

    void CPU::handleTrap(word_t instr)
    {
        word_t trapVec = INSTR_ARG(instr, 0, 7);
        switch(trapVec)
        {
            case GETC:
            {
                m_registers[R_R0] = static_cast<word_t>(getchar());
                updateFlags(R_R0);
                break;
            }
            case OUT:
            {
                putc(static_cast<char>(m_registers[R_R0]), stdout);
                fflush(stdout);
                break;
            }
            case PUTS:
            {
                address_t addr = m_registers[R_R0];
                word_t val;
                while(val=m_memory.read(addr))
                {
                    putc(((char)val), stdout);
                    addr++;
                }
                fflush(stdout);
                break;

                break;
            }
            case IN:
            {
                char c = getchar();
                putc(c, stdout);
                fflush(stdout);
                m_registers[R_R0] = static_cast<word_t>(c);
                updateFlags(R_R0);
                break;
            }
            case PUTSP:
            {
                address_t addr = m_registers[R_R0];
                word_t val;
                while((val = m_memory.read(addr)) & 0x00FF)
                {
                     putc((val & 0x00FF), stdout);
                     if(!((val >> 8) & 0x00FF))
                        break;
                     putc(((val >> 8) & 0x00FF), stdout);
                    addr++;
                }
                fflush(stdout);
                break;
            }
            case HALT:
            {
                m_haltCallback();
                break;
            }
        }
    }

    word_t CPU::sext(uint16_t val, uint16_t bitsize)
    {
        if((val >> (bitsize-1)) & 1)
        {
            return val | (~((1 << bitsize)-1));
        }
        else
        {
            return val & ((1 << bitsize)-1);
        }
    }
}