#include <platform/emu_memory.h>
#include <core/memorymap.h>
#include <assert.h>

/* Context for each memory region. */
typedef struct memory_ctx
{
    addr_t start_addr;
    addr_t end_addr;
    uint8_t *mem_buffer;
} memory_ctx_t;

static uint8_t memory_read(void *context, addr_t addr){
    memory_ctx_t *mem_ctx = (memory_ctx_t *) context;   
    
    addr_t offset = addr - mem_ctx->start_addr;
    assert (offset <= mem_ctx->end_addr);
    return mem_ctx->mem_buffer[offset];
}

static void memory_write(void *context, addr_t addr, uint8_t val){
    memory_ctx_t *mem_ctx = (memory_ctx_t *) context;   
    
    addr_t offset = addr - mem_ctx->start_addr;
    assert (offset <= mem_ctx->end_addr);
    mem_ctx->mem_buffer[offset] = val;
}

#define memory_buffer(name) name_##buf
#define memory_context(name) name_##ctx
#define memory_ms_conn(name) name_##ms_conn

/**
 *  Function: `DECLARE_MEM_REGION(name, addr_t start, addr_t end)`
 *  Declares a memory region within the boundaries of [start, end]
 *  (both inclusive), generating the memory buffer in static memory, 
 *  a memory_ctx struct, and the master slave interface for each one.
 */
#define DECLARE_MEM_REGION(name, start, end) \  
    static uint8_t memory_buffer(name) [MEM_REGION_SIZE((start), (end))]; \
    static memory_ctx_t memory_context(name) = (memory_ctx_t) {     \
        .start_addr = (start), \
        .end_addr = (end), \ 
        .mem_buffer = &memory_buffer(name) \ 
    };  \ 
    static master_slave_conn_t memory_ms_conn(name) = (master_slave_conn_t){ \ 
        .start_addr = (start), \ 
        .end_addr = (end), \ 
        .slave_context = (void *) &memory_context(name), \ 
        .slave_read = memory_read, \ 
        .slave_write = memory_write \ 
    }; 

DECLARE_MEM_REGION(vram, VRAM_BASE, VRAM_END);
DECLARE_MEM_REGION(wram0, WRAM0_BASE, WRAM0_END);
DECLARE_MEM_REGION(wram1, WRAM1_BASE, WRAM1_END);
DECLARE_MEM_REGION(hram, HIGH_RAM_BASE, HIGH_RAM_END);

/* Special care needs to be done for echo RAM */
static memory_ctx_t echo_ram_ctx = (memory_ctx_t) {     
        .start_addr = ECHO_RAM_BASE, 
        .end_addr = ECHO_RAM_END, 
        .mem_buffer = &memory_buffer(wram0) 
    };  

static master_slave_conn_t echo_ram_ms_conn = (master_slave_conn_t){  
    .start_addr = ECHO_RAM_BASE, 
    .end_addr = ECHO_RAM_END, 
    .slave_context = (void *) &echo_ram_ctx, 
    .slave_read = memory_read, 
    .slave_write = memory_write  
}; 

/* 0x8000 - 0x9FFF region */
master_slave_conn_t *memory_get_vram_ms_connection() 
    {return &memory_ms_conn(vram);}

/* 0xC000 - 0xCFFF region */
master_slave_conn_t *memory_get_wram0_ms_connection() 
    {return &memory_ms_conn(wram0);}

/* 0xD000 - 0xDFFF region */
master_slave_conn_t *memory_get_wram1_ms_connection()
    {return &memory_ms_conn(wram1);}

master_slave_conn_t *memory_get_echo_ms_connection()
    {return &echo_ram_ms_conn;}

/* 0xFF80 - 0xFFFE region */
master_slave_conn_t *memory_get_hram_ms_connection()
    {return &memory_ms_conn(hram);}





