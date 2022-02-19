/*
	Created by: Hector Soto
	Description: Program to test basic memory management
	of SimMemory.
	Will write to and load from memory at random addresses
	for a random amount of *contiguous* data.
*/

#include "SimMemory.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#define TEST_COUNT 0xFFFF //2^16 = 65,536 tests

int main()
{
	uint32_t Address = 0, Amount = 0; //What address and how many 4-byte words.
	uint32_t Success = 0, RanData = 0; //Success count and random data.
	uint32_t *Data = nullptr;
	uint64_t ExtraData = 0; //Keep track of extra data attempts.
	float SucRate = 0; //Success rate
	SimMemory Memory;
	srand(time(NULL)); //Initialize rand().
	for(uint32_t i = 0; i < TEST_COUNT; ++i)
	{
		Address = rand() % 0xFFFFFFFF; //Addresses from 0 -> 0xFFFFFFFE.
		Amount = rand() % 512 + 1; //Will write 1 to 512 words.
		RanData = rand() % 0xFFFFFFFF;
		ExtraData += Amount - 1;
		for(uint32_t j = 0; j < Amount; ++j)
		{
			Data = Memory.GetDataPointer(Address + j*4); //j*4 to get to next word location.
			*Data = RanData;
			Data = Memory.GetDataPointer(Address + j*4);
			if(*Data == RanData)
			{
				++Success;
			}
			else
			{
				printf("Error: Mismatch at address 0x%08X | 0x%08X != 0x%08X\n", Address + Amount, *Data, RanData);
			}
		}
	}
	SucRate = Success;
	SucRate /= TEST_COUNT + ExtraData;
	printf("Success/Failure: %i/%li | Success Rate: %.2f%%\n", Success, TEST_COUNT + ExtraData, SucRate * 100);
	return 0;
}

