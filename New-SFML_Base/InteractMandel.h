#pragma once

#include "AMPQuery.h"
#include "Framework/Input.h"

class InteractMandel
{
private:
	// Pointers to Framework objects.
	sf::RenderWindow* window;
	Input* input;

	Mandelbrot mandel;

	sf::Texture mandelTexture;
	sf::Sprite mandelSprite;

	sf::RectangleShape zoomWindow;
	sf::Vector2f zoomPosBegin, zoomPosEnd;

	void WASDTraversePlane();
	void ERZoomReset();
	void MaintainAspectRatio();

	bool mouseDrag;
	float left, right, zoom;
	float top, bottom;
	float detX, detY;

public:
	InteractMandel(sf::RenderWindow* hwnd, Input* in);

	void HandleInput(float frame_time);
	void Update(float frame_time);
	void Render();
};