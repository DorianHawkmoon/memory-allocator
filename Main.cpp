#include "Complex.h"
#include "Log.h"
#include "Timer.h"
#include "Utils.h"

void test() {
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
}

int main(int argc, char** argv) {
	LOG("Start main", "");

	int loop = 5;
	int count = 0;
	double total = 0;
	while (count < loop) {
		total += functionTime(test);
		count++;
	}
	double measure = total / (double) (loop);
	LOG(to_string(measure).c_str(), "");

	functionTime(test);

	LOG("End", "");
	return 0;
}