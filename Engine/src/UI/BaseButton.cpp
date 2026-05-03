#include "UI/BaseButton.h"

#include "Core/Window.h"

bool BaseButton::IsMouseOver() const {
    const auto window = Window::GetWindow();
    return window && IsMouseOver(*window);
}

bool BaseButton::IsClicked() {
    const auto window = Window::GetWindow();
    return window && IsClicked(*window);
}
