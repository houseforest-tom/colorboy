#ifndef INSTRUCTION_SET_H
#define INSTRUCTION_SET_H

#include "util.h"

namespace colorboy {

    // Forward declare cpu class.
    class cpu;

    // Operation to be executed by the cpu.
    using cpu_operation = function<void (cpu&, mmu&)>;

    class instruction {
    public:
        // Operation tied to this instruction.
        cpu_operation op;

        // Execute the operation on the given cpu.
        inline void execute(cpu &cpu, mmu &mmu) const {
            this->op(cpu, mmu);
        }
    };

    template<typename opcode_type, uint64_t size>
    class instruction_set {
    private:
        // List of instructions.
        instruction instrs[size];

    public:
        // Store / overwrite operation linked to the provided opcode.
        inline void store(opcode_type opcode, const cpu_operation &op) {
            instrs[opcode].op = op;
        }

        // Decode the provided opcode and return the corresponding instruction.
        inline const instruction &decode(opcode_type opcode) const {
            return instrs[opcode];
        }
    };
}

#endif /* INSTRUCTION_SET_H */

