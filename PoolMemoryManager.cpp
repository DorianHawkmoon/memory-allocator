#include "PoolMemoryManager.h"

#include "Complex.h"

PoolMemoryManager::PoolMemoryManager() {
	head = 0;
	expandPoolSize();
}


PoolMemoryManager::~PoolMemoryManager() {
	cleanUp();
}

void* PoolMemoryManager::allocate(size_t size) {
	if (0 == head) {
		expandPoolSize();
	}

	Store* newStore = head;
	head = newStore->next;
	return newStore;
}

void PoolMemoryManager::freeMemory(void* pointer) {
	Store* previous = static_cast <Store*> (pointer);
	previous->next = head;
	head = previous;
}

void PoolMemoryManager::expandPoolSize() {
	//convert it for more class, not only complex
	size_t size = (sizeof(Complex) > sizeof(Store*)) ?
		sizeof(Complex) : sizeof(Store*);
	Store* head2 = reinterpret_cast <Store*> (new char[size]);
	head = head2;

	for (int i = 0; i < POOLSIZE; i++) {
		head2->next = reinterpret_cast <Store*> (new char[size]);
		head2 = head2->next;
	}

	head2->next = 0;
}

void PoolMemoryManager::cleanUp() {
	Store* nextPtr = head;
	for (; nextPtr; nextPtr = head) {
		head = head->next;
		delete[] nextPtr; // remember this was a char array
	}
}