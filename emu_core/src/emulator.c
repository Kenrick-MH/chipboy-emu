#include <common.h>

typedef struct emulator_context
{
    uint64_t
} emulator_context_t;




static uint64_t global_tick;

/**
 *  Initializes the emulator
 */
static void emulator_init()
{   
    global_tick = 0;

    /* Initialize devices tick and state */
}


/**
 *  Emulator core loop, this is an infinite 
 *  superloop that advances the global tick by 1 
 *  every iteration of the loop.
 * 
 *  This acts as the central scheduling system for 
 *  every device within the gameboy device. 
 * 
 */
static void emulator_loop()
{
    while(true) 
    {
        /* Tick every device in the emulator */
        
        global_tick++;
    }
}




