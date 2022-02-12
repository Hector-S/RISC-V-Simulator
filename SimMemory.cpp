/*
    Created By: Hector Soto
*/

#include "SimMemory.h"
#include "General.h"

#include <thread> //For threads.
#include <iostream>

/*
    Frees a portion of memory.
*/
bool FreeMemory(SimMemory &Memory, int ThreadCount, int ThreadNumber)
{
    int A = 0, B = 0;

    if((ThreadCount % 2) && (ThreadCount != 1) && (ThreadCount != 2) && (ThreadCount != 4)
        && (ThreadCount != 8) && (ThreadCount != 16) && (ThreadNumber < ThreadCount))
    {
        return false; //Can't split memory evenly among threads.
    }

    A = (MP_MAX / ThreadCount) * ThreadNumber; B = (MP_MAX / ThreadCount) + ((MP_MAX / ThreadCount) * ThreadNumber);
    for(int i = A; i < B; ++i)
    {
        if(Memory.MemoryPartition[i]) //If this has memory
        {
            for(int j = 0; j < MPE_MAX; ++j)
            {
                if(Memory.MemoryPartition[i]->MPElement[j])
                {
                    for(int k = 0; k < P_MAX; ++k)
                    {
                        if(Memory.MemoryPartition[i]->MPElement[j]->Page[k])
                        {
                            for(int l = 0; l < PE_MAX; ++l)
                            {
                                if(Memory.MemoryPartition[i]->MPElement[j]->Page[k]->PageElement[l])
                                {
                                    delete Memory.MemoryPartition[i]->MPElement[j]->Page[k]->PageElement[l];
                                }
                            }
                            delete Memory.MemoryPartition[i]->MPElement[j]->Page[k];
                        }
                    }
                    delete Memory.MemoryPartition[i]->MPElement[j];
                }
            }
            delete Memory.MemoryPartition[i];
        }
    }
    return true;
}

/*
    CONSTRUCTOR
*/
SimMemory::SimMemory()
{
    for(int i = 0; i < MP_MAX; ++i)
    {
        MemoryPartition[i] = nullptr;
    }
}

/*
    DESTRUCTOR
*/
SimMemory::~SimMemory()
{
    int ThreadCount = 16; //How many threads will simultaneously free memory.
    std::thread Thread[ThreadCount];
    for(int i = 0; i < ThreadCount; ++i)
    {
        Thread[i] = std::thread(FreeMemory, std::ref(*this), ThreadCount, i);
    }
    for(int i = 0; i < ThreadCount; ++i)
    {
        Thread[i].join();
    }
}

/*
    Returns pointer to Data at given Address. Will allocate memory if needed.
    Function assumes allocation of memory will never fail.
*/
uint32_t *SimMemory::GetDataPointer(uint32_t Address)
{
    //0x00 00 00 00
    uint32_t MP = Address & MP_MASK;
    uint32_t MPE = Address & MPE_MASK;
    uint32_t P = Address & P_MASK;
    uint32_t PE = Address & PE_MASK;
    uint32_t DATA = Address & DATA_MASK;
    //PE = 256 4-byte words => 2^8 = next 8 bits used to select word.
    //Last 2 bits used to select byte. Not needed in this function.
    if(!MemoryPartition[MP]) //Allocate MP if it does not exist.
    {
        MemoryPartition[MP] = new SimMemory::MP;
        for(int i = 0; i < MPE_MAX; ++i)
        {
            MemoryPartition[MP]->MPElement[i] = nullptr;
        }
    }
    if(!MemoryPartition[MP]->MPElement[MPE]) //Allocate if MPE does not exist.
    {
        MemoryPartition[MP]->MPElement[MPE] = new SimMemory::MPE;
        for(int i = 0; i < P_MAX; ++i)
        {
            MemoryPartition[MP]->MPElement[MPE]->Page[i] = nullptr;
        }
    }
    if(!MemoryPartition[MP]->MPElement[MPE]->Page[P]) //Allocate if Page does not exist.
    {
        MemoryPartition[MP]->MPElement[MPE]->Page[P] = new SimMemory::P;
        for(int i = 0; i < PE_MAX; ++i)
        {
            MemoryPartition[MP]->MPElement[MPE]->Page[P]->PageElement[i] = nullptr;
        }
    }
    if(!MemoryPartition[MP]->MPElement[MPE]->Page[P]->PageElement[PE]) //Allocate if PE does not exist.
    {
        MemoryPartition[MP]->MPElement[MPE]->Page[P]->PageElement[PE] = new SimMemory::PE;
    }
    return &MemoryPartition[MP]->MPElement[MPE]->Page[P]->PageElement[PE]->Data[DATA]; //Return address of data.
}

