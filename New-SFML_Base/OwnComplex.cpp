#include "OwnComplex.h"

#include <amp.h>
#include <amp_math.h>

// Need to access the concurrency libraries 
using namespace concurrency;

OwnComplex::OwnComplex() restrict(cpu, amp)
	: x(0.0f), y(0.0f) {}

void OwnComplex::SetXY(float setX, float setY) restrict(cpu, amp) {
	x = setX;
	y = setY;
}

// restrict keyword - able to execute this function on the GPU and CPU.
void OwnComplex::Add(OwnComplex c2) restrict(cpu, amp) {
	x = x + c2.x;
	y = y + c2.y;
}

float OwnComplex::Absolute() restrict(cpu, amp) {

	return concurrency::fast_math::sqrt(x * x + y * y);
}

void OwnComplex::Multiply(OwnComplex c2) restrict(cpu, amp)
{
	// Temporary complex object required as second calculation
	// depends on the x value not being changed in the first.
	OwnComplex ctmp;

	
	ctmp.x = x * c2.x - y * c2.y;
	ctmp.y = y * c2.x + x * c2.y;

	x = ctmp.x; y = ctmp.y;
}