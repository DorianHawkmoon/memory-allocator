#pragma once
#ifndef I_MEMORY_MANAGER_H
#define I_MEMORY_MANAGER_H
class IMemoryManager {
public:
	virtual void* allocate(size_t) = 0;
	virtual void freeMemory(void*) = 0;
};
#endif