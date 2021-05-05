#include "InteractMandel.h"

InteractMandel::InteractMandel(sf::RenderWindow* hwnd, Input* in)
	: mouseDrag(false),
	left(-2.0f),
	right(2.0f),
	top(1.125f),
	bottom(-1.125f),
	zoom(1.0f),
	detX(0.0f),
	detY(0.0f)
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
	/////////////////////////////////////////////////////
	/*left = -0.751085; right = -0.734975;
	top = 0.134488; bottom = 0.118378;*/
	/*left = -2.0f; right = 2.0f;
	top = 2.0f; bottom = -2.0f;*/
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
		left = left * ((WIDTH / 2) - zoomPosBegin.x) / (WIDTH / 2); // left = left * (960 - zoomPosBegin.x) / 960;
		top = top * ((HEIGHT / 2) - zoomPosBegin.y) / (HEIGHT / 2); // top = top * (600 - zoomPosBegin.y) / 600;

		MaintainAspectRatio();

		/*right = right * (input->getMouseX() - 960) / 960;
		bottom = bottom * (input->getMouseY() - 600) / 600;*/
		right = right * (detX - (WIDTH / 2)) / (WIDTH / 2); // right = right * (detX - 960) / 960;
		bottom = bottom * (detY - (HEIGHT / 2)) / (HEIGHT / 2); // bottom = bottom * (detY - 600) / 600;
		

		zoomWindow.setSize(sf::Vector2f(0.0f, 0.0f));

		mouseDrag = false;
	}

	/*input->setLeftMouse(Input::MouseState::UP);
	zoomPosBegin = sf::Vector2f(0.0f, 0.0f);
	std::cout << "mouse : " << input->getMouseX() << ", "
			<< input->getMouseY() << std::endl;
	std::cout << "IS DOWN.\n";*/
	/*std::cout << "mouse : " << zoomPosBegin.x << ", "
		<< zoomPosBegin.y << std::endl;
	std::cout << "Release.\n";*/
	/*std::cout << "initX " << zoomPosBegin.x << " initY " << zoomPosBegin.y << "; endX " << input->getMouseX() << ", endY " << input->getMouseY() << ";\n";
		std::cout << "left " << left << ", top " << top << ", right " << right << ", bottom " << bottom << ";\n";*/
}

void InteractMandel::MaintainAspectRatio()
{
	float rectH = input->getMouseY() - zoomPosBegin.y;
	float rectW = input->getMouseX() - zoomPosBegin.x;

	if (rectH > rectW) {
		detX = input->getMouseX();

		float newH = rectW * ((float)HEIGHT / (float)WIDTH);
		detY = newH + zoomPosBegin.y;
	}
	else {
		float newW = rectH * ((float)WIDTH / (float)HEIGHT);
		detX = newW + zoomPosBegin.x;

		detY = input->getMouseY();
	}
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

		left = -2.0f; right = 2.0f;
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