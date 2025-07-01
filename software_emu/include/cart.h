
#include <common.h>


/**
 *  Based on the documentation in 
 *  https://gbdev.io/pandocs/The_Cartridge_Header.html
 */

 /**
  *  Struct defining the ROM header metadata upon loading.
  */

typedef struct cart 
{
    u8 nintendo_logo[0x30];
    char title[0x10];
    char manu_code[4];
    u32 rom_size;
    u32 ram_size;
    u8 destination_code;
    u8 cart_type;
    u8 old_lisencee_code;
    u8 new_lisencee_code;
    u8 version;
    u8 header_checksum;
    u16 global_checksum;
    u8 cgb_flag :1;
    u8 sgb_flag :1;
    u8 *rom_data;

} cart_meta_t;


















