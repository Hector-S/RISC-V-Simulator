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
    uint32_t imm = (Instruction >> 12) & 0xFFFFF; //Next 20 bits.
    if(rd == REG_ZERO)
    {
        cout << "Error: Destination is x0, which is invalid. Instruction ignored." << endl;
        return false;
    }
    switch(opcode)
    {
        case OP_LUI: //Load Upper Immediate.
            Register[rd] = imm << 12; //LUI fills in low 12 bits with zeros.
            if(!SilentMode){cout << "LUI ";}
            break;
        case OP_AUIPC: //Add Upper Immediate to PC.
            Register[rd] = Register[REG_PC] + (imm << 12); //If before JALR, it saves the address before JALR or its own address.
            if(!SilentMode){cout << "AUIPC ";}
            break;
    }
    if(!SilentMode) //Not in silent mode.
    {
        if(DebugMode)
        {
            cout << RegtoStr(rd) << ",0x" << hex << imm << " | ";
        }
        cout << RegValtoStr(rd);
    }
    return false;
}
