#include "mmu.h"

namespace colorboy {
    
    byte mmu::read8(word addr){
        return m_mem[addr];
    }

    word mmu::read16(word addr){
        return concat(m_mem[addr], m_mem[addr + 1]);
    }

    void mmu::write8(word addr, byte value){
        m_mem[addr] = value;
    }

    void mmu::write16(word addr, word value){
        m_mem[addr] = (value >> 8) & 0xFF;
        m_mem[addr + 1] = value & 0xFF;
    }
}