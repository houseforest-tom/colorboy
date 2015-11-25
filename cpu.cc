#include "cpu.h"

// Register -> register.
#define LDrr(destination, source) \
[](cpu &cpu, mmu &mmu){ \
    cpu.regs.destination = cpu.regs.source; \
    cpu.clk = 1; \
}

// Memory -> register.
#define LDrm(destination, hi, lo) \
[](cpu &cpu, mmu &mmu){ \
    cpu.regs.destination = mmu.read8(concat(cpu.regs.h, cpu.regs.l)); \
    cpu.clk = 2; \
}

// Register -> memory.
#define LDmr(hi, lo, source) \
[](cpu &cpu, mmu &mmu){ \
    mmu.write8(concat(cpu.regs.h, cpu.regs.l), cpu.regs.source); \
    cpu.clk = 2; \
}

// Constant address -> register.
#define LDrn(destination) \
[](cpu &cpu, mmu &mmu){ \
    cpu.regs.destination = mmu.read8(cpu.regs.pc++); \
    cpu.clk = 2; \
}

// Memory -> memory.
#define LDmm(destination_hi, destination_lo) \
[](cpu &cpu, mmu &mmu){ \
    mmu.write8( \
        concat(cpu.regs.hi, cpu.regs.lo), \
        mmu.read8(concat(cpu.regs.h, cpu.regs.l)) \
    ); \
    cpu.clk = 3; \
}

namespace colorboy {

    cpu::cpu(void) {
        initInstructionSet();
    }

    void cpu::initInstructionSet(void) {

        word opcode = 0x0000;

        /* Load registers with values from another register. */

        iset.store(opcode++, LDrr(b, b));
        iset.store(opcode++, LDrr(b, c));
        iset.store(opcode++, LDrr(b, d));
        iset.store(opcode++, LDrr(b, e));
        iset.store(opcode++, LDrr(b, h));
        iset.store(opcode++, LDrr(b, l));
        iset.store(opcode++, LDrr(b, a));

        iset.store(opcode++, LDrr(c, b));
        iset.store(opcode++, LDrr(c, c));
        iset.store(opcode++, LDrr(c, d));
        iset.store(opcode++, LDrr(c, e));
        iset.store(opcode++, LDrr(c, h));
        iset.store(opcode++, LDrr(c, l));
        iset.store(opcode++, LDrr(c, a));

        iset.store(opcode++, LDrr(d, b));
        iset.store(opcode++, LDrr(d, c));
        iset.store(opcode++, LDrr(d, d));
        iset.store(opcode++, LDrr(d, e));
        iset.store(opcode++, LDrr(d, h));
        iset.store(opcode++, LDrr(d, l));
        iset.store(opcode++, LDrr(d, a));

        iset.store(opcode++, LDrr(e, b));
        iset.store(opcode++, LDrr(e, c));
        iset.store(opcode++, LDrr(e, d));
        iset.store(opcode++, LDrr(e, e));
        iset.store(opcode++, LDrr(e, h));
        iset.store(opcode++, LDrr(e, l));
        iset.store(opcode++, LDrr(e, a));

        iset.store(opcode++, LDrr(h, b));
        iset.store(opcode++, LDrr(h, c));
        iset.store(opcode++, LDrr(h, d));
        iset.store(opcode++, LDrr(h, e));
        iset.store(opcode++, LDrr(h, h));
        iset.store(opcode++, LDrr(h, l));
        iset.store(opcode++, LDrr(h, a));

        iset.store(opcode++, LDrr(l, b));
        iset.store(opcode++, LDrr(l, c));
        iset.store(opcode++, LDrr(l, d));
        iset.store(opcode++, LDrr(l, e));
        iset.store(opcode++, LDrr(l, h));
        iset.store(opcode++, LDrr(l, l));
        iset.store(opcode++, LDrr(l, a));

        iset.store(opcode++, LDrr(a, b));
        iset.store(opcode++, LDrr(a, c));
        iset.store(opcode++, LDrr(a, d));
        iset.store(opcode++, LDrr(a, e));
        iset.store(opcode++, LDrr(a, h));
        iset.store(opcode++, LDrr(a, l));
        iset.store(opcode++, LDrr(a, a));

        /* Load registers with values from memory location stored in HL */

        iset.store(opcode++, LDrm(b, h, l));
        iset.store(opcode++, LDrm(c, h, l));
        iset.store(opcode++, LDrm(d, h, l));
        iset.store(opcode++, LDrm(e, h, l));
        iset.store(opcode++, LDrm(h, h, l));
        iset.store(opcode++, LDrm(l, h, l));
        iset.store(opcode++, LDrm(a, h, l));

        /* Store value from registers at memory location stored in HL. */

        iset.store(opcode++, LDmr(h, l, b));
        iset.store(opcode++, LDmr(h, l, c));
        iset.store(opcode++, LDmr(h, l, d));
        iset.store(opcode++, LDmr(h, l, e));
        iset.store(opcode++, LDmr(h, l, h));
        iset.store(opcode++, LDmr(h, l, l));
        iset.store(opcode++, LDmr(h, l, a));

        /* Store value from constant address in register. */

        iset.store(opcode++, LDrn(b));
        iset.store(opcode++, LDrn(c));
        iset.store(opcode++, LDrn(d));
        iset.store(opcode++, LDrn(e));
        iset.store(opcode++, LDrn(h));
        iset.store(opcode++, LDrn(l));
        iset.store(opcode++, LDrn(a));

        /* More load & store operations. */

        // LDHLmn
        iset.store(opcode++, [ ](cpu& cpu, mmu& mmu) {
            mmu.write8(concat(cpu.regs.h, cpu.regs.l), mmu.read8(cpu.regs.pc++));
            cpu.clk = 3;
        });

        // LDBCmA
        iset.store(opcode++, [ ](cpu& cpu, mmu& mmu) {
            mmu.write8(concat(cpu.regs.b, cpu.regs.c), mmu.read8(cpu.regs.a));
            cpu.clk = 2;
        });

        // LDDEmA
        iset.store(opcode++, [ ](cpu& cpu, mmu& mmu) {
            mmu.write8(concat(cpu.regs.d, cpu.regs.e), mmu.read8(cpu.regs.a));
            cpu.clk = 2;
        });

        // LDmmA
        iset.store(opcode++, [ ](cpu& cpu, mmu& mmu) {
            mmu.write8(mmu.read8(cpu.regs.pc), cpu.regs.a);
            cpu.regs.pc += 2;
            cpu.clk = 4;
        });

        // LDABCm
        iset.store(opcode++, [ ](cpu& cpu, mmu& mmu) {
            cpu.regs.a = mmu.read8(concat(cpu.regs.b, cpu.regs.c));
            cpu.clk = 2;
        });

        // LDADEm
        iset.store(opcode++, [ ](cpu& cpu, mmu& mmu) {
            cpu.regs.a = mmu.read8(concat(cpu.regs.d, cpu.regs.e));
            cpu.clk = 2;
        });

        // LDAmm
        iset.store(opcode++, [ ](cpu& cpu, mmu& mmu) {
            cpu.regs.a = mmu.read8(mmu.read16(cpu.regs.pc));
            cpu.regs.pc += 2;
            cpu.clk = 4;
        });

        // LDBCnn
        iset.store(opcode++, [ ](cpu& cpu, mmu& mmu) {
            cpu.regs.c = mmu.read8(cpu.regs.pc++);
            cpu.regs.b = mmu.read8(cpu.regs.pc++);
            cpu.clk = 3;
        });

        // LDDEnn
        iset.store(opcode++, [ ](cpu& cpu, mmu& mmu) {
            cpu.regs.e = mmu.read8(cpu.regs.pc++);
            cpu.regs.d = mmu.read8(cpu.regs.pc++);
            cpu.clk = 3;
        });

        // LDHLnn
        iset.store(opcode++, [ ](cpu& cpu, mmu& mmu) {
            cpu.regs.l = mmu.read8(cpu.regs.pc++);
            cpu.regs.h = mmu.read8(cpu.regs.pc++);
            cpu.clk = 3;
        });

        // LDSPnn
        iset.store(opcode++, [ ](cpu& cpu, mmu& mmu) {
            cpu.regs.sp = mmu.read16(cpu.regs.pc);
            cpu.regs.pc += 2;
            cpu.clk = 3;
        });

        // LDHLmm 
        iset.store(opcode++, [ ](cpu& cpu, mmu& mmu) {
            word value = mmu.read16(cpu.regs.pc);
            cpu.regs.pc += 2;
            cpu.regs.l = mmu.read8(value);
            cpu.regs.h = mmu.read8(value + 1);
            cpu.clk = 5;
        });

        // LDmmHL
        iset.store(opcode++, [ ](cpu& cpu, mmu& mmu) {
            word value = mmu.read16(cpu.regs.pc);
            cpu.regs.pc += 2;
            mmu.write16(value, concat(cpu.regs.h, cpu.regs.l));
            cpu.clk = 5;
        });

        // LDHLIA
        iset.store(opcode++, [ ](cpu& cpu, mmu& mmu) {
            mmu.write8(concat(cpu.regs.h, cpu.regs.l), cpu.regs.a);
            cpu.regs.l += 1;
            if (!cpu.regs.l) { cpu.regs.h += 1; }
            cpu.clk = 2;
        });

        // LDAHLI
        iset.store(opcode++, [ ](cpu& cpu, mmu& mmu) {
            cpu.regs.a = mmu.read8(concat(cpu.regs.h, cpu.regs.l));
            cpu.regs.l += 1;
            if (!cpu.regs.l) { cpu.regs.h += 1; }
            cpu.clk = 2;
        });

        /*
        LDHLDA: function() { MMU.wb((Z80._r.h<<8)+Z80._r.l, Z80._r.a); Z80._r.l=(Z80._r.l-1)&255; if(Z80._r.l==255) Z80._r.h=(Z80._r.h-1)&255; Z80._r.m=2; Z80._r.t=8; },
        LDAHLD: function() { Z80._r.a=MMU.rb((Z80._r.h<<8)+Z80._r.l); Z80._r.l=(Z80._r.l-1)&255; if(Z80._r.l==255) Z80._r.h=(Z80._r.h-1)&255; Z80._r.m=2; Z80._r.t=8; },

        LDAIOn: function() { Z80._r.a=MMU.rb(0xFF00+MMU.rb(Z80._r.pc)); Z80._r.pc++; Z80._r.m=3; Z80._r.t=12; },
        LDIOnA: function() { MMU.wb(0xFF00+MMU.rb(Z80._r.pc),Z80._r.a); Z80._r.pc++; Z80._r.m=3; Z80._r.t=12; },
        LDAIOC: function() { Z80._r.a=MMU.rb(0xFF00+Z80._r.c); Z80._r.m=2; Z80._r.t=8; },
        LDIOCA: function() { MMU.wb(0xFF00+Z80._r.c,Z80._r.a); Z80._r.m=2; Z80._r.t=8; },

        LDHLSPn: function() { var i=MMU.rb(Z80._r.pc); if(i>127) i=-((~i+1)&255); Z80._r.pc++; i+=Z80._r.sp; Z80._r.h=(i>>8)&255; Z80._r.l=i&255; Z80._r.m=3; Z80._r.t=12; },

        SWAPr_b: function() { var tr=Z80._r.b; Z80._r.b=MMU.rb((Z80._r.h<<8)+Z80._r.l); MMU.wb((Z80._r.h<<8)+Z80._r.l,tr); Z80._r.m=4; Z80._r.t=16; },
        SWAPr_c: function() { var tr=Z80._r.c; Z80._r.c=MMU.rb((Z80._r.h<<8)+Z80._r.l); MMU.wb((Z80._r.h<<8)+Z80._r.l,tr); Z80._r.m=4; Z80._r.t=16; },
        SWAPr_d: function() { var tr=Z80._r.d; Z80._r.d=MMU.rb((Z80._r.h<<8)+Z80._r.l); MMU.wb((Z80._r.h<<8)+Z80._r.l,tr); Z80._r.m=4; Z80._r.t=16; },
        SWAPr_e: function() { var tr=Z80._r.e; Z80._r.e=MMU.rb((Z80._r.h<<8)+Z80._r.l); MMU.wb((Z80._r.h<<8)+Z80._r.l,tr); Z80._r.m=4; Z80._r.t=16; },
        SWAPr_h: function() { var tr=Z80._r.h; Z80._r.h=MMU.rb((Z80._r.h<<8)+Z80._r.l); MMU.wb((Z80._r.h<<8)+Z80._r.l,tr); Z80._r.m=4; Z80._r.t=16; },
        SWAPr_l: function() { var tr=Z80._r.l; Z80._r.l=MMU.rb((Z80._r.h<<8)+Z80._r.l); MMU.wb((Z80._r.h<<8)+Z80._r.l,tr); Z80._r.m=4; Z80._r.t=16; },
        SWAPr_a: function() { var tr=Z80._r.a; Z80._r.a=MMU.rb((Z80._r.h<<8)+Z80._r.l); MMU.wb((Z80._r.h<<8)+Z80._r.l,tr); Z80._r.m=4; Z80._r.t=16; },

        ADDr_b: function() { Z80._r.a+=Z80._r.b; Z80._ops.fz(Z80._r.a); if(Z80._r.a>255) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        ADDr_c: function() { Z80._r.a+=Z80._r.c; Z80._ops.fz(Z80._r.a); if(Z80._r.a>255) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        ADDr_d: function() { Z80._r.a+=Z80._r.d; Z80._ops.fz(Z80._r.a); if(Z80._r.a>255) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        ADDr_e: function() { Z80._r.a+=Z80._r.e; Z80._ops.fz(Z80._r.a); if(Z80._r.a>255) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        ADDr_h: function() { Z80._r.a+=Z80._r.h; Z80._ops.fz(Z80._r.a); if(Z80._r.a>255) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        ADDr_l: function() { Z80._r.a+=Z80._r.l; Z80._ops.fz(Z80._r.a); if(Z80._r.a>255) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        ADDr_a: function() { Z80._r.a+=Z80._r.a; Z80._ops.fz(Z80._r.a); if(Z80._r.a>255) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        ADDHL: function() { Z80._r.a+=MMU.rb((Z80._r.h<<8)+Z80._r.l); Z80._ops.fz(Z80._r.a); if(Z80._r.a>255) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=2; Z80._r.t=8; },
        ADDn: function() { Z80._r.a+=MMU.rb(Z80._r.pc); Z80._r.pc++; Z80._ops.fz(Z80._r.a); if(Z80._r.a>255) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=2; Z80._r.t=8; },
        ADDHLBC: function() { var hl=(Z80._r.h<<8)+Z80._r.l; hl+=(Z80._r.b<<8)+Z80._r.c; if(hl>65535) Z80._r.f|=0x10; else Z80._r.f&=0xEF; Z80._r.h=(hl>>8)&255; Z80._r.l=hl&255; Z80._r.m=3; Z80._r.t=12; },
        ADDHLDE: function() { var hl=(Z80._r.h<<8)+Z80._r.l; hl+=(Z80._r.d<<8)+Z80._r.e; if(hl>65535) Z80._r.f|=0x10; else Z80._r.f&=0xEF; Z80._r.h=(hl>>8)&255; Z80._r.l=hl&255; Z80._r.m=3; Z80._r.t=12; },
        ADDHLHL: function() { var hl=(Z80._r.h<<8)+Z80._r.l; hl+=(Z80._r.h<<8)+Z80._r.l; if(hl>65535) Z80._r.f|=0x10; else Z80._r.f&=0xEF; Z80._r.h=(hl>>8)&255; Z80._r.l=hl&255; Z80._r.m=3; Z80._r.t=12; },
        ADDHLSP: function() { var hl=(Z80._r.h<<8)+Z80._r.l; hl+=Z80._r.sp; if(hl>65535) Z80._r.f|=0x10; else Z80._r.f&=0xEF; Z80._r.h=(hl>>8)&255; Z80._r.l=hl&255; Z80._r.m=3; Z80._r.t=12; },
        ADDSPn: function() { var i=MMU.rb(Z80._r.pc); if(i>127) i=-((~i+1)&255); Z80._r.pc++; Z80._r.sp+=i; Z80._r.m=4; Z80._r.t=16; },

        ADCr_b: function() { Z80._r.a+=Z80._r.b; Z80._r.a+=(Z80._r.f&0x10)?1:0; Z80._ops.fz(Z80._r.a); if(Z80._r.a>255) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        ADCr_c: function() { Z80._r.a+=Z80._r.c; Z80._r.a+=(Z80._r.f&0x10)?1:0; Z80._ops.fz(Z80._r.a); if(Z80._r.a>255) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        ADCr_d: function() { Z80._r.a+=Z80._r.d; Z80._r.a+=(Z80._r.f&0x10)?1:0; Z80._ops.fz(Z80._r.a); if(Z80._r.a>255) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        ADCr_e: function() { Z80._r.a+=Z80._r.e; Z80._r.a+=(Z80._r.f&0x10)?1:0; Z80._ops.fz(Z80._r.a); if(Z80._r.a>255) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        ADCr_h: function() { Z80._r.a+=Z80._r.h; Z80._r.a+=(Z80._r.f&0x10)?1:0; Z80._ops.fz(Z80._r.a); if(Z80._r.a>255) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        ADCr_l: function() { Z80._r.a+=Z80._r.l; Z80._r.a+=(Z80._r.f&0x10)?1:0; Z80._ops.fz(Z80._r.a); if(Z80._r.a>255) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        ADCr_a: function() { Z80._r.a+=Z80._r.a; Z80._r.a+=(Z80._r.f&0x10)?1:0; Z80._ops.fz(Z80._r.a); if(Z80._r.a>255) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        ADCHL: function() { Z80._r.a+=MMU.rb((Z80._r.h<<8)+Z80._r.l); Z80._r.a+=(Z80._r.f&0x10)?1:0; Z80._ops.fz(Z80._r.a); if(Z80._r.a>255) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=2; Z80._r.t=8; },
        ADCn: function() { Z80._r.a+=MMU.rb(Z80._r.pc); Z80._r.pc++; Z80._r.a+=(Z80._r.f&0x10)?1:0; Z80._ops.fz(Z80._r.a); if(Z80._r.a>255) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=2; Z80._r.t=8; },

        SUBr_b: function() { Z80._r.a-=Z80._r.b; Z80._ops.fz(Z80._r.a,1); if(Z80._r.a<0) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        SUBr_c: function() { Z80._r.a-=Z80._r.c; Z80._ops.fz(Z80._r.a,1); if(Z80._r.a<0) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        SUBr_d: function() { Z80._r.a-=Z80._r.d; Z80._ops.fz(Z80._r.a,1); if(Z80._r.a<0) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        SUBr_e: function() { Z80._r.a-=Z80._r.e; Z80._ops.fz(Z80._r.a,1); if(Z80._r.a<0) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        SUBr_h: function() { Z80._r.a-=Z80._r.h; Z80._ops.fz(Z80._r.a,1); if(Z80._r.a<0) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        SUBr_l: function() { Z80._r.a-=Z80._r.l; Z80._ops.fz(Z80._r.a,1); if(Z80._r.a<0) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        SUBr_a: function() { Z80._r.a-=Z80._r.a; Z80._ops.fz(Z80._r.a,1); if(Z80._r.a<0) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        SUBHL: function() { Z80._r.a-=MMU.rb((Z80._r.h<<8)+Z80._r.l); Z80._ops.fz(Z80._r.a,1); if(Z80._r.a<0) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=2; Z80._r.t=8; },
        SUBn: function() { Z80._r.a-=MMU.rb(Z80._r.pc); Z80._r.pc++; Z80._ops.fz(Z80._r.a,1); if(Z80._r.a<0) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=2; Z80._r.t=8; },

        SBCr_b: function() { Z80._r.a-=Z80._r.b; Z80._r.a-=(Z80._r.f&0x10)?1:0; Z80._ops.fz(Z80._r.a,1); if(Z80._r.a<0) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        SBCr_c: function() { Z80._r.a-=Z80._r.c; Z80._r.a-=(Z80._r.f&0x10)?1:0; Z80._ops.fz(Z80._r.a,1); if(Z80._r.a<0) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        SBCr_d: function() { Z80._r.a-=Z80._r.d; Z80._r.a-=(Z80._r.f&0x10)?1:0; Z80._ops.fz(Z80._r.a,1); if(Z80._r.a<0) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        SBCr_e: function() { Z80._r.a-=Z80._r.e; Z80._r.a-=(Z80._r.f&0x10)?1:0; Z80._ops.fz(Z80._r.a,1); if(Z80._r.a<0) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        SBCr_h: function() { Z80._r.a-=Z80._r.h; Z80._r.a-=(Z80._r.f&0x10)?1:0; Z80._ops.fz(Z80._r.a,1); if(Z80._r.a<0) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        SBCr_l: function() { Z80._r.a-=Z80._r.l; Z80._r.a-=(Z80._r.f&0x10)?1:0; Z80._ops.fz(Z80._r.a,1); if(Z80._r.a<0) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        SBCr_a: function() { Z80._r.a-=Z80._r.a; Z80._r.a-=(Z80._r.f&0x10)?1:0; Z80._ops.fz(Z80._r.a,1); if(Z80._r.a<0) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=1; Z80._r.t=4; },
        SBCHL: function() { Z80._r.a-=MMU.rb((Z80._r.h<<8)+Z80._r.l); Z80._r.a-=(Z80._r.f&0x10)?1:0; Z80._ops.fz(Z80._r.a,1); if(Z80._r.a<0) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=2; Z80._r.t=8; },
        SBCn: function() { Z80._r.a-=MMU.rb(Z80._r.pc); Z80._r.pc++; Z80._r.a-=(Z80._r.f&0x10)?1:0; Z80._ops.fz(Z80._r.a,1); if(Z80._r.a<0) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=2; Z80._r.t=8; },

        CPr_b: function() { var i=Z80._r.a; i-=Z80._r.b; Z80._ops.fz(i,1); if(i<0) Z80._r.f|=0x10; i&=255; Z80._r.m=1; Z80._r.t=4; },
        CPr_c: function() { var i=Z80._r.a; i-=Z80._r.c; Z80._ops.fz(i,1); if(i<0) Z80._r.f|=0x10; i&=255; Z80._r.m=1; Z80._r.t=4; },
        CPr_d: function() { var i=Z80._r.a; i-=Z80._r.d; Z80._ops.fz(i,1); if(i<0) Z80._r.f|=0x10; i&=255; Z80._r.m=1; Z80._r.t=4; },
        CPr_e: function() { var i=Z80._r.a; i-=Z80._r.e; Z80._ops.fz(i,1); if(i<0) Z80._r.f|=0x10; i&=255; Z80._r.m=1; Z80._r.t=4; },
        CPr_h: function() { var i=Z80._r.a; i-=Z80._r.h; Z80._ops.fz(i,1); if(i<0) Z80._r.f|=0x10; i&=255; Z80._r.m=1; Z80._r.t=4; },
        CPr_l: function() { var i=Z80._r.a; i-=Z80._r.l; Z80._ops.fz(i,1); if(i<0) Z80._r.f|=0x10; i&=255; Z80._r.m=1; Z80._r.t=4; },
        CPr_a: function() { var i=Z80._r.a; i-=Z80._r.a; Z80._ops.fz(i,1); if(i<0) Z80._r.f|=0x10; i&=255; Z80._r.m=1; Z80._r.t=4; },
        CPHL: function() { var i=Z80._r.a; i-=MMU.rb((Z80._r.h<<8)+Z80._r.l); Z80._ops.fz(i,1); if(i<0) Z80._r.f|=0x10; i&=255; Z80._r.m=2; Z80._r.t=8; },
        CPn: function() { var i=Z80._r.a; i-=MMU.rb(Z80._r.pc); Z80._r.pc++; Z80._ops.fz(i,1); if(i<0) Z80._r.f|=0x10; i&=255; Z80._r.m=2; Z80._r.t=8; },

        ANDr_b: function() { Z80._r.a&=Z80._r.b; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=1; Z80._r.t=4; },
        ANDr_c: function() { Z80._r.a&=Z80._r.c; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=1; Z80._r.t=4; },
        ANDr_d: function() { Z80._r.a&=Z80._r.d; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=1; Z80._r.t=4; },
        ANDr_e: function() { Z80._r.a&=Z80._r.e; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=1; Z80._r.t=4; },
        ANDr_h: function() { Z80._r.a&=Z80._r.h; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=1; Z80._r.t=4; },
        ANDr_l: function() { Z80._r.a&=Z80._r.l; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=1; Z80._r.t=4; },
        ANDr_a: function() { Z80._r.a&=Z80._r.a; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=1; Z80._r.t=4; },
        ANDHL: function() { Z80._r.a&=MMU.rb((Z80._r.h<<8)+Z80._r.l); Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=2; Z80._r.t=8; },
        ANDn: function() { Z80._r.a&=MMU.rb(Z80._r.pc); Z80._r.pc++; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=2; Z80._r.t=8; },

        ORr_b: function() { Z80._r.a|=Z80._r.b; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=1; Z80._r.t=4; },
        ORr_c: function() { Z80._r.a|=Z80._r.c; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=1; Z80._r.t=4; },
        ORr_d: function() { Z80._r.a|=Z80._r.d; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=1; Z80._r.t=4; },
        ORr_e: function() { Z80._r.a|=Z80._r.e; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=1; Z80._r.t=4; },
        ORr_h: function() { Z80._r.a|=Z80._r.h; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=1; Z80._r.t=4; },
        ORr_l: function() { Z80._r.a|=Z80._r.l; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=1; Z80._r.t=4; },
        ORr_a: function() { Z80._r.a|=Z80._r.a; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=1; Z80._r.t=4; },
        ORHL: function() { Z80._r.a|=MMU.rb((Z80._r.h<<8)+Z80._r.l); Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=2; Z80._r.t=8; },
        ORn: function() { Z80._r.a|=MMU.rb(Z80._r.pc); Z80._r.pc++; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=2; Z80._r.t=8; },

        XORr_b: function() { Z80._r.a^=Z80._r.b; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=1; Z80._r.t=4; },
        XORr_c: function() { Z80._r.a^=Z80._r.c; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=1; Z80._r.t=4; },
        XORr_d: function() { Z80._r.a^=Z80._r.d; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=1; Z80._r.t=4; },
        XORr_e: function() { Z80._r.a^=Z80._r.e; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=1; Z80._r.t=4; },
        XORr_h: function() { Z80._r.a^=Z80._r.h; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=1; Z80._r.t=4; },
        XORr_l: function() { Z80._r.a^=Z80._r.l; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=1; Z80._r.t=4; },
        XORr_a: function() { Z80._r.a^=Z80._r.a; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=1; Z80._r.t=4; },
        XORHL: function() { Z80._r.a^=MMU.rb((Z80._r.h<<8)+Z80._r.l); Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=2; Z80._r.t=8; },
        XORn: function() { Z80._r.a^=MMU.rb(Z80._r.pc); Z80._r.pc++; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=2; Z80._r.t=8; },

        INCr_b: function() { Z80._r.b++; Z80._r.b&=255; Z80._ops.fz(Z80._r.b); Z80._r.m=1; Z80._r.t=4; },
        INCr_c: function() { Z80._r.c++; Z80._r.c&=255; Z80._ops.fz(Z80._r.c); Z80._r.m=1; Z80._r.t=4; },
        INCr_d: function() { Z80._r.d++; Z80._r.d&=255; Z80._ops.fz(Z80._r.d); Z80._r.m=1; Z80._r.t=4; },
        INCr_e: function() { Z80._r.e++; Z80._r.e&=255; Z80._ops.fz(Z80._r.e); Z80._r.m=1; Z80._r.t=4; },
        INCr_h: function() { Z80._r.h++; Z80._r.h&=255; Z80._ops.fz(Z80._r.h); Z80._r.m=1; Z80._r.t=4; },
        INCr_l: function() { Z80._r.l++; Z80._r.l&=255; Z80._ops.fz(Z80._r.l); Z80._r.m=1; Z80._r.t=4; },
        INCr_a: function() { Z80._r.a++; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=1; Z80._r.t=4; },
        INCHLm: function() { var i=MMU.rb((Z80._r.h<<8)+Z80._r.l)+1; i&=255; MMU.wb((Z80._r.h<<8)+Z80._r.l,i); Z80._ops.fz(i); Z80._r.m=3; Z80._r.t=12; },

        DECr_b: function() { Z80._r.b--; Z80._r.b&=255; Z80._ops.fz(Z80._r.b); Z80._r.m=1; Z80._r.t=4; },
        DECr_c: function() { Z80._r.c--; Z80._r.c&=255; Z80._ops.fz(Z80._r.c); Z80._r.m=1; Z80._r.t=4; },
        DECr_d: function() { Z80._r.d--; Z80._r.d&=255; Z80._ops.fz(Z80._r.d); Z80._r.m=1; Z80._r.t=4; },
        DECr_e: function() { Z80._r.e--; Z80._r.e&=255; Z80._ops.fz(Z80._r.e); Z80._r.m=1; Z80._r.t=4; },
        DECr_h: function() { Z80._r.h--; Z80._r.h&=255; Z80._ops.fz(Z80._r.h); Z80._r.m=1; Z80._r.t=4; },
        DECr_l: function() { Z80._r.l--; Z80._r.l&=255; Z80._ops.fz(Z80._r.l); Z80._r.m=1; Z80._r.t=4; },
        DECr_a: function() { Z80._r.a--; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.m=1; Z80._r.t=4; },
        DECHLm: function() { var i=MMU.rb((Z80._r.h<<8)+Z80._r.l)-1; i&=255; MMU.wb((Z80._r.h<<8)+Z80._r.l,i); Z80._ops.fz(i); Z80._r.m=3; Z80._r.t=12; },

        INCBC: function() { Z80._r.c=(Z80._r.c+1)&255; if(!Z80._r.c) Z80._r.b=(Z80._r.b+1)&255; Z80._r.m=1; Z80._r.t=4; },
        INCDE: function() { Z80._r.e=(Z80._r.e+1)&255; if(!Z80._r.e) Z80._r.d=(Z80._r.d+1)&255; Z80._r.m=1; Z80._r.t=4; },
        INCHL: function() { Z80._r.l=(Z80._r.l+1)&255; if(!Z80._r.l) Z80._r.h=(Z80._r.h+1)&255; Z80._r.m=1; Z80._r.t=4; },
        INCSP: function() { Z80._r.sp=(Z80._r.sp+1)&65535; Z80._r.m=1; Z80._r.t=4; },

        DECBC: function() { Z80._r.c=(Z80._r.c-1)&255; if(Z80._r.c==255) Z80._r.b=(Z80._r.b-1)&255; Z80._r.m=1; Z80._r.t=4; },
        DECDE: function() { Z80._r.e=(Z80._r.e-1)&255; if(Z80._r.e==255) Z80._r.d=(Z80._r.d-1)&255; Z80._r.m=1; Z80._r.t=4; },
        DECHL: function() { Z80._r.l=(Z80._r.l-1)&255; if(Z80._r.l==255) Z80._r.h=(Z80._r.h-1)&255; Z80._r.m=1; Z80._r.t=4; },
        DECSP: function() { Z80._r.sp=(Z80._r.sp-1)&65535; Z80._r.m=1; Z80._r.t=4; },

        BIT0b: function() { Z80._ops.fz(Z80._r.b&0x01); Z80._r.m=2; Z80._r.t=8; },
        BIT0c: function() { Z80._ops.fz(Z80._r.c&0x01); Z80._r.m=2; Z80._r.t=8; },
        BIT0d: function() { Z80._ops.fz(Z80._r.d&0x01); Z80._r.m=2; Z80._r.t=8; },
        BIT0e: function() { Z80._ops.fz(Z80._r.e&0x01); Z80._r.m=2; Z80._r.t=8; },
        BIT0h: function() { Z80._ops.fz(Z80._r.h&0x01); Z80._r.m=2; Z80._r.t=8; },
        BIT0l: function() { Z80._ops.fz(Z80._r.l&0x01); Z80._r.m=2; Z80._r.t=8; },
        BIT0a: function() { Z80._ops.fz(Z80._r.a&0x01); Z80._r.m=2; Z80._r.t=8; },
        BIT0m: function() { Z80._ops.fz(MMU.rb((Z80._r.h<<8)+Z80._r.l)&0x01); Z80._r.m=3; Z80._r.t=12; },

        BIT1b: function() { Z80._ops.fz(Z80._r.b&0x02); Z80._r.m=2; Z80._r.t=8; },
        BIT1c: function() { Z80._ops.fz(Z80._r.c&0x02); Z80._r.m=2; Z80._r.t=8; },
        BIT1d: function() { Z80._ops.fz(Z80._r.d&0x02); Z80._r.m=2; Z80._r.t=8; },
        BIT1e: function() { Z80._ops.fz(Z80._r.e&0x02); Z80._r.m=2; Z80._r.t=8; },
        BIT1h: function() { Z80._ops.fz(Z80._r.h&0x02); Z80._r.m=2; Z80._r.t=8; },
        BIT1l: function() { Z80._ops.fz(Z80._r.l&0x02); Z80._r.m=2; Z80._r.t=8; },
        BIT1a: function() { Z80._ops.fz(Z80._r.a&0x02); Z80._r.m=2; Z80._r.t=8; },
        BIT1m: function() { Z80._ops.fz(MMU.rb((Z80._r.h<<8)+Z80._r.l)&0x02); Z80._r.m=3; Z80._r.t=12; },

        BIT2b: function() { Z80._ops.fz(Z80._r.b&0x04); Z80._r.m=2; Z80._r.t=8; },
        BIT2c: function() { Z80._ops.fz(Z80._r.c&0x04); Z80._r.m=2; Z80._r.t=8; },
        BIT2d: function() { Z80._ops.fz(Z80._r.d&0x04); Z80._r.m=2; Z80._r.t=8; },
        BIT2e: function() { Z80._ops.fz(Z80._r.e&0x04); Z80._r.m=2; Z80._r.t=8; },
        BIT2h: function() { Z80._ops.fz(Z80._r.h&0x04); Z80._r.m=2; Z80._r.t=8; },
        BIT2l: function() { Z80._ops.fz(Z80._r.l&0x04); Z80._r.m=2; Z80._r.t=8; },
        BIT2a: function() { Z80._ops.fz(Z80._r.a&0x04); Z80._r.m=2; Z80._r.t=8; },
        BIT2m: function() { Z80._ops.fz(MMU.rb((Z80._r.h<<8)+Z80._r.l)&0x04); Z80._r.m=3; Z80._r.t=12; },

        BIT3b: function() { Z80._ops.fz(Z80._r.b&0x08); Z80._r.m=2; Z80._r.t=8; },
        BIT3c: function() { Z80._ops.fz(Z80._r.c&0x08); Z80._r.m=2; Z80._r.t=8; },
        BIT3d: function() { Z80._ops.fz(Z80._r.d&0x08); Z80._r.m=2; Z80._r.t=8; },
        BIT3e: function() { Z80._ops.fz(Z80._r.e&0x08); Z80._r.m=2; Z80._r.t=8; },
        BIT3h: function() { Z80._ops.fz(Z80._r.h&0x08); Z80._r.m=2; Z80._r.t=8; },
        BIT3l: function() { Z80._ops.fz(Z80._r.l&0x08); Z80._r.m=2; Z80._r.t=8; },
        BIT3a: function() { Z80._ops.fz(Z80._r.a&0x08); Z80._r.m=2; Z80._r.t=8; },
        BIT3m: function() { Z80._ops.fz(MMU.rb((Z80._r.h<<8)+Z80._r.l)&0x08); Z80._r.m=3; Z80._r.t=12; },

        BIT4b: function() { Z80._ops.fz(Z80._r.b&0x10); Z80._r.m=2; Z80._r.t=8; },
        BIT4c: function() { Z80._ops.fz(Z80._r.c&0x10); Z80._r.m=2; Z80._r.t=8; },
        BIT4d: function() { Z80._ops.fz(Z80._r.d&0x10); Z80._r.m=2; Z80._r.t=8; },
        BIT4e: function() { Z80._ops.fz(Z80._r.e&0x10); Z80._r.m=2; Z80._r.t=8; },
        BIT4h: function() { Z80._ops.fz(Z80._r.h&0x10); Z80._r.m=2; Z80._r.t=8; },
        BIT4l: function() { Z80._ops.fz(Z80._r.l&0x10); Z80._r.m=2; Z80._r.t=8; },
        BIT4a: function() { Z80._ops.fz(Z80._r.a&0x10); Z80._r.m=2; Z80._r.t=8; },
        BIT4m: function() { Z80._ops.fz(MMU.rb((Z80._r.h<<8)+Z80._r.l)&0x10); Z80._r.m=3; Z80._r.t=12; },

        BIT5b: function() { Z80._ops.fz(Z80._r.b&0x20); Z80._r.m=2; Z80._r.t=8; },
        BIT5c: function() { Z80._ops.fz(Z80._r.c&0x20); Z80._r.m=2; Z80._r.t=8; },
        BIT5d: function() { Z80._ops.fz(Z80._r.d&0x20); Z80._r.m=2; Z80._r.t=8; },
        BIT5e: function() { Z80._ops.fz(Z80._r.e&0x20); Z80._r.m=2; Z80._r.t=8; },
        BIT5h: function() { Z80._ops.fz(Z80._r.h&0x20); Z80._r.m=2; Z80._r.t=8; },
        BIT5l: function() { Z80._ops.fz(Z80._r.l&0x20); Z80._r.m=2; Z80._r.t=8; },
        BIT5a: function() { Z80._ops.fz(Z80._r.a&0x20); Z80._r.m=2; Z80._r.t=8; },
        BIT5m: function() { Z80._ops.fz(MMU.rb((Z80._r.h<<8)+Z80._r.l)&0x20); Z80._r.m=3; Z80._r.t=12; },

        BIT6b: function() { Z80._ops.fz(Z80._r.b&0x40); Z80._r.m=2; Z80._r.t=8; },
        BIT6c: function() { Z80._ops.fz(Z80._r.c&0x40); Z80._r.m=2; Z80._r.t=8; },
        BIT6d: function() { Z80._ops.fz(Z80._r.d&0x40); Z80._r.m=2; Z80._r.t=8; },
        BIT6e: function() { Z80._ops.fz(Z80._r.e&0x40); Z80._r.m=2; Z80._r.t=8; },
        BIT6h: function() { Z80._ops.fz(Z80._r.h&0x40); Z80._r.m=2; Z80._r.t=8; },
        BIT6l: function() { Z80._ops.fz(Z80._r.l&0x40); Z80._r.m=2; Z80._r.t=8; },
        BIT6a: function() { Z80._ops.fz(Z80._r.a&0x40); Z80._r.m=2; Z80._r.t=8; },
        BIT6m: function() { Z80._ops.fz(MMU.rb((Z80._r.h<<8)+Z80._r.l)&0x40); Z80._r.m=3; Z80._r.t=12; },

        BIT7b: function() { Z80._ops.fz(Z80._r.b&0x80); Z80._r.m=2; Z80._r.t=8; },
        BIT7c: function() { Z80._ops.fz(Z80._r.c&0x80); Z80._r.m=2; Z80._r.t=8; },
        BIT7d: function() { Z80._ops.fz(Z80._r.d&0x80); Z80._r.m=2; Z80._r.t=8; },
        BIT7e: function() { Z80._ops.fz(Z80._r.e&0x80); Z80._r.m=2; Z80._r.t=8; },
        BIT7h: function() { Z80._ops.fz(Z80._r.h&0x80); Z80._r.m=2; Z80._r.t=8; },
        BIT7l: function() { Z80._ops.fz(Z80._r.l&0x80); Z80._r.m=2; Z80._r.t=8; },
        BIT7a: function() { Z80._ops.fz(Z80._r.a&0x80); Z80._r.m=2; Z80._r.t=8; },
        BIT7m: function() { Z80._ops.fz(MMU.rb((Z80._r.h<<8)+Z80._r.l)&0x80); Z80._r.m=3; Z80._r.t=12; },

        RLA: function() { var ci=Z80._r.f&0x10?1:0; var co=Z80._r.a&0x80?0x10:0; Z80._r.a=(Z80._r.a<<1)+ci; Z80._r.a&=255; Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=1; Z80._r.t=4; },
        RLCA: function() { var ci=Z80._r.a&0x80?1:0; var co=Z80._r.a&0x80?0x10:0; Z80._r.a=(Z80._r.a<<1)+ci; Z80._r.a&=255; Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=1; Z80._r.t=4; },
        RRA: function() { var ci=Z80._r.f&0x10?0x80:0; var co=Z80._r.a&1?0x10:0; Z80._r.a=(Z80._r.a>>1)+ci; Z80._r.a&=255; Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=1; Z80._r.t=4; },
        RRCA: function() { var ci=Z80._r.a&1?0x80:0; var co=Z80._r.a&1?0x10:0; Z80._r.a=(Z80._r.a>>1)+ci; Z80._r.a&=255; Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=1; Z80._r.t=4; },

        RLr_b: function() { var ci=Z80._r.f&0x10?1:0; var co=Z80._r.b&0x80?0x10:0; Z80._r.b=(Z80._r.b<<1)+ci; Z80._r.b&=255; Z80._ops.fz(Z80._r.b); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RLr_c: function() { var ci=Z80._r.f&0x10?1:0; var co=Z80._r.c&0x80?0x10:0; Z80._r.c=(Z80._r.c<<1)+ci; Z80._r.c&=255; Z80._ops.fz(Z80._r.c); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RLr_d: function() { var ci=Z80._r.f&0x10?1:0; var co=Z80._r.d&0x80?0x10:0; Z80._r.d=(Z80._r.d<<1)+ci; Z80._r.d&=255; Z80._ops.fz(Z80._r.d); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RLr_e: function() { var ci=Z80._r.f&0x10?1:0; var co=Z80._r.e&0x80?0x10:0; Z80._r.e=(Z80._r.e<<1)+ci; Z80._r.e&=255; Z80._ops.fz(Z80._r.e); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RLr_h: function() { var ci=Z80._r.f&0x10?1:0; var co=Z80._r.h&0x80?0x10:0; Z80._r.h=(Z80._r.h<<1)+ci; Z80._r.h&=255; Z80._ops.fz(Z80._r.h); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RLr_l: function() { var ci=Z80._r.f&0x10?1:0; var co=Z80._r.l&0x80?0x10:0; Z80._r.l=(Z80._r.l<<1)+ci; Z80._r.l&=255; Z80._ops.fz(Z80._r.l); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RLr_a: function() { var ci=Z80._r.f&0x10?1:0; var co=Z80._r.a&0x80?0x10:0; Z80._r.a=(Z80._r.a<<1)+ci; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RLHL: function() { var i=MMU.rb((Z80._r.h<<8)+Z80._r.l); var ci=Z80._r.f&0x10?1:0; var co=i&0x80?0x10:0; i=(i<<1)+ci; i&=255; Z80._ops.fz(i); MMU.wb((Z80._r.h<<8)+Z80._r.l,i); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=4; Z80._r.t=16; },

        RLCr_b: function() { var ci=Z80._r.b&0x80?1:0; var co=Z80._r.b&0x80?0x10:0; Z80._r.b=(Z80._r.b<<1)+ci; Z80._r.b&=255; Z80._ops.fz(Z80._r.b); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RLCr_c: function() { var ci=Z80._r.c&0x80?1:0; var co=Z80._r.c&0x80?0x10:0; Z80._r.c=(Z80._r.c<<1)+ci; Z80._r.c&=255; Z80._ops.fz(Z80._r.c); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RLCr_d: function() { var ci=Z80._r.d&0x80?1:0; var co=Z80._r.d&0x80?0x10:0; Z80._r.d=(Z80._r.d<<1)+ci; Z80._r.d&=255; Z80._ops.fz(Z80._r.d); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RLCr_e: function() { var ci=Z80._r.e&0x80?1:0; var co=Z80._r.e&0x80?0x10:0; Z80._r.e=(Z80._r.e<<1)+ci; Z80._r.e&=255; Z80._ops.fz(Z80._r.e); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RLCr_h: function() { var ci=Z80._r.h&0x80?1:0; var co=Z80._r.h&0x80?0x10:0; Z80._r.h=(Z80._r.h<<1)+ci; Z80._r.h&=255; Z80._ops.fz(Z80._r.h); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RLCr_l: function() { var ci=Z80._r.l&0x80?1:0; var co=Z80._r.l&0x80?0x10:0; Z80._r.l=(Z80._r.l<<1)+ci; Z80._r.l&=255; Z80._ops.fz(Z80._r.l); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RLCr_a: function() { var ci=Z80._r.a&0x80?1:0; var co=Z80._r.a&0x80?0x10:0; Z80._r.a=(Z80._r.a<<1)+ci; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RLCHL: function() { var i=MMU.rb((Z80._r.h<<8)+Z80._r.l); var ci=i&0x80?1:0; var co=i&0x80?0x10:0; i=(i<<1)+ci; i&=255; Z80._ops.fz(i); MMU.wb((Z80._r.h<<8)+Z80._r.l,i); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=4; Z80._r.t=16; },

        RRr_b: function() { var ci=Z80._r.f&0x10?0x80:0; var co=Z80._r.b&1?0x10:0; Z80._r.b=(Z80._r.b>>1)+ci; Z80._r.b&=255; Z80._ops.fz(Z80._r.b); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RRr_c: function() { var ci=Z80._r.f&0x10?0x80:0; var co=Z80._r.c&1?0x10:0; Z80._r.c=(Z80._r.c>>1)+ci; Z80._r.c&=255; Z80._ops.fz(Z80._r.c); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RRr_d: function() { var ci=Z80._r.f&0x10?0x80:0; var co=Z80._r.d&1?0x10:0; Z80._r.d=(Z80._r.d>>1)+ci; Z80._r.d&=255; Z80._ops.fz(Z80._r.d); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RRr_e: function() { var ci=Z80._r.f&0x10?0x80:0; var co=Z80._r.e&1?0x10:0; Z80._r.e=(Z80._r.e>>1)+ci; Z80._r.e&=255; Z80._ops.fz(Z80._r.e); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RRr_h: function() { var ci=Z80._r.f&0x10?0x80:0; var co=Z80._r.h&1?0x10:0; Z80._r.h=(Z80._r.h>>1)+ci; Z80._r.h&=255; Z80._ops.fz(Z80._r.h); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RRr_l: function() { var ci=Z80._r.f&0x10?0x80:0; var co=Z80._r.l&1?0x10:0; Z80._r.l=(Z80._r.l>>1)+ci; Z80._r.l&=255; Z80._ops.fz(Z80._r.l); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RRr_a: function() { var ci=Z80._r.f&0x10?0x80:0; var co=Z80._r.a&1?0x10:0; Z80._r.a=(Z80._r.a>>1)+ci; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RRHL: function() { var i=MMU.rb((Z80._r.h<<8)+Z80._r.l); var ci=Z80._r.f&0x10?0x80:0; var co=i&1?0x10:0; i=(i>>1)+ci; i&=255; MMU.wb((Z80._r.h<<8)+Z80._r.l,i); Z80._ops.fz(i); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=4; Z80._r.t=16; },

        RRCr_b: function() { var ci=Z80._r.b&1?0x80:0; var co=Z80._r.b&1?0x10:0; Z80._r.b=(Z80._r.b>>1)+ci; Z80._r.b&=255; Z80._ops.fz(Z80._r.b); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RRCr_c: function() { var ci=Z80._r.c&1?0x80:0; var co=Z80._r.c&1?0x10:0; Z80._r.c=(Z80._r.c>>1)+ci; Z80._r.c&=255; Z80._ops.fz(Z80._r.c); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RRCr_d: function() { var ci=Z80._r.d&1?0x80:0; var co=Z80._r.d&1?0x10:0; Z80._r.d=(Z80._r.d>>1)+ci; Z80._r.d&=255; Z80._ops.fz(Z80._r.d); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RRCr_e: function() { var ci=Z80._r.e&1?0x80:0; var co=Z80._r.e&1?0x10:0; Z80._r.e=(Z80._r.e>>1)+ci; Z80._r.e&=255; Z80._ops.fz(Z80._r.e); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RRCr_h: function() { var ci=Z80._r.h&1?0x80:0; var co=Z80._r.h&1?0x10:0; Z80._r.h=(Z80._r.h>>1)+ci; Z80._r.h&=255; Z80._ops.fz(Z80._r.h); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RRCr_l: function() { var ci=Z80._r.l&1?0x80:0; var co=Z80._r.l&1?0x10:0; Z80._r.l=(Z80._r.l>>1)+ci; Z80._r.l&=255; Z80._ops.fz(Z80._r.l); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RRCr_a: function() { var ci=Z80._r.a&1?0x80:0; var co=Z80._r.a&1?0x10:0; Z80._r.a=(Z80._r.a>>1)+ci; Z80._r.a&=255; Z80._ops.fz(Z80._r.a); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        RRCHL: function() { var i=MMU.rb((Z80._r.h<<8)+Z80._r.l); var ci=i&1?0x80:0; var co=i&1?0x10:0; i=(i>>1)+ci; i&=255; MMU.wb((Z80._r.h<<8)+Z80._r.l,i); Z80._ops.fz(i); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=4; Z80._r.t=16; },

        SLAr_b: function() { var co=Z80._r.b&0x80?0x10:0; Z80._r.b=(Z80._r.b<<1)&255; Z80._ops.fz(Z80._r.b); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SLAr_c: function() { var co=Z80._r.c&0x80?0x10:0; Z80._r.c=(Z80._r.c<<1)&255; Z80._ops.fz(Z80._r.c); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SLAr_d: function() { var co=Z80._r.d&0x80?0x10:0; Z80._r.d=(Z80._r.d<<1)&255; Z80._ops.fz(Z80._r.d); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SLAr_e: function() { var co=Z80._r.e&0x80?0x10:0; Z80._r.e=(Z80._r.e<<1)&255; Z80._ops.fz(Z80._r.e); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SLAr_h: function() { var co=Z80._r.h&0x80?0x10:0; Z80._r.h=(Z80._r.h<<1)&255; Z80._ops.fz(Z80._r.h); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SLAr_l: function() { var co=Z80._r.l&0x80?0x10:0; Z80._r.l=(Z80._r.l<<1)&255; Z80._ops.fz(Z80._r.l); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SLAr_a: function() { var co=Z80._r.a&0x80?0x10:0; Z80._r.a=(Z80._r.a<<1)&255; Z80._ops.fz(Z80._r.a); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },

        SLLr_b: function() { var co=Z80._r.b&0x80?0x10:0; Z80._r.b=(Z80._r.b<<1)&255+1; Z80._ops.fz(Z80._r.b); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SLLr_c: function() { var co=Z80._r.c&0x80?0x10:0; Z80._r.c=(Z80._r.c<<1)&255+1; Z80._ops.fz(Z80._r.c); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SLLr_d: function() { var co=Z80._r.d&0x80?0x10:0; Z80._r.d=(Z80._r.d<<1)&255+1; Z80._ops.fz(Z80._r.d); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SLLr_e: function() { var co=Z80._r.e&0x80?0x10:0; Z80._r.e=(Z80._r.e<<1)&255+1; Z80._ops.fz(Z80._r.e); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SLLr_h: function() { var co=Z80._r.h&0x80?0x10:0; Z80._r.h=(Z80._r.h<<1)&255+1; Z80._ops.fz(Z80._r.h); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SLLr_l: function() { var co=Z80._r.l&0x80?0x10:0; Z80._r.l=(Z80._r.l<<1)&255+1; Z80._ops.fz(Z80._r.l); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SLLr_a: function() { var co=Z80._r.a&0x80?0x10:0; Z80._r.a=(Z80._r.a<<1)&255+1; Z80._ops.fz(Z80._r.a); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },

        SRAr_b: function() { var ci=Z80._r.b&0x80; var co=Z80._r.b&1?0x10:0; Z80._r.b=((Z80._r.b>>1)+ci)&255; Z80._ops.fz(Z80._r.b); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SRAr_c: function() { var ci=Z80._r.c&0x80; var co=Z80._r.c&1?0x10:0; Z80._r.c=((Z80._r.c>>1)+ci)&255; Z80._ops.fz(Z80._r.c); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SRAr_d: function() { var ci=Z80._r.d&0x80; var co=Z80._r.d&1?0x10:0; Z80._r.d=((Z80._r.d>>1)+ci)&255; Z80._ops.fz(Z80._r.d); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SRAr_e: function() { var ci=Z80._r.e&0x80; var co=Z80._r.e&1?0x10:0; Z80._r.e=((Z80._r.e>>1)+ci)&255; Z80._ops.fz(Z80._r.e); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SRAr_h: function() { var ci=Z80._r.h&0x80; var co=Z80._r.h&1?0x10:0; Z80._r.h=((Z80._r.h>>1)+ci)&255; Z80._ops.fz(Z80._r.h); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SRAr_l: function() { var ci=Z80._r.l&0x80; var co=Z80._r.l&1?0x10:0; Z80._r.l=((Z80._r.l>>1)+ci)&255; Z80._ops.fz(Z80._r.l); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SRAr_a: function() { var ci=Z80._r.a&0x80; var co=Z80._r.a&1?0x10:0; Z80._r.a=((Z80._r.a>>1)+ci)&255; Z80._ops.fz(Z80._r.a); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },

        SRLr_b: function() { var co=Z80._r.b&1?0x10:0; Z80._r.b=(Z80._r.b>>1)&255; Z80._ops.fz(Z80._r.b); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SRLr_c: function() { var co=Z80._r.c&1?0x10:0; Z80._r.c=(Z80._r.c>>1)&255; Z80._ops.fz(Z80._r.c); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SRLr_d: function() { var co=Z80._r.d&1?0x10:0; Z80._r.d=(Z80._r.d>>1)&255; Z80._ops.fz(Z80._r.d); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SRLr_e: function() { var co=Z80._r.e&1?0x10:0; Z80._r.e=(Z80._r.e>>1)&255; Z80._ops.fz(Z80._r.e); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SRLr_h: function() { var co=Z80._r.h&1?0x10:0; Z80._r.h=(Z80._r.h>>1)&255; Z80._ops.fz(Z80._r.h); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SRLr_l: function() { var co=Z80._r.l&1?0x10:0; Z80._r.l=(Z80._r.l>>1)&255; Z80._ops.fz(Z80._r.l); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },
        SRLr_a: function() { var co=Z80._r.a&1?0x10:0; Z80._r.a=(Z80._r.a>>1)&255; Z80._ops.fz(Z80._r.a); Z80._r.f=(Z80._r.f&0xEF)+co; Z80._r.m=2; Z80._r.t=8; },

        CPL: function() { Z80._r.a = (~Z80._r.a)&255; Z80._ops.fz(Z80._r.a,1); Z80._r.m=1; Z80._r.t=4; },
        NEG: function() { Z80._r.a=0-Z80._r.a; Z80._ops.fz(Z80._r.a,1); if(Z80._r.a<0) Z80._r.f|=0x10; Z80._r.a&=255; Z80._r.m=2; Z80._r.t=8; },

        CCF: function() { var ci=Z80._r.f&0x10?0:0x10; Z80._r.f=(Z80._r.f&0xEF)+ci; Z80._r.m=1; Z80._r.t=4; },
        SCF: function() { Z80._r.f|=0x10; Z80._r.m=1; Z80._r.t=4; },

        PUSHBC: function() { Z80._r.sp--; MMU.wb(Z80._r.sp,Z80._r.b); Z80._r.sp--; MMU.wb(Z80._r.sp,Z80._r.c); Z80._r.m=3; Z80._r.t=12; },
        PUSHDE: function() { Z80._r.sp--; MMU.wb(Z80._r.sp,Z80._r.d); Z80._r.sp--; MMU.wb(Z80._r.sp,Z80._r.e); Z80._r.m=3; Z80._r.t=12; },
        PUSHHL: function() { Z80._r.sp--; MMU.wb(Z80._r.sp,Z80._r.h); Z80._r.sp--; MMU.wb(Z80._r.sp,Z80._r.l); Z80._r.m=3; Z80._r.t=12; },
        PUSHAF: function() { Z80._r.sp--; MMU.wb(Z80._r.sp,Z80._r.a); Z80._r.sp--; MMU.wb(Z80._r.sp,Z80._r.f); Z80._r.m=3; Z80._r.t=12; },

        POPBC: function() { Z80._r.c=MMU.rb(Z80._r.sp); Z80._r.sp++; Z80._r.b=MMU.rb(Z80._r.sp); Z80._r.sp++; Z80._r.m=3; Z80._r.t=12; },
        POPDE: function() { Z80._r.e=MMU.rb(Z80._r.sp); Z80._r.sp++; Z80._r.d=MMU.rb(Z80._r.sp); Z80._r.sp++; Z80._r.m=3; Z80._r.t=12; },
        POPHL: function() { Z80._r.l=MMU.rb(Z80._r.sp); Z80._r.sp++; Z80._r.h=MMU.rb(Z80._r.sp); Z80._r.sp++; Z80._r.m=3; Z80._r.t=12; },
        POPAF: function() { Z80._r.f=MMU.rb(Z80._r.sp); Z80._r.sp++; Z80._r.a=MMU.rb(Z80._r.sp); Z80._r.sp++; Z80._r.m=3; Z80._r.t=12; },

        JPnn: function() { Z80._r.pc = MMU.rw(Z80._r.pc); Z80._r.m=3; Z80._r.t=12; },
        JPHL: function() { Z80._r.pc=Z80._r.hl; Z80._r.m=1; Z80._r.t=4; },
        JPNZnn: function() { Z80._r.m=3; Z80._r.t=12; if((Z80._r.f&0x80)==0x00) { Z80._r.pc=MMU.rw(Z80._r.pc); Z80._r.m++; Z80._r.t+=4; } else Z80._r.pc+=2; },
        JPZnn: function()  { Z80._r.m=3; Z80._r.t=12; if((Z80._r.f&0x80)==0x80) { Z80._r.pc=MMU.rw(Z80._r.pc); Z80._r.m++; Z80._r.t+=4; } else Z80._r.pc+=2; },
        JPNCnn: function() { Z80._r.m=3; Z80._r.t=12; if((Z80._r.f&0x10)==0x00) { Z80._r.pc=MMU.rw(Z80._r.pc); Z80._r.m++; Z80._r.t+=4; } else Z80._r.pc+=2; },
        JPCnn: function()  { Z80._r.m=3; Z80._r.t=12; if((Z80._r.f&0x10)==0x10) { Z80._r.pc=MMU.rw(Z80._r.pc); Z80._r.m++; Z80._r.t+=4; } else Z80._r.pc+=2; },

        JRn: function() { var i=MMU.rb(Z80._r.pc); if(i>127) i=-((~i+1)&255); Z80._r.pc++; Z80._r.m=2; Z80._r.t=8; Z80._r.pc+=i; Z80._r.m++; Z80._r.t+=4; },
        JRNZn: function() { var i=MMU.rb(Z80._r.pc); if(i>127) i=-((~i+1)&255); Z80._r.pc++; Z80._r.m=2; Z80._r.t=8; if((Z80._r.f&0x80)==0x00) { Z80._r.pc+=i; Z80._r.m++; Z80._r.t+=4; } },
        JRZn: function()  { var i=MMU.rb(Z80._r.pc); if(i>127) i=-((~i+1)&255); Z80._r.pc++; Z80._r.m=2; Z80._r.t=8; if((Z80._r.f&0x80)==0x80) { Z80._r.pc+=i; Z80._r.m++; Z80._r.t+=4; } },
        JRNCn: function() { var i=MMU.rb(Z80._r.pc); if(i>127) i=-((~i+1)&255); Z80._r.pc++; Z80._r.m=2; Z80._r.t=8; if((Z80._r.f&0x10)==0x00) { Z80._r.pc+=i; Z80._r.m++; Z80._r.t+=4; } },
        JRCn: function()  { var i=MMU.rb(Z80._r.pc); if(i>127) i=-((~i+1)&255); Z80._r.pc++; Z80._r.m=2; Z80._r.t=8; if((Z80._r.f&0x10)==0x10) { Z80._r.pc+=i; Z80._r.m++; Z80._r.t+=4; } },

        DJNZn: function() { var i=MMU.rb(Z80._r.pc); if(i>127) i=-((~i+1)&255); Z80._r.pc++; Z80._r.m=2; Z80._r.t=8; Z80._r.b--; if(Z80._r.b) { Z80._r.pc+=i; Z80._r.m++; Z80._r.t+=4; } },

        CALLnn: function() { Z80._r.sp-=2; MMU.ww(Z80._r.sp,Z80._r.pc+2); Z80._r.pc=MMU.rw(Z80._r.pc); Z80._r.m=5; Z80._r.t=20; },
        CALLNZnn: function() { Z80._r.m=3; Z80._r.t=12; if((Z80._r.f&0x80)==0x00) { Z80._r.sp-=2; MMU.ww(Z80._r.sp,Z80._r.pc+2); Z80._r.pc=MMU.rw(Z80._r.pc); Z80._r.m+=2; Z80._r.t+=8; } else Z80._r.pc+=2; },
        CALLZnn: function() { Z80._r.m=3; Z80._r.t=12; if((Z80._r.f&0x80)==0x80) { Z80._r.sp-=2; MMU.ww(Z80._r.sp,Z80._r.pc+2); Z80._r.pc=MMU.rw(Z80._r.pc); Z80._r.m+=2; Z80._r.t+=8; } else Z80._r.pc+=2; },
        CALLNCnn: function() { Z80._r.m=3; Z80._r.t=12; if((Z80._r.f&0x10)==0x00) { Z80._r.sp-=2; MMU.ww(Z80._r.sp,Z80._r.pc+2); Z80._r.pc=MMU.rw(Z80._r.pc); Z80._r.m+=2; Z80._r.t+=8; } else Z80._r.pc+=2; },
        CALLCnn: function() { Z80._r.m=3; Z80._r.t=12; if((Z80._r.f&0x10)==0x10) { Z80._r.sp-=2; MMU.ww(Z80._r.sp,Z80._r.pc+2); Z80._r.pc=MMU.rw(Z80._r.pc); Z80._r.m+=2; Z80._r.t+=8; } else Z80._r.pc+=2; },

        RET: function() { Z80._r.pc=MMU.rw(Z80._r.sp); Z80._r.sp+=2; Z80._r.m=3; Z80._r.t=12; },
        RETI: function() { Z80._r.ime=1; Z80._r.pc=MMU.rw(Z80._r.sp); Z80._r.sp+=2; Z80._r.m=3; Z80._r.t=12; },
        RETNZ: function() { Z80._r.m=1; Z80._r.t=4; if((Z80._r.f&0x80)==0x00) { Z80._r.pc=MMU.rw(Z80._r.sp); Z80._r.sp+=2; Z80._r.m+=2; Z80._r.t+=8; } },
        RETZ: function() { Z80._r.m=1; Z80._r.t=4; if((Z80._r.f&0x80)==0x80) { Z80._r.pc=MMU.rw(Z80._r.sp); Z80._r.sp+=2; Z80._r.m+=2; Z80._r.t+=8; } },
        RETNC: function() { Z80._r.m=1; Z80._r.t=4; if((Z80._r.f&0x10)==0x00) { Z80._r.pc=MMU.rw(Z80._r.sp); Z80._r.sp+=2; Z80._r.m+=2; Z80._r.t+=8; } },
        RETC: function() { Z80._r.m=1; Z80._r.t=4; if((Z80._r.f&0x10)==0x10) { Z80._r.pc=MMU.rw(Z80._r.sp); Z80._r.sp+=2; Z80._r.m+=2; Z80._r.t+=8; } },

        RST00: function() { Z80._r.sp-=2; MMU.ww(Z80._r.sp,Z80._r.pc); Z80._r.pc=0x00; Z80._r.m=3; Z80._r.t=12; },
        RST08: function() { Z80._r.sp-=2; MMU.ww(Z80._r.sp,Z80._r.pc); Z80._r.pc=0x08; Z80._r.m=3; Z80._r.t=12; },
        RST10: function() { Z80._r.sp-=2; MMU.ww(Z80._r.sp,Z80._r.pc); Z80._r.pc=0x10; Z80._r.m=3; Z80._r.t=12; },
        RST18: function() { Z80._r.sp-=2; MMU.ww(Z80._r.sp,Z80._r.pc); Z80._r.pc=0x18; Z80._r.m=3; Z80._r.t=12; },
        RST20: function() { Z80._r.sp-=2; MMU.ww(Z80._r.sp,Z80._r.pc); Z80._r.pc=0x20; Z80._r.m=3; Z80._r.t=12; },
        RST28: function() { Z80._r.sp-=2; MMU.ww(Z80._r.sp,Z80._r.pc); Z80._r.pc=0x28; Z80._r.m=3; Z80._r.t=12; },
        RST30: function() { Z80._r.sp-=2; MMU.ww(Z80._r.sp,Z80._r.pc); Z80._r.pc=0x30; Z80._r.m=3; Z80._r.t=12; },
        RST38: function() { Z80._r.sp-=2; MMU.ww(Z80._r.sp,Z80._r.pc); Z80._r.pc=0x38; Z80._r.m=3; Z80._r.t=12; },
        RST40: function() { Z80._r.sp-=2; MMU.ww(Z80._r.sp,Z80._r.pc); Z80._r.pc=0x40; Z80._r.m=3; Z80._r.t=12; },
        RST48: function() { Z80._r.sp-=2; MMU.ww(Z80._r.sp,Z80._r.pc); Z80._r.pc=0x48; Z80._r.m=3; Z80._r.t=12; },
        RST50: function() { Z80._r.sp-=2; MMU.ww(Z80._r.sp,Z80._r.pc); Z80._r.pc=0x50; Z80._r.m=3; Z80._r.t=12; },
        RST58: function() { Z80._r.sp-=2; MMU.ww(Z80._r.sp,Z80._r.pc); Z80._r.pc=0x58; Z80._r.m=3; Z80._r.t=12; },
        RST60: function() { Z80._r.sp-=2; MMU.ww(Z80._r.sp,Z80._r.pc); Z80._r.pc=0x60; Z80._r.m=3; Z80._r.t=12; },

        NOP: function() { Z80._r.m=1; Z80._r.t=4; },
        HALT: function() { Z80._halt=1; Z80._r.m=1; Z80._r.t=4; },

        DI: function() { Z80._r.ime=0; Z80._r.m=1; Z80._r.t=4; },
        EI: function() { Z80._r.ime=1; Z80._r.m=1; Z80._r.t=4; },
    */
    }

    void cpu::dumpstate(void) {
        cout << "###################################################" << endl;
        cout << "# CPU State:" << "                                      #" << endl;
        cout << "#                                                 #" << endl;
        cout << "# b: " << hex(regs.b) << " c: " << hex(regs.c) << " d: " << hex(regs.d)
        << " e: " << hex(regs.e) << " h: " << hex(regs.h) << " l: " << hex(regs.l)
        << " #" << endl;
        cout << "# a: " << hex(regs.a) << " f: " << hex(regs.f) << "                                 #" << endl;
        cout << "#                                                 #" << endl;
        cout << "# pc: " << hex(regs.pc) << " sp: " << hex(regs.sp) << "                           #" << endl;
        cout << "#                                                 #" << endl;
        cout << "# clk: " << clk << "                                          #" << endl;
        cout << "###################################################" << endl << endl;
    }
}