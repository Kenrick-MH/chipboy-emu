#ifndef BUS_H
#define BUS_H

/** 
 *  This file is responsible for managing memory reads/
 *  writes in-between devices.
 */

#include <common.h>
#include <master_slave.h>

typedef uint8_t addr_t;

uint8_t bus_write(addr_t addr, uint8_t value);

uint8_t bus_read(addr_t add);









#endif
