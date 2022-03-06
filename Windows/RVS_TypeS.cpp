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
            //For GUI
            StoreType = LS_WORD;
            LastAddress = Register[rs1] + temp;
            LAOldData = Memory.Load(Register[rs1] + temp, LS_WORD);
            //Normal instruction process.
            Memory.Store(Register[rs1] + temp, Register[rs2], LS_WORD);
            if(!SilentMode && DebugMode){cout << "SW ";}
            break;
        case FUNC_SH:
            //For GUI
            StoreType = LS_HALF;
            LastAddress = Register[rs1] + temp;
            LAOldData = Memory.Load(Register[rs1] + temp, LS_HALF);
            //Normal instruction process.
            Memory.Store(Register[rs1] + temp, Register[rs2], LS_HALF);
            if(!SilentMode && DebugMode){cout << "SH ";}
            break;
        case FUNC_SB:
            //For GUI
            StoreType = LS_BYTE;
            LastAddress = Register[rs1] + temp;
            LAOldData = Memory.Load(Register[rs1] + temp, LS_BYTE);
            //Normal instruction process.
            Memory.Store(Register[rs1] + temp, Register[rs2], LS_BYTE);
            if(!SilentMode && DebugMode){cout << "SB ";}
            break;
    }
    if(!SilentMode) //Not in silent mode.
    {
        if(DebugMode)
        {
            cout << RegtoStr(rs2) << ',' << dec << temp << '(' << RegtoStr(rs1) << ") | ";
        }
        cout << RegValtoStr(rs1) << ", " << RegValtoStr(rs2);
    }
    return false;
}
