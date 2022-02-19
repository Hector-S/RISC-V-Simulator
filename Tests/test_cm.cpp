 /*
	Created by: Hector Soto
	Description: Program to test loading/storing data in
	memory using advanced load/store functions.
	Will write to and load from memory starting from
	a random address.
*/

#include "SimMemory.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#define TEST_COUNT 0xFFFF //2^16 = 65,536 tests

int main()
{
	uint32_t Address = 0; //What address.
	uint32_t Success = 0, RanData = 0; //Success count and random data.
	uint64_t ExtraData = 0; //Keep track of extra data attempts.
	float SucRate = 0; //Success rate
	SimMemory Memory;
	srand(time(NULL)); //Initialize rand().
	Address = rand() % 0xFFFFFFFF; //Addresses from 0 -> 0xFFFFFFFE.
	RanData = rand() % 0xFFFFFFFF;
	for(uint32_t i = 0; i < TEST_COUNT; ++i)
	{
		Memory.Store(Address + 7*i, RanData, LS_BYTE); //Store LSByte.
		Memory.Store(Address + 1 + 7*i, RanData, LS_HALF); //Store first half of data.
		Memory.Store(Address + 3 + 7*i, RanData, LS_WORD);
		ExtraData += 2;
	}
	for(uint32_t i = 0; i < TEST_COUNT; ++i)
	{
		if(Memory.Load(Address + 7*i, LS_BYTE) == (RanData & 0xFF))
		{
			++Success;
		}
		else
		{
			printf("Address: 0x%08X Data: 0x%08X | BYTE: 0x%08lX\n", Address + 7*i, RanData, Memory.Load(Address + 7*i, LS_BYTE));
		}
		if(Memory.Load(Address + 1 + 7*i, LS_HALF) == (RanData & 0xFFFF))
		{
			++Success;
		}
		else
		{
			printf("Address: 0x%08X Data: 0x%08X | HALF: 0x%08lX\n", Address + 1 + 7*i, RanData, Memory.Load(Address + 7*i, LS_HALF));
		}
		if(Memory.Load(Address + 3 + 7*i, LS_WORD) == RanData)
		{
			++Success;
		}
		else
		{
			printf("Address: 0x%08X Data: 0x%08X | WORD: 0x%08lX\n", Address + 3 + 7*i, RanData, Memory.Load(Address + 7*i, LS_WORD));
		}
	}
	SucRate = Success;
	SucRate /= TEST_COUNT + ExtraData;
	printf("Success/Failure: %i/%li | Success Rate: %.2f%%\n", Success, TEST_COUNT + ExtraData, SucRate * 100);
	return 0;
}








