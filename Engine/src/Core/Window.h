#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>

class Window
{
public:
	static std::shared_ptr<sf::RenderWindow> Init(int width, int height, std::string title);
	static void Destroy();

	static std::shared_ptr<sf::RenderWindow> const GetWindow()
	{
		return m_Window;
	}

private:
	static std::shared_ptr<sf::RenderWindow> m_Window;
};