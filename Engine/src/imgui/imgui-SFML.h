#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include "imgui-SFML_export.h"

namespace ImGui::SFML {
IMGUI_SFML_API bool Init(sf::RenderWindow &window, bool loadDefaultFont = true);
IMGUI_SFML_API void ProcessEvent(const sf::Event &event);
IMGUI_SFML_API void Update(sf::RenderWindow &window, sf::Time dt);
IMGUI_SFML_API void Render(sf::RenderWindow &window);
IMGUI_SFML_API void Shutdown();
} // namespace ImGui::SFML
