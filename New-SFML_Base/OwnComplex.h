#pragma once

// Class NOT IN USE, until output is realised
// and can be used for debugging.

class OwnComplex
{
private:
	float x, y;

public:
	// Specified constructors.
	OwnComplex(float xval = 0.0f, float yval = 0.0f);
		//restrict(cpu, amp);
	//OwnComplex();

	// Operations.
	void c_add(OwnComplex c2) restrict(cpu, amp);
	float c_abs() restrict(cpu, amp);
	void c_mul(OwnComplex c2) restrict(cpu, amp);
};