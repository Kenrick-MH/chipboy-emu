#include <machdep/cart.h>
#include <string.h>

void read_rom_meta(cart_data_t *rom_data, const uint8_t *raw_buffer, size_t rom_size)
{
    memcpy((void *) rom_data->metadata.nintendo_logo, &raw_buffer[0x104], 0x30);
    memcpy((void *) rom_data->metadata.title, &raw_buffer[0x134], 0x10);
    memcpy((void *) rom_data->metadata.manu_code, &raw_buffer[0x13F], 0x4);
    rom_data->metadata.cgb_flag = raw_buffer[0x143];
    memcpy((void *) rom_data->metadata.new_licensee_code, &raw_buffer[0x144], 0x2);
    rom_data->metadata.sgb_flag = raw_buffer[0x146];
    rom_data->metadata.cart_type = raw_buffer[0x147];
    
    // ROM
    // RAM

    rom_data->metadata.destination_code = raw_buffer[0x14A];
    rom_data->metadata.old_licensee_code = raw_buffer[0x14B];
    rom_data->metadata.version = raw_buffer[0x14C];
    rom_data->metadata.header_checksum = raw_buffer[0x14D];
    
    // Need to make this poratble, since number is in big endian.
    // metadata->global_checksum = 
}




