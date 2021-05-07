#pragma once


// A custom Complex number class is required as the Complex
// type is not available in the Concurrency namespace.


class OwnComplex
{
private:
	// Complex coordinates.
	float x, y;

public:
	// Specified constructor and setter.
	OwnComplex() restrict(cpu, amp);
	void SetXY(float setX, float setY) restrict(cpu, amp);

	// Operations.
	void Add(OwnComplex c2) restrict(cpu, amp);
	float Absolute() restrict(cpu, amp);
	void Multiply(OwnComplex c2) restrict(cpu, amp);
};