#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

int main() {
    // Create a window
    sf::RenderWindow window(sf::VideoMode(800, 600), "FNAF Office");

    // Load textures
    sf::Texture officeTexture;
    if (!officeTexture.loadFromFile("assets/office/NormalOffice.png")) {
        return -1;
    }

    sf::Texture buttonTexture;
    if (!buttonTexture.loadFromFile("assets/office/button.png")) {
        return -1;
    }

    sf::Texture doorTexture;
    if (!doorTexture.loadFromFile("assets/office/door.png")) {
        return -1;
    }

    // Create sprites
    sf::Sprite officeSprite(officeTexture);
    sf::Sprite leftButtonSprite(buttonTexture);
    sf::Sprite rightButtonSprite(buttonTexture);
    sf::Sprite leftDoorSprite(doorTexture);
    sf::Sprite rightDoorSprite(doorTexture);

    // Scale the office sprite
    officeSprite.setScale(0.9f, 0.9f);

    // Initial positions
    leftButtonSprite.setPosition(50, 500);   // Example position
    rightButtonSprite.setPosition(700, 500); // Example position
    leftDoorSprite.setPosition(100, 500);    // Example position
    rightDoorSprite.setPosition(650, 500);   // Example position

    float scrollOffset = 0.0f; // Initial scroll offset

    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Right) {
                    scrollOffset -= 10.0f; // Scroll right
                }
                if (event.key.code == sf::Keyboard::Left) {
                    scrollOffset += 10.0f; // Scroll left
                }

                // Handle button press
                if (event.key.code == sf::Keyboard::Space) {
                    // Example button press logic
                }
            }
        }

        // Update positions based on scroll offset
        officeSprite.setPosition(scrollOffset, 0);

        // Clear screen
        window.clear();

        // Draw the scene
        window.draw(officeSprite);
        window.draw(buttonSprite);
        window.draw(doorSprite);

        // Display
        window.display();
    }

    return 0;
}
