#pragma once
#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <sstream>

std::string to_string(double x) {
	std::ostringstream ss;
	ss << x;
	return ss.str();
}
#endif