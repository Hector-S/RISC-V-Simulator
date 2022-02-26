/*
    Created By: Hector Soto
*/

#include "RVSimulator.h"

using namespace std;

/*
    Generates string based off given register value.
*/
string RegtoStr(uint8_t RegisterValue)
{
    string ReturnString = "BAD_REG";
    if(RegisterValue == 0)
    {
        ReturnString = "zero";
    }
    else if(RegisterValue == 1)
    {
        ReturnString = "ra";
    }
    else if(RegisterValue == 2)
    {
        ReturnString = "sp";
    }
    else if(RegisterValue == 3)
    {
        ReturnString = "gp";
    }
    else if(RegisterValue == 4)
    {
        ReturnString = "tp";
    }
    else if((RegisterValue >= 5) && (RegisterValue <= 7))
    {
        ReturnString = "t" + to_string(RegisterValue - 5);
    }
    else if(RegisterValue == 8)
    {
        ReturnString = "s0";
    }
    else if(RegisterValue == 9)
    {
        ReturnString = "s1";
    }
    else if((RegisterValue >= 10) && (RegisterValue <= 17))
    {
        ReturnString = "a" + to_string(RegisterValue - 10);
    }
    else if((RegisterValue >= 18) && (RegisterValue <= 27))
    {
        ReturnString = "s" + to_string(RegisterValue - 16);
    }
    else if((RegisterValue >= 28) && (RegisterValue <= 31))
    {
        ReturnString = "t" + to_string(RegisterValue - 25);
    }
    else if(RegisterValue == 32)
    {
        ReturnString = "pc";
    }
    return ReturnString;
}

/*
    Generates string value based off given register.
*/
string RVSimulator::RegValtoStr(uint8_t RegisterValue)
{
    std::stringstream ReturnString;
    if((RegisterValue >= 0) && (RegisterValue <= 32))
    {
        if(HexRegister == true)
        {
            ReturnString << "0x" << hex << setw(8) << setfill('0') << Register[RegisterValue];
        }
        else
        {
            ReturnString << dec << Register[RegisterValue];
        }
    }
    else
    {
        ReturnString << "BAD_REG";
    }
    return ReturnString.str();
}

/*
    CONSTRUCTOR
*/
RVSimulator::RVSimulator()
{
    Register[REG_PC] = 0;
    Register[REG_ZERO] = 0;
    Register[REG_RA] = 0;
    Register[REG_SP] = 0;
}

/*
    DESTRUCTOR
*/
RVSimulator::~RVSimulator()
{

}

void RVSimulator::Simulate(const char *MemoryFile)
{
    ifstream File; //Filed being loaded.
    ofstream OutputFile; //Output file.
    uint32_t Address = 0, Instruction = 0; //To hold address and instruction loaded from file.
    uint32_t Opcode = 0; //Identify opcode.
    string InputData; //Hold input data as string.
    bool Reason = false; //Set to true for specific read fail reason.
    bool DataType = true;
    bool EndSim = false;
    if(!MemoryFile) //Need memory file to simulate.
    {
        cout << "Error: No memory file given." << endl;
        Reason = true; goto TRACE_FAIL;
        return;
    }
    File.open(MemoryFile); //Open the selected file for simulation.
    if(!File.is_open()) //Need to be able to open file.
    {
        cout << "Error: Unable to open '" << MemoryFile << "'." << endl;
        Reason = true; goto TRACE_FAIL;
    }
    cout << hex << setfill('0');
    if(DebugMode && !SilentMode)
    {
        cout << "---| Data Read from File |---" << endl;
    }
    while(File.peek() != EOF) //Loads data until EOF.
    {
        File >> InputData;
        InputData = "0x" + InputData;
        if(DataType) //Get Address
        {
            InputData.pop_back(); //Remove last charater ':' from address.
            try
            {
                Address = std::stoul(InputData, nullptr, 16);
            }
            catch(invalid_argument &)
            {
                cout << "Failed to get address. Was given '" << InputData << "'" << endl;
                Reason = true; goto TRACE_FAIL;
            }
        }
        else //Get instruction data.
        {
            try
            {
                Instruction = std::stoul(InputData, nullptr, 16);
                if(ProtectInstructions) //If we want to protect instructions from being overwritten.
                {
                    InstructionMemory.Store(Address, Instruction, LS_WORD);
                }
                else
                {
                    Memory.Store(Address, Instruction, LS_WORD);
                }
                Memory.Store(Address, Instruction, LS_WORD);
                if(DebugMode && !SilentMode)
                {
                    cout << "Address 0x" << setw(8) << Address << " | Data: 0x" << setw(8) << Instruction << endl;
                }
            }
            catch(invalid_argument &)
            {
                cout << "Failed to get instruction. Was given '" << InputData << "'" << endl;
                Reason = true; goto TRACE_FAIL;
            }
        }
        DataType = !DataType;
    }

    //-----| SIMULATION STARTS HERE |-----
    cout << "---| Simulation Messages |---" << endl;
    if(!SilentMode)
    {
        cout << "  Address  | Instruction ";
        if(DebugMode)
        {
            cout << "| Decoded | rd, rs1, rs2" << endl;
        }
        else
        {
            cout << "| rd, rs1, rs2" << endl;
        }
    }
    while(!EndSim) //While given no signal to end simulation.
    {
        if(ProtectInstructions) //If we're protecting instructions.
        {
            Instruction = InstructionMemory.Load(Register[REG_PC], LS_WORD);
        }
        else
        {
            Instruction = Memory.Load(Register[REG_PC], LS_WORD);
        }
        if(Instruction == 0) //All 0s indicate a trap.
        {
            cout << "WARNING: Sim ended because it encountered a trap instruction of 0x00000000" << endl;
            break;
        }
        if(!SilentMode)
        {
            cout << "0x" << hex << setw(8) << Register[REG_PC] << " | 0x" << setw(8) << Instruction << "  | "; //Print address and current instruction.
        }
        Opcode = Instruction & 0x0000007F;
        switch(Opcode)
        {
            case OP_IMMEDIATE: //I-type instructions.
                I_Instructions(Instruction);
                break;
            case OP_JALR:
                EndSim = I_Instructions(Instruction);
                break;
            case OP_LOAD:
                I_Instructions(Instruction);
                break;
            case OP_STORE:  //S-type instructions.
                S_Instructions(Instruction);
                break;
            case OP_REGISTER: //R-type instructions.
                R_Instructions(Instruction);
                break;
            case OP_BRANCH: //B-type instructions.
                B_Instructions(Instruction);
                break;
            case OP_LUI: //Load upper immediate.
                U_Instructions(Instruction);
                break;
            case OP_AUIPC: //Load upper immediate.
                U_Instructions(Instruction);
                break;
            default:
                cout << "WARNING: Unrecognized instruction found. Instruction ignored.";
                break;
        }
        if(!SilentMode)
        {
            cout << endl; //End line.
        }
        if(!EndSim) //Don't increment PC, sim has ended.
        {
            Register[REG_PC] += 4;
        }
    }
    if(SilentMode)
    {
        cout << endl; //End line.
    }
    if(DebugMode || SilentMode) //Print out registers.
    {
        cout << "---| Register Data (PC = 32) |---" << endl;
        for(int i = 0; i < 33; ++i)
        {
            cout << "Register-" << setw(2) << left << dec << setfill(' ') << i << ": 0x" << setw(8) << right << setfill('0') <<  hex << Register[i] << " | dec: " << dec << Register[i] << endl;
        }
    }
    if(MemoryTraceFile && !SilentMode)
    {
        cout << "---| Memory Trace File Info |---" << endl;
        Memory.PrintFiles("MemoryData");
    }
    cout << dec << setfill(' ');
    return;
TRACE_FAIL: //Failed to read a request.
    cout << dec << setfill(' ');
    if(Reason) //If a reason was given.
    {
        cout << "Failed to simulate file, reason is above." << endl;
    }
    return;
}
