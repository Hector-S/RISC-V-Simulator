/*
    Performs U-Type instructions.
    Created by: Hector Soto
*/
#include "RVSimulator.h"

using namespace std;

/*
    Perform U-type instructions.
*/
bool RVSimulator::U_Instructions(uint32_t Instruction)
{
    uint8_t opcode = Instruction & 0x7F; //First 7 bits.
    uint8_t rd = (Instruction >> 7) & 0x1F; //Next 5 bits.
    uint8_t imm = (Instruction >> 12) & 0xFFFFF; //Next 20 bits.
    switch(opcode)
    {
        case OP_LUI: //Load Upper Immediate.
            Register[rd] = imm << 12; //LUI fills in low 12 bits with zeros.
            break;
        case OP_AUIPC: //Add Upper Immediate to PC.
            Register[rd] = Register[REG_PC] + (imm << 12);
            break;
    }
    return false;
}
