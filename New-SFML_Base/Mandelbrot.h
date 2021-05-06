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

// The size of the image to generate.
const int WIDTH = 1024; // 1920
const int HEIGHT = 1024; // 1200
// ^- problems with scale, window and such may be a result
//	  of misuse of HEIGHT, WIDTH order (keep in mind).

class Mandelbrot
{
private:
	//sf::Uint8* pixels = new sf::Uint8[1200 * 1920 * 4];
	uint8_t* pixels = new uint8_t[HEIGHT * WIDTH * 4]; // 1200 * 1920 * 4
	/// ^^^ - Uint8* CAN point to a uint8_t (^^ALSO CLEAN UP - *n e w*).

	// The number of times to iterate before we assume that a point isn't in the
	// Mandelbrot set. A higher maximum value results in a higher quality image.
	int MAX_ITERATIONS = 500;

public:
	void ComputeMandelbrot(float left, float right,
		float top, float bottom, float zoom = 1.0f);

	int getMaxIterations() { return MAX_ITERATIONS; };
	void setMaxIterations(float iterations);

	void WriteTga(const char* filename);
	sf::Uint8* GetMandelPixels();
};