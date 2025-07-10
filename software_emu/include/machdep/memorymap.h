#ifndef MEMORYMAP_H
#define MEMORYMAP_H

/**
 *  Address definitions / memory map for the GameBoy system.
 */


/* Fixed bank */
#define ROM_BANK_00_BASE                0x0000
#define ROM_BANK_00_END                 0x3FFF

/* Switchable bank */
#define ROM_BANKS_BASE                  0x4000
#define ROM_BANKS_END                   0x7FFF

#define VRAM_BASE                       0x8000
#define VRAM_END                        0x9FFF

#define EXTERNAL_RAM_BASE               0xA000
#define EXTERNAL_RAM_END                0xBFFF

#define WRAM1_BASE                      0xC000
#define WRAM1_END                       0xCFFF

#define WRAM2_BASE                      0xD000
#define WRAM2_END                       0xDFFF

/* Reserved */
#define ECHO_RAM_BASE                   0xE000
#define ECHO_RAM_END                    0xFDFF

#define OAM_BASE                        0xFE00
#define OAM_END                         0xFE9F

/* Reserved */
#define UNUSED_RAM_BASE                 0xFEA0
#define UNUSED_RAM_END                  0xFEFF

#define IO_REGS_BASE                    0xFF00
#define IO_REGS_END                     0xFF7F

#define HIGH_RAM_BASE                   0xFF80
#define HIGH_RAM_END                    0xFFFE

#define INTERUPT_ENABLE_BASE            0xFFFF
#define INTERUPT_ENABLE_END             0xFFFF

/* I/O Memory Map */


#endif // MEMORYMAP_H