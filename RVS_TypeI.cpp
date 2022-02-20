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
        //Need to sign->extend immediate.
        if(rd == REG_ZERO) //Check for JR pseudo-instruction.
        {
            if(rs1 == 0)
            {
                cout << "JALR Error: rs1 = x0. This indicates an EBREAK. Instruction ignored." << endl;
                return false;
            }
            if(rs1 == REG_RA)
            {
                if(imm != 0)
                {
                    cout << "JR Error: Immediate should be zero. Instruction ignored." << endl;
                }
                else
                {
                    if(Register[REG_RA] == 0)
                    {
                        cout << "At Address 0x" << hex << setw(8) << Register[REG_PC] << ": Encountered 'jr ra' with ra = 0. Indicating end of simulation." << endl;
                        cout << dec;
                        return true;
                    }
                    cout << "JR Error: ra != 0. Instruction ignored." << endl;
                }
            }
            else
            {
                cout << "JALR Error: Can't write to x0. Instruction ignored." << endl;
            }
            return false;
        }
        //Perform JALR.
        temp = imm << 20; temp = temp >> 20; //Get sign-extended immediate.
        Register[REG_PC] = (Register[rs1] + temp) & 0xFFFFFFFE;
        Register[rd] = Register[REG_PC] + 4;
        return false;
    }
    if(opcode == OP_LOAD) //If we have a load instruction.
    {
        if(rd == REG_ZERO)
        {
            cout << "LOAD Error: Destination is x0, which is invalid. Instruction ignored." << endl;
            return false;
        }
        temp = imm << 20; temp = temp >> 20; //Get sign-extended immediate.
        switch(funct3)
        {
            case FUNC_LW: //Load a word.
                Register[rd] = Memory.Load(Register[rs1] + temp, LS_WORD);
                break;
            case FUNC_LH: //Load 2 bytes.
                temp = Memory.Load(Register[rs1] + temp, LS_HALF);
                temp = temp << 16; temp = temp >> 16; //Sign-extend 16-bit value.
                Register[rd] = temp;
                break;
            case FUNC_LB: //Load 1 byte.
                temp = Memory.Load(Register[rs1] + temp, LS_BYTE);
                temp = temp << 24; temp = temp >> 24; //Sign-extend 8-bit value.
                Register[rd] = temp;
                break;
            case FUNC_LHU: //Load 2 bytes unsigned.
                Register[rd] = Memory.Load(Register[rs1] + temp, LS_HALF);
                break;
            case FUNC_LBU: //Load 1 byte unsigned.
                Register[rd] = Memory.Load(Register[rs1] + temp, LS_BYTE);
                break;
        }
        return false;
    }
    switch(funct3)
    {
        case FUNC_ADDI: //Add immediate.
            temp = imm << 20; temp = temp >> 20; //Get sign-extended immediate.
            Register[rd] = temp + Register[rs1];
            break;
        case FUNC_SLLI: //Logical shift left.
            Register[rd] = Register[rs1] << (imm & 0x1F);
            break;
        case FUNC_SRLI: //Logical shift right.
            if((imm >> 5) == 0x00) //If unsigned, fill with zeros.
            {
                utemp = Register[rs1];
                utemp = utemp >> (imm & 0x1F); //Shift right.
                Register[rd] = utemp;
            }
            else if((imm >> 5) == 0x20) //SRAI. If signed.
            {
                Register[rd] = Register[rs1] >> (imm & 0x1F); //Fill with signed bit.
            }
            else
            {
                cout << "SRLI/SRAI Error: Invalid upper immediate given. Instruction ignored." << endl;
            }
            break;
    }
    return false;
}
