#pragma once

#include <SFML/Graphics.hpp>

#include <chrono>
#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <complex>
#include <array>

#include <amp.h>
#include <amp_math.h>

#include <fstream>


// Number of samples for gathering of
// parallel function timings.
const int SAMPLE_SIZE = 200;

// The size of the image to generate.
const int WIDTH = 1024; // 1920
const int HEIGHT = 1024; // 1200


class Mandelbrot
{
private:
	// The number of times to iterate before we assume
	// that a point isn't in the Mandelbrot set. A higher
	// maximum value results in a higher quality image.
	int MAX_ITERATIONS = 500;


	// An array of pixels to update an sf::Texture.
	uint8_t* pixels = new uint8_t[HEIGHT * WIDTH * 4];

	// A container of results of timings.
	std::array<long long, SAMPLE_SIZE> results;

public:
	// Specify destructor for clean up.
	~Mandelbrot();

	// Compute mandelbrot image based off of minimum and
	// maximum complex coordinates.
	void ComputeMandelbrot(float left, float right,
		float top, float bottom, bool blur = false, int sample = -1);

	// Apply Gaussian blur to image.
	void ApplyBlur();

	// Maximum iterations getter and setter.
	int getMaxIterations() { return MAX_ITERATIONS; };
	void setMaxIterations(float iterations);

	// Write/Receive the generated image data to a file/to window.
	void WriteTga(const char* filename);
	sf::Uint8* GetMandelPixels();

	// Print timing results without annotations (to copy).
	void PrintResults();
};