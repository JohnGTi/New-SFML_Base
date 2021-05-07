#include "Mandelbrot.h"
#include "OwnComplex.h"



// Import things we need from the standard library
using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::cout;
using std::endl;
using std::ofstream;

// Define the alias "the_amp_clock" for the clock type we're going to use.
typedef std::chrono::steady_clock the_amp_clock;

// Need to access the concurrency libraries 
using namespace concurrency;


// The image data.
// Each pixel is represented as 0xRRGGBB.
uint32_t image[HEIGHT][WIDTH];

// Size of separated filter dimension.
const int KERNEL_SIZE = 7;



// Write the image to a TGA file with the given name.
// Format specification: http://www.gamers.org/dEngine/quake3/TGA.txt
void Mandelbrot::WriteTga(const char* filename)
{
	// (Sampson, A(2020) [3]) \\

	ofstream outfile(filename, ofstream::binary);

	uint8_t header[18] = {
		0, // no image ID
		0, // no colour map
		2, // uncompressed 24-bit image
		0, 0, 0, 0, 0, // empty colour map specification
		0, 0, // X origin
		0, 0, // Y origin
		WIDTH & 0xFF, (WIDTH >> 8) & 0xFF, // width
		HEIGHT & 0xFF, (HEIGHT >> 8) & 0xFF, // height
		24, // bits per pixel
		0, // image descriptor
	};
	outfile.write((const char*)header, 18);

	for (int y = HEIGHT - 1; y > -1; --y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			uint8_t pixel[3] = {
				image[y][x] & 0xFF, // blue channel
				(image[y][x] >> 8) & 0xFF, // green channel
				(image[y][x] >> 16) & 0xFF, // red channel
			};
			outfile.write((const char*)pixel, 3);
		}
	}

	outfile.close();
	if (!outfile)
	{
		// An error has occurred at some point since we opened the file.
		cout << "Error writing to " << filename << endl;
		exit(1);
	}
}


sf::Uint8* Mandelbrot::GetMandelPixels()
{
	// An independant counter for pixel;
	int pIndex = 0;

	for (int y = 0; y < HEIGHT; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			pixels[pIndex] = (image[y][x] >> 16) & 0xFF;	// red channel
			pixels[pIndex + 1] = (image[y][x] >> 8) & 0xFF;	// green channel
			pixels[pIndex + 2] = image[y][x] & 0xFF;		// blue channel
			pixels[pIndex + 3] = 0xFF;						// alpha channel

			pIndex += 4;
		}
	}
	/*sf::Uint8* sfpixels = pixels;
	return sfpixels;*/

	return pixels;
}


// Render the Mandelbrot set into the image array [3].
// The parameters specify the region on the complex plane to plot.

void Mandelbrot::ComputeMandelbrot(float left, float right, float top, float bottom, bool blur, int sample)
{
	// Local pointer to the globally declared image data.
	uint32_t* pImage = &(image[0][0]);

	// array_view object will permit the image data to be available
	// on the CPU and GPU when needed.
	extent<2> aex(HEIGHT, WIDTH);
	array_view<uint32_t, 2> a(aex, pImage);

	// Don't need to transfer data from CPU to GPU as all
	// calculations are done on the GPU.
	a.discard_data();


	// Local copy, for restricted use, of MAX_ITERATIONS.
	unsigned int maxIterations = MAX_ITERATIONS;

	// Define the tile size.
	const int TS = 8; // 32


	// start clock for GPU version after array allocation
	the_amp_clock::time_point start = the_amp_clock::now();

	// It is wise to use exception handling here - AMP can fail for many reasons
	// and it useful to know why (e.g. using double precision when there is limited or no support).
	try
	{
		parallel_for_each(a.extent.tile<TS, TS>(), [=](tiled_index<TS, TS> t_idx) restrict(amp) {
			// Compute Mandelbrot here i.e. Mandelbrot kernel/shader...

			// USE THREAD ID/INDEX TO MAP INTO THE COMPLEX PLANE.
			unsigned int y = t_idx.global[0];
			unsigned int x = t_idx.global[1];

			// Work out the point in the complex plane that
			// corresponds to this pixel in the output image.
			OwnComplex c;
			c.SetXY(
				left + (x * (right - left) / WIDTH),
				bottom + (y * (top - bottom) / HEIGHT)
			);

			// Start off z at (0, 0).
			OwnComplex z;

			// Iterate z = z^2 + c until z moves more than 2 units
			// away from (0, 0), or we've iterated too many times.
			unsigned int iterations = 0;
			while (z.Absolute() < 2.0 && iterations < maxIterations)
			{
				z.Multiply(z);
				z.Add(c);

				++iterations;
			}

			if (iterations == maxIterations)
			{
				// z didn't escape from the circle.
				// This point is in the Mandelbrot set.
				a[t_idx] = 0x000000; // Black.
			}
			else
			{
				// z escaped within less than maxIterations
				// iterations. This point isn't in the set.

				// Make up a greyscale value performing bitwise operations on 'iterations.'
				a[t_idx] = ((iterations << 16) | (iterations << 8) | iterations);
			}
		});
		a.synchronize();
	}
	catch (const Concurrency::runtime_exception& ex)
	{
		MessageBoxA(NULL, ex.what(), "Error", MB_ICONERROR);
	}

	// Stop timing
	the_amp_clock::time_point end = the_amp_clock::now();

	// Compute the difference between the two times in milliseconds
	auto time_taken = duration_cast<nanoseconds>(end - start).count();

	if (sample < SAMPLE_SIZE && sample != -1) {
		results.at(sample) = time_taken;
		std::cout << "AMP, TS " << TS << ", sample " << sample << ", takes : " << time_taken << " ns." << endl;
	}



	// If necessary, apply blur.
	if (blur) { ApplyBlur(); }
}


struct ConvolutionKernel {
	// Workaround for AMP pointer restriction.

	float filter[KERNEL_SIZE] = {
		0.006, 0.061, 0.242, 0.383, 0.242, 0.061, 0.006
	};
};

void Mandelbrot::ApplyBlur()
{
	// For local capture.
	ConvolutionKernel Separable;

	// Local pointer to the globally declared image data.
	uint32_t* pImage = &(image[0][0]);


	// Device memory resources...
	extent<2> exImg(HEIGHT, WIDTH);

	array_view<uint32_t, 2> avImageIn(exImg, pImage);
	array_view<uint32_t, 2> avImageOut(exImg, pImage);

	// Don't need to transfer data from CPU to GPU as all
	// calculations are done on the GPU.
	avImageOut.discard_data();



	// For the horizontal pass, the lambda function was separated from
	// the parallel_for_each call so that - if a solution to vary the
	// tile size at runtime was found - both passes could use the same lambda.



	auto singleBlurPass = [=](tiled_index<WIDTH, 1> t_idx) restrict(amp) {
		// Local copy of global index.
		index<2> idx = t_idx.global;

		// Load array_view contents into shared memory.
		tile_static uint32_t horizontalPoints[WIDTH];
		horizontalPoints[idx[0]] = avImageIn[idx];
		t_idx.barrier.wait();

		// Location of the pixel to be operated on.
		unsigned int texturePosX = idx[0] - (KERNEL_SIZE / 2);

		// Temporary colour/channel values.
		float red = 0.0f, green = 0.0f, blue = 0.0f;
		float pixelBlur = 0.0f;

		// Multiply neighbouring values with filter weights and accumulate.
		for (int i = 0; i < KERNEL_SIZE; ++i) {
			red += (float)((horizontalPoints[texturePosX + i] >> 16) & 0xFF) * Separable.filter[i];
			green += (float)((horizontalPoints[texturePosX + i] >> 8) & 0xFF) * Separable.filter[i];
			blue += (float)(horizontalPoints[texturePosX + i] & 0xFF) * Separable.filter[i];

			t_idx.barrier.wait();
			avImageOut[idx] = (((uint32_t)red << 16) | ((uint32_t)green << 8) | (uint32_t)blue);
		}
	};
	parallel_for_each(avImageIn.extent.tile<WIDTH, 1>(), singleBlurPass);

	avImageOut.synchronize();


	// Swap source and destination in preparation for vertical pass.
	std::swap(avImageIn, avImageOut);
	avImageOut.discard_data();


	parallel_for_each(avImageIn.extent.tile<1, HEIGHT>(), [=](tiled_index<1, HEIGHT> t_idx) restrict(amp) {
		// Local copy of global index.
		index<2> idx = t_idx.global;

		// Load array_view contents into shared memory.
		tile_static uint32_t verticalPoints[HEIGHT];
		verticalPoints[idx[1]] = avImageIn[idx];
		t_idx.barrier.wait();

		// Location of the pixel to be operated on.
		unsigned int texturePosX = idx[1] - (KERNEL_SIZE / 2);

		// Temporary colour/channel values.
		float red = 0.0f, green = 0.0f, blue = 0.0f;
		float pixelBlur = 0.0f;

		// Multiply neighbouring values with filter weights and accumulate.
		for (int i = 0; i < KERNEL_SIZE; ++i) {
			red += (float)((verticalPoints[texturePosX + i] >> 16) & 0xFF) * Separable.filter[i];
			green += (float)((verticalPoints[texturePosX + i] >> 8) & 0xFF) * Separable.filter[i];
			blue += (float)(verticalPoints[texturePosX + i] & 0xFF) * Separable.filter[i];

			t_idx.barrier.wait();
			avImageOut[idx] = (((uint32_t)red << 16) | ((uint32_t)green << 8) | (uint32_t)blue);
		}
	});
	avImageOut.synchronize();
}


void Mandelbrot::setMaxIterations(float iterations)
{
	// Update maximum iterations.
	MAX_ITERATIONS = iterations;
}

void Mandelbrot::PrintResults()
{
	// After a double line break, print results.
	std::cout << std::endl << std::endl;

	for (const auto &ns : results) {

		std::cout << ns << std::endl;
	}

	std::cout << "\n\nPerformance measurements taken, and available to copy.\n";
}

Mandelbrot::~Mandelbrot()
{
	// Delete uint8_t array of pixels.
	delete pixels;

	/*for (uint8_t* p : pixels) {

		delete p;
	}*/
}