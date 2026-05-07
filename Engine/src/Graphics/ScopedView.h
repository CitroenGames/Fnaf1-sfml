#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/View.hpp>

class ScopedView {
public:
    ScopedView(sf::RenderTarget &target, const sf::View &view)
        : m_Target(target)
        , m_PreviousView(target.getView()) {
        m_Target.setView(view);
    }

    ScopedView(const ScopedView &) = delete;
    ScopedView &operator=(const ScopedView &) = delete;

    ~ScopedView() {
        m_Target.setView(m_PreviousView);
    }

private:
    sf::RenderTarget &m_Target;
    sf::View m_PreviousView;
};
