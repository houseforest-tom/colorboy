#include <random>
#include "cpu.h"
#include "mmu.h"

using rng = std::mt19937_64;

int main(int argc, char** argv) {
    using namespace colorboy;

    // Initialize hardware components.
    cpu cpu;
    mmu mmu;

    // Setup RNG.
    rng rng;
    rng.seed(util::timing::currentTime().time_since_epoch().count());

    // Create test binary.
    byte binary[0xff];
    for(size_t offset=0;offset<sizeof(binary);++offset){
        binary[offset] = rng() % 64;
    }

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