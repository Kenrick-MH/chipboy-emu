#ifndef MASTER_SLAVE_H
#define MASTER_SLAVE_H

#include <common.h>
#include <emu_error.h>

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

    void *slave_context;

    /* 
        Slave implemented functions, reads in master address.
        Slaves register both the context and the function to serve the context.
    */
    error_code_t (*slave_read)(void *context, addr_t addr, uint8_t *read_val);
    error_code_t (*slave_write)(void *context, addr_t addr, uint8_t value);

} master_slave_conn_t;

/**
 *  Master's interface to read from a connection.
 *  
 *  Returns the error code of the read operation,
 *  and also writes `read_val` to contain the
 *  read byte.
 */
error_code_t msconn_master_read(master_slave_conn_t *conn, addr_t addr, uint8_t *read_val);

/**
 *  Master's interface to write to a slave.
 *  
 *  Returns the error code of the write operation.
 */
error_code_t msconn_master_write(master_slave_conn_t *conn, addr_t addr, uint8_t value);

#endif // MASTER_SLAVE_H

