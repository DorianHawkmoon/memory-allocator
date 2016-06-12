#include "BaseMemoryManager.h"
#include <stdlib.h>
#include "Log.h"

BaseMemoryManager::BaseMemoryManager() {
}
BaseMemoryManager::~BaseMemoryManager() {}

void* BaseMemoryManager::allocate(size_t size) {
	return malloc(size);
}
void BaseMemoryManager::freeMemory(void* pointer) {
	free(pointer);
}
