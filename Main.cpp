#include "Complex.h"
#include "Log.h"
#include "Timer.h"
#include "Utils.h"
#include "Globals.h"

template<class T>
void deleteOverride(IMemoryManager& manager, T* &pointer) {
	if (pointer) {
		pointer->~T();
		manager.freeMemory(pointer);
		pointer = nullptr;
	}
}

void test() {
	
	int iterations = 5000;
	
	
	for (int i = 0; i < iterations; i++) {
		Complex* arrayNumber=new(*poolBitmapMemory) Complex[1000];
		//create the numbers
		//for (int j = 0; j < 1000; j++) {
		//	arrayNumber[j] = new(*poolBitmapMemory) Complex(i, j);
		//}

		////delete the numbers
		//for (int j = 0; j < 1000; j++) {
		//	deleteOverride(*poolBitmapMemory, arrayNumber[j]);
		//}
		deleteOverride(*poolBitmapMemory, arrayNumber);
	}
}

int main(int argc, char** argv) {
	LOG("Start main", "");

	baseMemory = new BaseMemoryManager();
	poolMemory = new PoolMemoryManager();
	poolBitmapMemory = new PoolBitmappedMemoryManager();
	
	int loop = 5;
	int count = 0;
	double total = 0;
	while (count < loop) {
		total += functionTime(test);
		count++;
	}
	double measure = total / (double) (loop);
	LOG(to_string(measure).c_str(), "");

	LOG("End\n", "");
	return 0;
}