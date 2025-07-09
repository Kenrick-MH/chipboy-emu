
#include <cpu.h>
#include <bus.h>

/* Register Placeholder values */
#define REG_B 

/* Get opcode of 16 bit instruction */
#define GET_OPCODE(instr) ((instr) >> 8)

/* 
    Singleton object for CPU.
*/
static cpu_context_t cpu_context;

/** INSTRUCTIONS, GROUPED BY FUNCTIONALITY */

/**
 *  Corresponds to the instruction `LD reg8 <val>` 
 */
static void mov8();

/**
 *  Corresponds to the instruction `LD reg16 <val>` 
 */
static void mov16();


static void nop()
{
    /* Does nothing */
}

static void jr()
{
    /* Does nothing */
}

/**
 *  Decode instruction
 */
static void cpu_decode(uint8_t opcode)
{

}


/**
 *  Fetch instruction in memory.
 */
inline static uint8_t cpu_fetch()
{
    // Access memory, increment pc by 1
    return bus_read(++cpu_context.pc);
}

void cpu_init()
{
    /* Initialize PC */
    cpu_context.pc = 0x0;
    cpu_context.cycles = 0x0;
}

void cpu_step()
{
    uint8_t opcode = cpu_fetch();
    cpu_decode(opcode);
}

