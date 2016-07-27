#include "PoolBitmappedMemoryManager.h"
#include "Complex.h"
#include "Globals.h"
#include "Utils.h"


void PoolBitmappedMemoryManager::BitMapEntry::SetBit(int position, bool flag) {
	BlocksAvailable += flag ? 1 : -1;
	//index of block
	int elementNo = position / INT_SIZE;
	//position of bit inside the block
	int bitNo = position % INT_SIZE;
	//in the block, store the needed displacement of bits to set it as zero
	BitMap[elementNo] = (flag) ? BitMap[elementNo] | (1 << bitNo) 
								: BitMap[elementNo] & ~(1 << bitNo);
}

void PoolBitmappedMemoryManager::BitMapEntry::SetMultipleBits(int position, bool flag, int count) {
	BlocksAvailable += flag ? count : -count;
	int elementNo = position / INT_SIZE;
	int bitNo = position % INT_SIZE;

	int bitSize = (count <= INT_SIZE - bitNo) ? count : INT_SIZE - bitNo;
	SetRangeOfInt(&BitMap[elementNo], bitNo + bitSize - 1, bitNo, flag);
	count -= bitSize;
	if (!count) return;

	int i = ++elementNo;
	//for every block to set
	while (count >= 0) {
		if (count <= INT_SIZE) {
			SetRangeOfInt(&BitMap[i], count - 1, 0, flag);
			return;
		} else
			BitMap[i] = flag ? unsigned(-1) : 0;
		count -= 32;
		i++;
	}
}

void PoolBitmappedMemoryManager::BitMapEntry::SetRangeOfInt(int* element, int msb, int lsb, bool flag) {
	if (flag) {
		int mask = (unsigned(-1) << lsb) & (unsigned(-1) >> INT_SIZE - msb - 1);
		*element |= mask;
	} else {
		int mask = (unsigned(-1) << lsb) & (unsigned(-1) >> INT_SIZE - msb - 1);
		*element &= ~mask;
	}
}

void* PoolBitmappedMemoryManager::BitMapEntry::FirstFreeBlock(size_t size) {
	//iterate all positions (in blocks) of the bitmap
	for (int i = 0; i < BIT_MAP_ELEMENTS; ++i) {
		if (BitMap[i] == 0) //if is zero, no free bit was found, see below for explanation
			continue;

		// this expression yields the first 
		// bit position which is 1 in an int from right.
		int result = BitMap[i] & -(BitMap[i]); 
		//better explanation, a free block is all 1'
		//the bits & bits(-number) leads to all bits to zero except
		//the position in which is 1
		//example, 00110100 leads to 00100000
											   
		void* address = 0;
		//position of the first bit of the block
		int basePos = (INT_SIZE * i);

		//plus the bit position inside the block
		switch (result) {
			//make the corresponding bit 0 meaning block is no longer free
			case 0x00000001: return ObjectAddress(basePos + 0);
			case 0x00000002: return ObjectAddress(basePos + 1);
			case 0x00000004: return ObjectAddress(basePos + 2);
			case 0x00000008: return ObjectAddress(basePos + 3);
			case 0x00000010: return ObjectAddress(basePos + 4);
			case 0x00000020: return ObjectAddress(basePos + 5);
			case 0x00000040: return ObjectAddress(basePos + 6);
			case 0x00000080: return ObjectAddress(basePos + 7);
			case 0x00000100: return ObjectAddress(basePos + 8);
			case 0x00000200: return ObjectAddress(basePos + 9);
			case 0x00000400: return ObjectAddress(basePos + 10);
			case 0x00000800: return ObjectAddress(basePos + 11);
			case 0x00001000: return ObjectAddress(basePos + 12);
			case 0x00002000: return ObjectAddress(basePos + 13);
			case 0x00004000: return ObjectAddress(basePos + 14);
			case 0x00008000: return ObjectAddress(basePos + 15);
			case 0x00010000: return ObjectAddress(basePos + 16);
			case 0x00020000: return ObjectAddress(basePos + 17);
			case 0x00040000: return ObjectAddress(basePos + 18);
			case 0x00080000: return ObjectAddress(basePos + 19);
			case 0x00100000: return ObjectAddress(basePos + 20);
			case 0x00200000: return ObjectAddress(basePos + 21);
			case 0x00400000: return ObjectAddress(basePos + 22);
			case 0x00800000: return ObjectAddress(basePos + 23);
			case 0x01000000: return ObjectAddress(basePos + 24);
			case 0x02000000: return ObjectAddress(basePos + 25);
			case 0x04000000: return ObjectAddress(basePos + 26);
			case 0x08000000: return ObjectAddress(basePos + 27);
			case 0x10000000: return ObjectAddress(basePos + 28);
			case 0x20000000: return ObjectAddress(basePos + 29);
			case 0x40000000: return ObjectAddress(basePos + 30);
			case 0x80000000: return ObjectAddress(basePos + 31);
			default: break;
		}
	}
	return 0;
}

void* PoolBitmappedMemoryManager::BitMapEntry::ObjectAddress(int pos) {
	SetBit(pos, false);
	//to the head plus the blocks of memory and the concrete bit of the block
	return &((static_cast<Complex*>(Head()) + (pos / INT_SIZE))
		[INT_SIZE - ((pos % INT_SIZE) + 1)]);
}

void* PoolBitmappedMemoryManager::BitMapEntry::Head() {
	return poolBitmapMemory->GetMemoryPoolList()[Index];
}




PoolBitmappedMemoryManager::PoolBitmappedMemoryManager() {}

PoolBitmappedMemoryManager::~PoolBitmappedMemoryManager() {}


void* PoolBitmappedMemoryManager::allocate(size_t size) {
	//non array version
	if (size == sizeof(Complex)) {
		std::set<BitMapEntry*>::iterator freeMapI = FreeMapEntries.begin();
		if (freeMapI != FreeMapEntries.end()) {
			BitMapEntry* mapEntry = *freeMapI;
			return mapEntry->FirstFreeBlock(size);
		} else {
			AllocateChunkAndInitBitMap();
			FreeMapEntries.insert(&(BitMapEntryList[BitMapEntryList.size() - 1]));
			return BitMapEntryList[BitMapEntryList.size() - 1].FirstFreeBlock(size);
		}

	//array version
	} else  {
		if (ArrayMemoryList.empty()) {
			return AllocateArrayMemory(size);
		} else {
			std::map<void*, ArrayMemoryInfo>::iterator infoI = ArrayMemoryList.begin();
			std::map<void*, ArrayMemoryInfo>::iterator infoEndI = ArrayMemoryList.end();
			while (infoI != infoEndI) {
				ArrayMemoryInfo info = (*infoI).second;
				// search only in those mem blocks  
				// where allocation is done from first byte
				if (info.StartPosition != 0)  continue;             
				
				else {
					BitMapEntry* entry = &BitMapEntryList[info.MemPoolListIndex];
					if (entry->BlocksAvailable < (size / sizeof(Complex)))
						return AllocateArrayMemory(size);
					else {
						info.StartPosition = BIT_MAP_SIZE - entry->BlocksAvailable;
						info.Size = size / sizeof(Complex);
						Complex* baseAddress = static_cast<Complex*>(
							MemoryPoolList[info.MemPoolListIndex]) + info.StartPosition;

						ArrayMemoryList[baseAddress] = info;
						SetMultipleBlockBits(&info, false);

						return baseAddress;
					}
				}
			}
		}
	}
}

void PoolBitmappedMemoryManager::freeMemory(void* pointer) {
	if (ArrayMemoryList.find(pointer) == ArrayMemoryList.end())
		SetBlockBit(pointer, true);         // simple block deletion 
	else {//memory block deletion
		ArrayMemoryInfo *info = &ArrayMemoryList[pointer];
		SetMultipleBlockBits(info, true);
	}
}

std::vector<void*>& PoolBitmappedMemoryManager::GetMemoryPoolList() {
	return MemoryPoolList;
}

void PoolBitmappedMemoryManager::SetBlockBit(void* object, bool flag) {
	int i = BitMapEntryList.size() - 1;
	//iterate the list of bitmap entries
	for (; i >= 0; i--) {
		BitMapEntry* bitMap = &BitMapEntryList[i];
		
		if ((bitMap->Head() <= object) && 
			(&(static_cast<Complex*>(bitMap->Head()))[BIT_MAP_SIZE - 1] >= object)) {

			int position = static_cast<Complex*>(object) -
				static_cast<Complex*>(bitMap->Head());
			bitMap->SetBit(position, flag);
			flag ? bitMap->BlocksAvailable++ : bitMap->BlocksAvailable--;
		}
	}
}

void PoolBitmappedMemoryManager::SetMultipleBlockBits(ArrayMemoryInfo* info, bool flag) {
	//get the bitmap entry of the specified index
	BitMapEntry* mapEntry = &BitMapEntryList[info->MemPoolListIndex];
	//set the requested bits of the chunk of memory
	mapEntry->SetMultipleBits(info->StartPosition, flag, info->Size);
}

void* PoolBitmappedMemoryManager::AllocateArrayMemory(size_t size) {
	void* chunkAddress = AllocateChunkAndInitBitMap();
	ArrayMemoryInfo info;
	info.MemPoolListIndex = MemoryPoolList.size() - 1;
	info.StartPosition = 0;
	info.Size = size / sizeof(Complex);
	ArrayMemoryList[chunkAddress] = info;
	SetMultipleBlockBits(&info, false);
	return chunkAddress;
}

void* PoolBitmappedMemoryManager::AllocateChunkAndInitBitMap() {
	//prepare a chunk of memory (bitmap entry
	BitMapEntry mapEntry;
	//allocate a char space for various complex data
	Complex* memoryBeginAddress = reinterpret_cast<Complex*>
		(new char[sizeof(Complex) * BIT_MAP_SIZE]);
	//store the pointer/address to the memory pool list
	MemoryPoolList.push_back(memoryBeginAddress);
	//set the index in the bitmap memory
	//this bitmap memory has a index X on memory pool list
	mapEntry.Index = MemoryPoolList.size() - 1;
	//store the bitmap memory in its list
	BitMapEntryList.push_back(mapEntry);
	//return the new begin of the memory
	return memoryBeginAddress;
}