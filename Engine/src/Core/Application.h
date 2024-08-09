#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

class Application
{
public:
	static void Init();
	static void Run();
	static void Destroy();

private:
	static sf::RenderWindow* window;
};