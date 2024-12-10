#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

class Application
{
public:
	static void Init(const int width = 1280, const int height = 720, const std::string& title = "Window");
	static void Run();
	static void Destroy();

private:
	static std::shared_ptr<sf::RenderWindow> m_Window;
};