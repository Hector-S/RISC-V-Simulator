/*
    Performs S-Type instructions.
    Created by: Hector Soto
*/
#include "RVSimulator.h"

using namespace std;

/*
    Perform S-type instructions.
*/
bool RVSimulator::S_Instructions(uint32_t Instruction)
{
    //uint8_t opcode = Instruction & 0x7F; //First 7 bits.
    uint8_t imm0 = (Instruction >> 7) & 0x1F; //Next 5 bits.
    uint8_t funct3 = (Instruction >> 12) & 0x07; //Next 3 bits.
    uint8_t rs1 = (Instruction >> 15) & 0x1F; //Next 5 bits.
    uint8_t rs2 = (Instruction >> 20) & 0x1F; //Next 5 bits.
    uint16_t imm1 = (Instruction >> 25) & 0x7F; //Next 7 bits.
    int32_t temp = 0; //Hold signed data.
    imm1 = (imm1 << 5) | imm0; //Combine immediate value.
    temp = imm1 << 20; temp = temp >> 20; //Get sign-extended immediate.
    switch(funct3)
    {
        case FUNC_SW:
            Memory.Store(Register[rs1] + temp, Register[rs2], LS_WORD);
            break;
        case FUNC_SH:
            Memory.Store(Register[rs1] + temp, Register[rs2], LS_HALF);
            break;
        case FUNC_SB:
            Memory.Store(Register[rs1] + temp, Register[rs2], LS_BYTE);
            break;
    }
    return false;
}
