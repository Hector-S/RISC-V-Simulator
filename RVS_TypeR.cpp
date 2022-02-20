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

    switch(funct3)
    {
        case FUNC_ADD: //ADD or SUB
            if(funct7 == 0x00) //ADD
            {
                Register[rd] = Register[rs1] + Register[rs2];
            }
            else if(funct7 == 0x20) //SUB
            {
                Register[rd] = Register[rs1] - Register[rs2];
            }
            else
            {
                cout << "ADD/SUB Error: Invalid funct7 given. Instruction ignored." << endl;
            }
            break;
    }
    return false;
}
