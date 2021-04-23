#include "OwnComplex.h"

#include <amp.h>
#include <amp_math.h>

// Need to access the concurrency libraries 
using namespace concurrency;

OwnComplex::OwnComplex(float xval, float yval) //restrict(cpu, amp)
	: x(xval), y(yval) {}


/*OwnComplex::OwnComplex() restrict(cpu, amp)
	: x(0.0f), y(0.0f) {}*/

// restrict keyword - able to execute this function on the GPU and CPU.
void OwnComplex::c_add(OwnComplex c2) restrict(cpu, amp) {
	x += c2.x;
	y += c2.y;

	// Consider shared resources...
}

float OwnComplex::c_abs() restrict(cpu, amp) {

	return concurrency::fast_math::sqrt(x * x + y * y);
}

void OwnComplex::c_mul(OwnComplex c2) restrict(cpu, amp) {
	x = x * c2.x - y * c2.y;
	y = y * c2.x + x * c2.y;

	// Keep in mind, refined c_mull, c_add and such
	// will require updated implementation in Mandelbrot.
}