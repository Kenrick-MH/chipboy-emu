
#include <common.h>
#include <core/interrupt.h>
#include <platform/memory.h>
#include <bus.h>

static bus_context_t bus_context;

/**
 *  Finds a peripheral has addr in its address range.
 */
static master_slave_conn_t *find_peripheral(addr_t addr)
{
    for (int i = 0; i < bus_context.connections_size; ++i){ 
        master_slave_conn_t *conn = bus_context.connections[i];
        if ((conn->start_addr <= addr) && (addr <= conn->end_addr))
        {
            return conn;
        }
    }

    /* Should never happen */
    return NULL;
}


void bus_init()
{
    bus_context.connections_size = MAX_DEVICE_NUMBER;
    /* These must be inserted in order */
    master_slave_conn_t *dev_arr[MAX_DEVICE_NUMBER] = {
        memory_get_vram_ms_connection(),
        memory_get_wram0_ms_connection(),
        memory_get_wram1_ms_connection(),
        memory_get_echo_ms_connection(),
        interrupt_get_ie_ms_connection(),
        memory_get_hram_ms_connection(),
        interrupt_get_if_ms_connection(),       
    };
    
    for (unsigned i = 0; i < MAX_DEVICE_NUMBER; ++i){
        bus_context.connections[i] = dev_arr[i];
    }
}

/*
    Read data from bus.
    Returns:
        - error status
        - `read_result (uint8_t)`:  Read result from bus 
*/
int bus_read(addr_t addr, uint8_t *read_result)
{
    error_code_t error_code;
    master_slave_conn_t *periph_conn = find_peripheral(addr);
    
    /* SEGFAULT */
    if (periph_conn == NULL){
        return -1;
    }

    return msconn_master_read(periph_conn, addr, read_result);
}

/*
    Write data to bus.
    Returns:
        - error status
*/
int bus_write(addr_t addr, uint8_t value)
{
    master_slave_conn_t *periph_conn = find_peripheral(addr);
    /* SEGFAULT */
    if (periph_conn == NULL){
        return -1;
    }

    return msconn_master_write(periph_conn, addr, value);
}