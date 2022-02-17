#ifndef RVSIMULATOR_H_INCLUDED
#define RVSIMULATOR_H_INCLUDED

/*
    Created By: Hector Soto
*/

#include <fstream> //For files.
#include <iomanip> //For IO stuff.
#include <iostream> //For IO.
#include <vector> //For string manipulation.

#include "SimMemory.h"

#define REG_PC      32
#define REG_ZERO    0
#define REG_RA      1
#define REG_SP      2

#define OP_IMMEDIATE    0x13
#define OP_LOAD         0x03
#define OP_STORE        0x23
#define OP_BRANCH       0x63

class RVSimulator
{
    public:
        RVSimulator(); //CONSTRUCTOR
        ~RVSimulator(); //DESTRUCTOR
        void Simulate(const char *MemoryFile);

        int Register[33]; //Registers
        bool DebugMode = true;
    private:
        SimMemory Memory; //Simulated memory
};

#endif // RVSIMULATOR_H_INCLUDED
