/*
    Created By: Hector Soto
*/

#include "SimMemory.h"
#include "General.h"

#include <thread> //For threads.
#include <iostream>

using namespace std;

/*
    Frees a portion of memory.
*/
bool FreeMemory(SimMemory &Memory, int ThreadCount, int ThreadNumber)
{
    int A = 0, B = 0;

    if(ThreadNumber >= ThreadCount)
    {
        return false; //Invalid thread number given.
    }

    A = (MP_MAX / ThreadCount) * ThreadNumber; B = (MP_MAX / ThreadCount) + ((MP_MAX / ThreadCount) * ThreadNumber);
    for(int i = A; i < B - 1; ++i)
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
                                    //delete Memory.MemoryPartition[i]->MPElement[j]->Page[k]->PageElement[l];
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
    if((THREAD_COUNT % 2) && (THREAD_COUNT != 1) && (THREAD_COUNT != 2) && (THREAD_COUNT != 4)
        && (THREAD_COUNT != 8) && (THREAD_COUNT != 16))
    {
        //Can't split memory evenly among threads. Need to fix define that caused this.
        throw std::bad_alloc(); //Couldn't initialize SimMemory, could lead to thread errors.
    }
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
    int ThreadCount = THREAD_COUNT; //How many threads will simultaneously free memory.
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
            ReturnData &= 0x0000FFFF; //Get only 2 bytes.
            break;
        case LS_WORD: //Load 4 bytes.
            if((Address & 0x00000003) != 0) //Need to call GetDataPointer() again.
            {
                ReturnData = ReturnData >> ((Address & 0x00000003)*8); //Select wanted bytes.

                Data = GetDataPointer(Address + 4); //Get next word.
                TempData = *Data;
                ReturnData |= TempData << ((4 - (Address & 0x00000003))*8); //Merge data
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
                Mask = 0xFFFFFFFF << ((Address & 0x00000003)*8);
                *DataPointer &= Mask ^ 0xFFFFFFFF; //Erase bytes.
                *DataPointer |= Data << ((Address & 0x00000003)*8); //Write bytes to data.

                DataPointer = GetDataPointer(Address + 4); //Load next word to store next bytes.
                Mask = 0xFFFFFFFF << ((4 - (Address & 0x00000003))*8); //Highlight bytes to erase.
                *DataPointer &= Mask; //Shift leftover bytes into place.
                Data = Data >> ((4 - (Address & 0x00000003))*8);
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

/*
    Recursive of PrintFiles().
    Assumes given file was opened.
*/
bool RPrintFiles(SimMemory &Memory, int ThreadCount, int ThreadNumber, const char *FileName, int &PECount)
{
    int A = 0, B = 0;
    uint32_t Address = 0;
    string FileString = FileName;
    ofstream File; //Output file.
    if(ThreadNumber >= ThreadCount)
    {
        return false; //Invalid thread number given.
    }
    A = (MP_MAX / ThreadCount) * ThreadNumber; B = (MP_MAX / ThreadCount) + ((MP_MAX / ThreadCount) * ThreadNumber);
    FileString += "_P" + to_string(A) + "-" + to_string(B - 1) + ".txt";
    File.open(FileString.c_str());
    if(!File.is_open()) //Need to be able to open file.
    {
        cout << "Error: Unable to open '" << FileString.c_str() << "'." << endl;
        return false;
    }
    File << "  Address  |    Data   " << endl;
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
                                    ++PECount; //Increment amount of PEs that have been allocated.
                                    for(int m = 0; m < DATA_MAX; ++m)
                                    {
                                        //Must update shift values if there's changes in bit length of masks.
                                        Address = (i << 27) + (j << 21) + (k << 15) + (l << 10) + (m << 2);
                                        File << "0x" << setw(8) << hex << setfill('0') << Address << " | 0x" << setw(8) << Memory.MemoryPartition[i]->MPElement[j]->Page[k]->PageElement[l]->Data[m] << endl;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

/*
    Will print files of memory addresses with their content.
*/
bool SimMemory::PrintFiles(const char *FileName)
{
    int ThreadCount = THREAD_COUNT; //How many threads will simultaneously free memory.
    int PECount[ThreadCount];
    int TotalMemoryUsage = 0;
    thread Thread[ThreadCount];
    if(!FileName)
    {
        return false; //No file name given to load file.
    }
    for(int i = 0; i < ThreadCount; ++i)
    {
        PECount[i] = 0;
        Thread[i] = thread(RPrintFiles, std::ref(*this), ThreadCount, i, FileName, std::ref(PECount[i]));
    }
    for(int i = 0; i < ThreadCount; ++i)
    {
        Thread[i].join();
    }
    for(int i = 0; i < ThreadCount; ++i)
    {
        TotalMemoryUsage += PECount[i];
    }
    cout << "Thread Count: " << dec << THREAD_COUNT << endl;
    cout << "Total memory usage: " << TotalMemoryUsage << " KiB." << endl;
    return true; //Printed file.
}









