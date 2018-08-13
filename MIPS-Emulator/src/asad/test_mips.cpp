#include "mips.h"
#include "functions.h"
#include <iostream>
using namespace std;
using namespace functions; 

void mask_array(uint8_t* data, uint32_t instr); 
void validate_mips_error(mips_error e, mips_error ack, const char* message, mips_cpu* cpu);

uint8_t buffer[4]; 
mips_error e;
uint32_t pc, got; 

int main()
{
    FILE* fp;
    fp = fopen("test.txt", "w");
    mips_mem_h mem = mips_mem_create_ram(4096, 4);
    mips_cpu* cpu = new mips_cpu();
    cpu->mips_cpu_create(mem);
    e = mips_cpu_set_debug_level(cpu, 4, fp);

    validate_mips_error(e, mips_Success, "mips_cpu_set_debug_level : failed.\n", cpu);
    mips_test_begin_suite();

    e = cpu->set_register(6, 0x80000000);
    e = cpu->set_register(8, 0x80000000);
    func functions;

    uint32_t instr = (R_OpCode << 26) | (4ul << 21) | (5ul << 16) | (19ul << 11) | (0ul << 6) | Slt;
    slt(mem, cpu, instr, 0);

    instr = Addi_Opcode | (9ul << 21) | (19ul << 16) | 45;
    e = cpu->set_register(9, 10);
    addi(mem, cpu, instr, 55);

    instr = Beq_Opcode | (9ul << 21) | (19ul << 16) | (0x00000002ul << 0);
    e = cpu->set_register(9, 10);
    e = cpu->set_register(19, 10);
    beq(mem, cpu, instr, (8 + 4 +  (2 << 2)));

    instr = Beq_Opcode | (9ul << 21) | (19ul << 16) | (0x00000002ul << 0);
    e = cpu->set_register(9, 10);
    e = cpu->set_register(19, 11);
    beq(mem, cpu, instr, 16 + 4 + (2 << 2));

    instr = Bgtz_Opcode | (6ul << 21) | (0ul << 16) | (3ul << 0);
    e = cpu->set_register(6, 1);
    bgtz(mem, cpu, instr, 28 + 12 + 4);

    instr = Branch_Opcode | (6ul << 21) | Bltz_dest | (3ul << 0);
    e = cpu->set_register(6, 11);
    bltz(mem, cpu, instr, 28 + 12 + 4 + 4 + 4);

    instr = Branch_Opcode | (6ul << 21) | Bltz_dest | (3ul << 0) ;
    e = cpu->set_register(6, 2);
    bltz(mem, cpu, instr, 28 + 12 + 4 + 4 + 4 + 8);

    instr = Xori_Opcode | (9ul << 21) | (19ul << 16) | 12;
    e = cpu->set_register(9, 10);
    XORI(mem, cpu, instr, 10^12);

    instr = lw_OpCode | (9ul << 21) | (19ul << 16) | Lw;
    uint8_t rec[4] = {0xFF, 0x00, 0x00, 0x00};
    e = mips_mem_write(mem, 12, 4, rec);
    e = cpu->set_register(9, 4);
    lw(mem, cpu, instr, 0xFF00);

    instr = R_OpCode | (9ul << 21) | (10ul << 16) | (11ul << 11) | (0ul << 6)| Or;
    e = cpu->set_register(9, 40);
    e = cpu->set_register(10, 50);
    OR(mem, cpu, instr, 40 | 50);

    instr = Addiu_Opcode | (9ul << 21) | (19ul << 16) | 10;
    e = cpu->set_register(9, 40);
    addiu(mem, cpu, instr, 50);

    instr = R_OpCode | (22ul << 21) | (23ul << 16) | (24ul << 11) | (0ul << 6) | Subu;
    e = cpu->set_register(22, 50);
    e = cpu->set_register(23, 40);
    subu(mem, cpu, instr, 10);

    instr = R_OpCode | (25ul << 21) | (27ul << 16) | (28ul << 11) | (0ul << 6) | Xor;
    e = cpu->set_register(25, 1);
    e = cpu->set_register(27, 1);
    XOR(mem, cpu, instr, 0);

    instr = R_OpCode | (14ul << 21) | (15ul << 16) | (16ul << 11) | (0ul << 6) | And;
    e = cpu->set_register(14, 40);
    e = cpu->set_register(15, 50);
    AND(mem, cpu, instr, (40 & 50));

    instr = R_OpCode | (4ul << 21)| (5ul << 16) | (3ul << 11) | (0ul << 6) | Sllv;
    e = cpu->set_register(4, 1);
    e = cpu->set_register(5, 2);
    e = cpu->set_register(3, 0);
    sllv(mem, cpu, instr, 4);

    instr = R_OpCode | (4ul << 21) | (5ul << 16) | (3ul << 11) | (0ul << 6) | Addu;
    e = cpu->set_register(4, 40);
    e = cpu->set_register(5, 50);
    addu(mem, cpu, instr, 90);

    instr = R_OpCode | (4ul << 21) | (5ul << 16) | (19ul << 11) | (0ul << 6) | Add;
    e = cpu->set_register(4, 0xFFFFFFFF);
    e = cpu->set_register(5, 0xFFFFFFFF);
    add(mem, cpu, instr, 0xFFFFFFFE);

    instr = R_OpCode | (4ul << 21) | (5ul << 16) | (0ul << 11) | (0ul << 6)  | Addu;
    e = cpu->set_register(4, 4294967296);
    e = cpu->set_register(5, 4294967296);
    addu(mem, cpu, instr, 0);

    instr = Sw_Opcode | (9ul << 21) | (19ul << 16) | (4ul << 0);
    e = cpu->set_register(9, 0);  
    e = cpu->set_register(19, 28);
    sw(mem, cpu, instr, 28);

    mask_array(buffer, 0xA4228926);
    e = mips_mem_write(mem, 32, 4, buffer);
    instr = Lb_Opcode | (9ul << 21)  | (19ul << 16) | (4ul << 0); 
    e = cpu->set_register(9, 29);
    LB(mem, cpu, instr, 34);

    mask_array(buffer, 0xFFFF89FF); 
    e = mips_mem_write(mem, 32, 4, buffer);
    instr = Lb_Opcode | (9ul << 21) | (19ul << 16) | (3ul << 0); // dst = r0
    e = cpu->set_register(9, 29);
    LB(mem, cpu, instr, -1);

    mask_array(buffer, 0xFFFF89FF);
    e = mips_mem_write(mem, 32, 4, buffer);
    instr = Lbu_Opcode | (9ul << 21) | (19ul << 16) | (4ul << 0); // dst = r0
    e = cpu->set_register(9, 29);
    LBU(mem, cpu, instr, 0xFFFFFFFF);

    instr = Sb_Opcode | (9ul << 21) | (19ul << 16) | (4ul << 0);
    e = cpu->set_register(9, 25);
    e = cpu->set_register(19, 10);
    SB(mem, cpu, instr, 10);

    instr = R_OpCode | (6ul << 21) | (8ul << 16) | (10ul << 11) | (0ul << 6) | Sub;
    e = cpu->set_register(6, -8);
    e = cpu->set_register(8, -2);
    sub(mem, cpu, instr, -6);

    instr = Lui_Opcode | (0ul << 21)  | (19ul << 16) | (0xFFFFul << 0);
    lui(mem, cpu, instr, 0xFFFF0000);

    instr = R_OpCode | (6ul << 21) | (8ul << 16) | (10ul << 11) | (0ul << 6) | Sub;
    e = cpu->set_register(6, -2);
    e = cpu->set_register(8, -2);
    sub(mem, cpu, instr, 0);

    instr = Bne_Opcode | (9ul << 21) | (10ul << 16) | (0ul << 0); 
    e = cpu->set_register(9, -4);
    e = cpu->set_register(10, -4);
    bne(mem, cpu, instr, cpu->get_pc() + 8);

    instr = Bne_Opcode | (6ul << 21) | (8ul << 16) | (2ul << 0); // shift = 0
    e = cpu->set_register(6, 1);
    e = cpu->set_register(8, 1);
    bne(mem, cpu, instr, cpu->get_pc() + 8);

    instr = (lh_Opcode) | (6ul << 21) | (8ul << 16) | (0ul << 0); // shift = 0
    e = cpu->set_register(6, 56);
    mask_array(buffer, 0x89261234);
    e = mips_mem_write(mem, 56, 4, buffer);
    lh(mem, cpu, instr, 0x1234);

    instr = R_OpCode | (6ul << 21) | (7ul << 16) | (10ul << 11) | (0ul << 6) | Sltu;
    e = cpu->set_register(6, 0xFFFFFFFF);
    e = cpu->set_register(7, 0x00000002);
    SLTU(mem, cpu, instr, 0);

    instr = R_OpCode | (6ul << 21) | (7ul << 16) | (10ul << 11) | (0ul << 6) |  Slt;
    slt(mem, cpu, instr, 0);

    instr = R_OpCode | (0ul << 21) | (7ul << 16) | (10ul << 11) | (3ul << 6)  | Sra;
    e = cpu->set_register(7, 64);
    SRA(mem, cpu, instr, 8);

    instr = R_OpCode | (0ul << 21) | (7ul << 16) | (10ul << 11) | (3ul << 6)  | Sra;
    e = cpu->set_register(7, -64);
    SRA(mem, cpu, instr, -8);

    instr = R_OpCode | (0ul << 21) | (7ul << 16) | (10ul << 11) | (0ul << 6)  | Sra;
    e = cpu->set_register(7, 10);
    SRA(mem, cpu, instr, 10);

    instr = R_OpCode | (8ul << 21) | (7ul << 16) | (10ul << 11) | (0ul << 6)  | Srav;
    e = cpu->set_register(8, -1);
    e = cpu->set_register(7, 0xFFFFFFFF);
    SRAV(mem, cpu, instr, -1);

    instr = R_OpCode | (8ul << 21) | (7ul << 16) | (10ul << 11) | (0ul << 6)  | Srav;
    e = cpu->set_register(8, -1);
    e = cpu->set_register(7, 0xF0000000);
    SRAV(mem, cpu, instr, -1);

    instr = R_OpCode | (8ul << 21) | (7ul << 16) | (10ul << 11) | (0ul << 6)  | Srav;
    e = cpu->set_register(8, -1);
    e = cpu->set_register(7, 0x0F000000);
    SRAV(mem, cpu, instr, -1);

    instr = R_OpCode | (0ul << 21) | (7ul << 16) | (10ul << 11) | (24ul << 6) | Srl;
    e = cpu->set_register(7, 0x0F000000);
    SRL(mem, cpu, instr, 15);

    instr = R_OpCode | (0ul << 21) | (7ul << 16) | (10ul << 11) | (45ul << 6) | Srl;
    e = cpu->set_register(7, -1);
    SRL(mem, cpu, instr, 0x7FFFF);

    instr = R_OpCode | (0ul << 21) | (7ul << 16) | (10ul << 11) | (28ul << 6) | Srl;
    e = cpu->set_register(7, -2);
    SRL(mem, cpu, instr, 15);
    uint8_t recg[4] = {0, 0, 0, 0};
    uint32_t pc1 = cpu->get_pc();
    e = mips_mem_write( mem, pc1 + 4, 4, recg);
    
    instr = J_Opcode | (10ul << 0);
    J(mem, cpu, instr, 0x00000028);

    e = cpu->set_register(6, 0x0000000A);
    e = cpu->set_register(7, 0x0000000A);
    instr = R_OpCode | (6ul << 21) | (7ul << 16) | (0ul << 11)  | (0ul << 6)  | Add;
    e = cpu->set_register(6, 0); 
    e = cpu->set_register(7, 0);
    add(mem, cpu, instr, 0);

    instr = Addi_Opcode | (9ul << 21) | (19ul << 16) | (45ul << 0);
    e = cpu->set_register(9, 0);
    addi(mem, cpu, instr, 45);

    instr = Addi_Opcode | (9ul << 21) | (19ul << 16) | (45ul << 0);
    e = cpu->set_register(9, -90);
    addi(mem, cpu, instr, -45);

    instr = Addi_Opcode | (9ul << 21) | (19ul << 16) | (45ul << 0);
    e = cpu->set_register(9, -2);
    addi(mem, cpu, instr, 43);

    instr = J_Opcode | (12ul << 0);
    J(mem, cpu, instr, 0x00000030);
    mips_test_end_suite();

    return 0;
}

void mask_array(uint8_t* data, uint32_t instr){
    for(int i = 0; i < 4; i++){
        data[i] = (instr >> (24 - 8 * i)) & 0xFF;
    }
}

void validate_mips_error(mips_error e, mips_error ack, const char* message, mips_cpu* cpu){
    if(e != ack){
        fprintf(cpu->get_file_handler(), "mips_mem_write : failed.\n");
        exit(1);
    }
}

void slt(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    pc = cpu->get_pc();
    int testId = mips_test_begin_test("slt");
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    uint32_t address = (instr >> 11) & 0x1F;
    e = cpu->get_register(address, &got);
    int passed = got == val;
    mips_test_end_test(testId, passed, "r0 <> 0");
}

void addi(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    int testId = mips_test_begin_test("addi");
    pc = cpu->get_pc();
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    int passed;
    e = mips_cpu_step(cpu);
    if (e == mips_ExceptionArithmeticOverflow) {
        fprintf(cpu->get_file_handler(), "overflow detected!.\n");
        passed = 1;
    }
    else
          validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    if (e != mips_ExceptionArithmeticOverflow) {
        uint32_t address = (instr >> 16) & 0x1F;
        e = cpu->get_register(address, &got);
        passed = got == val;
    }
    mips_test_end_test(testId, passed, "50 + 50 != 90");
}

void beq(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    int testId = mips_test_begin_test("beq");
    pc = cpu->get_pc();
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    int passed = cpu -> get_pc() == val;
    mips_test_end_test(testId, passed, "50 + 50 != 90");
}

void lw(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    int testId = mips_test_begin_test("lw");
    pc = cpu->get_pc();
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    uint32_t address = (instr >> 16) & 0x1F;
    e = cpu->get_register(address, &got);
    int passed = got == val;
    mips_test_end_test(testId, passed, "50 + 50 != 90");
}

void OR(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    int testId = mips_test_begin_test("or");
    pc = cpu->get_pc();
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    uint32_t address = (instr >> 11) & 0x1F;
    e = cpu->get_register(address, &got);
    int passed = got == val;
    mips_test_end_test(testId, passed, "50 + 50 != 90");
}

void addu(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    int testId = mips_test_begin_test("addu");
    pc = cpu->get_pc();
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    uint32_t address = ((instr >> 11) & 0x1F);
    e = cpu->get_register(address, &got);
    int passed = got == val;
    mips_test_end_test(testId, passed, "50 + 50 != 90");
}

void add(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    int passed; 
    int testId = mips_test_begin_test("add");
    pc = cpu->get_pc();
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);

    if (e == mips_ExceptionArithmeticOverflow) {
        fprintf(cpu->get_file_handler(), "overflow detected!.\n");
        passed = 1;
    }
    else
        validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);   

    if (e == mips_Success) {
        uint32_t address = ((instr >> 11) & 0x1F);
        e = cpu->get_register(address, &got);
        passed = got == val;
    }
    mips_test_end_test(testId, passed, "50 + 50 != 90");
}

void addiu(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    int testId = mips_test_begin_test("addiu");
    pc = cpu->get_pc();
    mask_array(buffer, instr);
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu); 
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    uint32_t address = ((instr >> 16) & 0x1F);
    e = cpu->get_register(address, &got);
    int passed = got == val;
    mips_test_end_test(testId, passed, "50 + 50 != 90");
}

void XOR(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    int testId = mips_test_begin_test("xor");
    pc = cpu->get_pc();
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    uint32_t address = (instr >> 11) & 0x1F;
    e = cpu->get_register(address, &got);
    int passed = got == val;
    mips_test_end_test(testId, passed, "50 + 50 != 90");
}

void AND(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    int testId = mips_test_begin_test("and");
    pc = cpu->get_pc();
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    uint32_t address = (instr >> 11) & 0x1F;
    e = cpu->get_register(address, &got);
    int passed = got == val;
    mips_test_end_test(testId, passed, "50 + 50 != 90");
}

void subu(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    int testId = mips_test_begin_test("subu");
    pc = cpu->get_pc();
    mask_array(buffer, instr);
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    uint32_t address = (instr >> 11) & 0x1F;
    e = cpu->get_register(address, &got);
    int passed = got == val;
    mips_test_end_test(testId, passed, "50 + 50 != 90");
}

void sllv(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    int testId = mips_test_begin_test("sllv");
    pc = cpu->get_pc();
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    uint32_t address = (instr >> 11) & 0x1F;
    e = cpu->get_register(address, &got);
    int passed = got == val;
    mips_test_end_test(testId, passed, "50 + 50 != 90");
}

void sw(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    int testId = mips_test_begin_test("sw");
    pc = cpu->get_pc();
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    uint8_t m[4];
    uint32_t x; 
    e = cpu->get_register( (instr >> 21) & 0x1F, &x);
    uint32_t effective_address = x + (instr & 0xFFFF);
    e = mips_mem_read(mem, effective_address, 4, m);
    got = (((uint32_t)m[0]) << 24)
        | (((uint32_t)m[1]) << 16)
        | (((uint32_t)m[2]) << 8)
        | (((uint32_t)m[3]) << 0);
    int passed = got == val;
    mips_test_end_test(testId, passed, "50 + 50 != 90");
}

void XORI(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    int testId = mips_test_begin_test("xori");
    uint32_t pc = cpu->get_pc();
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    uint32_t address = (instr >> 16) & 0x1F;
    e = cpu->get_register(address, &got);
    int passed = got == val;
    mips_test_end_test(testId, passed, "50 + 50 != 90");
}

void LB(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    int testId = mips_test_begin_test("lb");
    uint32_t pc = cpu->get_pc();
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    uint32_t address = (instr >> 16) & 0x1F;
    e = cpu -> get_register(address, &got);
    int passed = got == val;
    mips_test_end_test(testId, passed, "50 + 50 != 90");
}

void SB(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    int testId = mips_test_begin_test("sb");
    pc = cpu->get_pc();
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    uint32_t src1 = (instr >> 21) & 0x1F;
    uint16_t src2 = (instr >> 0) & 0xFFFF;
    int32_t dat = (src2 >> 15) ? int32_t(src2) : uint32_t(src2);
    e = cpu->get_register(src1, &got);
    uint32_t y = dat + got;
    uint32_t mod = y % 4;
    uint8_t data[4];
    e = mips_mem_read(mem, y - mod, 4, data);
    int passed = data[mod] == val;
    mips_test_end_test(testId, passed, "50 + 50 != 90");
}

void sub(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    int testId = mips_test_begin_test("sub");
    pc = cpu->get_pc();
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    int passed;
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);
    if (e == mips_ExceptionArithmeticOverflow) {
        fprintf(cpu->get_file_handler(), "overflow detected!.\n");
        passed = 1;
    }
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    if (e == mips_Success) {
        uint32_t address = ((instr >> 11) & 0x1F);
        e = cpu->get_register(address, &got);
        passed = got == val;
    }
    mips_test_end_test(testId, passed, "50 + 50 != 90");
}

void bne(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    int testId = mips_test_begin_test("bne");
    pc = cpu->get_pc();
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    uint8_t d[4] = {0, 0, 0, 0};
    e = mips_mem_write(mem, pc + 4, 4, d);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu); 
    e = mips_cpu_step(cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    int passed = cpu->get_pc() == val;
    mips_test_end_test(testId, passed, "50 + 50 != 90");
}

void lh(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    uint32_t address; 
    int testId = mips_test_begin_test("lh");
    pc = cpu->get_pc();
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    address = (instr >> 16) & 0x1F;
    e = cpu->get_register(address, &got);
    cout << got << endl;
    cout << val << endl; 
    int passed = got == val;
    mips_test_end_test(testId, passed, "50 + 50 != 90");
}

void LBU(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    int testId = mips_test_begin_test("lbu"); 
    pc = cpu->get_pc();
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    uint32_t address = (instr >> 16) & 0x1F;
    e = cpu->get_register(address, &got);
    int passed = got == val;
    mips_test_end_test(testId, passed, "50 + 50 != 90");
}

void SLTU(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    pc = cpu->get_pc();
    int testId = mips_test_begin_test("sltu");
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    uint32_t address = (instr >> 11) & 0x1F;
    e = cpu->get_register(address, &got);
    int passed = got == val;
    mips_test_end_test(testId, passed, "r0 <> 0");
}

void SRA(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    pc = cpu->get_pc();
    int testId = mips_test_begin_test("sra");
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    uint32_t address = (instr >> 11) & 0x1F;
    e = cpu->get_register(address, &got);
    int passed = got == val;
    mips_test_end_test(testId, passed, "r0 <> 0");
}

void SRAV(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    pc = cpu->get_pc();
    int testId = mips_test_begin_test("srav");
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    uint32_t address = (instr >> 11) & 0x1F;
    e = cpu->get_register(address, &got);
    int passed = got == val;
    mips_test_end_test(testId, passed, "r0 <> 0");
}

void SRL(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    pc = cpu->get_pc();
    int testId = mips_test_begin_test("srl");
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    uint32_t address = ((instr >> 11) & 0x1F);
    e = cpu->get_register(address, &got);
    int passed = got == val;
    mips_test_end_test(testId, passed, "r0 <> 0");
}

void J(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    pc = cpu->get_pc();
    int testId = mips_test_begin_test("j");
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    pc = cpu->get_pc();
    int passed = pc == val;
    mips_test_end_test(testId, passed, "r0 <> 0");
}

void JALR(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    int testId = mips_test_begin_test("jalr");
    pc = cpu->get_pc();
    uint32_t imp = pc + 4;
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    pc = cpu->get_pc();
    uint32_t rd = (instr >> 11) & 0x1F;
    int passed;
    uint32_t get_reg;
    e = cpu->get_register(rd, &get_reg);
    pc = cpu->get_pc();
    passed = (pc == val) && (get_reg == imp) ? 1 : 0;
    mips_test_end_test(testId, passed, "r0 <> 0");
}

void bgtz(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    uint32_t address, reg1, res;
    int testId = mips_test_begin_test("bgtz");
    pc = cpu->get_pc();
    mask_array(buffer, instr); 
    e = mips_mem_write(mem,  pc,  4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    uint8_t d[4] = {0, 0, 0, 0};
    e = mips_mem_write(mem, pc + 4, 4, d);
    e = mips_cpu_step(cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    int passed = cpu->get_pc() == val;
    mips_test_end_test(testId, passed, "50 + 50 != 90");
}

void bltz(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    int testId = mips_test_begin_test("bltz");
    pc = cpu->get_pc();
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    uint8_t d[4] = {0, 0, 0, 0};
    e = mips_mem_write(mem, pc + 4, 4, d);
    e = mips_cpu_step(cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    int passed = val == cpu->get_pc();
    mips_test_end_test(testId, passed, "50 + 50 != 90");
}

void lui(mips_mem_h mem, mips_cpu* cpu, uint32_t instr, uint32_t val)
{
    int testId = mips_test_begin_test("lui");
    uint32_t passed;
    pc = cpu->get_pc();
    mask_array(buffer, instr); 
    e = mips_mem_write(mem, pc, 4, buffer);
    validate_mips_error(e, mips_Success, "mips_mem_write : failed.\n", cpu);
    e = mips_cpu_step(cpu);
    validate_mips_error(e, mips_Success, "mips_cpu_step : failed.\n", cpu);
    uint32_t address = (instr >> 16) & 0x1F;
    e = cpu->get_register(address, &got);
    passed = got == val;
    mips_test_end_test(testId, passed, "50 + 50 != 90");
}