#pragma once
#ifndef POOL_MEMORY_MANAGER_H
#define POOL_MEMORY_MANAGER_H

#define POOLSIZE 32

#include "IMemoryManager.h"


class PoolMemoryManager : public IMemoryManager {
public:
	PoolMemoryManager();
	virtual ~PoolMemoryManager();

	virtual void* allocate(size_t);
	virtual void freeMemory(void*);

private:
	struct Store {
		Store* next;

	};

	Store* head;

private:
	void expandPoolSize();
	void cleanUp();

};

#endif