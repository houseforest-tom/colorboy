#include "cpu.h"
#include "mmu.h"

int main(int argc, char** argv) {
    using namespace gbhd;

    cpu cpu;
    mmu mmu;

    // Example binary.
    byte binary[0xff];
    memset(binary, 0x00, sizeof(binary));

    cpu.dumpstate();

    // CPU dispatch loop.
    while (cpu.regs.pc < sizeof(binary)) {

        // fetch -> decode -> execute.
        const instruction& instr = cpu.iset.decode(binary[cpu.regs.pc++]);
        instr.execute(cpu, mmu);

        cpu.dumpstate();
    }

    return 0;
}