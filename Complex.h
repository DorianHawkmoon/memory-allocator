#pragma once
class Complex {
public:
	Complex(double a, double b): r(a), c(b) {}
	~Complex() {}

private:
	//Real part
	double r;
	//Complex part
	double c;
};

