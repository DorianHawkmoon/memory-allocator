#include "Complex.h"
#include "Log.h"

int main(int argc, char** argv) {
	LOG("Start main", "");

	Complex* array[1000];
	int iterations = 5000;
	for (int i = 0; i < iterations; i++) {
		//create the numbers
		for (int j = 0; j < 1000; j++) {
			array[j] = new Complex(i, j);
		}

		//delete the numbers
		for (int j = 0; j < 1000; j++) {
			delete array[j];
		}
	}

	LOG("End", "");
	return 0;
}