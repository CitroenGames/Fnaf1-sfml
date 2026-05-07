#pragma once

#include <memory>

#include <SFML/Graphics/Texture.hpp>

std::shared_ptr<sf::Texture> MakeTextureTransparent(const std::shared_ptr<sf::Texture> &inputTexture,
                                                    float alpha = 0.5f);

std::shared_ptr<sf::Texture> ProcessText(const std::shared_ptr<sf::Texture> &inputTexture,
                                         int tolerance = 5);

std::shared_ptr<sf::Texture> RemoveBlackBackground(const std::shared_ptr<sf::Texture> &inputTexture,
                                                   int threshold = 30);
