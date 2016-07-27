#pragma once
#ifndef POOL_BITMAPPED_MEMORY_MANAGER_H
#define POOL_BITMAPPED_MEMORY_MANAGER_H

#define BIT_MAP_SIZE 2048
#define INT_SIZE  sizeof(int) * 8
#define BIT_MAP_ELEMENTS  BIT_MAP_SIZE / INT_SIZE

#include "IMemoryManager.h"
#include <vector>
#include <set>
#include <map>



class PoolBitmappedMemoryManager :	public IMemoryManager {

private:
	typedef struct ArrayInfo {
		int   MemPoolListIndex;
		int   StartPosition;
		int   Size;
	} ArrayMemoryInfo;

	//store data about a chunk of memory
	typedef struct BitMapEntry {
		int      Index;
		int      BlocksAvailable;
		int      BitMap[BIT_MAP_SIZE];
	public:
		BitMapEntry() : BlocksAvailable(BIT_MAP_SIZE) {
			memset(BitMap, 0xff, BIT_MAP_SIZE / sizeof(char));
			// initially all blocks are free and bit value 1 in the map denotes 
			// available block
		}
		void SetBit(int position, bool flag);
		void SetMultipleBits(int position, bool flag, int count);
		void SetRangeOfInt(int* element, int msb, int lsb, bool flag);
		void* FirstFreeBlock(size_t size);
		void* ObjectAddress(int pos);
		void* Head();
	} BitMapEntry;

public:
	PoolBitmappedMemoryManager();
	~PoolBitmappedMemoryManager();

	virtual void* allocate(size_t);
	virtual void freeMemory(void*);

	std::vector<void*>& GetMemoryPoolList();

private:
	void* AllocateArrayMemory(size_t size);
	void* AllocateChunkAndInitBitMap();
	void SetBlockBit(void* object, bool flag);
	void SetMultipleBlockBits(ArrayMemoryInfo* info, bool flag);

private:
	std::vector<void*> MemoryPoolList;
	std::vector<BitMapEntry> BitMapEntryList;
	//the above two lists will maintain one-to-one correspondence and hence 
	//should be of same  size.
	std::set<BitMapEntry*> FreeMapEntries;
	std::map<void*, ArrayMemoryInfo> ArrayMemoryList;


};

#endif