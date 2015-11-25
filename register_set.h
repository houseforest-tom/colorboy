#ifndef REGISTER_SET_H
#define REGISTER_SET_H

#include "util.h"

namespace gbhd{
    
    // Contains all of the Z80's registers.
    class register_set{
    public:
        // General purpose registers.
        byte a;
        byte b;
        byte c;
        byte d;
        byte e;
        byte h;
        byte l;
        
        // Flags register.
        byte f;
        
        // Program counter.
        word pc;
        
        // Stack pointer.
        word sp;
    };
}

#endif /* REGISTER_SET_H */

