#ifndef MBC_H
#define MBC_H

#include <common.h>
#include <emu_error.h>

typedef struct mbc_handler
{
    /* Internal context handler */
    void * const internal_context;    

    /* Implemented by the MBC */
    error_code_t (* mbc_read)(mbc_handler_t *handler_ptr, uint8_t address, uint8_t *rd_data);
    error_code_t (* mbc_write)(mbc_handler_t *handler_ptr, uint8_t address, uint8_t wr_data);

} mbc_handler_t;

#endif // MBC_H