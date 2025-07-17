#ifndef BUS_H
#define BUS_H

/** 
 *  This file is responsible for managing memory reads/
 *  writes in-between devices.
 */

#include <common.h>
#include <master_slave.h>
#include <core/memorymap.h>

typedef struct bus
{
    /* Array of master-slave connections, and size */
    master_slave_conn_t **connections;
    const unsigned connections_size;



} bus_context_t;

/*
    Initializes bus.
*/
void bus_init();

/*
    Internals of bus
*/

/*
    Write address to bus.
*/
uint8_t bus_write(addr_t addr, uint8_t value);

/*
    Read data from bus.
*/
uint8_t bus_read(addr_t addr);

#endif
