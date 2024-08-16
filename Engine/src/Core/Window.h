#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

class Window
{
public:
	static sf::RenderWindow* Init(int width, int height, std::string title);
	static void Destroy();

	static sf::RenderWindow* const GetWindow()
	{
		return m_Window;
	}

private:
	static sf::RenderWindow* m_Window;
};