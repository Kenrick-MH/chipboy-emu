
#include <core/cpu.h>
#include <core/bus.h>
#include <core/interrupt.h>

/* Note that this is dynamically generated */
#include <optable.h>

/* 
    Singleton object for CPU.
*/
static cpu_context_t cpu_context;

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

void cpu_tick()
{
    /* TODO: Make CPU cycle accurate (have state machines) */

    /* Before executing any instructions, need to check for interrupts */
    if (cpu_context.ime){
        /* 
            Get interrupt type, clear interrupt bit
            If no interrupt then continue normal execution flow.    
        */

        /* Get interrupt vector */

        /* Two NOPS */

        /* LD [SP] PC (Two M-Cycles) */

    
        // If there is an interrupt, service it.
        cpu_context.ime = 0;

        /* Go to the next cycle */

        /* Effect takes place after 5-M cycles*/ 
        return;
    }
    
    
    uint8_t opcode = cpu_fetch();
    INSTR_FUNC op_func = optable[opcode];

    /* Call the op func */
    op_func(&cpu_context, opcode);
}

