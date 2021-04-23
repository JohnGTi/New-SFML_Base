#include "Framework/AudioManager.h"
#include "Framework/GameState.h"
#include "Framework/Input.h"

#include "AMPQuery.h"
#include "Mandelbrot.h"


void windowProcess(sf::RenderWindow* window, Input* input) {
	/*// Handle window events.
	sf::Event event;
	while (window->pollEvent(event)) {

		switch (event.type) {
		case sf::Event::Closed:
			window->close();
			break;
		case sf::Event::Resized:
			window->setView(sf::View(sf::FloatRect(0.0f, 0.0f, (float)event.size.width, (float)event.size.height)));
			break;
		case sf::Event::KeyPressed:
			// Update input class.
			input->setKeyDown(event.key.code);
			break;
		case sf::Event::KeyReleased:
			input->setKeyUp(event.key.code);
			break;
		case sf::Event::MouseMoved:
			input->setMousePosition(event.mouseMove.x, event.mouseMove.y);
			break;
		case sf::Event::MouseButtonPressed:
			if (event.mouseButton.button == sf::Mouse::Left) {
				input->setLeftMouse(Input::MouseState::DOWN);
			}
			else if (event.mouseButton.button == sf::Mouse::Right) {
				input->setRightMouse(Input::MouseState::DOWN);
			}
			break;
		case sf::Event::MouseButtonReleased:
			if (event.mouseButton.button == sf::Mouse::Left) {
				input->setLeftMouse(Input::MouseState::UP);
			}
			else if (event.mouseButton.button == sf::Mouse::Right) {
				input->setRightMouse(Input::MouseState::UP);
			}
			break;
		default: // Don't handle other events.
			break;
		}
	}*/
}

int main() {
	//Create the window
	sf::RenderWindow window(sf::VideoMode(1920, 1200), "Mandel");

	// Initialise objects for delta time.
	sf::Clock clock;
	float deltaTime;

	// Initialise input object.
	Input input;

	sf::CircleShape shape(100.0f);
	shape.setFillColor(sf::Color::Green);

	AMPQuery accelQuery;
	accelQuery.query_AMP_support();

	Mandelbrot mandel;
	/*mandel.compute_mandelbrot(-0.751085, -0.734975,
		0.118378, 0.134488);*/

	mandel.write_tga("output.tga");

	sf::Texture mandelTexture;
	sf::Sprite mandelSprite;

	if (!mandelTexture.create(1920, 1200)) {
		std::cout << "break";
	}

	float left = -2.0;
	float right = 1.0;
	float top = 1.125;
	float bottom = -1.125;

	while (window.isOpen()) {
		//Process window events
		windowProcess(&window, &input);

		/*
		// Initial run means initial event is not required. 
		window.clear();
		window.draw(shape);
		window.display();*/

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}

			// Calculate delta time. How much time has passed 
			// since it was last calculated (in seconds) and restart the clock.
			deltaTime = clock.restart().asSeconds();

			/*left += 0.000901; right -= 0.001101;
			top -= 0.001101; bottom += 0.001101;*/

			mandel.compute_mandelbrot(left, right, top, bottom);
			// -0.1, 0.1, 0.8, 0.6
			// -2.0, 1.0, 1.125, -1.125
			/*mandel.compute_mandelbrot(-0.751085, -0.734975,
				0.118378, 0.134488);*/

			const sf::Uint8* pTemp = mandel.mandelPixels();
			mandelTexture.update(pTemp);

			//mandelTexture.loadFromImage();

			mandelSprite.setTexture(mandelTexture);

			//window.clear();
			window.draw(mandelSprite);
			window.display();

			// If the following code writes to the entire
			// window, the window.clear() is not required.

			/*// Update input class, handle pressed keys
			// Must be done last.
			input.update();*/
		}
	}

	return 0;
}