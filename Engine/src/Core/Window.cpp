#include "Window.h"

std::shared_ptr<sf::RenderWindow> Window::m_Window = nullptr;

std::shared_ptr<sf::RenderWindow> Window::Init(int width, int height, std::string title)
{
	m_Window = std::make_shared<sf::RenderWindow>(sf::VideoMode(width, height), title);

	return m_Window;
}

void Window::Destroy()
{
	m_Window->close();
	m_Window.reset();
}
