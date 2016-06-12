#pragma once
#include "IMemoryManager.h"
#include "Log.h"

class Complex {
public:
	Complex(double a, double b): r(a), c(b) {
	}
	~Complex() {
	}

	void* operator new (size_t size, IMemoryManager& manager){
		return manager.allocate(size);
	}// class' default ctor implicitly called here

	void* operator new[](size_t size, IMemoryManager& manager) {
		return  manager.allocate(size);
	}// class default ctor implicitly called here

	void operator delete (void* pointerToDelete, IMemoryManager& manager) {
		manager.freeMemory(pointerToDelete);
	}// class' dtor implicitly called at this point

	void operator delete[](void* arrayToDelete, IMemoryManager& manager) {
		manager.freeMemory(arrayToDelete);
	}// class' dtor implicitly called at this point

private:
	//Real part
	double r;
	//Complex part
	double c;
};

