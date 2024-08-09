#include "Window.h"

sf::RenderWindow* Window::window = nullptr;

sf::RenderWindow* Window::Init(int width, int height, std::string title)
{
	window = new sf::RenderWindow(sf::VideoMode(width, height), title);

	return window;
}

void Window::Destroy()
{
	window->close();
	delete window;
}
