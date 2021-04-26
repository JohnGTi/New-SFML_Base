#pragma once

#include <SFML/Graphics.hpp>

#include <chrono>
#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <complex>

#include <amp.h>
#include <amp_math.h>

#include <fstream>

class Mandelbrot
{
private:


public:
	void ComputeMandelbrot(float left, float right,
		float top, float bottom, float zoom = 1.0f);

	void WriteTga(const char* filename);
	sf::Uint8* GetMandelPixels();
};