#include "Mandelbrot.h"
#include "OwnComplex.h"

// Import things we need from the standard library
using std::cout;
using std::endl;
using std::ofstream;

// Need to access the concurrency libraries 
using namespace concurrency;

// The size of the image to generate.
const int WIDTH = 1920;
const int HEIGHT = 1200;

// The number of times to iterate before we assume that a point isn't in the
// Mandelbrot set.
// (You may need to turn this up if you zoom further into the set.)
const int MAX_ITERATIONS = 500;

// The image data.
// Each pixel is represented as 0xRRGGBB.
uint32_t image[HEIGHT][WIDTH];

sf::Uint8* pixels = new sf::Uint8[HEIGHT * WIDTH * 4];
//sf::Uint8 pixels[HEIGHT * WIDTH * 4];
// ^- Remember to clean up.

// using our own Complex number structure and definitions as the Complex type is not available
// in the Concurrency namespace.
struct complex_c {
	float x, y;

	/*complex_c (float xval, float yval)
	: x(xval), y(yval) {}*/
};

// restrict keyword - able to execute this function on the GPU and CPU.
complex_c c_add(complex_c c1, complex_c c2) restrict(cpu, amp) {
	complex_c tmp;

	float a = c1.x;
	float b = c1.y;
	float c = c2.x;
	float d = c2.y;

	tmp.x = a + c;
	tmp.y = b + d;

	return tmp;
	// ^-refine this function??
}// c_add

float c_abs(complex_c c) restrict(cpu, amp) {
	return concurrency::fast_math::sqrt(c.x * c.x + c.y * c.y);
} // c_abs

complex_c c_mul(complex_c c1, complex_c c2) restrict(cpu, amp) {
	complex_c tmp;

	float a = c1.x;
	float b = c1.y;
	float c = c2.x;
	float d = c2.y;

	tmp.x = a * c - b * d;
	tmp.y = b * c + a * d;

	return tmp;
	// ^-refine this function??
}

// Write the image to a TGA file with the given name.
// Format specification: http://www.gamers.org/dEngine/quake3/TGA.txt
void Mandelbrot::write_tga(const char* filename)
{
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

	for (int y = 0; y < HEIGHT; ++y)
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

sf::Uint8* Mandelbrot::mandelPixels()
{
	// Any way to parallelise?...

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

			// Is either implementation more expensive?
			// Ensure correct use of referencing, not copying data.

			/**(pixels + pIndex) = (image[y][x] >> 16) & 0xFF;	// red channel
			*(pixels + pIndex + 1) = (image[y][x] >> 8) & 0xFF;	// green channel
			*(pixels + pIndex + 2) = image[y][x] & 0xFF;		// blue channel
			*(pixels + pIndex + 3) = 0xFF;						// alpha channel*/

			pIndex += 4;
		}
	}

	return pixels;

	// Consider if a parallel_for_each will work with an
	// array_view created by a sf::Uint8 image[HEIGHT * WIDTH * 4];

	// I believe the parallel_for_each will cooperate with sf.
	// sf::Uint32 behaved, if I recall correctly.

	/*		***		*/

	/// ^^- a temporary solution? Create a working parallel_for_each
	/// using an array_view created by a sf::Uint8 image[HEIGHT * WIDTH * 4];
}

// Render the Mandelbrot set into the image array.
// The parameters specify the region on the complex plane to plot.
void Mandelbrot::compute_mandelbrot(float left, float right, float top, float bottom)
{
	// Local pointer to the globally declared image data.
	uint32_t* pImage = &(image[0][0]);

	// array_view object will permit the image data to be available
	// on the CPU and GPU when needed.
	array_view<uint32_t, 2> a(HEIGHT, WIDTH, pImage);

	// Don't need to transfer data from CPU to GPU as all
	// calculations are done on the GPU.
	a.discard_data();

	parallel_for_each(a.extent, [=](index<2> idx) restrict(amp) {
		// Compute Mandelbrot here i.e. Mandelbrot kernel/shader...
		
		// USE THREAD ID/INDEX TO MAP INTO THE COMPLEX PLANE.
		int y = idx[0];
		int x = idx[1];

		// Work out the point in the complex plane that
		// corresponds to this pixel in the output image.
		complex_c c;
		c.x = left + (x * (right - left) / WIDTH);
		c.y = top + (y * (bottom - top) / HEIGHT);

		/*OwnComplex c(left + (x * (right - left) / WIDTH),
			top + (y * (bottom - top) / HEIGHT));*/

		// Start off z at (0, 0).
		complex_c z;
		z.x = 0.0f;
		z.y = 0.0f;

		//OwnComplex z; // OwnComplex z(); // ?

		// Iterate z = z^2 + c until z moves more than 2 units
		// away from (0, 0), or we've iterated too many times.
		int iterations = 0;
		while (c_abs(z) < 2.0 && iterations < MAX_ITERATIONS) // z.c_abs()
		{
			//z = (z * z) + c;
			z = c_add(c_mul(z, z), c);

			/*z.c_mul(z);
			z.c_add(c);*/

			++iterations;
		}

		if (iterations == MAX_ITERATIONS)
		{
			// z didn't escape from the circle.
			// This point is in the Mandelbrot set.
			a[idx] = 0x000000; // black
		}
		else
		{
			// z escaped within less than MAX_ITERATIONS
			// iterations. This point isn't in the set.

			// Make up a greyscale value performing bitwise operations on 'iterations.'
			a[idx] = ((iterations << 16) | (iterations << 8) | iterations);
		}
	});
}

/*// Compute Mandelbrot here i.e. Mandelbrot kernel/shader...

		// USE THREAD ID/INDEX TO MAP INTO THE COMPLEX PLANE.
		int y = idx[0];
		int x = idx[1];

		// Work out the point in the complex plane that
		// corresponds to this pixel in the output image.
		complex_c c;
		c.x = left + (x * (right - left) / WIDTH);
		c.y = top + (y * (bottom - top) / HEIGHT);


		// Start off z at (0, 0).
		complex_c z;
		z.x = 0.0f;
		z.y = 0.0f;

		// Iterate z = z^2 + c until z moves more than 2 units
		// away from (0, 0), or we've iterated too many times.
		int iterations = 0;
		while (c_abs(z) < 2.0 && iterations < MAX_ITERATIONS)
		{
			//z = (z * z) + c;
			z = c_add(c_mul(z, z), c);

			++iterations;
		}*/