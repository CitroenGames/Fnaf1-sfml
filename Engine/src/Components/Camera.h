#pragma once

#include <cstddef>
#include <utility>
#include <vector>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>

class Camera2D {
public:
    struct Config {
        sf::Vector2f center;
        sf::Vector2f resolution;
        float initialZoom;
        float minZoom;
        float maxZoom;
        float smoothingFactor;
        bool maintainResolution;

        Config();
        explicit Config(sf::Vector2f customCenter,
                        sf::Vector2f customResolution = sf::Vector2f(800.0f, 600.0f));
    };

    explicit Camera2D(const Config &config = Config{});

    Camera2D(const Camera2D &) = delete;
    Camera2D &operator=(const Camera2D &) = delete;
    Camera2D(Camera2D &&) noexcept = default;
    Camera2D &operator=(Camera2D &&) noexcept = default;
    ~Camera2D() = default;

    void setPosition(const sf::Vector2f &newPosition);
    void move(const sf::Vector2f &offset) noexcept;
    void setZoom(float zoom);
    void zoom(float factor) noexcept;
    void setRotation(float angle) noexcept;
    void rotate(float angle) noexcept;
    void focusOn(const sf::Vector2f &target, float duration = 1.0f);

    void setBounds(const sf::FloatRect &newBounds);
    void clearBounds() noexcept;

    void setMaintainResolution(bool maintain) noexcept;
    void setBaseResolution(const sf::Vector2f &resolution);

    void update(float deltaTime);
    void applyTo(sf::RenderTarget &target);

    [[nodiscard]] sf::Vector2f screenToWorldCoords(const sf::Vector2f &screenPos,
                                                   const sf::RenderTarget &target) const;
    [[nodiscard]] sf::Vector2f worldToScreenCoords(const sf::Vector2f &worldPos,
                                                   const sf::RenderTarget &target) const;

    [[nodiscard]] const sf::Vector2f &getPosition() const noexcept;
    [[nodiscard]] float getZoomLevel() const noexcept;
    [[nodiscard]] float getRotation() const noexcept;
    [[nodiscard]] sf::FloatRect getViewport() const noexcept;
    [[nodiscard]] const sf::Vector2f &getBaseResolution() const noexcept;
    [[nodiscard]] bool isMaintainingResolution() const noexcept;
    [[nodiscard]] const std::vector<std::pair<float, sf::Vector2f> > &getMotionTrail() const noexcept;
    [[nodiscard]] sf::FloatRect getVisibleArea() const noexcept;

private:
    sf::View view;
    sf::Vector2f position;
    sf::Vector2f baseResolution;
    float zoomLevel;
    float rotation;
    bool maintainResolution;
    float minZoom;
    float maxZoom;
    sf::FloatRect bounds;
    bool boundingEnabled;

    sf::Vector2f targetPosition;
    float smoothingFactor;

    float lastDeltaTime;
    std::vector<std::pair<float, sf::Vector2f> > positionHistory;
    static constexpr size_t MAX_HISTORY_SIZE = 10;

    void validateConfig(const Config &config);
    void updateView() noexcept;
    void updateViewport(const sf::Vector2u &actualWindowSize) noexcept;
    void updateViewport() noexcept;
    void setTargetPosition(const sf::Vector2f &target) noexcept;
    [[nodiscard]] sf::Vector2f clampToBounds(const sf::Vector2f &pos) const noexcept;
    void updateSmoothMovement(float deltaTime) noexcept;
    void updatePositionHistory();
    [[nodiscard]] float calculateSmoothingForSpeed(float dx, float dy) const noexcept;
};
