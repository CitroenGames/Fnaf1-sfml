#pragma once

#include <cstdint>
#include <cstdlib>
#include <memory>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>

inline std::shared_ptr<sf::Texture> MakeTextureTransparent(const std::shared_ptr<sf::Texture> &inputTexture,
                                                           float alpha = 0.5f) {
    if (!inputTexture || !inputTexture->getSize().x || !inputTexture->getSize().y) {
        return nullptr;
    }

    auto outputTexture = std::make_shared<sf::Texture>();
    outputTexture->setSmooth(inputTexture->isSmooth());
    outputTexture->setRepeated(inputTexture->isRepeated());

    sf::Image image = inputTexture->copyToImage();
    const auto size = image.getSize();

    uint8_t alphaValue = static_cast<uint8_t>(alpha * 255);

    for (unsigned int x = 0; x < size.x; x++) {
        for (unsigned int y = 0; y < size.y; y++) {
            const sf::Color pixel = image.getPixel(x, y);
            image.setPixel(x, y, sf::Color(pixel.r, pixel.g, pixel.b, alphaValue));
        }
    }

    if (!outputTexture->loadFromImage(image)) {
        return nullptr;
    }

    return outputTexture;
}

static std::shared_ptr<sf::Texture> ProcessText(const std::shared_ptr<sf::Texture> &inputTexture, int tolerance = 5) {
    if (!inputTexture || !inputTexture->getSize().x || !inputTexture->getSize().y) {
        return nullptr;
    }

    auto outputTexture = std::make_shared<sf::Texture>();
    outputTexture->setSmooth(inputTexture->isSmooth());
    outputTexture->setRepeated(inputTexture->isRepeated());

    sf::Image image = inputTexture->copyToImage();
    const auto size = image.getSize();

    for (unsigned int x = 0; x < size.x; x++) {
        for (unsigned int y = 0; y < size.y; y++) {
            const sf::Color pixel = image.getPixel(x, y);

            bool isGrey = std::abs(pixel.r - pixel.g) <= tolerance &&
                          std::abs(pixel.g - pixel.b) <= tolerance &&
                          std::abs(pixel.r - pixel.b) <= tolerance &&
                          pixel.r < 240; // Don't process white pixels

            if (isGrey) {
                image.setPixel(x, y, sf::Color(pixel.r, pixel.g, pixel.b, 0));
            }
        }
    }

    if (!outputTexture->loadFromImage(image)) {
        return nullptr;
    }

    return outputTexture;
}

inline std::shared_ptr<sf::Texture> RemoveBlackBackground(const std::shared_ptr<sf::Texture> &inputTexture,
                                                          int threshold = 30) {
    if (!inputTexture || !inputTexture->getSize().x || !inputTexture->getSize().y) {
        return nullptr;
    }

    auto outputTexture = std::make_shared<sf::Texture>();
    outputTexture->setSmooth(inputTexture->isSmooth());
    outputTexture->setRepeated(inputTexture->isRepeated());

    sf::Image image = inputTexture->copyToImage();
    const auto size = image.getSize();

    for (unsigned int x = 0; x < size.x; x++) {
        for (unsigned int y = 0; y < size.y; y++) {
            const sf::Color pixel = image.getPixel(x, y);

            if (pixel.r <= threshold && pixel.g <= threshold && pixel.b <= threshold) {
                image.setPixel(x, y, sf::Color(pixel.r, pixel.g, pixel.b, 0));
            }
        }
    }

    if (!outputTexture->loadFromImage(image)) {
        return nullptr;
    }

    return outputTexture;
}
