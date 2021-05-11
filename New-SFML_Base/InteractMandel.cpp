#include "InteractMandel.h"
#include <cmath>

InteractMandel::InteractMandel(sf::RenderWindow* hwnd, Input* in)
	: leftMouseDrag(false),
	middleMouseDrag(false),
	left(-2.0f),
	right(1.0f), // 2.0f
	top(1.125f),
	bottom(-1.125f),
	sample(0),
	blurApplied(false)
{
	window = hwnd;
	input = in;

	// Create an empty texture.
	if (!mandelTexture.create(WIDTH, HEIGHT)) {
		std::cout << "Failed to create mandelTexture.";
		abort();
	}

	// Initialise zoom window qualities.
	zoomWindow.setFillColor(sf::Color::Transparent);
	zoomWindow.setOutlineColor(sf::Color::White);
	zoomWindow.setOutlineThickness(-3.0f);

	// Compute Mandelbrot - initialise image data.
	mandel.ComputeMandelbrot(left, right, top, bottom);
}

void InteractMandel::HandleInput(float frame_time)
{
	// Press Esc key to close window.
	if (input->isKeyDown(sf::Keyboard::Escape)) {
		window->close();
	}
	// Enter to output screen to a .tga.
	if (input->isKeyDown(sf::Keyboard::Enter)) {
		mandel.WriteTga("output.tga");
	}

	ERZoomReset();
	ComputeZoomWindow();
	DragViewWindow();
	ControlIterations();
}

void InteractMandel::ERZoomReset()
{
	// Reset view region to full, upon Z key pressed.
	if (input->isKeyDown(sf::Keyboard::Z)) {

		// Press should not be mistaken as a hold.
		input->setKeyUp(sf::Keyboard::Z);

		left = -2.0f; right = 1.0f; // 2.0f
		top = 1.125; bottom = -1.125;

		// Compute Mandelbrot - update image data.
		mandel.ComputeMandelbrot(left, right, top, bottom, blurApplied);
	}

	// Scale back - previously, a zoom 'undo.'
	if (input->isRightMousePressed()) {
		TransformImage((WIDTH / 2.0f), (HEIGHT / 2.0f), 1.0f / 5.0f);

		// Compute Mandelbrot - update image data.
		mandel.ComputeMandelbrot(left, right, top, bottom, blurApplied);
	}

	// Toggle image blur effect on C press.
	if (input->isKeyDown(sf::Keyboard::C)) {
		
		// Press should not be mistaken as a hold.
		input->setKeyUp(sf::Keyboard::C);

		blurApplied = !blurApplied;

		if (blurApplied) {
			// Compute blur - update image data.
			mandel.ApplyBlur();
		}
		else {
			// Compute Mandelbrot - overwrite image data.
			mandel.ComputeMandelbrot(left, right, top, bottom);
		}
	}
	/*if (input->isKeyDown(sf::Keyboard::R)) {
		zoom += 0.010001f;
	}
	if (input->isKeyDown(sf::Keyboard::E)) {
		zoom -= 0.010001f;
	}
	// Rudimentary attempt at zoom in/out.
	// I kept the ER function name as it sounded really neato.*/
}

void InteractMandel::ComputeZoomWindow()
{
	// Initial left mouse click will store mouse position
	// and switch a boolean indicating that the mouse is being dragged.

	// The zoom window will be updated with the initial and
	// and current mouse positions for the duration of the drag.

	if (input->isLeftMouseDown()) {
		if (!leftMouseDrag) {
			zoomPosBegin.x = input->getMouseX();
			zoomPosBegin.y = input->getMouseY();

			leftMouseDrag = true;
		}
		zoomWindow.setPosition(zoomPosBegin);

		sf::Vector2f zoomArea(input->getMouseX() - zoomPosBegin.x,
			input->getMouseY() - zoomPosBegin.y);

		zoomWindow.setSize(zoomArea);

	}
	else if (leftMouseDrag) {
		// Local zoom window dimensions and scale.
		float windowWidth = input->getMouseX() - zoomPosBegin.x;
		float windowHeight = input->getMouseY() - zoomPosBegin.y;

		float SCALE = 1.0f;

		// Calculate centre point of the drawn rectangle.
		int centreX = (windowWidth / 2 + zoomPosBegin.x);
		int centreY = (windowHeight / 2 + zoomPosBegin.y);

		// Calculate the scale according to the drawn rectangle.
		if (windowHeight > windowWidth) {
			SCALE = WIDTH / windowWidth;
		}
		else {
			SCALE = HEIGHT / windowHeight;
		}
		SCALE = std::abs(SCALE);

		TransformImage(centreX, centreY, SCALE);
		zoomWindow.setSize(sf::Vector2f(0.0f, 0.0f));

		// Compute Mandelbrot - update image data.
		mandel.ComputeMandelbrot(left, right, top, bottom, blurApplied);

		leftMouseDrag = false;
	}
}

void InteractMandel::DragViewWindow()
{
	if (input->isMiddleMouseDown()) {
		if (!middleMouseDrag) {
			// Store mouse position upon beginning of drag.
			dragPosPrev.x = (float)input->getMouseX();
			dragPosPrev.y = (float)input->getMouseY();

			middleMouseDrag = true;
		}
		// Upon mouse movement...
		if (input->getMouseX() != dragPosPrev.x && input->getMouseY() != dragPosPrev.y) {

			// Update the image centre with the difference
			// in current to previous mouse position.
			float centreX = (WIDTH / 2.0f) - ((float)input->getMouseX() - dragPosPrev.x);
			float centreY = (HEIGHT / 2.0f) - ((float)input->getMouseY() - dragPosPrev.y);

			// Future mouse coordinates will be compared against
			// the current position.
			dragPosPrev.x = (float)input->getMouseX();
			dragPosPrev.y = (float)input->getMouseY();

			// Apply recentring.
			TransformImage(centreX, centreY, 1.0f);

			// Compute Mandelbrot - update image data.
			mandel.ComputeMandelbrot(left, right, top, bottom, blurApplied);
		}
	}
	else if (middleMouseDrag) { middleMouseDrag = false; }
}

void InteractMandel::TransformImage(float x, float y, float z)
{
	// Align the centre point of the drawn rectangle
	// with the centre of the complex plane.

	float real = left + (right - left) * x / WIDTH;
	float imaginary = bottom + (top - bottom) * y / HEIGHT;

	// Apply transformation to the Mandelbrot frame.
	float leftTemp = real - (right - left) / 2 / z;
	right = real + (right - left) / 2 / z;
	left = leftTemp;

	float bottomTemp = imaginary - (top - bottom) / 2 / z;
	top = imaginary + (top - bottom) / 2 / z;
	bottom = bottomTemp;
}

void InteractMandel::ControlIterations()
{
	// Increase the maximum iterations with a forward* scroll.

	if (input->isVerticalWheelScrolling()) {
		if (input->getScrollDelta() > 0.0f) {

			mandel.setMaxIterations(mandel.getMaxIterations() * 2);

			// Compute Mandelbrot - update image data.
			mandel.ComputeMandelbrot(left, right, top, bottom, blurApplied);
		}
		else {
			// Scrolling in the opposite direction halves the
			// maximum iterations.
			mandel.setMaxIterations(mandel.getMaxIterations() / 2);

			// Prevent MAX_ITERATIONS from reducing past O N E.
			if (mandel.getMaxIterations() < 1) { mandel.setMaxIterations(1); }

			// Compute Mandelbrot - update image data.
			mandel.ComputeMandelbrot(left, right, top, bottom, blurApplied);
		}
	}
	// *depends on a particular mouse's scroll direction.

	// Reset to default iterations threshold.
	if (input->isKeyDown(sf::Keyboard::Q)) {

		mandel.setMaxIterations(500);

		// Compute Mandelbrot - update image data.
		mandel.ComputeMandelbrot(left, right, top, bottom, blurApplied);
	}

	// Computation struggles with such a sharp increase in max iterations.
	// ONLY COMPUTE MANDELBROT WHEN REQUIRED.
}

void InteractMandel::Update(float frame_time)
{
	// Compute the mandelbrot image for the explicit purpose
	//  of recording timing data.

	if (sample < SAMPLE_SIZE) {

		// Compute Mandelbrot - update performance data.
		mandel.ComputeMandelbrot(left, right, top, bottom, blurApplied, sample);

		++sample;

		// Upon completion of sample timings,
		// print raw results.
		if (sample == SAMPLE_SIZE) {

			mandel.PrintResults();
		}
	}


	// Update texture from array of pixels.
	mandelTexture.update(mandel.GetMandelPixels());

	// Assign texture to sprite (to draw).
	mandelSprite.setTexture(mandelTexture);
}

void InteractMandel::Render()
{
	// Render Mandelbrot and zoomWindow graphic.
	window->draw(mandelSprite);
	window->draw(zoomWindow);

	window->display();
}