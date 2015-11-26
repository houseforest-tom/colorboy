#ifndef REGISTER_SET_H
#define REGISTER_SET_H

#include "util.h"

namespace colorboy {

    // Contains all of the Z80's registers.
    class register_set {
    public:
        // General purpose register.
        byte a;

        // General purpose register.
        byte b;

        // General purpose register.
        byte c;

        // General purpose register.
        byte d;

        // General purpose register.
        byte e;

        // General purpose register, HL often used as memory address.
        byte h;

        // General purpose register, HL often used as memory address.
        byte l;

        // Flags register. 0x80: Zero, 0x40: Operation, 0x20: Half-Carry, 0x10 Carry.
        byte f;

        // Program counter.
        word pc;

        // Stack pointer.
        word sp;

        // Initialize registers to zero.
        register_set(void)
            : a(0x00),
              b(0x00),
              c(0x00),
              d(0x00),
              e(0x00),
              h(0x00),
              l(0x00),
              f(0x00),
              pc(0x0000),
              sp(0x0000) {
        }
    };
}

#endif /* REGISTER_SET_H */

