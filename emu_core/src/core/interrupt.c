#include <core/interrupt.h>
#include <master_slave.h>
#include <emu_error.h>

typedef struct interrupt_context {
    uint8_t ie_reg;
    uint8_t if_reg;
    master_slave_conn_t interrupt_ie_ms_conn;
    master_slave_conn_t interrupt_if_ms_conn;
} interrupt_context_t;

static interrupt_context_t interrupt_context;

/**  
 *  Function callbacks for interrupt register bus connections.
*/
static error_code_t ie_read(void *context, addr_t addr, uint8_t *read_val)
{
    assert(addr == 0xFFFF);
    assert(read_val != NULL);

    interrupt_context_t *int_ctx = (interrupt_context_t *) context;
    *read_val = int_ctx->ie_reg;  
    return STATUS_OK;
}

static error_code_t ie_write(void *context, addr_t addr, uint8_t value)
{
    assert(addr == 0xFFFF);
    interrupt_context_t *int_ctx = (interrupt_context_t *) context;
    int_ctx->ie_reg = value;  
    return STATUS_OK;
}


static error_code_t if_read(void *context, addr_t addr, uint8_t *read_val)
{
    assert(addr == 0xFF0F);
    assert(read_val != NULL);

    interrupt_context_t *int_ctx = (interrupt_context_t *) context;
    *read_val = int_ctx->if_reg;  
    return STATUS_OK;
}

static error_code_t if_write(void *context, addr_t addr, uint8_t value)
{
    assert(addr == 0xFF0F);
    interrupt_context_t *int_ctx = (interrupt_context_t *) context;
    int_ctx->if_reg = value;  
    return STATUS_OK;
}


/**
 *  Initializes the interrupt module.
 */
void interrupt_init() {
    /* Enable all interrupts initially */
    interrupt_context.ie_reg = 0b00011111u;

    /* IF is still 0, since this is reserved for device. */
    interrupt_context.if_reg = 0x0u;
    interrupt_context.interrupt_ie_ms_conn = (master_slave_conn_t) {
        .start_addr = (addr_t) 0xFFFFu, 
        .end_addr = (addr_t) 0xFFFFu,   
        .slave_context = (void *) &interrupt_context,
        .slave_read = ie_read,
        .slave_write = ie_write
    };

    interrupt_context.interrupt_if_ms_conn = (master_slave_conn_t) {
        .start_addr = (addr_t) 0xFF0Fu, 
        .end_addr = (addr_t) 0xFF0Fu,   
        .slave_context = (void *) &interrupt_context,
        .slave_read = if_read,
        .slave_write = if_write
    };
}

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

master_slave_conn_t *interrupt_get_ie_ms_connection()
{
    master_slave_conn_t *res = &(interrupt_context.interrupt_ie_ms_conn);
    assert(res->slave_context != NULL);
    assert(res->slave_read != NULL);
    assert(res->slave_write != NULL);

    return res;
}

master_slave_conn_t *interrupt_get_if_ms_connection()
{
    master_slave_conn_t *res = &(interrupt_context.interrupt_if_ms_conn);
    assert(res->slave_context != NULL);
    assert(res->slave_read != NULL);
    assert(res->slave_write != NULL);

    return res;
}