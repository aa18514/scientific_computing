/*! \file mips_test.h
    Defines the functions used to test a processor.
*/
#ifndef functions_header
#define functions_header
#include "mips.h"


/* This allows the header to be used from both C and C++, so
programs can be written in either (or both) languages. */
#ifdef __cplusplus
extern "C"{
#endif 
void slt(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void addi(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void beq(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void lw(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void addiu(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void addu(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void AND(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void OR(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void sllv(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void subu(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void sw(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void XOR(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void add(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void XORI(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void LB(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void SB(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void sub(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void bne(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void lh(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void LBU(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void SLTU(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void SRA(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void SRAV(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void SRL(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void J(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void bgtz(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void bltz(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);
void lui(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val);

namespace functions
{
enum dest {
    Bltz_dest = 0x00 << 16, 
    Bltzal_dest = 0x10 << 16,
    Bgez_dest = 0x01 << 16, 
    Bgezal_dest = 0x11 << 16
};
enum opcodes {
	R_OpCode = 0x00 << 26,
	lw_OpCode = 0x23 << 26,
	Addiu_Opcode = 0x09 << 26,
	Xori_Opcode = 0x0E << 26,
	Addi_Opcode = 0x08 << 26,
	Beq_Opcode = 0x04 << 26,
	lh_Opcode = 0x21 << 26,
	Bgtz_Opcode = 0x07 << 26,
	Sw_Opcode = 0x2B << 26,
	Lb_Opcode = 0x20 << 26,
	Lbu_Opcode = 0x24 << 26,
	J_Opcode = 0x02 << 26,
    JAL_Opcode = 0x03 << 26,
	Bne_Opcode = 0x05 << 26,
	Sb_Opcode = 0x28 << 26,
	Lui_Opcode = 0x0F << 26, 
    Slti_Opcode = 0x0A << 26, 
    Sltiu_Opcode = 0x0B << 26, 
    Andi_Opcode = 0x0C << 26, 
    Ori_Opcode = 0x0D << 26,
    Branch_Opcode = 0x01 << 26,
    Lhu_Opcode = 0x25 << 26,
    Sh_Opcode = 0x29 << 26,
    Blez_Opcode = 0x26 << 26
};
enum func { 
		Sll = 0x00,
        Srl = 0x02,
        Sra = 0x03,
        Sllv = 0x04,
        Srlv = 0x06,
        Srav = 0x07,
        Jr = 0x08,
        Jalr = 0x09,
        Mfhi = 0x10,
        Mthi = 0x11,
        Add = 0x20, 
        Slt = 0x2A, 
        Sltu = 0x2B,
        Sub = 0x22, 
        Subu = 0x23, 
        Stlu = 0x29, 
        Addu = 0x21, 
        Xor = 0x26, 
        And = 0x24,
        Addi = 0x45,
        Xori = 0x16, 
        Or = 0x25, 
    	Lw = 0x08,
        Mflo = 0x12,
        Mtlo = 0x13, 
        Mult = 0x18, 
        Multu = 0x19
    };
}
#ifdef __cplusplus
};
#endif

#endif
