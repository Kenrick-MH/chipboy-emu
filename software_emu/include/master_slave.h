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

    /* Slave implemented functions */

    /* Master calls the functions below, these wrap around the slave functions */
    
} master_slave_conn_t;

#endif // MASTER_SLAVE_H






















