#include "Power.h"
#include "Core/Window.h"
#include "Assets/Resources.h"
#include "Graphics/LayerManager.h"
#include "layerdefines.h"

void PowerIndicator::Init()
{
	// Initialize the screen dimensions
	int screenWidth = Window::GetWindow()->getSize().x;
	int screenHeight = Window::GetWindow()->getSize().y;
    // Initialize the power level
    powerLevel = 100;
    timeSinceLastDrain = 0;

    // Initialize the usage level
    usageLevel = 1;

    font = Resources::GetFont("Font/five-nights-at-freddys.ttf");

    // Set up the power level text
    powerText.setFont(*font);
    powerText.setCharacterSize(24);
    powerText.setFillColor(sf::Color::White);
    powerText.setPosition(screenWidth * 0.05f, screenHeight * 0.85f);
    LayerManager::AddDrawable(UI_LAYER, powerText);

    // Set up the usage bars
    //usageBars.setSize(sf::Vector2f(screenWidth * 0.15f, screenHeight * 0.03f));
    //usageBars.setPosition(screenWidth * 0.05f, screenHeight * 0.90f);
}

void PowerIndicator::Update(double deltaTime)
{
    timeSinceLastDrain += deltaTime;

    // Determine drain interval based on usage level
    float drainInterval = calculateDrainInterval(usageLevel);

    // Drain power if time has passed the drain interval
    if (timeSinceLastDrain >= drainInterval) {
        if (powerLevel > 0) {
            powerLevel--;
            timeSinceLastDrain = 0;
        }
    }

    // Update the power level text
    powerText.setString(std::to_string(static_cast<int>(powerLevel)) + "%");

    //// Update the usage bar color based on usage level
    //if (usageLevel == 1) {
    //    usageBars.setFillColor(sf::Color::Green);
    //}
    //else if (usageLevel == 2) {
    //    usageBars.setFillColor(sf::Color::Yellow);
    //}
    //else if (usageLevel >= 3) {
    //    usageBars.setFillColor(sf::Color::Red);
    //}
}