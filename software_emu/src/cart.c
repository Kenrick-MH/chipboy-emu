#include <cart.h>
#include <string.h>

void read_rom_meta(cart_meta_t *metadata, const uint8_t *rom_data, size_t rom_size)
{
    memcpy((void *) metadata->nintendo_logo, &rom_data[0x104], 0x30);
    memcpy((void *) metadata->title, &rom_data[0x134], 0x10);
    memcpy((void *) metadata->manu_code, &rom_data[0x13F], 0x4);
    metadata->cgb_flag = rom_data[0x143];
    memcpy((void *) metadata->new_licensee_code, &rom_data[0x144], 0x2);
    metadata->sgb_flag = rom_data[0x146];
    metadata->cart_type = rom_data[0x147];
    
    // ROM
    // RAM

    metadata->destination_code = rom_data[0x14A];
    metadata->old_licensee_code = rom_data[0x14B];
    metadata->version = rom_data[0x14C];
    metadata->header_checksum = rom_data[0x14D];
    
    // Need to make this poratble, since number is in big endian.
    // metadata->global_checksum = 
    
    return 0;
}




