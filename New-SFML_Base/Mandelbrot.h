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
	//sf::Uint8* pixels = new sf::Uint8[1200 * 1920 * 4];
	uint8_t* pixels = new uint8_t[1200 * 1920 * 4];

	/// ^^^ - Uint8* CAN point to a uint8_t (^^ALSO CLEAN UP - *n e w*).

public:
	void ComputeMandelbrot(float left, float right,
		float top, float bottom, float zoom = 1.0f);

	void WriteTga(const char* filename);
	sf::Uint8* GetMandelPixels();
};