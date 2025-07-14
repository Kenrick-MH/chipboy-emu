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
#define CPU_STATUS_MASK_Z    0x80
#define CPU_STATUS_MASK_N    0x40
#define CPU_STATUS_MASK_H    0x20
#define CPU_STATUS_MASK_C    0x10

/* Extract Status */
#define CPU_STATUS_TEST(status_reg, status_mask) \
   ( ((status_reg) & (status_mask)) ? 1 : 0)    

#define CPU_STATUS_Z_TEST(status_reg)   ((((status_reg) & CPU_STATUS_MASK_Z) >> 7)) 
#define CPU_STATUS_N_TEST(status_reg)   ((((status_reg) & CPU_STATUS_MASK_N) >> 6))
#define CPU_STATUS_H_TEST(status_reg)   ((((status_reg) & CPU_STATUS_MASK_H) >> 5))
#define CPU_STATUS_C_TEST(status_reg)   ((((status_reg) & CPU_STATUS_MASK_C) >> 4))

/* Set individual status */

#define CPU_STATUS_SET(status_reg, status_code, value) \
 ( (value) ? ((status_reg) & ~(status_code)) : ((status_reg) | (status_code)))

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
#define R8_HL_MEM       0x6
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
        case R8_HL_MEM:
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
        case R8_HL_MEM:
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
static void write_reg16(cpu_context_t *context, uint8_t r16_code, uint16_t val)
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

static uint8_t read_status(cpu_context_t *context)
{ 
    return context->af.lo; 
}

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
            if (CHECK_CARRY(accumulator, wide_operand))       flags |= CPU_STATUS_MASK_C;
            if (CHECK_HALF_CARRY(accumulator, wide_operand))  flags |= CPU_STATUS_MASK_H;
            accumulator += wide_operand; 
            break;

        case ALU_SUB    :
        case ALU_SUBC   :
        case ALU_CP     : 
            wide_operand -= (alu8_opcode == ALU_SUBC) ? CPU_STATUS_C_TEST(prev_flags) : 0;
            if (CHECK_BORROW(accumulator, wide_operand))       flags |= CPU_STATUS_MASK_C;
            if (CHECK_HALF_BORROW(accumulator, wide_operand))  flags |= CPU_STATUS_MASK_H;
            flags |= CPU_STATUS_MASK_N;
            accumulator -= wide_operand; break;
            
        case ALU_AND    :             
            accumulator &= wide_operand;
            flags |= CPU_STATUS_MASK_H; 
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
    if ((accumulator & 0xff) == 0) flags |= CPU_STATUS_MASK_Z;
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

/*
    Rotates 8-bit register right through the carry
    flag.
*/
static void rr_r8(cpu_context_t *context, uint8_t r8_code)
{
    uint8_t new_val;
    uint8_t prev_status = read_status(context);
    uint8_t status = 0x0;
    uint8_t r8_val = read_reg8(context, r8_code);
    uint8_t lsb = r8_val & 0x1;
    uint8_t prev_carry = CPU_STATUS_C_TEST(prev_status);
    
    /* Previous carry becomes MSB */
    new_val = (r8_val >> 1) | (prev_carry << 7);

    /* Set carry to be previous LSB */
    status = CPU_STATUS_SET(status, CPU_STATUS_MASK_C, lsb);

    /* Set zero flag */
    /* Weird behaviour, need to zero out the Z flag if register is A*/
    status = CPU_STATUS_SET(status, CPU_STATUS_MASK_Z, (new_val == 0));
 
    write_reg8(context, r8_code, new_val);
    set_status(context, status);    
}

/*
    Rotates 8-bit register left through the carry
    flag.
*/
static void rl_r8(cpu_context_t *context, uint8_t r8_code)
{
    uint8_t new_val;
    uint8_t prev_status = read_status(context);
    uint8_t status = 0x0;
    uint8_t r8_val = read_reg8(context, r8_code);
    uint8_t msb = r8_val >> 7;
    uint8_t prev_carry = CPU_STATUS_C_TEST(prev_status);
    
    /* Prev carry becomes the LSB */
    new_val = (r8_val << 1) | (prev_carry);
    
    /* Set MSB to be carry */
    status = CPU_STATUS_SET(status, CPU_STATUS_MASK_C, msb);

    /* Set Z flag if zero */
    /* Weird behaviour, need to zero out the Z flag if register is A*/
    status = CPU_STATUS_SET(status, CPU_STATUS_MASK_Z, (new_val == 0));

    set_status(context, status);
    write_reg8(context, r8_code, new_val);
}


/*
    Rotates 8-bit register left. Sets the carry flag
    if MSB is 1. 
*/
static void rlc_r8(cpu_context_t *context, uint8_t r8_code)
{
    uint8_t status = 0x0;
    uint8_t r8_val = read_reg8(context, r8_code);
    uint8_t msb = r8_val >> 7;
    uint8_t result = (r8_val << 1) | msb;

    status = CPU_STATUS_SET(status, CPU_STATUS_MASK_Z, (result == 0));
    status = CPU_STATUS_SET(status, CPU_STATUS_MASK_C, msb);
    write_reg8(context, r8_code, result);
    set_status(context, status);
}

/*
    Rotates 8-bit register right. Sets the carry flag
    if LSB is 1. 
*/
static void rrc_r8(cpu_context_t *context, uint8_t r8_code)
{
    uint8_t status = 0x0;
    uint8_t r8_val = read_reg8(context, r8_code);
    uint8_t lsb = r8_val & 0x1;
    uint8_t result = (r8_val >> 1) | (lsb << 7);

    status = CPU_STATUS_SET(status, CPU_STATUS_MASK_Z, (result == 0));
    status = CPU_STATUS_SET(status, CPU_STATUS_MASK_C, lsb);
    write_reg8(context, r8_code, result);
    set_status(context, status);
}

static uint16_t read_imm16(cpu_context_t *context){
    uint8_t lo = bus_read(context->pc++);
    uint8_t hi = bus_read(context->pc++);
    return (hi << 8) | lo;
}

static uint8_t read_imm8(cpu_context_t *context){
    return bus_read(context->pc++);
}

void instr_nop(cpu_context_t *context, uint8_t opcode)
{
    (void) opcode;
    
    /* Do nothing, only increment cycle count */
    context->cycles += 1; 
}

void instr_ld_r16_imm16     (cpu_context_t *context, uint8_t opcode)
{
    /* Eat two bytes from the bus (note that this is little endian) */
    uint16_t imm16 = read_imm16(context);
    uint8_t r16code = (opcode >> 4) & 0x3;

    write_reg16(context, r16code, imm16);
    context->cycles += 4;
}

void instr_ld_r16mem_a      (cpu_context_t *context, uint8_t opcode){
    
}

void instr_ld_a_r16mem      (cpu_context_t *context, uint8_t opcode);

void instr_ld_imm16mem_sp   (cpu_context_t *context, uint8_t opcode)
{
    addr_t addr = read_imm16(context);
    uint16_t sp = read_reg16(context, R16_SP);

    /* Store low half */
    bus_write(addr, sp & 0xff);
    bus_write(addr+1, sp >> 8);
    context->cycles += 5;
}

void instr_add_hl_r16       (cpu_context_t *context, uint8_t opcode);

void instr_inc_r16          (cpu_context_t *context, uint8_t opcode)
{
    uint8_t reg16_code = (opcode >> 4) & 0x3;
    uint16_t old_val = read_reg16(context, reg16_code);
    write_reg16(context, reg16_code, (uint16_t) (old_val+1));    
    context->cycles += 2;
}

void instr_dec_r16          (cpu_context_t *context, uint8_t opcode)
{
    uint8_t reg16_code = (opcode >> 4) & 0x3;
    uint16_t old_val = read_reg16(context, reg16_code);
    write_reg16(context, reg16_code, (uint16_t) (old_val-1));    
    context->cycles += 2;
}

void instr_ld_r8_imm8       (cpu_context_t *context, uint8_t opcode)
{
    uint8_t imm8 = read_imm8(context);
    uint8_t dest_num = (opcode >> 3) & 0x7;
    
    write_reg8(context, dest_num, imm8);
    
    if (dest_num == R8_HL_MEM) {
        context->cycles += 3;
        return;
    }

    context->cycles += 2;
}

void instr_rlca             (cpu_context_t *context, uint8_t opcode)
{
    (void) opcode;
    uint8_t status = read_status(context);
    rlc_r8(context, R8_A);

    /* Weird behaviour: zero flag is set regardless */
    set_status(context, status & (~CPU_STATUS_MASK_Z));
    context->cycles += 1;
}

void instr_rrca             (cpu_context_t *context, uint8_t opcode)
{
    (void) opcode;
    uint8_t status = read_status(context);
    rrc_r8(context, R8_A);

    /* Weird behaviour: zero flag is set regardless */
    set_status(context, status & (~CPU_STATUS_MASK_Z));
    context->cycles += 1;
}

void instr_rla              (cpu_context_t *context, uint8_t opcode)
{
    (void) opcode;
    uint8_t status = read_status(context);
    rl_r8(context, R8_A);

    /* Weird behaviour: zero flag is set regardless */
    set_status(context, status & (~CPU_STATUS_MASK_Z));
    context->cycles += 1;
}

void instr_rra              (cpu_context_t *context, uint8_t opcode)
{
    (void) opcode;
    uint8_t status = read_status(context);
    rr_r8(context, R8_A);

    /* Weird behaviour: zero flag is set regardless */
    set_status(context, status & (~CPU_STATUS_MASK_Z));
    context->cycles += 1;
}

void instr_daa              (cpu_context_t *context, uint8_t opcode);

void instr_cpl              (cpu_context_t *context, uint8_t opcode)
{
    (void) opcode;

    context->af.hi = ~context->af.hi;
    set_status(context, CPU_STATUS_MASK_N | CPU_STATUS_MASK_H);
    context->cycles += 1;
}

void instr_scf              (cpu_context_t *context, uint8_t opcode)
{
    (void) opcode;

    uint8_t status = 0x0;
    uint8_t prev_status = read_status(context);

    /* Preserve the Z flag, set C flag */
    status |= (prev_status & CPU_STATUS_MASK_Z) | CPU_STATUS_MASK_C; 
    set_status(context, status);
    context->cycles += 1;
}

void instr_ccf              (cpu_context_t *context, uint8_t opcode)
{
    uint8_t status = 0x0;
    uint8_t prev_status = read_status(context);

    /* Preserve the Z flag, complement C flag */
    status |= (prev_status & CPU_STATUS_MASK_Z) | (~prev_status & CPU_STATUS_MASK_C); 
    set_status(context, status);
    context->cycles += 1;
}

void instr_jr_imm8          (cpu_context_t *context, uint8_t opcode)
{
    uint8_t condition = (opcode >> 3) & 0x3;
    int8_t addr_offset = (int8_t) read_imm8(context);
    uint16_t new_addr;

    uint8_t abs_val = (addr_offset > 0) ? addr_offset : -addr_offset;

    /* Calculate address */
    if (addr_offset > 0){
        context->pc += abs_val;
    } else {
        context->pc -= abs_val;
    }

    context->cycles += 3;
}

void instr_jr_cond_imm8     (cpu_context_t *context, uint8_t opcode)
{
    uint8_t condition = (opcode >> 3) & 0x3;
    int8_t addr_offset = (int8_t) read_imm8(context);
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


void instr_stop             (cpu_context_t *context, uint8_t opcode);


void instr_ld_r8_r8         (cpu_context_t *context, uint8_t opcode)
{
    uint8_t src_num = opcode & 0x7;
    uint8_t dest_num = (opcode >> 3) & 0x7;

    uint8_t reg_val = read_reg8(context, src_num);
    write_reg8(context, dest_num, reg_val);

    if (dest_num == R8_HL_MEM) {
        context->cycles += 2;
        return;
    }

    context->cycles += 1;
}

void instr_halt             (cpu_context_t *context, uint8_t opcode);
void instr_alu_op_r8        (cpu_context_t *context, uint8_t opcode)
{
    uint8_t reg8 = opcode & 0x7;
    uint8_t alu_opcode = (opcode >> 3) & 0x7;
    uint8_t reg_val;

    reg_val = read_reg8(context, reg8);
    
    /* Set accumulator to the proper values */
    alu_op8(context, alu_opcode, reg_val);
    if (reg8 == R8_HL_MEM){
        context->cycles += 2;
    } else context->cycles += 1;
}

void instr_alu_op_imm8      (cpu_context_t *context, uint8_t opcode)
{
    uint8_t alu_opcode = (opcode >> 3) & 0x7;
    uint8_t imm8 = read_imm8(context);
    alu_op8(context, alu_opcode, imm8);

    /* Regardless of OP, this always takes two cycles */
    context->cycles += 2;
}

void instr_ret_cond         (cpu_context_t *context, uint8_t opcode)
{
    uint8_t reg_high, reg_low;
    uint8_t condition = (opcode >> 3) & 0x3;

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

void instr_ret              (cpu_context_t *context, uint8_t opcode)
{
    (void) opcode;

    /* Equivalent to POP PC */
    uint8_t reg_high, reg_low;

    reg_low = bus_read(context->sp++);
    reg_high = bus_read(context->sp++);

    context->pc = REGFULL(reg_high, reg_low);
    context->cycles += 4;
}

void instr_reti             (cpu_context_t *context, uint8_t opcode);


void instr_jp_cond          (cpu_context_t *context, uint8_t opcode)
{

    addr_t address = read_imm16(context);
    uint8_t condition = (opcode >> 3) & 0x3;
    
    if (!is_branch_taken(context, condition)) {
        context->cycles += 3;
        return;
    }

    context->pc = address;
    context->cycles += 4;
}

void instr_jp_imm16         (cpu_context_t *context, uint8_t opcode)
{
    (void) opcode;
    
    addr_t address = read_imm16(context);
    context->pc = address;
    context->cycles += 4;
}


void instr_jp_hl            (cpu_context_t *context, uint8_t opcode)
{
    (void) opcode;

    context->pc = context->hl.full;
    context->cycles += 1;
}


void instr_call_cond_imm16  (cpu_context_t *context, uint8_t opcode){
    addr_t label = read_imm16(context);
    uint8_t condition = (opcode >> 3) & 3;

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

void instr_call_imm16       (cpu_context_t *context, uint8_t opcode){
    addr_t label = read_imm16(context);

    /* Push PC to stack */
    bus_write(context->sp--, REGHIGH(context->pc));
    bus_write(context->sp--, REGLOW(context->pc));

    context->pc = label;
    context->cycles += 6;
}

void instr_rst_tgt3         (cpu_context_t *context, uint8_t opcode);


void instr_pop_r16stk       (cpu_context_t *context, uint8_t opcode)
{
    uint8_t r16stk = (opcode >> 4) & 0x3;
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


void instr_push_r16stk      (cpu_context_t *context, uint8_t opcode)
{
    uint8_t r16stk = (opcode >> 4) & 0x3;
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

void instr_cb_prefix        (cpu_context_t *context, uint8_t opcode);

void instr_ldh_cmem_a       (cpu_context_t *context, uint8_t opcode)
{
    (void) opcode;  
    addr_t addr = 0xFF00 + read_reg8(context, R8_C);
    bus_write(addr, read_reg8(context, R8_A));
    context->cycles += 2;
}

void instr_ldh_imm8mem_a    (cpu_context_t *context, uint8_t opcode)
{
    (void) opcode;  
    uint8_t imm8 = read_imm8(context);
    addr_t addr = 0xFF00 + imm8;
    bus_write(addr, read_reg8(context, R8_A));
    context->cycles += 3;
}

void instr_ld_imm16mem_a    (cpu_context_t *context, uint8_t opcode)
{
    (void) opcode;
    uint16_t imm16 = read_imm16(context);
    bus_write((addr_t) imm16, read_reg8(context, R8_A));
    context->cycles += 4;
}

void instr_ldh_a_cmem       (cpu_context_t *context, uint8_t opcode)
{
    (void) opcode;  
    addr_t addr = 0xFF00 + read_reg8(context, R8_C);
    write_reg8(context, R8_A, bus_read(addr));
    context->cycles += 2;
}

void instr_ldh_a_imm8mem    (cpu_context_t *context, uint8_t opcode)
{
    (void) opcode;
    uint8_t imm8 = read_imm8(context);
    addr_t addr = 0xFF00 + imm8;
    write_reg8(context, R8_A, bus_read(addr));
    context->cycles += 3;
}

void instr_ld_a_imm16mem    (cpu_context_t *context, uint8_t opcode)
{
    (void) opcode;
    uint16_t imm16 = read_imm16(context);
    write_reg8(context, R8_A, bus_read((addr_t) imm16));
    context->cycles += 4;
}

void instr_add_sp_imm8      (cpu_context_t *context, uint8_t opcode);
void instr_ld_hl_sp_imm8    (cpu_context_t *context, uint8_t opcode);
void instr_ld_sp_hl         (cpu_context_t *context, uint8_t opcode)
{   
    (void) opcode;
    write_reg16(context, R16_SP, read_reg16(context, R16_HL));
    context->cycles += 2;
}

void instr_di               (cpu_context_t *context, uint8_t opcode);
void instr_ei               (cpu_context_t *context, uint8_t opcode);

/**
 * ============================================================
 *                       PREFIXED CODE
 * ===========================================================
 */

void instr_rlc_r8     (cpu_context_t *context, uint8_t opcode)
{
    uint8_t r8_code = opcode & 0x7;
    rlc_r8(context, r8_code);

    if (r8_code == R8_HL_MEM){
        context->cycles += 4;
        return;
    }

    context->cycles += 2;
}

void instr_rrc_r8     (cpu_context_t *context, uint8_t opcode)
{
    uint8_t r8_code = opcode & 0x7;
    rrc_r8(context, r8_code);

    if (r8_code == R8_HL_MEM){
        context->cycles += 4;
        return;
    }

    context->cycles += 2;
}

void instr_rl_r8      (cpu_context_t *context, uint8_t opcode)
{
    uint8_t r8_code = opcode & 0x7;
    rl_r8(context, r8_code);

    if (r8_code == R8_HL_MEM){
        context->cycles += 4;
        return;
    }

    context->cycles += 2;
}

void instr_rr_r8      (cpu_context_t *context, uint8_t opcode)
{
    uint8_t r8_code = opcode & 0x7;
    rr_r8(context, r8_code);

    if (r8_code == R8_HL_MEM){
        context->cycles += 4;
        return;
    }

    context->cycles += 2;
}

void instr_sla_r8     (cpu_context_t *context, uint8_t opcode)
{
    uint8_t r8_code = opcode & 0x7;
    uint8_t status = 0x0;
    uint8_t r8_val = read_reg8(context, r8_code);
    uint8_t res = r8_val << 1;

    write_reg8(context, r8_code, res);

    status = CPU_STATUS_SET(status, 
                        CPU_STATUS_MASK_Z, (res == 0));
    
    /* Set carry if MSB is 1*/
    status = CPU_STATUS_SET(status, 
                        CPU_STATUS_MASK_C, (r8_val >> 7));

    set_status(context, status);
    if (r8_code == R8_HL_MEM){
        context->cycles += 4;
        return;
    }

    context->cycles += 2;
}

void instr_sra_r8     (cpu_context_t *context, uint8_t opcode)
{
    uint8_t r8_code = opcode & 0x7;
    uint8_t status = 0x0;
    uint8_t r8_val = read_reg8(context, r8_code);
    uint8_t sign_mask = r8_val & 0x80;

    /* Preserve sign in result */
    uint8_t res = (r8_val >> 1) | sign_mask;
    
    write_reg8(context, r8_code, res);

    status = CPU_STATUS_SET(status, 
                        CPU_STATUS_MASK_Z, (res == 0));
    
    /* Set carry if LSB is 1*/
    status = CPU_STATUS_SET(status, 
                        CPU_STATUS_MASK_C, (r8_val & 0x1));

    set_status(context, status);
    if (r8_code == R8_HL_MEM){
        context->cycles += 4;
        return;
    }

    context->cycles += 2;
}

void instr_swap_r8    (cpu_context_t *context, uint8_t opcode)
{
    uint8_t r8_code = opcode & 0x7;
    uint8_t status = 0x0;
    uint8_t r8_val = read_reg8(context, r8_code);
    uint8_t r8_swapped = 
    //    hi -> lo            lo -> hi
        (r8_val >> 4) | ((r8_val & 0xF) << 4);

    write_reg8(context, r8_code, r8_swapped);
    if (r8_code == R8_HL_MEM){
        context->cycles += 4;
        return;
    }

    status = CPU_STATUS_SET(status, 
                        CPU_STATUS_MASK_Z, (r8_swapped == 0)); 
    set_status(context, status);

    context->cycles += 2;
}

void instr_srl_r8     (cpu_context_t *context, uint8_t opcode)
{
    uint8_t r8_code = opcode & 0x7;
    uint8_t status = 0x0;
    uint8_t r8_val = read_reg8(context, r8_code);
    uint8_t res = r8_val >> 1;

    write_reg8(context, r8_code, res);

    status = CPU_STATUS_SET(status, 
                        CPU_STATUS_MASK_Z, (res == 0));
    
    /* Set carry if LSB is 1*/
    status = CPU_STATUS_SET(status, 
                        CPU_STATUS_MASK_C, (r8_val & 0x1));

    set_status(context, status);
    if (r8_code == R8_HL_MEM){
        context->cycles += 4;
        return;
    }

    context->cycles += 2;
}

void instr_bit_b3_r8  (cpu_context_t *context, uint8_t opcode)
{
    uint8_t reg_val;
    uint8_t status = read_status(context);
    uint8_t r8_code = opcode & 0x7;
    uint8_t bit_select = (opcode >> 3) & 0x7;
    uint8_t bit_set;
    uint8_t select_mask;
    uint8_t res;

    reg_val = read_reg8(context, r8_code);
    select_mask = 0x1 << bit_select;
    bit_set = (reg_val >> bit_select) & 0x1;
    res = bit_set ? 
            (reg_val | select_mask) : (reg_val & ~select_mask);
    
    status = CPU_STATUS_SET(status, CPU_STATUS_MASK_Z, (res == 0));
    status = CPU_STATUS_SET(status, CPU_STATUS_MASK_N, 0);
    status = CPU_STATUS_SET(status, CPU_STATUS_MASK_H, 1);

    set_status(context, status);
    
    if (r8_code == R8_HL_MEM){
        context->cycles += 3;
    } else context->cycles += 2;
}

void instr_res_b3_r8  (cpu_context_t *context, uint8_t opcode)
{   
    uint8_t reg_val;
    uint8_t mask;
    uint8_t r8_code = opcode & 0x7;
    uint8_t bit_select = (opcode >> 3) & 0x7;

    reg_val = read_reg8(context, r8_code);
    mask = 0x1 << bit_select;
    write_reg8(context, r8_code, reg_val & (~mask));
    
    if (r8_code == R8_HL_MEM){
        context->cycles += 4;
    } else context->cycles += 2;
}

void instr_set_b3_r8  (cpu_context_t *context, uint8_t opcode)
{
    uint8_t reg_val;
    uint8_t mask;
    uint8_t r8_code = opcode & 0x7;
    uint8_t bit_select = (opcode >> 3) & 0x7;
    assert (bit_select < 0x8);

    reg_val = read_reg8(context, r8_code);
    mask = 0x1 << bit_select;
    write_reg8(context, r8_code, reg_val | mask);
    
    if (r8_code == R8_HL_MEM){
        context->cycles += 4;
    } else context->cycles += 2;
}