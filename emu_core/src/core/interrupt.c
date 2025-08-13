#include <core/interrupt.h>

typedef struct interrupt_context {
    uint8_t ie_reg;
    uint8_t if_reg;
} interrupt_context_t;

static interrupt_context_t interrupt_context;

void interrupt_set_flag(interrupt_type_t interrupt_type)
{
    switch (interrupt_type)
    {
        case INTERRUPT_TYPE_VBLANK:
            interrupt_context.if_reg |= INTERRUPT_REG_VBLANK_BITMASK;
            break;

        case INTERRUPT_TYPE_JOYPAD:
            interrupt_context.if_reg |= INTERRUPT_REG_JOYPAD_BITMASK;
            break;

        case INTERRUPT_TYPE_TIMER:
            interrupt_context.if_reg |= INTERRUPT_REG_TIMER_BITMASK;
            break;
        
        case INTERRUPT_TYPE_SERIAL:
            interrupt_context.if_reg |= INTERRUPT_REG_SERIAL_BITMASK;
            break;
        
        default:
            break;
    }    
}

void interrupt_clear_flag(interrupt_type_t interrupt_type)
{
    switch (interrupt_type)
    {
        case INTERRUPT_TYPE_VBLANK:
            interrupt_context.if_reg &= ~INTERRUPT_REG_VBLANK_BITMASK;
            break;

        case INTERRUPT_TYPE_JOYPAD:
            interrupt_context.if_reg &= ~INTERRUPT_REG_JOYPAD_BITMASK;
            break;

        case INTERRUPT_TYPE_TIMER:
            interrupt_context.if_reg &= ~INTERRUPT_REG_TIMER_BITMASK;
            break;
        
        case INTERRUPT_TYPE_SERIAL:
            interrupt_context.if_reg &= ~INTERRUPT_REG_SERIAL_BITMASK;
            break;
        
        default:
            break;
    }    
}

/**
 *  Get the top priority interrupt to service.
 *  The top priority interrupt is the enabled interrupt that has
 *  the lowest bit position
 */
interrupt_type_t interrupt_get_top(uint8_t ime)
{
    uint8_t interrupt_vals = ime ? (interrupt_context.ie_reg & interrupt_context.if_reg) : 0;
    
    /* Eat bits one by one, in the bit order */
    if (interrupt_vals & INTERRUPT_REG_VBLANK_BITMASK) return INTERRUPT_TYPE_VBLANK;
    if (interrupt_vals & INTERRUPT_REG_TIMER_BITMASK) return INTERRUPT_TYPE_TIMER;
    if (interrupt_vals & INTERRUPT_REG_SERIAL_BITMASK) return INTERRUPT_TYPE_SERIAL;
    if (interrupt_vals & INTERRUPT_REG_JOYPAD_BITMASK) return INTERRUPT_TYPE_JOYPAD;
    
    return INTERRUPT_TYPE_NONE;
}

/**
 *  Gets the interrupt vector corresponding to the interrupt type.
 *  Interrupt types can only be of VBLANK, STAT, TIMER, SERIAL or JOYPAD
 */
addr_t interrupt_get_vector_addr(interrupt_type_t interrupt_type)
{
    return interrupt_vector_addrs[interrupt_type];
}