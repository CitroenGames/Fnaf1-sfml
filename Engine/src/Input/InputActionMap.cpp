#include "Input/InputActionMap.h"

#include <algorithm>
#include <utility>

namespace {
    InputActionMap g_Actions;

    bool IsAnyKeyDown(const std::vector<sf::Keyboard::Key> &keys) {
        return std::any_of(keys.begin(), keys.end(), [](sf::Keyboard::Key key) {
            return sf::Keyboard::isKeyPressed(key);
        });
    }
}

void InputActionMap::BindAction(const std::string &action, sf::Keyboard::Key key) {
    BindAction(action, std::vector<sf::Keyboard::Key>{key});
}

void InputActionMap::BindAction(const std::string &action, std::vector<sf::Keyboard::Key> keys) {
    m_Bindings[action].keys = std::move(keys);
}

void InputActionMap::UnbindAction(const std::string &action) {
    m_Bindings.erase(action);
}

void InputActionMap::Clear() {
    m_Bindings.clear();
}

void InputActionMap::Update() {
    for (auto &[action, binding]: m_Bindings) {
        (void) action;
        const bool wasDown = binding.down;
        binding.down = IsAnyKeyDown(binding.keys);
        binding.pressed = binding.down && !wasDown;
        binding.released = !binding.down && wasDown;
    }
}

bool InputActionMap::IsDown(const std::string &action) const {
    if (const Binding *binding = GetBinding(action)) {
        return binding->down;
    }

    return false;
}

bool InputActionMap::WasPressed(const std::string &action) const {
    if (const Binding *binding = GetBinding(action)) {
        return binding->pressed;
    }

    return false;
}

bool InputActionMap::WasReleased(const std::string &action) const {
    if (const Binding *binding = GetBinding(action)) {
        return binding->released;
    }

    return false;
}

const InputActionMap::Binding *InputActionMap::GetBinding(const std::string &action) const {
    const auto it = m_Bindings.find(action);
    if (it == m_Bindings.end()) {
        return nullptr;
    }

    return &it->second;
}

InputActionMap &Input::Actions() {
    return g_Actions;
}

void Input::ConfigureNarrativeDefaults() {
    g_Actions.Clear();
    g_Actions.BindAction("move_left", {sf::Keyboard::Left, sf::Keyboard::A});
    g_Actions.BindAction("move_right", {sf::Keyboard::Right, sf::Keyboard::D});
    g_Actions.BindAction("move_up", {sf::Keyboard::Up, sf::Keyboard::W});
    g_Actions.BindAction("move_down", {sf::Keyboard::Down, sf::Keyboard::S});
    g_Actions.BindAction("confirm", {sf::Keyboard::Enter, sf::Keyboard::Space, sf::Keyboard::Z});
    g_Actions.BindAction("cancel", {sf::Keyboard::Escape, sf::Keyboard::B});
    g_Actions.BindAction("run", {sf::Keyboard::LShift, sf::Keyboard::RShift});
    g_Actions.BindAction("inspect", {sf::Keyboard::X});
    g_Actions.BindAction("memory", {sf::Keyboard::Y});
    g_Actions.BindAction("dialogue_prev", {sf::Keyboard::Q});
    g_Actions.BindAction("dialogue_next", {sf::Keyboard::E});
    g_Actions.BindAction("journal", {sf::Keyboard::J, sf::Keyboard::Tab});
}

void Input::Update() {
    g_Actions.Update();
}

bool Input::IsActionDown(const std::string &action) {
    return g_Actions.IsDown(action);
}

bool Input::WasActionPressed(const std::string &action) {
    return g_Actions.WasPressed(action);
}

bool Input::WasActionReleased(const std::string &action) {
    return g_Actions.WasReleased(action);
}
