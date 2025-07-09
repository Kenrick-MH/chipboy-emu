#include <cpu_instrs.h>
#include <bus.h>

/* ALU OP FLAGS */
#define ALU_ADD         0x0
#define ALU_ADDC        0x1
#define ALU_SUB         0x2
#define ALU_SUBC        0x3
#define ALU_OR          0x4
#define ALU_AND         0x5
#define ALU_XOR         0x6
#define ALU_COMPARE     0x7

/* Status flags */
#define FLAG_ALWAYS
#define FLAG_N
#define FLAG_C

/* Register Numbers */
#define R8_B            0x0
#define R8_C            0x1
#define R8_D            0x2
#define R8_E            0x3
#define R8_H            0x4
#define R8_L            0x5
#define R8_HL_VAL       0x6
#define R8_A            0x7

#define R16_BC          0x0
#define R16_DE          0x1
#define R16_HL          0x2
#define R16_SP          0x3

#define R16STK_BC       0x0
#define R16STK_DE       0x1
#define R16STK_HL       0x2
#define R16STK_AF       0x3

#define R16STK_BC       0x0
#define R16STK_DE       0x1
#define R16STK_HL       0x2
#define R16STK_AF       0x3

void instr_nop(cpu_context_t *context);

/* ALU Operations */
void instr_alu_op_reg(cpu_context_t *context, uint8_t reg_num,
                        uint8_t alu_opcode);
void instr_alu_op_imm(cpu_context_t *context, uint8_t imm8, uint8_t alu_opcode);

void instr_mov_reg(cpu_context_t *context, uint8_t src_num, uint8_t dst_num);
void instr_mov_imm(cpu_context_t *context, uint8_t imm8);

/* Load / Store instructions */
void instr_alu_ld(cpu_context_t *context);
void instr_ldr();
void instr_ld_imm();
void instr_str();
void instr_st_imm();

/* Push/Pop */
void instr_push(cpu_context_t *context, uint8_t r16stk);
void instr_pop(cpu_context_t *context, uint8_t r16stk);


void instr_ret(cpu_context_t *context);



/* Ungrouped */
void instr_cpl(cpu_context_t *context);




void instr_nop(cpu_context_t *context)
{
    /* Do nothing, only increment cycle count */
    ++context->cycles; 
}

void instr_alu_op_reg(cpu_context_t *context, uint8_t reg_num,
                        uint8_t alu_opcode)
{
    uint8_t reg_val;

    /* Load in the register value */
    switch (reg_num)
    {
        /* Single cycle registers to load */
        case R8_A: reg_val = context->af.hi; context->cycles += 1; break;
        case R8_B: reg_val = context->bc.hi; context->cycles += 1; break;
        case R8_C: reg_val = context->bc.lo; context->cycles += 1; break;
        case R8_D: reg_val = context->de.hi; context->cycles += 1; break;
        case R8_E: reg_val = context->de.lo; context->cycles += 1; break;
        case R8_H: reg_val = context->hl.hi; context->cycles += 1; break;
        case R8_L: reg_val = context->hl.lo; context->cycles += 1; break;

        /* Two cycle instructions */
        case R8_HL_VAL:
            addr_t addr = (addr_t) context->hl.full;
            reg_val = bus_read(addr); 
            context->cycles += 2;
            break;

        default:
            break;
    }

    /* Now, operate on the values */
    switch (alu_opcode)
    {
        /* Single cycle registers to load */
        case ALU_ADD : context->af.lo += reg_val; break;
        case ALU_ADDC: context->af.lo += reg_val; break;
        case ALU_SUB : context->af.lo -= reg_val; break;
        case ALU_SUBC : context->af.lo -= reg_val; break;
        case ALU_OR  : context->af.lo |= reg_val; break;
        case ALU_AND  : context->af.lo &= reg_val; break;
        case ALU_XOR  : context->af.lo ^= reg_val; break;

        default:
            break;
    }
}

void instr_alu_op_imm(cpu_context_t *context, uint8_t imm8, uint8_t alu_opcode)
{
    /* Now, operate on the values */
    switch (alu_opcode)
    {
        /* Single cycle registers to load */
        case ALU_ADD    : context->af.lo += imm8;   break;
        case ALU_ADDC   : context->af.lo += imm8;   break;
        case ALU_SUB    : context->af.lo -= imm8;   break;
        case ALU_SUBC   : context->af.lo -= imm8;   break;
        case ALU_OR     : context->af.lo |= imm8;   break;
        case ALU_AND    : context->af.lo &= imm8;   break;
        case ALU_XOR    : context->af.lo ^= imm8;   break;

        default:
            break;
    }

    /* Regardless of OP, this always takes two cycles */
    context->cycles += 2;
}

void instr_mov_reg(cpu_context_t *context, uint8_t src_num, uint8_t dst_num);

void instr_cpl(cpu_context_t *context)
{
    context->af.hi = ~context->af.hi;
    context->cycles += 1;
    /* Set status bits */
}

void instr_push(cpu_context_t *context, uint8_t r16stk)
{
    uint16_t reg_data;
    switch (r16stk)
    {
        case R16STK_BC: reg_data = context->bc.full; break;
        case R16STK_DE: reg_data = context->de.full; break;
        case R16STK_HL: reg_data = context->hl.full; break;
        case R16STK_AF: reg_data = context->af.full; break;
    
        default:
            break;
    }
    
    /* 
        TODO: 
            - Handle stack overflow mechanism, for now, let the bus handle it
    */

    /* Write HIGH part */
    bus_write(context->sp--, REGHIGH(reg_data));
    bus_write(context->sp--, REGLOW(reg_data));
    context->cycles += 4;
}


void instr_pop(cpu_context_t *context, uint8_t r16stk)
{
    uint8_t reg_high, reg_low;
    uint16_t full_val;

    reg_low = bus_read(context->sp++);
    reg_high = bus_read(context->sp++);

    switch (r16stk)
    {
        case R16STK_BC: context->bc.full = REGFULL(reg_high, reg_low); break;
        case R16STK_DE: context->de.full = REGFULL(reg_high, reg_low); break;
        case R16STK_HL: context->hl.full = REGFULL(reg_high, reg_low); break;
        case R16STK_AF: context->af.full = REGFULL(reg_high, reg_low); break;
    
        default:
            break;
    }

    context->cycles += 3;
}

void instr_ret(cpu_context_t *context)
{
    /* Equivalent to POP PC */
    uint8_t reg_high, reg_low;
    uint16_t full_val;

    reg_low = bus_read(context->sp++);
    reg_high = bus_read(context->sp++);

    context->pc = REGFULL(reg_high, reg_low);
    context->cycles += 4;
}
