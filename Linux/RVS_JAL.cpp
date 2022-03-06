/*
    Performs the JAL instruction.
    Created by: Hector Soto
*/
#include "RVSimulator.h"

using namespace std;

/*
    Performs the JAL instruction.
*/
bool RVSimulator::JAL_Instruction(uint32_t Instruction)
{
    //uint8_t opcode = Instruction & 0x7F; //First 7 bits.
    uint8_t rd = (Instruction >> 7) & 0x1F; //Next 5 bits.
    uint32_t temp = (Instruction >> 12) & 0xFFFFF; //Next 20 bits.
    int32_t imm = 0; //The signed immediate.
    imm = temp >> 9; // First 10 bits.
    imm |= ((temp >> 8) & 0x1) << 10; //Get bit 11.
    imm |= (temp & 0x0FF) << 11; //Get next 8 bits
    imm |= temp & 0x80000; //Get bit 20.
    imm *= 2; //Get multiples of 2 bytes.

    if(rd != REG_ZERO)
    {
        Register[rd] = Register[REG_PC] + 4;
    }
    Register[REG_PC] += imm;
    Register[REG_PC] -= 4; //We jumped, cancel increment of PC at end of main sim loop after this instruction.
    if(!SilentMode) //Not in silent mode.
    {
        if(DebugMode)
        {
            cout << "JAL " << RegtoStr(rd) << ",0x" << hex << imm << " | ";
        }
        cout << RegValtoStr(rd);
    }
    return false;
}
