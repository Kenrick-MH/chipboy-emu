#ifndef MASTER_SLAVE_H
#define MASTER_SLAVE_H

#include <common.h>

/**
 *  Interface representing a master/slave connection.
 *  Represented by a read/write request, where
 *  the slave would implement read/write requests to master, 
 *  and the connection would only.
 * 
 *  Slave would implement the read/write functionalities, and
 *  the master would call them to send/recieve data.
 */
typedef struct master_slave_conn
{
    /* Starting address (inclusive) of memory region (absolute address) */
    addr_t start_addr;

    /* Ending address (inclusive) of memory region (absolute address) */
    addr_t end_addr;

    /* Slave implemented functions, reads in master address. */
    uint8_t (*slave_read)(void *context, addr_t addr);
    void (*slave_write)(void *context, addr_t addr, uint8_t value);

} master_slave_conn_t;

#endif // MASTER_SLAVE_H






















