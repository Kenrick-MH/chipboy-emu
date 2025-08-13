
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
    interrupt_type_t i_type;
    i_type = interrupt_get_top(cpu_context.ime);

    /* Before executing any instructions, need to check for interrupts */
    if (i_type != INTERRUPT_TYPE_NONE){
        interrupt_clear_flag(i_type);
        cpu_context.ime = 0;
        addr_t i_vector = interrupt_get_vector_addr(i_type);

        /* Two NOPS */

        /* LD [SP] PC (Two M-Cycles) */
        bus_write(--cpu_context.sp, (uint8_t) (cpu_context.pc >> 0x8));
        bus_write(--cpu_context.sp, (uint8_t) cpu_context.pc);

        cpu_context.pc = i_vector;
        cpu_context.cycles += 5;
    
        return;
    }
    
    
    uint8_t opcode = cpu_fetch();
    INSTR_FUNC op_func = optable[opcode];

    /* Call the op func */
    op_func(&cpu_context, opcode);
}

