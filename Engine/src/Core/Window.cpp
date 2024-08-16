#include "Window.h"

sf::RenderWindow* Window::m_Window = nullptr;

sf::RenderWindow* Window::Init(int width, int height, std::string title)
{
	m_Window = new sf::RenderWindow(sf::VideoMode(width, height), title);

	return m_Window;
}

void Window::Destroy()
{
	m_Window->close();
	delete m_Window;
}
