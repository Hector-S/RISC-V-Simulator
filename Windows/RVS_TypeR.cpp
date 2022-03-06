/*
    Performs R-Type instructions.
    Created by: Hector Soto
*/
#include "RVSimulator.h"

using namespace std;

/*
    Perform R-type instructions.
*/
bool RVSimulator::R_Instructions(uint32_t Instruction)
{
    //uint8_t opcode = Instruction & 0x7F; //First 7 bits.
    uint8_t rd = (Instruction >> 7) & 0x1F; //Next 5 bits.
    uint8_t funct3 = (Instruction >> 12) & 0x07; //Next 3 bits.
    uint8_t rs1 = (Instruction >> 15) & 0x1F; //Next 5 bits.
    uint8_t rs2 = (Instruction >> 20) & 0x1F; //Next 5 bits.
    uint8_t funct7 = (Instruction >> 25) & 0x7F; //Next 7 bits.
    if(rd == REG_ZERO)
    {
            cout << "Error: Destination is x0, which is invalid. Instruction ignored." << endl;
            return false;
    }
    switch(funct3)
    {
        case FUNC_ADD: //ADD or SUB
            if(funct7 == 0x00) //ADD
            {
                Register[rd] = Register[rs1] + Register[rs2];
                if(!SilentMode && DebugMode){cout << "ADD ";}
            }
            else if(funct7 == 0x20) //SUB
            {
                Register[rd] = Register[rs1] - Register[rs2];
                if(!SilentMode && DebugMode){cout << "SUB ";}
            }
            else
            {
                cout << "ADD/SUB Error: Invalid funct7 given. Instruction ignored." << endl;
            }
            break;
        case FUNC_SLL:
            if(funct7 == 0x00)
            {
                Register[rd] = Register[rs1] << (Register[rs2] & 0x1F); //Only use first 5 bits.
                if(!SilentMode && DebugMode){cout << "SLL ";}
            }
            else
            {
                cout << "SLL Error: funct7 != 0. Instruction ignored." << endl;
            }
            break;
        case FUNC_SLT:
            if(funct7 == 0x00)
            {
                Register[rd] = Register[rs1] < Register[rs2];
                if(!SilentMode && DebugMode){cout << "SLT ";}
            }
            else
            {
                cout << "SLT Error: funct7 != 0. Instruction ignored." << endl;
            }
            break;
        case FUNC_SLTU:
            if(funct7 == 0x00)
            {
                Register[rd] = ((unsigned)Register[rs1]) < ((unsigned)Register[rs2]);
                if(!SilentMode && DebugMode){cout << "SLTU ";}
            }
            else
            {
                cout << "SLTU Error: funct7 != 0. Instruction ignored." << endl;
            }
            break;
        case FUNC_XOR:
            if(funct7 == 0x00)
            {
                Register[rd] = Register[rs1] ^ Register[rs2];
                if(!SilentMode && DebugMode){cout << "XOR ";}
            }
            else
            {
                cout << "XOR Error: funct7 != 0. Instruction ignored." << endl;
            }
            break;
        case FUNC_SRL:
            if(funct7 == 0x00) //SRL
            {
                Register[rd] = (unsigned)Register[rs1] >> (Register[rs2] & 0x1F);
                if(!SilentMode && DebugMode){cout << "SRL ";}
            }
            else if(funct7 == 0x20) //SRA
            {
                Register[rd] = Register[rs1] >> (Register[rs2] & 0x1F);
                if(!SilentMode && DebugMode){cout << "SRA ";}
            }
            else
            {
                cout << "SRL/SRA Error: Invalid funct7 given. Instruction ignored." << endl;
            }
            break;
        case FUNC_OR:
            if(funct7 == 0x00)
            {
                Register[rd] = Register[rs1] | Register[rs2];
                if(!SilentMode && DebugMode){cout << "OR ";}
            }
            else
            {
                cout << "OR Error: funct7 != 0. Instruction ignored." << endl;
            }
            break;
        case FUNC_AND:
            if(funct7 == 0x00)
            {
                Register[rd] = Register[rs1] & Register[rs2];
                if(!SilentMode && DebugMode){cout << "AND ";}
            }
            else
            {
                cout << "AND Error: funct7 != 0. Instruction ignored." << endl;
            }
            break;
    }
    if(!SilentMode) //Not in silent mode.
    {
        if(DebugMode)
        {
            cout << RegtoStr(rd) << ',' << RegtoStr(rs1) << ',' << RegtoStr(rs2) << " | ";
        }
        cout << RegValtoStr(rd) << ", " << RegValtoStr(rs1) << ", " << RegValtoStr(rs1);
    }
    return false;
}
