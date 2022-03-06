/*
    Performs I-Type instructions.
    Created by: Hector Soto
*/
#include "RVSimulator.h"

using namespace std;

/*
    Perform I-type instructions.
    If this returns true, we hit jr ra, with ra = 0, so end simulation.
    This function assumes the opcode was already checked.
*/
bool RVSimulator::I_Instructions(uint32_t Instruction)
{
    uint8_t opcode = Instruction & 0x7F; //First 7 bits.
    uint8_t rd = (Instruction >> 7) & 0x1F; //Next 5 bits.
    uint8_t funct3 = (Instruction >> 12) & 0x07; //Next 3 bits.
    uint8_t rs1 = (Instruction >> 15) & 0x1F; //Next 5 bits.
    uint16_t imm = (Instruction >> 20) & 0x0FFF; //Last 12 bits.
    int32_t temp = 0; //Hold signed data.
    uint32_t utemp = 0; //Hold unsigned data.
    if((opcode == OP_JALR) && (funct3 == FUNC_JALR)) //JALR Instruction.
    {
        temp = imm << 20; temp = temp >> 20; //Get sign-extended immediate.
        if(rs1 == REG_RA) //Detect end of sim or jr ra with ra = 0.
        {
            if(Register[REG_RA] == 0)
            {
                if(!SilentMode) //Not in silent mode.
                {
                    if(DebugMode)
                    {
                        cout << "JALR " << RegtoStr(rd) << ',' << dec << temp << '(' << RegtoStr(rs1) << ") | ";
                    }
                    cout << RegValtoStr(rd) << ", " << RegValtoStr(rs1);
                }
                cout << "\nAt Address 0x" << hex << setw(8) << Register[REG_PC] << ": Encountered 'jr ra' with ra = 0. Indicating end of simulation.";
                cout << dec;
                return true;
            }
        }
        utemp = Register[REG_PC]; //Save address before PC is changed.
        Register[REG_PC] = (Register[rs1] + temp) & 0xFFFFFFFE; //Do JALR.
        if(rd != REG_ZERO)
        {
            Register[rd] = utemp + 4; //Save next address before PC is overwritten.
        }
        if(!SilentMode) //Not in silent mode.
        {
            if(DebugMode)
            {
                cout << "JALR " << RegtoStr(rd) << ',' << dec << temp << '(' << RegtoStr(rs1) << ") | ";
            }
            cout << RegValtoStr(rd) << ", " << RegValtoStr(rs1);
        }
        Register[REG_PC] -= 4; //We jumped, cancel increment of PC at end of main sim loop after this instruction.
        return false;
    }
    if(Instruction == OP_IMMEDIATE) //Catch NOP instruction.
    {
        if(!SilentMode && DebugMode)
        {
            cout << "NOP ";
        }
        return false;
    }
    if(rd == REG_ZERO)
    {
        cout << "Error: Destination is x0, which is invalid. Instruction ignored." << endl;
        return false;
    }
    if(opcode == OP_LOAD) //If we have a load instruction.
    {
        temp = imm << 20; temp = temp >> 20; //Get sign-extended immediate.
        switch(funct3)
        {
            case FUNC_LW: //Load a word.
                Register[rd] = Memory.Load(Register[rs1] + temp, LS_WORD);
                if(!SilentMode){cout << "LW ";}
                break;
            case FUNC_LH: //Load 2 bytes.
                temp = Memory.Load(Register[rs1] + temp, LS_HALF);
                temp = temp << 16; temp = temp >> 16; //Sign-extend 16-bit value.
                Register[rd] = temp;
                if(!SilentMode){cout << "LH ";}
                break;
            case FUNC_LB: //Load 1 byte.
                temp = Memory.Load(Register[rs1] + temp, LS_BYTE);
                temp = temp << 24; temp = temp >> 24; //Sign-extend 8-bit value.
                Register[rd] = temp;
                if(!SilentMode){cout << "LB ";}
                break;
            case FUNC_LHU: //Load 2 bytes unsigned.
                Register[rd] = Memory.Load(Register[rs1] + temp, LS_HALF);
                if(!SilentMode){cout << "LHU ";}
                break;
            case FUNC_LBU: //Load 1 byte unsigned.
                Register[rd] = Memory.Load(Register[rs1] + temp, LS_BYTE);
                if(!SilentMode){cout << "LBU ";}
                break;
        }
        if(!SilentMode) //Not in silent mode.
        {
            if(DebugMode)
            {
                cout << RegtoStr(rd) << ',' << dec << temp << '(' << RegtoStr(rs1) << ") | ";
            }
            cout << RegValtoStr(rd) << ", " << RegValtoStr(rs1);
        }
        return false;
    }

    switch(funct3)
    {
        case FUNC_ADDI: //Add immediate.
            temp = imm << 20; temp = temp >> 20; //Get sign-extended immediate.
            Register[rd] = temp + Register[rs1];
            if(!SilentMode){cout << "ADDI ";}
            break;
        case FUNC_SLTI: //Set less than immediate.
            temp = imm << 20; temp = temp >> 20; //Get sign-extended immediate.
            if(Register[rs1] < temp)
            {
                Register[rd] = 1;
            }
            else
            {
                Register[rd] = 0;
            }
            if(!SilentMode){cout << "SLTI ";}
            break;
        case FUNC_SLTIU: //Set less than immediate unsigned.
            temp = imm << 20; temp = temp >> 20; //Get sign-extended immediate.
            if(((unsigned)Register[rs1]) < ((unsigned)temp))
            {
                Register[rd] = 1;
            }
            else
            {
                Register[rd] = 0;
            }
            if(!SilentMode){cout << "SLTIU ";}
            break;
        case FUNC_XORI: //XOR immediate.
            temp = imm << 20; temp = temp >> 20; //Get sign-extended immediate.
            Register[rd] = Register[rs1] ^ temp;
            if(!SilentMode){cout << "XORI ";}
            break;
        case FUNC_ORI: //OR immediate.
            temp = imm << 20; temp = temp >> 20; //Get sign-extended immediate.
            Register[rd] = Register[rs1] | temp;
            if(!SilentMode){cout << "ORI ";}
            break;
        case FUNC_ANDI: //AND immediate.
            temp = imm << 20; temp = temp >> 20; //Get sign-extended immediate.
            Register[rd] = Register[rs1] & temp;
            if(!SilentMode){cout << "ANDI ";}
            break;
        case FUNC_SLLI: //Logical shift left.
            Register[rd] = Register[rs1] << (imm & 0x1F);
            if(!SilentMode){cout << "SLLI ";}
            break;
        case FUNC_SRLI: //Logical shift right.
            if((imm >> 5) == 0x00) //If unsigned, fill with zeros.
            {
                utemp = Register[rs1];
                utemp = utemp >> (imm & 0x1F); //Shift right.
                Register[rd] = utemp;
                if(!SilentMode){cout << "SRLI ";}
            }
            else if((imm >> 5) == 0x20) //SRAI. If signed.
            {
                Register[rd] = Register[rs1] >> (imm & 0x1F); //Fill with signed bit.
                if(!SilentMode){cout << "SRAI ";}
            }
            else
            {
                cout << "SRLI/SRAI Error: Invalid upper immediate given. Instruction ignored." << endl;
            }
            break;
    }
    if(!SilentMode) //Not in silent mode.
    {
        if(DebugMode)
        {
            cout << RegtoStr(rd) << ',' << RegtoStr(rs1) << ',' << dec << temp << " | ";
        }
        cout << RegValtoStr(rd) << ", " << RegValtoStr(rs1);
    }
    return false;
}
