#pragma once
#include <string>
#include <sstream>

std::string to_string(double x) {
	std::ostringstream ss;
	ss << x;
	return ss.str();
}