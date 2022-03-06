#ifndef RVSIMULATOR_H_INCLUDED
#define RVSIMULATOR_H_INCLUDED

/*
    Created By: Hector Soto
*/

#include <cstdint> //For standard types.
#include <fstream> //For files.
#include <iomanip> //For IO stuff.
#include <iostream> //For IO.
#include <vector> //For string manipulation.

#include "SimMemory.h" //For simulated memory.

#define REG_PC      32
#define REG_ZERO    0
#define REG_RA      1
#define REG_SP      2

//Instruction formats.
#define OP_IMMEDIATE    0x13    //I-type format.
#define OP_LOAD         0x03    //Load instructions are in the immediate instructions function.
#define OP_STORE        0x23    //S-type format.
#define OP_BRANCH       0x63    //B-type format.
#define OP_REGISTER     0x33    //R-type format.
#define OP_AUIPC        0x17    //U-type format. IMPLEMENTED - COMPLETE
#define OP_LUI          0x37    //U-type format. IMPLEMENTED - COMPLETE
#define OP_JAL          0x6F    //J-type format.
#define OP_JALR         0x67    //I-type format. IMPLEMENTED - COMPLETE
//Funct3 codes for instructions. Total instructions = 36. Left to do: 19
#define FUNC_JALR       0x0     //IMPLEMENTED - COMPLETE
#define FUNC_BEQ        0x0     //IMPLEMENTED - COMPLETE
#define FUNC_BNE        0x1     //IMPLEMENTED - COMPLETE
#define FUNC_BLT        0x4     //IMPLEMENTED - COMPLETE
#define FUNC_BGE        0x5     //IMPLEMENTED - COMPLETE
#define FUNC_BLTU       0x6     //IMPLEMENTED - COMPLETE
#define FUNC_BGEU       0x7     //IMPLEMENTED - COMPLETE
#define FUNC_LB         0x0     //IMPLEMENTED - COMPLETE
#define FUNC_LH         0x1     //IMPLEMENTED - COMPLETE
#define FUNC_LW         0x2     //IMPLEMENTED - COMPLETE
#define FUNC_LBU        0x4     //IMPLEMENTED - COMPLETE
#define FUNC_LHU        0x5     //IMPLEMENTED - COMPLETE
#define FUNC_SB         0x0     //IMPLEMENTED - COMPLETE
#define FUNC_SH         0x1     //IMPLEMENTED - COMPLETE
#define FUNC_SW         0x2     //IMPLEMENTED - COMPLETE
#define FUNC_ADDI       0x0     //IMPLEMENTED - COMPLETE
#define FUNC_SLTI       0x2     //IMPLEMENTED - COMPLETE
#define FUNC_SLTIU      0x3     //IMPLEMENTED - COMPLETE
#define FUNC_XORI       0x4     //IMPLEMENTED - COMPLETE
#define FUNC_ORI        0x6     //IMPLEMENTED - COMPLETE
#define FUNC_ANDI       0x7     //IMPLEMENTED - COMPLETE
#define FUNC_SLLI       0x1     //IMPLEMENTED - COMPLETE
#define FUNC_SRLI       0x5     //IMPLEMENTED - COMPLETE
#define FUNC_SRAI       0x5     //IMPLEMENTED - COMPLETE
#define FUNC_ADD        0x0     //IMPLEMENTED - COMPLETE
#define FUNC_SUB        0x0     //IMPLEMENTED - COMPLETE
#define FUNC_SLL        0x1     //IMPLEMENTED - COMPLETE
#define FUNC_SLT        0x2     //IMPLEMENTED - COMPLETE
#define FUNC_SLTU       0x3     //IMPLEMENTED - COMPLETE
#define FUNC_XOR        0x4     //IMPLEMENTED - COMPLETE
#define FUNC_SRL        0x5     //IMPLEMENTED - COMPLETE
#define FUNC_SRA        0x5     //IMPLEMENTED - COMPLETE
#define FUNC_OR         0x6     //IMPLEMENTED - COMPLETE
#define FUNC_AND        0x7     //IMPLEMENTED - COMPLETE
//      FUNC_NOP                //IMPLEMENTED - COMPLETE



std::string RegtoStr(uint8_t RegisterValue); //To generate string based off given register.

class RVSimulator
{
    public:
        RVSimulator(); //CONSTRUCTOR
        ~RVSimulator(); //DESTRUCTOR
        void Simulate(const char *MemoryFile, bool GUI); //If GUI is true, the GUI will launch.

        int Register[33]; //Registers They're signed by default.
        std::string TraceFileName; //If this is not empty, output will redirect to this file instead of the terminal.
        bool DebugMode = false; //If true, will print debug info.
        bool MemoryTraceFile = false; //If true, will print files of memory addresses with their content.
        bool ProtectInstructions = false; //If enabled, instructions are stored in memory that can't be overwritten.
        bool HexRegister = false; //Register values will be printed in hex if true. In decimal otherwise.
        bool SilentMode = false; //Instructions won't print output to terminal.
    private:
        SimMemory Memory; //Simulated memory.
        SimMemory InstructionMemory; //Separate memory to hold instructions if enabled.

        bool I_Instructions(uint32_t Instruction); //Perform I-type instructions. If this returns true, we hit jr ra, with ra = 0, so end simulation.
        bool S_Instructions(uint32_t Instruction); //Perform S-type instructions.
        bool R_Instructions(uint32_t Instruction); //Perform R-type instructions.
        bool U_Instructions(uint32_t Instruction); //Perform U-type instructions.
        bool B_Instructions(uint32_t Instruction); //Perform B-type instructions.
        bool JAL_Instruction(uint32_t Instruction); //Perform the JAL Instruction.

        bool InstructionLoop(); //Does one iteration of the simulation.
        bool ReadFile(const char *MemoryFile);
        std::string RegValtoStr(uint8_t RegisterValue); //To generate string value based off given register.

        //GUI Stuff
        void RVS_GUI();
        //This is for the GUI to track what memory locations were written to so that it can reverse memory to a previous state.
        int StoreType = -1; //What kind of memory store was performed. -1 = no memory store happened.
        uint32_t LastAddress = 0; //Last address a store wrote to.
        uint32_t LAOldData = 0; //Data before the store occurred.
};

#endif // RVSIMULATOR_H_INCLUDED
