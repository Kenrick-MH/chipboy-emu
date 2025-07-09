
#include <cpu.h>

/**
 *  MACROS FOR INSTRUCTIONS
 */



/** 
 *  Instructions for the CPU. 
 *  
 *  Every instruction will increment the number of cycles incrementally.
 *  PC incrementing is already hadnled in the core module `cpu.h`.
 * 
 */
void instr_alu_op(uint8_t val, uint8_t op);
void instr_alu_ld();
void instr_ldr();
void instr_str();
void instr_mov();
void instr_mov();






