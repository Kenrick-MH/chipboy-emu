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
#define ALU_CP          0x7

/* Status flags */
#define CPU_STATUS_BIT_Z    0x80
#define CPU_STATUS_BIT_N    0x40
#define CPU_STATUS_BIT_H    0x20
#define CPU_STATUS_BIT_C    0x10

/* Extract Status */
#define CPU_STATUS_Z_TEST(status_reg)   ((((status_reg) & CPU_STATUS_BIT_Z) >> 7)) 
#define CPU_STATUS_N_TEST(status_reg)   ((((status_reg) & CPU_STATUS_BIT_N) >> 6))
#define CPU_STATUS_H_TEST(status_reg)   ((((status_reg) & CPU_STATUS_BIT_H) >> 5))
#define CPU_STATUS_C_TEST(status_reg)   ((((status_reg) & CPU_STATUS_BIT_C) >> 4))


// Carry (Addition) calculations
#define CHECK_CARRY(a, b)         (((uint16_t)(a) + (uint16_t)(b)) > 0xFF)
#define CHECK_HALF_CARRY(a, b)    ((((a) & 0x0F) + ((b) & 0x0F)) > 0x0F)

// Borrow calculations
#define CHECK_BORROW(a, b)        ((uint16_t)(a) < (uint16_t)(b))
#define CHECK_HALF_BORROW(a, b)   (((a & 0x0F) < (b & 0x0F)))

#define COND_NZ         0x0
#define COND_Z          0x1
#define COND_NC         0x2
#define COND_C          0x3

/* 
    This doesn't really exist, but only really done
    to reuse logic. 
*/
#define COND_ALWAYS     0x4


/* Register Numbers */
#define R8_B            0x0
#define R8_C            0x1
#define R8_D            0x2
#define R8_E            0x3
#define R8_H            0x4
#define R8_L            0x5
#define R8_HL_VAL       0x6
#define R8_A            0x7

/* 16-bit Register Indexes */
#define R16_BC          0x0
#define R16_DE          0x1
#define R16_HL          0x2
#define R16_SP          0x3
#define R16_PC          0x4
#define R16_AF          0x5

/* 16-bit Registers for stack instructions */
#define R16STK_BC       0x0
#define R16STK_DE       0x1
#define R16STK_HL       0x2
#define R16STK_AF       0x3

void instr_nop(cpu_context_t *context);

/* ALU Operations */
void instr_alu_op_reg(cpu_context_t *context, uint8_t reg_num,
                        uint8_t alu_opcode);
void instr_alu_op_imm(cpu_context_t *context, uint8_t imm8, uint8_t alu_opcode);
void instr_add_hl_r16(cpu_context_t *context, uint16_t src_reg);

void instr_incr8(cpu_context_t *context, uint8_t reg8_num);
void instr_decr8(cpu_context_t *context, uint8_t reg8_num);
void instr_incr16(cpu_context_t *context, uint8_t reg16_num);
void instr_decr16(cpu_context_t *context, uint8_t reg16_num);
void instr_mov_r8(cpu_context_t *context, uint8_t src_num, uint8_t dest_num);
void instr_mov_imm(cpu_context_t *context, uint8_t dest_num, uint8_t imm8);

/* Load / Store instructions */
void instr_ldr();
void instr_ld_imm();
void instr_str();
void instr_st_imm();

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
        Ungrouped
    =============== 
*/
void instr_cpl(cpu_context_t *context);

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




/**
 *  Reads from register or memory
 */
static uint8_t read_reg8(cpu_context_t *context, uint8_t r8_code)
{
    uint8_t reg_val = 0x0;
    /* Load in the register value */
    switch (r8_code)
    {
        /* Read from register */
        case R8_A: reg_val = context->af.hi; break;
        case R8_B: reg_val = context->bc.hi; break;
        case R8_C: reg_val = context->bc.lo; break;
        case R8_D: reg_val = context->de.hi; break;
        case R8_E: reg_val = context->de.lo; break;
        case R8_H: reg_val = context->hl.hi; break;
        case R8_L: reg_val = context->hl.lo; break;

        /* Memory read instruction from HL */
        case R8_HL_VAL:
            addr_t addr = (addr_t) context->hl.full;
            reg_val = bus_read(addr); 
            break;

        default:
            break;
    }

    return reg_val;
}

/**
 *  Writes to register
 */
static void write_reg8(cpu_context_t *context, uint8_t r8_code, uint8_t val)
{
    switch (r8_code)
    {
        /* Read from register */
        case R8_A: context->af.hi = val; break;
        case R8_B: context->bc.hi = val; break;
        case R8_C: context->bc.lo = val; break;
        case R8_D: context->de.hi = val; break;
        case R8_E: context->de.lo = val; break;
        case R8_H: context->hl.hi = val; break;
        case R8_L: context->hl.lo = val; break;

        /* Memory read instruction from HL */
        case R8_HL_VAL:
            addr_t addr = (addr_t) context->hl.full;
            return bus_write(addr, val); 
            break;

        default:
            /* UH-OH, invalid value! */
            assert(false);
            break;
    }

    return;
}

/**
 *  Reads from register or memory
 */
static uint16_t read_reg16(cpu_context_t *context, uint8_t r16_code)
{
    switch (r16_code)   
    {
        case R16_BC: return context->bc.full; break;
        case R16_DE: return context->de.full; break;
        case R16_HL: return context->hl.full; break;
        case R16_AF: return context->af.full; break;
        case R16_SP: return context->sp     ; break;
        case R16_PC: return context->pc     ; break;
        
        default:
            /* UH-OH, invalid value! */
            assert(false);
            break;
    }
}

/**
 *  Writes to register
 */
static uint8_t write_reg16(cpu_context_t *context, uint8_t r16_code, uint16_t val)
{
    switch (r16_code)   
    {
        case R16_BC: context->bc.full = val; break;
        case R16_DE: context->de.full = val; break;
        case R16_HL: context->hl.full = val; break;
        case R16_AF: context->af.full = val; break;
        case R16_SP: context->sp      = val; break;
        case R16_PC: context->pc      = val; break;
        
        default:
            /* UH-OH, invalid value! */
            assert(false);
            break;
    }
}

/**
 *  
 */
static uint8_t read_status(cpu_context_t *context)
{
    return context->af.lo;
}

/**
 *  
 */
static void set_status(cpu_context_t *context, uint8_t flags)
{
    context->af.lo = flags;
}

/**
 *  Manages 8-bit ALU operations
 */
static void alu_op8(cpu_context_t *context, 
                    uint8_t alu8_opcode, uint8_t operand)
{    
    uint16_t accumulator = context->af.hi;
    uint16_t wide_operand = operand;
    uint8_t prev_flags = read_status(context);
    uint8_t flags = 0x0;
    
    /* Now, operate on the values */
    switch (alu8_opcode)
    {
        /* Single cycle registers to load */
        case ALU_ADD    :
        case ALU_ADDC   : 
            wide_operand += (alu8_opcode == ALU_ADDC) ? CPU_STATUS_C_TEST(prev_flags) : 0;
            if (CHECK_CARRY(accumulator, wide_operand))       flags |= CPU_STATUS_BIT_C;
            if (CHECK_HALF_CARRY(accumulator, wide_operand))  flags |= CPU_STATUS_BIT_H;
            accumulator += wide_operand; 
            break;

        case ALU_SUB    :
        case ALU_SUBC   :
        case ALU_CP     : 
            wide_operand -= (alu8_opcode == ALU_SUBC) ? CPU_STATUS_C_TEST(prev_flags) : 0;
            if (CHECK_BORROW(accumulator, wide_operand))       flags |= CPU_STATUS_BIT_C;
            if (CHECK_HALF_BORROW(accumulator, wide_operand))  flags |= CPU_STATUS_BIT_H;
            flags |= CPU_STATUS_BIT_N;
            accumulator -= wide_operand; break;
            
        case ALU_AND    :             
            accumulator &= wide_operand;
            flags |= CPU_STATUS_BIT_H; 
            break;

        case ALU_OR     : accumulator |= wide_operand; break;
        case ALU_XOR    : accumulator ^= wide_operand; break;

        default:
            break;
    }

    /* 
        Common case:
        -  Z flag is set when result (accumulator) is zero. 
    */
    if ((accumulator & 0xff) == 0) flags |= CPU_STATUS_BIT_Z;
    set_status(context, flags);

    if (alu8_opcode != ALU_CP)  
        context->af.hi = (uint8_t) (accumulator & 0xff);

    return;
}

/**
 *  Check if branch is taken or not, depending on the condition.
 *  Return a boolean indicating to take branch or not.
 */
static bool is_branch_taken(cpu_context_t *context, uint8_t condition)
{
    uint8_t prev_status = read_status(context);
    switch (condition & 0b111)
    {
        case COND_C:    return CPU_STATUS_C_TEST(prev_status);  break;
        case COND_NC:   return !CPU_STATUS_C_TEST(prev_status); break;
        case COND_Z:    return CPU_STATUS_Z_TEST(prev_status);  break;
        case COND_NZ:   return !CPU_STATUS_Z_TEST(prev_status); break;
        
        case COND_ALWAYS:
        default:
            return true;
    }
}

void instr_nop(cpu_context_t *context)
{
    /* Do nothing, only increment cycle count */
    ++context->cycles; 
}


void instr_incr8(cpu_context_t *context, uint8_t reg8_num)
{
    uint8_t status;
    
    uint8_t old_val = read_reg8(context, reg8_num);
    write_reg8(context, reg8_num, old_val+1);
    
    /* Read status, clear other bits other than C */
    status = read_status(context) & (CPU_STATUS_BIT_C);

    if ((old_val & 0x0f) == 0x0f)            status |= CPU_STATUS_BIT_H;
    if ((uint8_t) (old_val+1) == 0x0 )     status |= CPU_STATUS_BIT_Z;

    set_status(context, status);

    if (reg8_num == R8_HL_VAL){
        context->cycles += 3;
    } else context->cycles += 1;

}

void instr_incr16(cpu_context_t *context, uint8_t reg16_num)
{
    uint16_t old_val = read_reg16(context, reg16_num);
    write_reg16(context, reg16_num, (uint16_t) (old_val+1));    
    context->cycles += 2;
}

void instr_decr8(cpu_context_t *context, uint8_t reg8_num)
{
    uint8_t status;
    
    uint8_t old_val = read_reg8(context, reg8_num);
    write_reg8(context, reg8_num, (uint8_t) (old_val-1));
    
    /* Read status, clear other bits other than C */
    status = read_status(context) & (CPU_STATUS_BIT_C);

    status |= CPU_STATUS_BIT_N;
    if ((old_val & 0x0f) == 0x00)             status |= CPU_STATUS_BIT_H;
    if ((uint8_t) (old_val-1) == 0x0)       status |= CPU_STATUS_BIT_Z;

    set_status(context, status);
    if (reg8_num == R8_HL_VAL){
        context->cycles += 3;
    } else context->cycles += 1;
}

void instr_decr16(cpu_context_t *context, uint8_t reg16_num)
{
    uint16_t old_val = read_reg16(context, reg16_num);
    write_reg16(context, reg16_num, (uint16_t) (old_val-1));    
    context->cycles += 2;
}


void instr_alu_op_reg(cpu_context_t *context, uint8_t reg_num,
                        uint8_t alu_opcode)
{
    uint8_t reg_val;

    reg_val = read_reg8(context, reg_num);
    
    /* Set accumulator to the proper values */
    alu_op8(context, alu_opcode, reg_val);
    if (reg_num == R8_HL_VAL){
        context->cycles += 2;
    } else context->cycles += 1;

}

void instr_alu_op_imm(cpu_context_t *context, uint8_t imm8, uint8_t alu_opcode)
{    
    alu_op8(context, alu_opcode, imm8);

    /* Regardless of OP, this always takes two cycles */
    context->cycles += 2;
}

void instr_add_hl_r16(cpu_context_t *context, uint16_t src_reg)
{
    assert(src_reg != R16_AF && src_reg != R16_PC);
    
    uint16_t reg_val = read_reg16(context, src_reg);
    
    /* 
        TODO:
            IMPLEMENT THIS
    */

}

void instr_cpl(cpu_context_t *context)
{
    context->af.hi = ~context->af.hi;
    set_status(context, CPU_STATUS_BIT_N | CPU_STATUS_BIT_H);
    context->cycles += 1;
}

void instr_push16(cpu_context_t *context, uint8_t r16stk)
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

void instr_pop16(cpu_context_t *context, uint8_t r16stk)
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

void instr_jp_hl(cpu_context_t *context)
{
    context->pc = context->hl.full;
    context->cycles += 1;
}

void instr_jp_cc(cpu_context_t *context, addr_t address, uint8_t condition)
{   
    if (!is_branch_taken(context, condition)) {
        context->cycles += 3;
        return;
    }

    context->pc = address;
    context->cycles += 4;
}

void instr_jr_cc(cpu_context_t *context, int8_t addr_offset, uint8_t condition)
{
    
    uint16_t new_addr;

    if (!is_branch_taken(context, condition)) {
        context->cycles += 2;
        return;
    }

    uint8_t abs_val = (addr_offset > 0) ? addr_offset : -addr_offset;

    /* Calculate address */
    if (addr_offset > 0){
        context->pc += abs_val;
    } else {
        context->pc -= abs_val;
    }

    context->cycles += 3;
}

void instr_ret(cpu_context_t *context)
{
    /* Equivalent to POP PC */
    uint8_t reg_high, reg_low;

    reg_low = bus_read(context->sp++);
    reg_high = bus_read(context->sp++);

    context->pc = REGFULL(reg_high, reg_low);
    context->cycles += 4;
}

void instr_ret_cc(cpu_context_t *context, uint8_t condition)
{
    uint8_t reg_high, reg_low;
    assert(condition != COND_ALWAYS);

    /* Condition not met */
    if (!is_branch_taken(context, condition)) {
        context->cycles += 2;
        return;
    }

    reg_low = bus_read(context->sp++);
    reg_high = bus_read(context->sp++);

    context->pc = REGFULL(reg_high, reg_low);
    context->cycles += 5;
}

void instr_reti(cpu_context_t *context)
{



}

void instr_call_cc(cpu_context_t *context, addr_t label, uint8_t condition)
{
    if (!is_branch_taken(context, condition)) {
        context->cycles += 3;
        return;
    }

    /* Push PC on stack */
    bus_write(context->sp--, REGHIGH(context->pc));
    bus_write(context->sp--, REGLOW(context->pc));

    context->pc = label;
    context->cycles += 6;
}

void instr_ccf(cpu_context_t *context)
{
    uint8_t status = 0x0;
    uint8_t prev_status = read_status(context);

    /* Preserve the Z flag, complement C flag */
    status |= (prev_status & CPU_STATUS_BIT_Z) | (~prev_status & CPU_STATUS_BIT_C); 
    set_status(context, status);
    context->cycles += 1;
}

void instr_scf(cpu_context_t *context)
{
    uint8_t status = 0x0;
    uint8_t prev_status = read_status(context);

    /* Preserve the Z flag, set C flag */
    status |= (prev_status & CPU_STATUS_BIT_Z) | CPU_STATUS_BIT_C; 
    set_status(context, status);
    context->cycles += 1;
}

void instr_mov_imm(cpu_context_t *context, uint8_t dest_num, uint8_t imm8)
{
    write_reg8(context, dest_num, imm8);
    
    if (dest_num == R8_HL_VAL) {
        context->cycles += 3;
        return;
    }

    context->cycles += 2;
}

void instr_mov_r8(cpu_context_t *context, uint8_t src_num, uint8_t dest_num)
{
    uint8_t reg_val = read_reg8(context, src_num);
    write_reg8(context, dest_num, reg_val);

    if (dest_num == R8_HL_VAL) {
        context->cycles += 2;
        return;
    }

    context->cycles += 1;
}

void instr_bit_r8(cpu_context_t *context, uint8_t regcode, uint8_t bit_select)
{
    /* 
        TODO: 
            CONTINUE IMPLEMENT THIS!
    */
    
    uint8_t reg_val;
    uint8_t status = 0x0;
    assert (bit_select < 0x8);

    reg_val = read_reg8(context, regcode);

    /* Keep carry bit, clear others */
    status = read_status(context) & CPU_STATUS_BIT_C;
    status |= CPU_STATUS_BIT_H;

    set_status(context, status);
    
    if (regcode == R8_HL_VAL){
        context->cycles += 3;
    } else context->cycles += 2;

}

void instr_res_r8(cpu_context_t *context, uint8_t regcode, uint8_t bit_select)
{
    uint8_t reg_val;
    uint8_t mask;
    assert (bit_select < 0x8);

    reg_val = read_reg8(context, regcode);
    mask = 0x1 << bit_select;
    write_reg8(context, regcode, reg_val & (~mask));
    
    if (regcode == R8_HL_VAL){
        context->cycles += 4;
    } else context->cycles += 2;
}

void instr_set_r8(cpu_context_t *context, uint8_t regcode, uint8_t bit_select){
    uint8_t reg_val;
    uint8_t mask;
    assert (bit_select < 0x8);

    reg_val = read_reg8(context, regcode);
    mask = 0x1 << bit_select;
    write_reg8(context, regcode, reg_val | mask);
    
    if (regcode == R8_HL_VAL){
        context->cycles += 4;
    } else context->cycles += 2;
}

