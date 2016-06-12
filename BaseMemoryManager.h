#pragma once
#ifndef BASE_MEMORY_MANAGER_H
#define BASE_MEMORY_MANAGER_H
#include "IMemoryManager.h"
class BaseMemoryManager : public IMemoryManager {
public:
	BaseMemoryManager();
	virtual ~BaseMemoryManager();

	virtual void* allocate(size_t);
	virtual void freeMemory(void*);
};
	
#endif