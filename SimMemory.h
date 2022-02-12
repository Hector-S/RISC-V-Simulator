#ifndef SIMMEMORY_H_INCLUDED
#define SIMMEMORY_H_INCLUDED

#include <cstdint>

#define LSTR_FAIL 0xF0F0F0F0F0F0F0F0 //If this value is returned, there was an error.

#define MP_MAX 32
#define MPE_MAX 64
#define P_MAX 64
#define PE_MAX 32
#define DATA_MAX 256 //32-bit Data -> 1 KiB of data or 256 4-byte words.
//^ Each MAX must be at least 16 to not break initialization of memory.

//These masks need to be updated if the X_MAXs are changed.
#define MP_MASK     0xF8000000 //32 = 2^5 = top 5 bits.
#define MPE_MASK    0x07E00000 //64 = 2^6 = next 6 bits
#define P_MASK      0x001F8000 //64 = 2^6 = next 6 bits
#define PE_MASK     0x00007C00 //32 = 2^5 = next 5 bits
#define DATA_MASK   0x000003FC //256 = 2^8 = next 8 bits

class SimMemory
{
    public:
        SimMemory(); //Constructor
        ~SimMemory(); //Destructor
        uint32_t *GetDataPointer(uint32_t Address); //Returns pointer to Data at given Address. Will allocate memory if needed.
        uint64_t Load(uint32_t Address, int Type); //Loads data from memory
        uint64_t Store(uint32_t Address, uint32_t Data, int Type); //Stores data into memory
    private:
        struct PE //Page Element
        {
            uint32_t Data[DATA_MAX];
        };
        struct P //Page
        {
            PE *PageElement[PE_MAX];
        };
        struct MPE //Memory Partition Element
        {
            P *Page[P_MAX];
        };
        struct MP //Memory Partition
        {
            MPE *MPElement[MPE_MAX];
        };
        MP *MemoryPartition[MP_MAX]; //Total memory. All pointers at all stages are initialized to nullptr.

        friend bool FreeMemory(SimMemory &Memory, int ThreadCount, int ThreadNumber); //Initialize portion of memory.
};

#endif // SIMMEMORY_H_INCLUDED
