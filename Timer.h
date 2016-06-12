#pragma once
#ifndef TIMER_H
#define TIMER_H
#include <chrono>

typedef std::chrono::high_resolution_clock::time_point TimeVar;

#define duration(a) std::chrono::duration_cast<std::chrono::milliseconds>(a).count()
#define timeNow() std::chrono::high_resolution_clock::now()

template<typename F, typename... Args>
double functionTime(F func, Args&&... args) {
	TimeVar t1 = timeNow();
	func(std::forward<Args>(args)...);
	TimeVar t2 = timeNow();
	auto total = duration(t2 - t1);
	return static_cast<double>(total);
}

template<typename E, typename... Args>
double measureFunctionTime(int loop, E func, Args&&... args) {
	if (loop == 0) return 0;
	int count = 0;
	double total = 0;
	while (count < loop) {
		total += functionTime(E,args);
		count++;
	}
	return total / (double)(loop);
}
#endif