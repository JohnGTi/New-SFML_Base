// [1] Robertson, P 2020, GitHub template repository for CMP105 (https://github.com/Abertay-University-SDI/CMP105_W1). IEEE, (CMP105_W1/Week1/CMP105App/Framework/).
//	^- 'windowProcess()' organisation and Input Framework.

// [2] Falconer, R 2021, Week 9 CPP file for CMP202 (https://mylearningspace.abertay.ac.uk/d2l/le/content/17327/viewContent/344998/View). IEEE, 'reductionBB.cpp'.
//	^- 'AMPQuery' functions to query AMP for available accelerators.


// *** \\


#include "InteractMandel.h"

void windowProcess(sf::RenderWindow* window, Input* input) { // [1]
	// Handle window events.
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
	}
}

int main() {
	//Create the window
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "MandelApp");

	// Initialise input object.
	Input input;

	// Initialise objects for delta time.
	sf::Clock clock;
	float deltaTime;

	// Create an interface for Mandelbrot interaction.
	InteractMandel mandelMain(&window, &input);

	// Write accelerator report to console.
	AMPQuery accelQuery;
	accelQuery.QueryAMPSupport();

	while (window.isOpen()) {
		//Process window events
		windowProcess(&window, &input); // [1]

		// Calculate delta time. How much time has passed since
		// it was last calculated (in seconds) and restart the clock.
		deltaTime = clock.restart().asSeconds();

		// Call key application loop functions.
		mandelMain.HandleInput(deltaTime);
		mandelMain.Update(deltaTime);
		mandelMain.Render();

		// Update input class, handle pressed keys
		// Must be done last.
		input.update();
	}

	return 0;
}