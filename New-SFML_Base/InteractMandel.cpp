#include "InteractMandel.h"

InteractMandel::InteractMandel(sf::RenderWindow* hwnd, Input* in)
	: mouseDrag(false),
	left(-2.0f),
	right(1.0f),
	top(1.125f),
	bottom(-1.125f),
	zoom(1.0f)
{
	window = hwnd;
	input = in;

	// Create an empty texture.
	if (!mandelTexture.create(1920, 1200)) {
		std::cout << "Failed to create mandelTexture.";
		abort();
	}

	// Initialise zoom window qualities.
	zoomWindow.setFillColor(sf::Color::Transparent);
	zoomWindow.setOutlineColor(sf::Color::White);
	zoomWindow.setOutlineThickness(-3.0f);
	/////////////////////////////////////////////////////
	/*left = -0.751085; right = -0.734975;
	top = 0.134488; bottom = 0.118378;*/
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

	WASDTraversePlane();
	ERZoomReset();

	if (input->isLeftMouseDown()) {
		if (!mouseDrag) {
			zoomPosBegin.x = input->getMouseX();
			zoomPosBegin.y = input->getMouseY();

			mouseDrag = true;
		}
		zoomWindow.setPosition(zoomPosBegin);

		sf::Vector2f zoomArea(input->getMouseX() - zoomPosBegin.x,
			input->getMouseY() - zoomPosBegin.y);

		zoomWindow.setSize(zoomArea);

	} else if (mouseDrag) {
		zoomWindow.setSize(sf::Vector2f(0.0f, 0.0f));

		left = left * (960 - zoomPosBegin.x) / 960;
		right = right * (input->getMouseX() - 960) / 960;
		top = top * (600 - zoomPosBegin.y) / 600;
		bottom = bottom * (input->getMouseY() - 600) / 600;
		/*float halfRect = (input->getMouseX() - zoomPosBegin.x) / 2;
		left = halfRect / 960 * left;*/

		/*left = left * (1920 - zoomPosBegin.x) / 1920;
		right = -0.734975;
		/*top = 0.134488;
		bottom = 0.118378;*/
		mandel.ComputeMandelbrot(left, right, top, bottom);


		mouseDrag = false;
	}

	/*if (mouseDrag) {
		if (!input->isLeftMouseDown()) {
			// Resolve : mouse released.
			mouseDrag = false;
		}
		// Resolve : drag in effect.

		std::cout << " IS DOWN.";
	}
	std::cout << std::endl;*/
	/*input->setLeftMouse(Input::MouseState::UP);
	zoomPosBegin = sf::Vector2f(0.0f, 0.0f);
	std::cout << "mouse : " << input->getMouseX() << ", "
			<< input->getMouseY() << std::endl;
	std::cout << "IS DOWN.\n";*/
	/*std::cout << "mouse : " << zoomPosBegin.x << ", "
		<< zoomPosBegin.y << std::endl;
	std::cout << "Release.\n";*/
}

void InteractMandel::WASDTraversePlane()
{
	// Float values to in/decrement view region.
	float vertical = 0.000;
	float horizontal = 0.000;

	if (input->isKeyDown(sf::Keyboard::W)) {
		vertical = 0.010001f;

		// Should scale vertical to suit zoom.
		// Opportunity with automatic iterations?
	}
	if (input->isKeyDown(sf::Keyboard::S)) {
		vertical = -0.010001f;
	}
	top += vertical;
	bottom += vertical;

	if (input->isKeyDown(sf::Keyboard::A)) {
		horizontal = -0.010001f;
	}
	if (input->isKeyDown(sf::Keyboard::D)) {
		horizontal = 0.010001f;
	}
	left += horizontal;
	right += horizontal;
	// Will the above structure allow for tidy further
	// implementation? Or is it daft and over complicated?
}

void InteractMandel::ERZoomReset()
{
	// Reset view region to full, upon Z key pressed.
	if (input->isKeyDown(sf::Keyboard::Z)) {
		zoom = 1.0f;

		left = -2.0f; right = 1.0f;
		top = 1.125; bottom = -1.125;
	}

	if (input->isKeyDown(sf::Keyboard::R)) {
		zoom += 0.010001f;
	}
	if (input->isKeyDown(sf::Keyboard::E)) {
		zoom -= 0.010001f;
	}

	// WASD movement could depend on zoom, though
	// use of the zoomWindow would disregard any effect.
	
	// Some sort of variable tracks depth, required as
	// multiplier for WASD, zoom AND iterations.
}

void InteractMandel::Update(float frame_time)
{
	// Compute Mandelbrot.
	mandel.ComputeMandelbrot(left, right, top, bottom, zoom);
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
	// Render Mandelbrot (and other) graphic(s).
	//window->clear();

	window->draw(mandelSprite);
	window->draw(zoomWindow);

	window->display();
}