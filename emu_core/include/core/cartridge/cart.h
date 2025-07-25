

#ifndef CART_H
#define CART_H
#include <common.h>


/**
 *  Based on the documentation in 
 *  https://gbdev.io/pandocs/The_Cartridge_Header.html
 */

const char *OLD_LICENSEE_NAMES[0x100] = {     
    [0x00] = "None",     
    [0x01] = "Nintendo",     
    [0x08] = "Capcom",     
    [0x09] = "HOT-B",     
    [0x0A] = "Jaleco",     
    [0x0B] = "Coconuts Japan",     
    [0x0C] = "Elite Systems",     
    [0x13] = "EA (Electronic Arts)",     
    [0x18] = "Hudson Soft",     
    [0x19] = "ITC Entertainment",     
    [0x1A] = "Yanoman",     
    [0x1D] = "Japan Clary",     
    [0x1F] = "Virgin Games Ltd.",     
    [0x24] = "PCM Complete",     
    [0x25] = "San-X",     
    [0x28] = "Kemco",     
    [0x29] = "SETA Corporation",     
    [0x30] = "Infogrames",     
    [0x31] = "Nintendo",     
    [0x32] = "Bandai",     
    [0x33] = "Indicates that the New licensee code should be used instead.",     
    [0x34] = "Konami",     
    [0x35] = "HectorSoft",     
    [0x38] = "Capcom",     
    [0x39] = "Banpresto",     
    [0x3C] = "Entertainment Interactive (stub)",     
    [0x3E] = "Gremlin",     
    [0x41] = "Ubi Soft",     
    [0x42] = "Atlus",     
    [0x44] = "Malibu Interactive",     
    [0x46] = "Angel",     
    [0x47] = "Spectrum HoloByte",     
    [0x49] = "Irem",     
    [0x4A] = "Virgin Games Ltd.",     
    [0x4D] = "Malibu Interactive",     
    [0x4F] = "U.S. Gold",     
    [0x50] = "Absolute",     
    [0x51] = "Acclaim Entertainment",     
    [0x52] = "Activision",     
    [0x53] = "Sammy USA Corporation",     
    [0x54] = "GameTek",     
    [0x55] = "Park Place",     
    [0x56] = "LJN",     
    [0x57] = "Matchbox",     
    [0x59] = "Milton Bradley Company",     
    [0x5A] = "Mindscape",     
    [0x5B] = "Romstar",     
    [0x5C] = "Naxat Soft",     
    [0x5D] = "Tradewest",     
    [0x60] = "Titus Interactive",     
    [0x61] = "Virgin Games Ltd.",     
    [0x67] = "Ocean Software",     
    [0x69] = "EA (Electronic Arts)",     
    [0x6E] = "Elite Systems",     
    [0x6F] = "Electro Brain",     
    [0x70] = "Infogrames",     
    [0x71] = "Interplay Entertainment",     
    [0x72] = "Broderbund",     
    [0x73] = "Sculptured Software",     
    [0x75] = "The Sales Curve Limited",     
    [0x78] = "THQ",     
    [0x79] = "Accolade",     
    [0x7A] = "Triffix Entertainment",     
    [0x7C] = "MicroProse",     
    [0x7F] = "Kemco",     
    [0x80] = "Misawa Entertainment",     
    [0x83] = "LOZC G.",     
    [0x86] = "Tokuma Shoten",     
    [0x8B] = "Bullet-Proof Software",     
    [0x8C] = "Vic Tokai Corp.",     
    [0x8E] = "Ape Inc.",     
    [0x8F] = "I’Max",     
    [0x91] = "Chunsoft Co.",     
    [0x92] = "Video System",     
    [0x93] = "Tsubaraya Productions",     
    [0x95] = "Varie",     
    [0x96] = "Yonezawa/S’Pal",     
    [0x97] = "Kemco",     
    [0x99] = "Arc",     
    [0x9A] = "Nihon Bussan",     
    [0x9B] = "Tecmo",     
    [0x9C] = "Imagineer",     
    [0x9D] = "Banpresto",     
    [0x9F] = "Nova",     
    [0xA1] = "Hori Electric",     
    [0xA2] = "Bandai",     
    [0xA4] = "Konami",     
    [0xA6] = "Kawada",     
    [0xA7] = "Takara",     
    [0xA9] = "Technos Japan",     
    [0xAA] = "Broderbund",     
    [0xAC] = "Toei Animation",     
    [0xAD] = "Toho",     
    [0xAF] = "Namco",     
    [0xB0] = "Acclaim Entertainment",     
    [0xB1] = "ASCII Corporation or Nexsoft",     
    [0xB2] = "Bandai",     
    [0xB4] = "Square Enix",     
    [0xB6] = "HAL Laboratory",     
    [0xB7] = "SNK",     
    [0xB9] = "Pony Canyon",     
    [0xBA] = "Culture Brain",     
    [0xBB] = "Sunsoft",     
    [0xBD] = "Sony Imagesoft",     
    [0xBF] = "Sammy Corporation",     
    [0xC0] = "Taito",     
    [0xC2] = "Kemco",     
    [0xC3] = "Square",     
    [0xC4] = "Tokuma Shoten",     
    [0xC5] = "Data East",     
    [0xC6] = "Tonkin House",     
    [0xC8] = "Koei",     
    [0xC9] = "UFL",     
    [0xCA] = "Ultra Games",     
    [0xCB] = "VAP, Inc.",     
    [0xCC] = "Use Corporation",     
    [0xCD] = "Meldac",     
    [0xCE] = "Pony Canyon",     
    [0xCF] = "Angel",     
    [0xD0] = "Taito",     
    [0xD1] = "SOFEL (Software Engineering Lab)",     
    [0xD2] = "Quest",     
    [0xD3] = "Sigma Enterprises",     
    [0xD4] = "ASK Kodansha Co.",     
    [0xD6] = "Naxat Soft",     
    [0xD7] = "Copya System",     
    [0xD9] = "Banpresto",     
    [0xDA] = "Tomy",     
    [0xDB] = "LJN",     
    [0xDD] = "Nippon Computer Systems",     
    [0xDE] = "Human Ent.",     
    [0xDF] = "Altron",     
    [0xE0] = "Jaleco",     
    [0xE1] = "Towa Chiki",     
    [0xE2] = "Yutaka",     
    [0xE3] = "Varie",     
    [0xE5] = "Epoch",     
    [0xE7] = "Athena",     
    [0xE8] = "Asmik Ace Entertainment",     
    [0xE9] = "Natsume",     
    [0xEA] = "King Records",     
    [0xEB] = "Atlus",     
    [0xEC] = "Epic/Sony Records",     
    [0xEE] = "IGS",     
    [0xF0] = "A Wave",     
    [0xF3] = "Extreme Entertainment",     
    [0xFF] = "LJN" 
};

typedef struct cart_meta 
{
    uint8_t     nintendo_logo[0x30];
    char        title[0x10];
    char        manu_code[4];
    uint32_t    rom_size;
    uint32_t    ram_size;
    uint8_t     destination_code;
    uint8_t     cart_type;
    uint8_t     old_licensee_code;
    char        new_licensee_code[2];
    uint8_t     version;
    uint8_t     header_checksum;
    uint16_t    global_checksum;
    uint8_t     cgb_flag;
    uint8_t     sgb_flag;

} cart_meta_t;

typedef struct cart_data
{
    /* Metadata of the cartridge, see above */
    cart_meta_t metadata;
    
    /*
        This would actually be the entire ROM dump,
        since entry point is not contiguous to the rest of the code
    */
    uint8_t     *rom_data;

} cart_data_t;

/*
    @brief Reads a ROM buffer and writes it to METADATA.
    @param metadata
*/
void read_rom_meta(cart_data_t *rom_data, const uint8_t *raw_buffer, size_t rom_size);

/** 
 *  Prints out cart metadata in a human readable format.
 */
void cart_print_metadata(cart_meta_t *metadata);

/**
 *  Validates checksum of the rom stored in the metadata.
 */
int validate_checksum(cart_data_t *rom_data);

/* 
    I think cartridge memory bank switching should go here instead of the bus
*/

#endif













