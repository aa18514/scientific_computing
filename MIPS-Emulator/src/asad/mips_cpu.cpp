#include "mips.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <map>
#include "functions.h"
using namespace std; 
using namespace functions;


mips_cpu_h mips_cpu::mips_cpu_create(mips_mem_h mem)
{
    mips_error e = this->reset();
    this->set_memory(&mem);
}

void mips_cpu::set_pc(uint32_t val)
{
    pc = val;
}

void mips_cpu::set_pcNext(uint32_t val)
{
    pcNext = val;
}

uint32_t mips_cpu::get_pc()
{
    return pc;
}

uint32_t mips_cpu::get_pcNext()
{
    return pcNext;
}

void mips_cpu::set_memory(mips_mem_h* memory)
{
    mem = *memory;
}

mips_mem_h mips_cpu::get_memory()
{
    return mem;
}

mips_error mips_cpu::get_register(unsigned index, uint32_t* value)
{
    *value = regs[index];
    return mips_Success;
}

mips_error mips_cpu::get_register(unsigned index, uint64_t* value)
{
    *value = uint64_t(regs[index]);
    return mips_Success;
}

mips_error mips_cpu::get_register(unsigned index, uint8_t* value)
{
    *value = regs[index] & 0x000000FF;
    return mips_Success;
}

mips_error mips_cpu::set_register(unsigned index, uint32_t value)
{
    regs[index] = value;
    return mips_Success;
}

void mips_cpu::set_HI_register(uint32_t value)
{
    HI = value;
}

void mips_cpu::set_LO_register(uint32_t value)
{
    LO = value;
}

uint32_t mips_cpu::get_LO_register()
{
    return LO;
}

uint32_t mips_cpu::get_HI_register()
{
    return HI;
}

void mips_cpu::log(unsigned level, const char* operand, uint32_t arg, uint32_t arg2, uint32_t arg3)
{
    if (level > 0)
        fprintf(logDst, "%u %u, %u, %u.\n", operand, arg, arg2, arg3);
}

void mips_cpu::log(unsigned level, const char* operand, uint32_t arg, uint32_t arg2)
{
    if (level > 0)
        fprintf(logDst, "%u %u, %u, %u.\n", operand, arg, arg2);
}

void mips_cpu::log(unsigned level, const char* operand, uint32_t arg)
{
    if (level > 0)
        fprintf(logDst, "%u %u, %u.\n", operand, arg);
}

void mips_cpu::copy_to_memory_block(uint8_t* data, uint32_t result)
{
    data[0] = result >> 24;
    data[1] = result >> 16;
    data[2] = result >> 8;
    data[3] = result;
}

mips_error mips_cpu::reset()
{
    mips_error e;
    int i = 0;
    this->set_pc(0);
    this->set_pcNext(4);
    this->set_HI_register(0);
    this->set_LO_register(0);
    for (; i < 32; i++) {
        e = this->set_register(i, 0);
    }
    return e;
}

FILE* mips_cpu::get_file_handler()
{
    return logDst;
}

unsigned mips_cpu::get_logLevel()
{
    return logLevel;
}
void mips_cpu::set_logLevel(unsigned level)
{
    logLevel = level;
}
void mips_cpu::set_file_handler(FILE* dest)
{
    logDst = dest;
}

mips_error mips_cpu_set_debug_level(mips_cpu* state, unsigned level, FILE* dest)
{
    state->set_logLevel(level);
    state->set_file_handler(dest);
    return mips_Success;
}

mips_error check_for_invalid_alignment(uint32_t src_data)
{
    if (src_data % 2 != 0)
        return mips_ExceptionInvalidAlignment;
}

mips_error check_for_invalid_word_address(mips_cpu* state, uint32_t result)
{
    if (result % 4 != 0)
        return mips_ExceptionInvalidAddress;
}

void mips_cpu_free(mips_cpu_h state)
{
    if (state != NULL)
        delete state;
}

/*! \param pData Array of 4 bytes
	    \retval The big-endian equivlent
	*/
uint32_t to_big(const uint8_t* pData)
{
    return (((uint32_t)pData[0]) << 24)
        | (((uint32_t)pData[1]) << 16)
        | (((uint32_t)pData[2]) << 8)
        | (((uint32_t)pData[3]) << 0);
}

mips_error check_for_arithmetic_overflow(uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
    if (arg1 == arg2 && arg1 != arg3)
        return mips_ExceptionArithmeticOverflow;
}

void load_byte(mips_cpu* state, std::string operand, uint32_t dest_register, uint32_t src_register, uint32_t data_i)
{
    uint32_t reg_data, mem_data, offset;
    uint8_t data_from_mem[4];
    mips_error e;
    if (state->get_logLevel() > 0)
        fprintf(state->get_file_handler(), "%u, %u, %u, %u.\n", operand, dest_register, src_register, data_i);
    e = state->get_register(src_register, &reg_data);
    offset = (reg_data + data_i) % 4;
    if (data_i >> 15)
        data_i |= 0xFFFF0000;
    e = mips_mem_read(
        state->get_memory(),
        reg_data + data_i - offset,
        4,
        data_from_mem);
    mem_data = data_from_mem[offset];
    if (mem_data >> 7)
        mem_data |= 0xFFFFFF00;
    e = state->set_register(dest_register, mem_data);
}

void trans_high_low(const char* operand, uint32_t address, uint32_t dst, mips_cpu* state)
{
    state->log(state->get_logLevel(), operand, address);
    mips_error e;
    uint32_t result;
    if (operand == "mthi") {
        e = state->get_register(dst, &result);
        state->set_HI_register(result);
    }
    else if (operand == "mtlo") {
        e = state->get_register(dst, &result);
        state->set_LO_register(result);
    }
    else if (operand == "mfhi") {
        e = state->set_register(dst, state->get_HI_register());
    }
    else if (operand == "mflo") {
        e = state->set_register(dst, state->get_LO_register());
    }
}

int32_t pShift(std::string operand, mips_cpu* state, uint32_t dst, uint32_t src2, uint32_t src1)
{
    mips_error e;
    if (state->get_logLevel() > 0)
        fprintf(state->get_file_handler(), "%u %u, %u, %u.\n", operand, dst, src2, src1);
    uint32_t val_to_shift, val_shift_by;
    e = state->get_register(src2, &val_to_shift); 
    e = state->get_register(src1, &val_shift_by);
    if(operand == "sllv")
    		return (val_to_shift << val_shift_by);
    else 
    		return (val_to_shift >> val_shift_by);
}

mips_error decode_J_instruction(uint32_t instruction, mips_cpu* state, uint32_t opcode)
{
    unsigned log_level = state->get_logLevel();
    mips_error e;
    std::string operand;
    uint32_t address = instruction & 0x3FFFFFF;
    if (log_level >= 2)
        fprintf(state->get_file_handler(), "J - type: address = %u.\n  instr=%08x\n", address, instruction);

    switch (opcode) {
    //This is J type
    case J_Opcode:
        operand = "j";
        break;

    case JAL_Opcode:
        operand = "jal";
        e = state->set_register(31, state->get_pcNext() + 4);
        break;
    default:;
    }

    if (log_level > 0)
        fprintf(state->get_file_handler(), "%u, %u.\n", operand, address);
    state->set_pc(state->get_pcNext());
    state->set_pcNext((address << 2) | (state->get_pcNext()) & 0xF00000);
    return e;
}

mips_error decode_R_instruction(uint32_t instruction, mips_cpu* state, uint32_t opcode)
{
    mips_error e; 
    std::map<func, const char*> get_string;
    get_string[Sll] = "sll";
    get_string[Srl] = "srl";
    get_string[Sra] = "sra";
    get_string[Sllv] = "sllv";
    get_string[Srlv] = "srlv";
    get_string[Srav] = "srav";
    get_string[Jr] = "jr";
    get_string[Jalr] = "jalr";
    get_string[Mfhi] = "mfhi";
    
    unsigned log_level = state->get_logLevel();
    uint8_t memory_jump = 4;
    uint32_t src1 = (instruction >> 21) & 0x1F;
    uint32_t src2 = (instruction >> 16) & 0x1F;
    uint32_t dst =  (instruction >> 11) & 0x1F;
    uint32_t shift = (instruction >> 6) & 0x1F;
    uint32_t address, relative_address, reg;
    uint64_t va, vb, vam, vbm, result;

    func functions = func((instruction >> 0) & 0x3F);

    std::string operand;
    e = state->get_register(src1, &va);
    e = state->get_register(src2, &vb);
    vam = va >> 31;
    vbm = vb >> 31;
    // This is R-type
    if (log_level > 1) {
        fprintf(state->get_file_handler(), "R-Type : dst=%u, src1=%u, src2=%u, shift=%u, function=%u.\n  instr=%08x\n",
            dst, src1, src2, shift, functions, instruction);
    }
    switch (functions) {
    /*logging functionality, identified operands with the same log structure*/
        case Sll:
        case Sra:
        case Srl:
            state->log(log_level, get_string[functions], dst, src2, shift);
        break;

        case Jr:
            state->log(log_level, get_string[functions], src1);
        break;

        case Add:
        case Slt:
        case Sltu:
        case Sub:
        case Subu:
        case Xor:
        case And:
        case Or:
        case Stlu:
        case Addu:
            state->log(log_level, get_string[functions], dst, src1, src2);
        break;
        
        default:;
    }

    switch (functions) {

        case Sll:
            if (e == mips_Success)
                result = vb << shift;
            break;

        case Mfhi:
            if (src1 || src2 || shift == 0)
                trans_high_low("mfhi", address, dst, state);
            break;

        case Mthi:
            if (src2 || dst || shift == 0)
                trans_high_low("mthi", address, src1, state);
            break;

        case Mflo:
            if (src1 || src2 || dst == 0)
                trans_high_low("mflo", address, dst, state);
            break;

        case Mtlo:
            if (src2 || dst || shift == 0)
                trans_high_low("mtlo", address, src1, state);
            break;

        case Jalr:
            if (src2 || shift == 0) {
                if (dst == 31)
                    state->log(log_level, "jalr", src1);
                else
                    state->log(log_level, "jalr", dst, src1);
                if (src1 == dst) {
                    e = mips_ExceptionInvalidInstruction;
                }
                else {
                    result = state->get_pcNext();
                    e = state->get_register(src1, &memory_jump);
                }
            }
        break;

        case Jr:
            if (src2 || dst || shift == 0) {
                if (e == mips_Success)
                    e = check_for_invalid_word_address(state, va);
            }
        break;

        case Sra:
            if (e == mips_Success){
                result = (vb >> shift);
                result = (vb >> 31) ? ((0xFFFFFFFF << (31 - shift)) | result) : result;
            }
        break;

        case Mult:
            if (shift || dst == 0) {
                state->log(log_level, "mult", dst, src1, src2);
                result = va * vb;
                state->set_HI_register(result << 32);
                state->set_LO_register(result);
            }
        break;

        case Srav:
            state->log(log_level, "srav", dst, src2, src1);
            result = va << (vb & 0x1F);
            result = (vam) ? ((0xFFFFFFFF << (31 - (vb & 0x1F))) | result) : 0;
            e = state->set_register(dst, result);
        break;

        case Sllv:
        	result = pShift("sllv", state, dst, src2, src1);
    	break;

        case Srlv:
            result = pShift("srlv", state, dst, src2, src1);
        break;

        case Srl:
            result = (e == mips_Success) ? vb >> shift : 0; 
            e = state->set_register(dst, result);
        break;

        case Add:
            if (shift == 0) {
                result = va + vb;
                e = check_for_arithmetic_overflow(vam, vbm, result >> 31);
            }
        break;

        case Multu:
            result = va * vb;
            state->set_HI_register(result << 32);
            state->set_LO_register(result);
        break;

        case Slt:
        case Sltu:
            result = (va < vb) ? 1 : 0;
        break;

        case Subu:
            result = va - vb;
            e = check_for_arithmetic_overflow(vam, vbm, result >> 31);
        break;

        case Sub:
            result = va - vb;
        break;

        case Xor:
            result = va ^ vb;
        break;

        case And:
            result = (!shift) ? va & vb : 0;
        break;

        case Or:
            result = va | vb;
        break;

        case Addu:
            result = (!shift) ? va + vb : 0; 
            break;
        default:;
    }
    if (e == mips_Success)
        e = state->set_register(dst, result);
    state->set_pc(state->get_pcNext());
    state->set_pcNext(state->get_pcNext() + 4);
    return e;
}

mips_error decode_I_instruction(uint32_t instruction, mips_cpu* state, uint32_t opcode)
{
    unsigned log_level = state->get_logLevel();
    uint32_t src_register, dest_register, data_i, address, relative_address, result, src_data, dest_data;
    uint16_t shift_left, half_word;
    uint8_t data_from_mem[4];
    uint8_t memory_jump = 4;
    src_register = (instruction >> 21) & 0x1F;
    dest_register = (instruction >> 16) & 0x1F;
    mips_error e = state->get_register(src_register, &src_data);
    e = state->get_register(dest_register, &dest_data);
    data_i = instruction & 0x0000FFFF;
    if (log_level >= 2) {
        fprintf(state->get_file_handler(), "I-Type : dst=%u, src1=%u, src2=%u, shift=%u,  instr=%08x\n",
            dest_register, src_register, data_i, instruction);
    }

    switch (opcode) {
        case Bgtz_Opcode:
        case Addi_Opcode:
            result = data_i << 2;
            result = (result >> 31) ? result | 0xFFFF0000 : result; 
        break;
        default:;
    }

    switch (opcode) {
        uint32_t offset, total, byte, data_m;
        case Blez_Opcode:
            state->log(log_level, "BLEZ", src_register, data_i);
            if ((src_data >> 31) | (src_data == 0)) {
                memory_jump = result;
                e = check_for_invalid_word_address(state, result);
            }
        break;

        case Bgtz_Opcode:
            if (dest_register == 0) {
            	state->log(log_level, "BGTZ", src_register, data_i);
                memory_jump = (!(src_data >> 31) && src_data != 0) ? result : 4; 
                e = check_for_invalid_word_address(state, result);
            }
        break;

        case Slti_Opcode:
        case Sltiu_Opcode:
            e = (src_data < data_i)  ? state->set_register(dest_register, 1) : state->set_register(dest_register, 0); 
            if (opcode == Slti_Opcode)
                state->log(log_level, "SLTI", dest_register, src_register, data_i);
            else
                state->log(log_level, "SLTIU", dest_register, src_register, data_i);
        break;

        case Addiu_Opcode:
            state->log(log_level, "addiu", dest_register, src_register, data_i);
            data_i = (data_i >> 15) ? data_i | 0xFFFF0000 : data_i; 
            e = state->set_register(dest_register, src_data + data_i);
        break;

        case Addi_Opcode:
            state->log(log_level, "addi", dest_register, src_register, data_i);
            data_i = (data_i >> 15) ? data_i | 0xFFFF0000 : data_i; 
            result = src_data + data_i;
            e = check_for_arithmetic_overflow(src_data >> 31, data_m, result >> 31);
            if (e != mips_ExceptionArithmeticOverflow)
                e = state->set_register(dest_register, result);
        break;

        case Andi_Opcode:
            state->log(log_level, "andi", dest_register, src_register, data_i);
            e = state->set_register(dest_register, src_data & data_i);
        break;

        case Ori_Opcode:
            state->log(log_level, "ori", dest_register, src_register, data_i);
            e = state->set_register(dest_register, src_data | data_i);
            break;

        case Xori_Opcode:
            state->log(log_level, "xori", dest_register, src_register, data_i);
            e = state->set_register(dest_register, src_data ^ data_i);
        break;

        case Sw_Opcode:
            state->log(log_level, "sw", dest_register, src_register);
            state->copy_to_memory_block(data_from_mem, dest_data);
            data_i += src_data;
            if (check_for_invalid_alignment(data_i) != mips_ExceptionInvalidAlignment) 
                e = mips_mem_write(state->get_memory(), data_i, 4, data_from_mem);
        break;

        case Lui_Opcode:
            if (src_register == 0) {
                state->log(log_level, "lui", dest_register, data_i);
                e = state->set_register(dest_register, data_i << 16);
            }
        break;

        case lh_Opcode: 
            state->log(log_level, "lh", dest_register, src_register, data_i);
            offset = data_i;
            address = src_data + offset;
            relative_address = address - (address % 4);
            dest_data = (data_i >> 15) ? (0xFFFF0000 | uint32_t(data_i)) : dest_data; 
            e = (address & 0x00000001) ? mips_ExceptionInvalidAddress : mips_Success;
            if(e == mips_Success)
            {
                e = mips_mem_read(state->get_memory(), relative_address, 4, data_from_mem);
                result = (!data_from_mem) ? (((uint16_t)data_from_mem[0] << 8) | (uint16_t)data_from_mem[1]) 
                                            : (((uint16_t)data_from_mem[2] << 8) | (uint16_t)data_from_mem[3]);  
            
                result = (result >> 31) ? (0xFFFF0000 | uint32_t(result)) : result; 
                e = state->set_register(dest_register, result);
            }
        break;

        case Sb_Opcode:
            state->log(log_level, "sb", dest_register, data_i, src_register);
            total = data_i + src_data;
            byte = total % 4;
            data_from_mem[byte] = dest_data;
            e = mips_mem_write(state->get_memory(), total - byte, 4, data_from_mem);
        break;

        case Lhu_Opcode:
            state->log(log_level, "lhu", dest_register, src_register, data_i);
            dest_data = data_i;
            dest_data = (data_i >> 15) ? (0xFFFF0000 | data_i) : dest_data; 
            if ((src_register + dest_data) & 0x00000001)
                return mips_ExceptionInvalidAddress;
            else {
                e = mips_mem_read(state->get_memory(), src_register + dest_data, 4, data_from_mem);
                e = state->set_register(dest_register, to_big(data_from_mem));
            }
        break;

        case lw_OpCode:
            state->log(log_level, "lw", dest_register, src_register, data_i);
            e = mips_mem_read(state->get_memory(), src_data + data_i, 4, data_from_mem);
            e = state->set_register(
                dest_register,
                ((((uint16_t)data_from_mem[0]) << 8) | (((uint16_t)data_from_mem[1]))));
        break;

        case Sh_Opcode:
            state->log(log_level, "sh", dest_register, data_i, src_register);
            src_data += data_i;
            half_word = 0x0000FFFF & dest_data;
            uint8_t a[2];
            a[0] = (half_word >> 8) & 0xFF;
            a[1] = a[0];
            e = check_for_invalid_alignment(src_data);
            if (e != mips_ExceptionInvalidAlignment) 
                e = mips_mem_write(state->get_memory(), src_data, 2, a);
        break;

        case Beq_Opcode:
            state->log(log_level, "beq", dest_register, src_register, data_i);
            if (src_data == dest_data) {
                result = data_i << 2;
                memory_jump = (result >> 31) ? (0xFFFF0000 | uint32_t(result)) : result; 
              e = check_for_invalid_word_address(state, result);
            }
        break;

        case Branch_Opcode:
            shift_left = data_i << 2;
            result = (shift_left >> 15) ? (shift_left | 0xFFFF0000) : shift_left; 
            switch (dest_register) {
                case Bgez_dest:
                    state->log(log_level, "BGEZ", src_register, data_i);
                    if (shift_left >> 15 == 1 && src_data >> 31 == 0)
                        memory_jump = 0xFFFF0000 | shift_left;
                    else if ((shift_left >> 15) & (src_data >> 31) == 0)
                        memory_jump = shift_left;
                break;

                case Bgezal_dest:
                    state->log(log_level, "BGEZAL", src_register, data_i);
                    e = state->set_register(31, state->get_pcNext() + 4);
                    memory_jump = (!src_data >> 31) ? result : 4; 
                break;

                case Bltz_dest:
                    state->log(log_level, "BLTZ", src_register, data_i);
                    memory_jump = (src_data >> 31) ? result : 4; 
                break;

                case Bltzal_dest:
                    state->log(log_level, "BLTZAL", src_register, data_i);
                    memory_jump = (src_data >> 31) ? result : 4; 
                break;
                
                default:;
        }
        break;

        case Bne_Opcode:
            state->log(log_level, "bne", dest_register, src_register, data_i);
            if (src_data != dest_data) {
                uint16_t shift_left = data_i << 2;
                result = (shift_left >> 15) ? 0xFFFF0000 | uint32_t(shift_left) : uint32_t(shift_left);
                memory_jump = result;
            }
        break;

        case Lb_Opcode:
            load_byte(state, "lb", dest_register, src_register, data_i);
        break;

        case Lbu_Opcode:
            load_byte(state, "lbu", dest_register, src_register, data_i);
        break;
        
        default:;
    }
    state->set_pc(state->get_pcNext());
    state->set_pcNext((state->get_pcNext()) + memory_jump);
    return e;
}

mips_error mips_cpu_step(mips_cpu* state)
{
    uint32_t instruction, opcode;
    uint8_t buffer[4];
    mips_error e = mips_mem_read(state->get_memory(), state->get_pc(), 4, buffer);
    e = state->set_register(0, 0);
    instruction = to_big(buffer);
    opcode = (instruction >> 26) & 0x3F;
    e = (opcode == 0x02 || opcode == 0x03) ? decode_J_instruction(instruction, state, opcode << 26) : 
        (opcode == R_OpCode) ? decode_R_instruction(instruction, state, opcode) : 
        decode_I_instruction(instruction, state, opcode << 26); 
    return e;
}