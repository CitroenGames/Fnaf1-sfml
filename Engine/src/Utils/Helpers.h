#pragma once

#include <SFML/Graphics.hpp>

inline std::shared_ptr<sf::Texture> MakeTextureTransparent(const std::shared_ptr<sf::Texture>& inputTexture, float alpha = 0.5f) {
    if (!inputTexture || !inputTexture->getSize().x || !inputTexture->getSize().y) {
        return nullptr;
    }

    // Create new texture and preserve settings
    auto outputTexture = std::make_shared<sf::Texture>();
    outputTexture->setSmooth(inputTexture->isSmooth());
    outputTexture->setRepeated(inputTexture->isRepeated());

    // Get image data
    sf::Image image = inputTexture->copyToImage();
    const auto size = image.getSize();

    // Convert alpha to 0-255 range
    uint8_t alphaValue = static_cast<uint8_t>(alpha * 255);

    // Process each pixel
    for (unsigned int x = 0; x < size.x; x++) {
        for (unsigned int y = 0; y < size.y; y++) {
            const sf::Color pixel = image.getPixel(x, y);
            image.setPixel(x, y, sf::Color(pixel.r, pixel.g, pixel.b, alphaValue));
        }
    }

    // Create the new texture
    if (!outputTexture->loadFromImage(image)) {
        return nullptr;
    }

    return outputTexture;
}

static std::shared_ptr<sf::Texture> ProcessText(const std::shared_ptr<sf::Texture>& inputTexture, int tolerance = 5) {
    if (!inputTexture || !inputTexture->getSize().x || !inputTexture->getSize().y) {
        return nullptr;
    }

    // Create new texture
    auto outputTexture = std::make_shared<sf::Texture>();
    outputTexture->setSmooth(inputTexture->isSmooth());
    outputTexture->setRepeated(inputTexture->isRepeated());

    // Get image data
    sf::Image image = inputTexture->copyToImage();
    const auto size = image.getSize();

    // Process each pixel
    for (unsigned int x = 0; x < size.x; x++) {
        for (unsigned int y = 0; y < size.y; y++) {
            const sf::Color pixel = image.getPixel(x, y);

            // Check if pixel is grey (R, G, and B values are similar)
            bool isGrey = std::abs(pixel.r - pixel.g) <= tolerance &&
                std::abs(pixel.g - pixel.b) <= tolerance &&
                std::abs(pixel.r - pixel.b) <= tolerance &&
                pixel.r < 240; // Don't process white pixels

            if (isGrey) {
                // Make grey pixels transparent
                image.setPixel(x, y, sf::Color(pixel.r, pixel.g, pixel.b, 0));
            }
        }
    }

    // Create the new texture
    if (!outputTexture->loadFromImage(image)) {
        return nullptr;
    }

    return outputTexture;
}

inline std::shared_ptr<sf::Texture> RemoveBlackBackground(const std::shared_ptr<sf::Texture>& inputTexture, int threshold = 30) {
    if (!inputTexture || !inputTexture->getSize().x || !inputTexture->getSize().y) {
        return nullptr;
    }

    // Create new texture
    auto outputTexture = std::make_shared<sf::Texture>();
    outputTexture->setSmooth(inputTexture->isSmooth());
    outputTexture->setRepeated(inputTexture->isRepeated());

    // Get image data
    sf::Image image = inputTexture->copyToImage();
    const auto size = image.getSize();

    // Process each pixel
    for (unsigned int x = 0; x < size.x; x++) {
        for (unsigned int y = 0; y < size.y; y++) {
            const sf::Color pixel = image.getPixel(x, y);

            // Check if pixel is black or very dark
            if (pixel.r <= threshold && pixel.g <= threshold && pixel.b <= threshold) {
                // Make dark pixels completely transparent
                image.setPixel(x, y, sf::Color(pixel.r, pixel.g, pixel.b, 0));
            }
        }
    }

    // Create the new texture
    if (!outputTexture->loadFromImage(image)) {
        return nullptr;
    }

    return outputTexture;
}