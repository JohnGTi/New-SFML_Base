#pragma once

// (Falconer, R(2021) [2]) \\

#include "Mandelbrot.h"

#include <iomanip>
#include <string>
#include <numeric>
#include <time.h>

// Need to access the concurrency libraries.
using namespace concurrency;

class AMPQuery
{
private:
	// List and select the accelerator to use.
	void ListAccelerators();
	void ReportAccelerator(const accelerator a);

public:
	// Query if AMP accelerator exists on hardware.
	void QueryAMPSupport();
};