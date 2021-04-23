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
	void compute_mandelbrot(float left, float right, float top, float bottom);

	void write_tga(const char* filename);
	sf::Uint8* mandelPixels();
};