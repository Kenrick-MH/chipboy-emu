#ifndef BUS_H
#define BUS_H

/**
 *  Bus/Memory related operations
 */

#include <common.h>


typedef uint8_t addr_t;

int bus_write(addr_t addr, uint8_t value);

int bus_read(addr_t add);









#endif
