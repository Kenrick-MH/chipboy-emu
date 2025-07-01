
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
    uint8_t     nintendo_logo[0x30];
    char        title[0x10];
    char        manu_code[4];
    u32         rom_size;
    u32         ram_size;
    uint8_t     destination_code;
    uint8_t     cart_type;
    uint8_t     old_licensee_code;
    char        new_licensee_code[2];
    uint8_t     version;
    uint8_t     header_checksum;
    uint16_t    global_checksum;
    uint8_t     cgb_flag;
    uint8_t     sgb_flag;

    /*
        This would actually be the entire ROM dump,
        since entry point is not contiguous to the rest of the code
    */
    uint8_t     *rom_data;

} cart_meta_t;

/*
    @brief Reads a ROM buffer and writes it to METADATA.
    @param metadata
*/
void read_rom_meta(cart_meta_t *metadata, const uint8_t rom_data, size_t rom_size);


















