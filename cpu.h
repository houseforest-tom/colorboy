#ifndef CPU_H
#define CPU_H

#include "util.h"
#include "mmu.h"
#include "register_set.h"
#include "instruction_set.h"

namespace colorboy {

    // Represents the GameBoy's cpu.
    class cpu {
    private:
        // Setup instruction set.
        void initInstructionSet(void);

    public:
        // Clock.
        word clk;

        // Registers.
        register_set regs;

        // Instruction set.
        instruction_set<word, ISET_SIZE> iset;

        // Initializes the cpu state.
        cpu(void);

        // Prints the cpu state to console.
        void dumpstate(void);
    };
}

#endif /* CPU_H */

