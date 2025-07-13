
#include <cpu.h>

/**
 *  MACROS FOR INSTRUCTIONS
 */

/** 
 *  Instructions for the CPU. 
 *  
 *  Every instruction will increment the number of cycles incrementally, 
 *  and reads "arguments" by decoding the next byte.
 * 
 *  The specific semantics of each instruction can be seen in 
 *  https://rgbds.gbdev.io/docs/v0.9.3/gbz80.7#Load_instructions
 * 
 *  All functions takes (cpu_context *, uint8_t opcode) as an argument.
 */
void instr_nop(cpu_context_t *context);

/* ALU Operations */

/*  */
void instr_alu_op_reg(cpu_context_t *context, uint8_t reg_num,
                        uint8_t alu_opcode);

/* Adds 8-bit immediate to the accumulator register */
void instr_alu_op_imm(cpu_context_t *context, uint8_t imm8, uint8_t alu_opcode);
void instr_add_hl_r16(cpu_context_t *context, uint16_t src_reg);

/* Increment 8-bit register by 8 */
void instr_incr8(cpu_context_t *context, uint8_t reg8_num);

/* Decrement 8-bit register by 8 */
void instr_decr8(cpu_context_t *context, uint8_t reg8_num);

/* Increment 16-bit register */
void instr_incr16(cpu_context_t *context, uint8_t reg16_num);

/* Decrement 16-bit register */
void instr_decr16(cpu_context_t *context, uint8_t reg16_num);


/**
 * =============================
 *  LOAD/STORE/MOV INSTRUCTIONS
 * =============================
 */

/* Moves value from source register src_num to dest_num */
void instr_mov_r8(cpu_context_t *context, uint8_t src_num, uint8_t dest_num);

/* Moves immediate value into 8-bit register */
void instr_mov_r8_imm8(cpu_context_t *context, uint8_t dest_num, uint8_t imm8);

/* Moves 16-bit immediate values to 16-bit register */
void instr_mov_r16_imm16(cpu_context_t *context);

/* 
    Load / Store instructions (these actually go to memory).
    Here, HL is the address argument.
*/
void instr_ld_r8_hl(cpu_context_t *context);
void instr_str_hl_r8(cpu_context_t *context);
void instr_str_hl_imm8(cpu_context_t *context);

/* 
    16-bit load/stores. 
    These take a 16-bit register/immediate as the address
    argument, 
    and loads/stores it to register A.
*/
void instr_ld_a_r16(cpu_context_t *context);
void instr_ld_a_imm16(cpu_context_t *context);
void instr_str_r16_a(cpu_context_t *context);
void instr_str_imm16_a(cpu_context_t *context);

/* 
    Store to a location 0xFF00 + 8-bit value (ldh) instructions.
    Data going to CPU is denoted as a ldh (load-half) instruction.
    Data going out from CPU is denoted as sth (store-half) instruction.
*/
void instr_sth_imm8_a(cpu_context_t *context);
void instr_sth_c_a(cpu_context_t *context);
void instr_ldh_c_a(cpu_context_t *context);
void instr_ldh_imm8_a(cpu_context_t *context);

/*
    Store and increment/decrement (corresponds to the instructions)
    ld_(incr/decr) -> ld A [hl(i/d)] 
    str_(incr/decr) -> ld [hl(i/d)] A
*/
void instr_ld_incr(cpu_context_t *context);
void instr_ld_decr(cpu_context_t *context);
void instr_str_incr(cpu_context_t *context);
void instr_str_decr(cpu_context_t *context);


/**
 * =============================
 *       STACK INSTRUCTIONS
 * =============================
 */

/* Push/Pop */
void instr_push16(cpu_context_t *context, uint8_t r16stk);
void instr_pop16(cpu_context_t *context, uint8_t r16stk);

/* Jumps */
void instr_jp_hl(cpu_context_t *context);
void instr_jp_cc(cpu_context_t *context, addr_t address, uint8_t condition);
void instr_jr_cc(cpu_context_t *context, int8_t addr_offset, uint8_t condition);

/* 
    Has 2 different versions, since the always variant and not taken variant have
    two differing clock cycles.
*/
void instr_ret(cpu_context_t *context);
void instr_ret_cc(cpu_context_t *context, uint8_t condition);
void instr_reti(cpu_context_t *context);

void instr_call_cc(cpu_context_t *context, addr_t label, uint8_t condition);

/* Bit Operations */
void instr_bit_r8(cpu_context_t *context, uint8_t regcode, uint8_t bit_select);
void instr_res_r8(cpu_context_t *context, uint8_t regcode, uint8_t bit_select);
void instr_set_r8(cpu_context_t *context, uint8_t regcode, uint8_t bit_select);


/*
    ===============
        
    =============== 
*/
void instr_cpl(cpu_context_t *context);


/**
 * =============================
 *       CARRY INSTRUCTIONS
 * =============================
 */
/* Complement carry flag */
void instr_ccf(cpu_context_t *context);

/* Set carry flag */
void instr_scf(cpu_context_t *context);


/*
    ===========================
        Prefix Instruction
    ===========================
*/

/* 
    Only acts a branching point for prefix instructions.
*/
void instr_exec_prefix(cpu_context_t *context);









