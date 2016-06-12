#pragma once
#ifndef POOL_MEMORY_MANAGER_H
#define POOL_MEMORY_MANAGER_H
#include "IMemoryManager.h"
class PoolMemoryManager : public IMemoryManager {
public:
	PoolMemoryManager();
	virtual ~PoolMemoryManager();

	virtual void* allocate(size_t);
	virtual void freeMemory(void*);
};

#endif