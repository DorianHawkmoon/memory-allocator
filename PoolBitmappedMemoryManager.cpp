#include "PoolBitmappedMemoryManager.h"
#include "Complex.h"
#include "Globals.h"
#include "Utils.h"


void BitMapEntry::SetBit(int position, bool flag) {
	BlocksAvailable += flag ? 1 : -1;
	int elementNo = position / INT_SIZE;
	int bitNo = position % INT_SIZE;
	if (flag)
		BitMap[elementNo] = BitMap[elementNo] | (1 << bitNo);
	else
		BitMap[elementNo] = BitMap[elementNo] & ~(1 << bitNo);
}

void BitMapEntry::SetMultipleBits(int position, bool flag, int count) {
	BlocksAvailable += flag ? count : -count;
	int elementNo = position / INT_SIZE;
	int bitNo = position % INT_SIZE;

	int bitSize = (count <= INT_SIZE - bitNo) ? count : INT_SIZE - bitNo;
	SetRangeOfInt(&BitMap[elementNo], bitNo + bitSize - 1, bitNo, flag);
	count -= bitSize;
	if (!count) return;

	int i = ++elementNo;
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

void BitMapEntry::SetRangeOfInt(int* element, int msb, int lsb, bool flag) {
	if (flag) {
		int mask = (unsigned(-1) << lsb) & (unsigned(-1) >> INT_SIZE - msb - 1);
		*element |= mask;
	} else {
		int mask = (unsigned(-1) << lsb) & (unsigned(-1) >> INT_SIZE - msb - 1);
		*element &= ~mask;
	}
}

void* BitMapEntry::FirstFreeBlock(size_t size) {
	for (int i = 0; i < BIT_MAP_ELEMENTS; ++i) {
		if (BitMap[i] == 0)
			continue;            // no free bit was found 

		int result = BitMap[i] & -(BitMap[i]); // this expression yields the first 
											   // bit position which is 1 in an int from right.
		void* address = 0;
		int basePos = (INT_SIZE * i);
		switch (result) {
			//make the corresponding bit 0 meaning block is no longer free
			case 0x00000001: return ComplexObjectAddress(basePos + 0);
			case 0x00000002: return ComplexObjectAddress(basePos + 1);
			case 0x00000004: return ComplexObjectAddress(basePos + 2);
			case 0x00000008: return ComplexObjectAddress(basePos + 3);
			case 0x00000010: return ComplexObjectAddress(basePos + 4);
			case 0x00000020: return ComplexObjectAddress(basePos + 5);
			case 0x00000040: return ComplexObjectAddress(basePos + 6);
			case 0x00000080: return ComplexObjectAddress(basePos + 7);
			case 0x00000100: return ComplexObjectAddress(basePos + 8);
			case 0x00000200: return ComplexObjectAddress(basePos + 9);
			case 0x00000400: return ComplexObjectAddress(basePos + 10);
			case 0x00000800: return ComplexObjectAddress(basePos + 11);
			case 0x00001000: return ComplexObjectAddress(basePos + 12);
			case 0x00002000: return ComplexObjectAddress(basePos + 13);
			case 0x00004000: return ComplexObjectAddress(basePos + 14);
			case 0x00008000: return ComplexObjectAddress(basePos + 15);
			case 0x00010000: return ComplexObjectAddress(basePos + 16);
			case 0x00020000: return ComplexObjectAddress(basePos + 17);
			case 0x00040000: return ComplexObjectAddress(basePos + 18);
			case 0x00080000: return ComplexObjectAddress(basePos + 19);
			case 0x00100000: return ComplexObjectAddress(basePos + 20);
			case 0x00200000: return ComplexObjectAddress(basePos + 21);
			case 0x00400000: return ComplexObjectAddress(basePos + 22);
			case 0x00800000: return ComplexObjectAddress(basePos + 23);
			case 0x01000000: return ComplexObjectAddress(basePos + 24);
			case 0x02000000: return ComplexObjectAddress(basePos + 25);
			case 0x04000000: return ComplexObjectAddress(basePos + 26);
			case 0x08000000: return ComplexObjectAddress(basePos + 27);
			case 0x10000000: return ComplexObjectAddress(basePos + 28);
			case 0x20000000: return ComplexObjectAddress(basePos + 29);
			case 0x40000000: return ComplexObjectAddress(basePos + 30);
			case 0x80000000: return ComplexObjectAddress(basePos + 31);
			default: break;
		}
	}
	return 0;
}

void* BitMapEntry::ComplexObjectAddress(int pos) {
	SetBit(pos, false);
	return &((static_cast<Complex*>(Head()) + (pos / INT_SIZE))
		[INT_SIZE - ((pos % INT_SIZE) + 1)]);
	return NULL;
}

void* BitMapEntry::Head() {
	return poolBitmapMemory->GetMemoryPoolList()[Index];
}




PoolBitmappedMemoryManager::PoolBitmappedMemoryManager() {
}


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
	BitMapEntry* mapEntry = &BitMapEntryList[info->MemPoolListIndex];
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
	BitMapEntry mapEntry;
	Complex* memoryBeginAddress = reinterpret_cast<Complex*>
		(new char[sizeof(Complex) * BIT_MAP_SIZE]);
	MemoryPoolList.push_back(memoryBeginAddress);
	mapEntry.Index = MemoryPoolList.size() - 1;
	BitMapEntryList.push_back(mapEntry);
	return memoryBeginAddress;
}