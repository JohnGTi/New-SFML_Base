#include "InteractMandel.h"

InteractMandel::InteractMandel(sf::RenderWindow* hwnd, Input* in)
	: leftMouseDrag(false),
	rightMouseDrag(false),
	left(-2.0f),
	right(2.0f),
	top(1.125f),
	bottom(-1.125f),
	scale(1.0f)
{
	window = hwnd;
	input = in;

	// Create an empty texture.
	if (!mandelTexture.create(WIDTH, HEIGHT)) { // 1920, 1200
		std::cout << "Failed to create mandelTexture.";
		abort();
	}

	// Initialise zoom window qualities.
	zoomWindow.setFillColor(sf::Color::Transparent);
	zoomWindow.setOutlineColor(sf::Color::White);
	zoomWindow.setOutlineThickness(-3.0f);
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
}

void InteractMandel::ERZoomReset()
{
	// Reset view region to full, upon Z key pressed.
	if (input->isKeyDown(sf::Keyboard::Z)) {
		scale = 1.0f;

		left = -2.0f; right = 2.0f;
		top = 1.125; bottom = -1.125;
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
		// Calculate centre point of the drawn rectangle.
		int centreX = ((input->getMouseX() - zoomPosBegin.x) / 2 + zoomPosBegin.x);
		int centreY = ((input->getMouseY() - zoomPosBegin.y) / 2 + zoomPosBegin.y);

		TransformImage(centreX, centreY, 5.0f);
		zoomWindow.setSize(sf::Vector2f(0.0f, 0.0f));

		leftMouseDrag = false;
	}
}

void InteractMandel::DragViewWindow()
{
	if (input->isRightMouseDown()) {
		if (!rightMouseDrag) {
			// Store mouse position upon beginning of drag.
			dragPosPrev.x = (float)input->getMouseX();
			dragPosPrev.y = (float)input->getMouseY();

			rightMouseDrag = true;
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
		}
	}
	else if (rightMouseDrag) { rightMouseDrag = false; }
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

void InteractMandel::Update(float frame_time)
{
	// Compute Mandelbrot.
	mandel.ComputeMandelbrot(left, right, top, bottom, scale);
	// -2.0, 1.0, 1.125, -1.125
	// -0.751085, -0.734975, 0.118378, 0.134488

	// ^- ammend to only be called when required.

	// Update texture from array of pixels.
	mandelTexture.update(mandel.GetMandelPixels());

	// Assign texture to sprite (to draw).
	mandelSprite.setTexture(mandelTexture);
}

void InteractMandel::Render()
{
	// Render Mandelbrot and zoomWindow graphic.
	//window->clear();

	window->draw(mandelSprite);
	window->draw(zoomWindow);

	window->display();
}