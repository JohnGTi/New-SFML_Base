#pragma once

#include "AMPQuery.h"
#include "Framework/Input.h"  // (Robertson, P(2020) [1])

class InteractMandel
{
private:
	// Pointers to Framework objects.
	sf::RenderWindow* window;
	Input* input;

	// A mandelbrot class to communicate with.
	Mandelbrot mandel;

	// Texture and sprite for image data to
	// be written to.
	sf::Texture mandelTexture;
	sf::Sprite mandelSprite;

	// Zoom window shape and position data.
	sf::RectangleShape zoomWindow;
	sf::Vector2f zoomPosBegin;
	sf::Vector2f dragPosPrev;

	void ERZoomReset();
	void ComputeZoomWindow();
	void DragViewWindow();
	void TransformImage(float x, float y, float z);
	void ControlIterations();

	// Additional member variables.
	bool leftMouseDrag;
	bool rightMouseDrag;

	float scale;
	float left, right;
	float top, bottom;

public:
	// Specified constructor and application core
	// loop functions.

	InteractMandel(sf::RenderWindow* hwnd, Input* in);

	void HandleInput(float frame_time);
	void Update(float frame_time);
	void Render();
};