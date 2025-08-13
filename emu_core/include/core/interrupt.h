#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <common.h>

#define INTERRUPT_REG_VBLANK_BITMASK      0x1
#define INTERRUPT_REG_TIMER_BITMASK       0x2
#define INTERRUPT_REG_SERIAL_BITMASK      0x4
#define INTERRUPT_REG_JOYPAD_BITMASK      0x8

typedef enum interrupt_type {
    INTERRUPT_TYPE_VBLANK,
    INTERRUPT_TYPE_STAT,
    INTERRUPT_TYPE_TIMER,
    INTERRUPT_TYPE_SERIAL,
    INTERRUPT_TYPE_JOYPAD,
    INTERRUPT_TYPE_UNKNOWN, 
    INTERRUPT_TYPE_NONE,
    INTERRUPT_TYPE_COUNT
} interrupt_type_t;

addr_t interrupt_vector_addrs[INTERRUPT_TYPE_COUNT] = {
    [INTERRUPT_TYPE_VBLANK]         = 0x40,
    [INTERRUPT_TYPE_STAT]           = 0x48,
    [INTERRUPT_TYPE_TIMER]          = 0x50,
    [INTERRUPT_TYPE_SERIAL]         = 0x58,
    [INTERRUPT_TYPE_JOYPAD]         = 0x60,
    [INTERRUPT_TYPE_UNKNOWN]        = 0x0,
    [INTERRUPT_TYPE_NONE]           = 0x0
};

/**
 *  Called by devices (PPU, Timer, Serial Comm, or Joypad)
 *  Sets the interrupt bit to HIGH for the interrrupt.
 */
void interrupt_set_flag(interrupt_type_t interrupt_type);


/**
 *  Called by devices (PPU, Timer, Serial Comm, or Joypad)
 *  Clears the interrupt bit to LOW for the interrrupt.
 */
void interrupt_clear_flag(interrupt_type_t interrupt_type);

/**
 *  Get the top priority interrupt to service.
 *  The top priority interrupt is the enabled interrupt that has
 *  the lowest bit position
 */
interrupt_type_t interrupt_get_top(uint8_t ime);

/**
 *  Gets the interrupt vector corresponding to the interrupt type.
 *  Interrupt types can only be of VBLANK, STAT, TIMER, SERIAL or JOYPAD
 */
addr_t interrupt_get_vector_addr(interrupt_type_t interrupt_type);


#endif // INTERRUPT_H