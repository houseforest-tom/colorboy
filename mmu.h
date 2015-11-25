#ifndef MMU_H
#define MMU_H

#include "util.h"

namespace gbhd{
    
    // Interface for memory access.
    template<typename address_type, uint64_t size>
    class memory_interface{
    protected:
        // Stored values.
        byte m_mem[size];

    public:
        // Initializes a new chunk of memory to zero.
        memory_interface(void){ clear(); }
        
        // Sets the memory region to zero.
        inline void clear(address_type offset, uint64_t length){ memset(m_mem + offset, 0x00, length); }
        
        // Sets the complete memory to zero.
        inline void clear(void){ clear(address_type(0), size); }
        
        // Reads from the specified memory location.
        virtual byte read8(address_type addr)  = 0;
        virtual word read16(address_type addr) = 0;
        
        // Writes to the specified memory location.
        virtual void write8(address_type addr, byte value)  = 0;
        virtual void write16(address_type addr, word value) = 0;
    };
    
    // Memory management unit.
    class mmu : public memory_interface<word, RAM_SIZE>{
    public:
        byte read8(word addr) override;
        word read16(word addr) override;
        void write8(word addr, byte value) override;
        void write16(word addr, word value) override;
    };
}

#endif /* MEMORY_H */

