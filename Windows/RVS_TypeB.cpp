/*
    Performs B-Type instructions.
    Created by: Hector Soto
*/
#include "RVSimulator.h"

using namespace std;

/*
    Perform U-type instructions.
*/
bool RVSimulator::B_Instructions(uint32_t Instruction)
{
    //uint8_t opcode = Instruction & 0x7F; //First 7 bits.
    uint8_t imm0 = (Instruction >> 7) & 0x1F; //Next 5 bits.
    uint8_t funct3 = (Instruction >> 12) & 0x07; //Next 3 bits.
    uint8_t rs1 = (Instruction >> 15) & 0x1F; //Next 5 bits.
    uint8_t rs2 = (Instruction >> 20) & 0x1F; //Next 5 bits.
    uint16_t imm1 = (Instruction >> 25) & 0x7F; //Next 7 bits.
    int32_t temp = 0; //Hold signed data.
    //Get signed immediate.
    temp = imm0 >> 1; //Insert first 4 bits.
    temp |= imm1 << 4; //Insert next 6 bits.
    temp |= (imm1 & 0x40) << 10; //Insert last bit.
    temp = temp << 20; temp = temp >> 20; //Sign extend temp.
    temp *= 2; //Branch instruction uses 2-byte interval.
    switch(funct3)
    {
        case FUNC_BEQ:
            if(Register[rs1] == Register[rs2])
            {
                Register[REG_PC] += temp; //Add immediate.
                if(temp != 0)
                {
                    Register[REG_PC] -= 4; //We jumped, cancel increment of PC at end of main sim loop after this instruction.
                }
                else
                {
                    cout << "WARNING-B: Branched to self. Instruction ignored." << endl;
                }
            }
            if(!SilentMode){cout << "BEQ ";}
            break;
        case FUNC_BNE:
            if(Register[rs1] != Register[rs2])
            {
                Register[REG_PC] += temp; //Add immediate.
                if(temp != 0)
                {
                    Register[REG_PC] -= 4; //We jumped, cancel increment of PC at end of main sim loop after this instruction.
                }
                else
                {
                    cout << "WARNING-B: Branched to self. Instruction ignored." << endl;
                }
            }
            if(!SilentMode){cout << "BNE ";}
            break;
        case FUNC_BLT:
            if(Register[rs1] < Register[rs2])
            {
                Register[REG_PC] += temp; //Add immediate.
                if(temp != 0)
                {
                    Register[REG_PC] -= 4; //We jumped, cancel increment of PC at end of main sim loop after this instruction.
                }
                else
                {
                    cout << "WARNING-B: Branched to self. Instruction ignored." << endl;
                }
            }
            if(!SilentMode){cout << "BLT ";}
            break;
        case FUNC_BGE:
            if(Register[rs1] >= Register[rs2])
            {
                Register[REG_PC] += temp; //Add immediate.
                if(temp != 0)
                {
                    Register[REG_PC] -= 4; //We jumped, cancel increment of PC at end of main sim loop after this instruction.
                }
                else
                {
                    cout << "WARNING-B: Branched to self. Instruction ignored." << endl;
                }
            }
            if(!SilentMode){cout << "BGE ";}
            break;
        case FUNC_BLTU:
            if(((unsigned)Register[rs1]) < ((unsigned)Register[rs2]))
            {
                Register[REG_PC] += temp; //Add immediate.
                if(temp != 0)
                {
                    Register[REG_PC] -= 4; //We jumped, cancel increment of PC at end of main sim loop after this instruction.
                }
                else
                {
                    cout << "WARNING-B: Branched to self. Instruction ignored." << endl;
                }
            }
            if(!SilentMode){cout << "BLTU ";}
            break;
        case FUNC_BGEU:
            if(((unsigned)Register[rs1]) >= ((unsigned)Register[rs2]))
            {
                Register[REG_PC] += temp; //Add immediate.
                if(temp != 0)
                {
                    Register[REG_PC] -= 4; //We jumped, cancel increment of PC at end of main sim loop after this instruction.
                }
                else
                {
                    cout << "WARNING-B: Branched to self. Instruction ignored." << endl;
                }
            }
            if(!SilentMode){cout << "BGEU ";}
            break;
    }
    if(!SilentMode) //Not in silent mode.
    {
        if(DebugMode)
        {
            cout << RegtoStr(rs1) << ',' << RegtoStr(rs2) << ',' << dec << temp << " | ";
        }
        cout << RegValtoStr(rs1) << ", " << RegValtoStr(rs2);
    }
    return false;
}
