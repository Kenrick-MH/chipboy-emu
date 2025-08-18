#ifndef MEMORY_H
#define MEMORY_H

#include <master_slave.h>

master_slave_conn_t *memory_get_vram_ms_connection();

/* 0xC000 - 0xCFFF region */
master_slave_conn_t *memory_get_wram0_ms_connection();

/* 0xD000 - 0xDFFF region */
master_slave_conn_t *memory_get_wram1_ms_connection();

master_slave_conn_t *memory_get_echo_ms_connection();

/* 0xFF80 - 0xFFFE region */
master_slave_conn_t *memory_get_hram_ms_connection();

#endif