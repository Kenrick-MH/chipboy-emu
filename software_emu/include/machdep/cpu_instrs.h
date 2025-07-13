#include <cpu.h>

/**
 *  MACROS FOR INSTRUCTIONS
 */
typedef void (*INSTR_FUNC)(cpu_context_t *);

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
void instr_ld_r16_imm16(cpu_context_t *context);
void instr_ld_r16mem_a(cpu_context_t *context);
void instr_ld_a_r16mem(cpu_context_t *context);
void instr_imm16mem_sp(cpu_context_t *context);
void instr_inc_r16(cpu_context_t *context);
void instr_dec_r16(cpu_context_t *context);
void instr_add_hl_r16(cpu_context_t *context);
void instr_inc_r8(cpu_context_t *context);
void instr_dec_r8(cpu_context_t *context);
void instr_ld_r8_imm8(cpu_context_t *context);
void instr_rlca(cpu_context_t *context);
void instr_rrca(cpu_context_t *context);
void instr_rla(cpu_context_t *context);
void instr_rra(cpu_context_t *context);
void instr_daa(cpu_context_t *context);
void instr_cpl(cpu_context_t *context);
void instr_scf(cpu_context_t *context);
void instr_ccf(cpu_context_t *context);
void instr_jr_imm8(cpu_context_t *context);
void instr_jr_cond_imm8(cpu_context_t *context);
void instr_stop(cpu_context_t *context);
void instr_ld_r8_r8(cpu_context_t *context);
void instr_halt(cpu_context_t *context);
void instr_alu_op_r8(cpu_context_t *context);
void instr_alu_op_imm8(cpu_context_t *context);
void instr_ret_cond(cpu_context_t *context);
void instr_ret(cpu_context_t *context);
void instr_reti(cpu_context_t *context);
void instr_jp_cond(cpu_context_t *context);
void instr_jp_imm16(cpu_context_t *context);
void instr_jp_hl(cpu_context_t *context);
void instr_call_cond_imm16(cpu_context_t *context);
void instr_call_imm16(cpu_context_t *context);
void instr_rst_tgt3(cpu_context_t *context);
void instr_pop_r16stk(cpu_context_t *context);
void instr_push_r16stk(cpu_context_t *context);
void instr_cb_prefix(cpu_context_t *context);
void instr_ldh_cmem_a(cpu_context_t *context);
void instr_ldh_imm8mem_a(cpu_context_t *context);
void instr_ld_imm16mem_a(cpu_context_t *context);
void instr_ldh_a_cmem(cpu_context_t *context);
void instr_ld_a_imm16mem(cpu_context_t *context);
void instr_add_sp_imm8(cpu_context_t *context);
void instr_ld_hl_sp_imm8(cpu_context_t *context);
void instr_ld_sp_hl(cpu_context_t *context);
void instr_di(cpu_context_t *context);
void instr_ei(cpu_context_t *context);
void instr_unimplemented(cpu_context_t *context);

/*
    Prefix instructions, is called by `instr_cb_prefix`
*/
void instr_rlc_r8(cpu_context_t *context);
void instr_rrc_r8(cpu_context_t *context);
void instr_rl_r8(cpu_context_t *context);
void instr_rr_r8(cpu_context_t *context);
void instr_sla_r8(cpu_context_t *context);
void instr_sra_r8(cpu_context_t *context);
void instr_swap_r8(cpu_context_t *context);
void instr_srl_r8(cpu_context_t *context);
void instr_bit_b3_r8(cpu_context_t *context);
void instr_res_b3_r8(cpu_context_t *context);
void instr_set_b3_r8(cpu_context_t *context);

