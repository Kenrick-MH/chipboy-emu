
#include <common.h>
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