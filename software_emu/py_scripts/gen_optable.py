import os
from collections import Counter

InstructionStr = str
BitFilter = str

OPTABLE_FILTERS : dict[BitFilter, InstructionStr] = {
    # Block 0
    '0000_0000': 'instr_nop',
    '00xx_0001': 'instr_ld_r16_imm16',
    '00xx_0010': 'instr_ld_r16mem_a',
    '00xx_1010': 'instr_ld_a_r16mem',
    '0000_1000': 'instr_ld_imm16mem_sp',
    
    '00xx_0011': 'instr_inc_r16',
    '00xx_1011': 'instr_dec_r16',
    '00xx_1001': 'instr_add_hl_r16',
    '00xx_x100': 'instr_inc_r8',
    '00xx_x101': 'instr_dec_r8',  # decr
    '00xx_x110': 'instr_ld_r8_imm8', # ld r8 imm8
    
    '0000_0111': 'instr_rlca', 
    '0000_1111': 'instr_rrca', 
    '0001_0111': 'instr_rla', 
    '0001_1111': 'instr_rra',
    '0010_0111': 'instr_daa',
    '0010_1111': 'instr_cpl',
    '0011_0111': 'instr_scf',
    '0011_1111': 'instr_ccf',
    
    '0001_1000': 'instr_jr_imm8',
    '001x_x000': 'instr_jr_cond_imm8',
    '0001_0000': 'instr_stop',

    # Block 1
    '01xx_xyyy': 'instr_ld_r8_r8',
    '0111_0110': 'instr_halt',
    
    # Block 2 (8 bit ALU operations)
    '10xx_xyyy': 'instr_alu_op_r8',
    
    # Block 3
    '11xx_x110': 'instr_alu_op_imm8',    # immediate 8-bit alu
    '110x_x000': 'instr_ret_cond',
    '1100_1001': 'instr_ret',
    '1101_1001': 'instr_reti',
    '110x_x010': 'instr_jp_cond',
    '1100_0011': 'instr_jp_imm16',
    '1110_1001': 'instr_jp_hl',
    '110x_x100': 'instr_call_cond_imm16',
    '1100_1101': 'instr_call_imm16',
    '11xx_x111': 'instr_rst_tgt3',
    
    '11xx_0001': 'instr_pop_r16stk',
    '11xx_0101': 'instr_push_r16stk',
    
    '1100_1011': 'instr_cb_prefix',
    
    '1110_0010': 'instr_ldh_cmem_a',
    '1110_0000': 'instr_ldh_imm8mem_a',
    '1110_1010': 'instr_ld_imm16mem_a',
    '1111_0010': 'instr_ldh_a_cmem',
    '1111_0000': 'instr_ldh_a_imm8mem',
    '1111_1010': 'instr_ld_a_imm16mem',
    
    '1110_1000': 'instr_add_sp_imm8',
    '1111_1000': 'instr_ld_hl_sp_imm8',
    '1111_1001': 'instr_ld_sp_hl',
    
    '1111_0011': 'instr_di',
    '1111_1011': 'instr_ei',
}

PREFIX_OPFILTERS = {
    '0000_0xxx': 'instr_rlc_r8',
    '0000_1xxx': 'instr_rrc_r8',
    '0001_0xxx': 'instr_rl_r8',
    '0001_1xxx': 'instr_rr_r8',
    '0010_0xxx': 'instr_sla_r8',
    '0010_1xxx': 'instr_sra_r8',
    '0011_0xxx': 'instr_swap_r8',
    '0011_1xxx': 'instr_srl_r8',
    
    '01xxx_xxx': 'instr_bit_b3_r8',
    '10xxx_xxx': 'instr_res_b3_r8',
    '11xxx_xxx': 'instr_set_b3_r8',
}

# Recursive helper function to expand the filters
def generate_expansion(og_str: str, index: int, generated_str, str_set: set):
    if (index == len(og_str)):
        str_set.add(generated_str)
        return
    
    s_i = og_str[index]
    if s_i != '0' and s_i != '1':
        generate_expansion(og_str, index+1, generated_str + '0', str_set)
        generate_expansion(og_str, index+1, generated_str + '1', str_set)
    else:
        generate_expansion(og_str, index+1, generated_str + s_i, str_set)

def expand_bitfilter(bitfilter: BitFilter):
    bitfilter = bitfilter.replace('_', '')
    replacement_sets : set = set()
    generate_expansion(bitfilter, 0, '', replacement_sets)
    return replacement_sets

def create_optable(opfilter: dict[BitFilter, InstructionStr]):
    optable = {}
    
    for bitfilter, func_name in opfilter.items():
        expansion_set = expand_bitfilter(bitfilter)
        for op_byte in expansion_set:
            optable[int(op_byte,2)] = func_name
            
    return dict(sorted(optable.items()))

def main():
    
    optable = create_optable(OPTABLE_FILTERS)
    prefix_optable = create_optable(PREFIX_OPFILTERS)
    py_dir = os.path.dirname(__file__)
    template_path = os.path.join(py_dir, 'template_optable.h')
    
    with open(template_path, 'r') as f:
        template_file_str = f.read()
    
    replacement_str = "\n"
    prefix_entries = "\n"
    
    uninmp_func_name = 'instr_unimplemented'
    
    for i in range(0, 0x100):
        replacement_str += f"\t[0x{i:02x}] \t= \t{optable.get(i, uninmp_func_name)}, \n"
        prefix_entries += f"\t[0x{i:02x}] \t= \t{prefix_optable.get(i, uninmp_func_name)}, \n"
    
    # for index, func_name in optable.items():
    #     replacement_str += f"\t[0x{index:02x}] \t= \t{func_name}, \n"
    # pass
    # for index, func_name in prefix_optable.items():
        
    # pass

    optable_file = template_file_str.replace("/*[REPLACE_ME]*/", replacement_str)
    optable_file = optable_file.replace("/*PREFIX_OPTABLE*/", prefix_entries)
    
    with open(os.path.join(py_dir, 'optable.h'), 'w') as f:
        f.write(optable_file)
    
    
if __name__ == '__main__':
    main()