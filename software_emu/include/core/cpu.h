#ifndef CPU_H
#define CPU_H

#include <common.h>
#include <core/bus.h>


/* Regs */
#define REGHIGH(reg16)              ((uint8_t) (((reg16) >> 8) & 0xff)  )
#define REGLOW(reg16)               ((uint8_t)  ((reg16) & 0xff)        )
#define REGFULL(reg_hi, reg_lo)     ((uint8_t)  ((reg_lo) & 0xff) | (((reg_hi) & 0xff) << 8) )

/**
 *  Register datatype, comes in two forms,
 */
typedef union reg
{
    uint16_t full;
    struct 
    {
        uint8_t hi;
        uint8_t lo;
    };
    
} register_t;

typedef union status_reg
{
    uint16_t full;
    struct 
    {
        uint8_t hi;
        uint8_t lo;
    };
    
} status_register_t;

/**
 *  Data structure to contain execution context 
 *  (registers, sp, lr)
 */
typedef struct cpu_context {
    
    /* Registers */
    register_t af;
    register_t bc;
    register_t de;
    register_t hl;
    uint16_t sp;
    uint16_t pc;

    /* Cycles elapsed for CPU in m_cycles*/
    uint64_t cycles;

    /* 
        Interrupt enable flag. 
    */
    uint8_t ie;

} cpu_context_t;

// Stub, wait to define bus structures
void cpu_init();

/**
 *  Steps over one CPU cycle, i.e. steps over one instruction
 */
void cpu_step();

void cpu_ei();
void cpu_di();

#endif // CPU_H