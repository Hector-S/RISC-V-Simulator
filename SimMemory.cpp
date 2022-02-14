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
    //Must update shift values if there's changes in bit length of masks.
    uint32_t MP = (Address & MP_MASK) >> (32 - 5);
    uint32_t MPE = (Address & MPE_MASK) >> (32 - 11);
    uint32_t P = (Address & P_MASK) >> (32 - 17);
    uint32_t PE = (Address & PE_MASK) >> (32 - 22);
    uint32_t DATA = (Address & DATA_MASK) >> (32 - 30);
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

/*
    Loads data from memory;
*/
uint64_t SimMemory::Load(uint32_t Address, int Type)
{
    uint64_t ReturnData = LS_FAIL; //Return failure code if not overwritten.
    uint64_t Mask = 0; //For holding masks.
    uint64_t TempData = 0; //To hold extra data.
    uint32_t *Data = nullptr;

    Data = GetDataPointer(Address);
    ReturnData = *Data;
    switch(Type)
    {
        case LS_BYTE: //Load a byte.
            ReturnData = ReturnData >> ((Address & 0x00000003)*8); //Select correct byte of data to return.
            ReturnData &= 0x000000FF; //Get only 1 byte.
            break;
        case LS_HALF: //Load 2 bytes.
            if((Address & 0x00000003) != 3) //Load 2-bytes. Only 1 GetDataPointer() call needed.
            {
                ReturnData = ReturnData >> ((Address & 0x00000003)*8); //Select correct bytes of data to return.
                ReturnData &= 0x0000FFFF; //Get only 2 bytes.
            }
            else
            {
                ReturnData = ReturnData >> ((Address & 0x00000003)*8); //Select correct byte of data to return.
                ReturnData &= 0x000000FF; //Get only 1 byte.
                Data = GetDataPointer(Address + 4); //Load next word to get next byte.
                TempData = *Data;
                TempData &= 0x000000FF; //Isolate byte.
                ReturnData |= TempData << 8; //Insert byte.
            }
            break;
        case LS_WORD: //Load 4 bytes.
            if((Address & 0x00000003) != 0) //Need to call GetDataPointer() again.
            {
                ReturnData = ReturnData >> ((Address & 0x00000003)*8); //Select wanted bytes.

                Data = GetDataPointer(Address + 4); //Get next word.
                TempData = *Data;
                Mask = (0xFFFFFFFF << ((Address & 0x00000003)*8)) ^ 0xFFFFFFFF;
                TempData &= Mask << ((4 - (Address & 0x00000003))*8);
                ReturnData |= TempData; //Merge data
                ReturnData &= 0xFFFFFFFF; //Make sure we're not sending garbage.
            }
            //Else all bytes already in ReturnData.
            break;
    }
    return ReturnData; //Return failure code if there's an error for some reason.
}

/*
    Stores data into memory.
*/
uint64_t SimMemory::Store(uint32_t Address, uint32_t Data, int Type)
{
    uint64_t ReturnData = LS_FAIL; //Return failure code if not overwritten.
    uint64_t Mask = 0; //For holding masks.
    uint32_t *DataPointer = nullptr;
    DataPointer = GetDataPointer(Address);
    switch(Type)
    {
        case LS_BYTE: //Store a byte.
            Data &= 0x000000FF;
            Mask = (0x000000FF << ((Address & 0x00000003)*8)) ^ 0xFFFFFFFF; //Highlight byte to erase.
            *DataPointer &= Mask; //Erase byte.
            *DataPointer |= Data << ((Address & 0x00000003)*8); //Write byte to data.
            ReturnData = 0; //Success.
            break;
        case LS_HALF: //Store 2 bytes.
            Data &= 0x0000FFFF;
            if((Address & 0x00000003) != 3) //Load 2-bytes. Only 1 GetDataPointer() call needed.
            {
                Mask = (0x0000FFFF << ((Address & 0x00000003)*8)) ^ 0xFFFFFFFF; //Highlight bytes to erase.
                *DataPointer &= Mask; //Erase bytes.
                *DataPointer |= Data << ((Address & 0x00000003)*8); //Write bytes to data.
            }
            else
            {
                Mask = 0x00FFFFFF; //Highlight byte to erase.
                *DataPointer &= Mask; //Erase byte.
                *DataPointer |= (Data & 0x000000FF) << (3*8); //Write byte to data.

                DataPointer = GetDataPointer(Address + 4); //Load next word to store next byte.
                Data = Data >> 8; //Get only second byte.
                *DataPointer &= 0xFFFFFF00; //Erase byte.
                *DataPointer |= Data; //Write byte to data.
            }
            ReturnData = 0; //Success.
            break;
        case LS_WORD: //Store 4 bytes.
            if((Address & 0x00000003) != 0) //Need to call GetDataPointer() again.
            {
                Mask = 0xFFFFFFFF << ((Address & 0x00000003)*8); //Highlight bytes to erase.
                *DataPointer &= Mask ^ 0xFFFFFFFF; //Erase bytes.
                *DataPointer |= Data << ((Address & 0x00000003)*8); //Write bytes to data.

                DataPointer = GetDataPointer(Address + 4); //Load next word to store next bytes.
                Data = Data; //Shift leftover bytes into place.
                *DataPointer |= Data;
            }
            else //No bit manipulation needed
            {
                *DataPointer = Data; //Store word.
            }
            ReturnData = 0; //Success.
            break;
    }
    return ReturnData;
}











