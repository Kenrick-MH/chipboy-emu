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
    master_slave_conn_t *connections[MAX_DEVICE_NUMBER];
    unsigned connections_size;
    uint64_t owner_dev_id;
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

/*
    Bus synchronization primitives,
    used by a bus master (CPU, PPU)
    to acquire ownership of bus.
*/
void bus_lock(uint64_t device_id);
void bus_unlock(uint64_t device_id);

#endif
