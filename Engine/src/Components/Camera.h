#pragma once

#include <SFML/Graphics.hpp>

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

        // Default constructor with initialization
        Config()
            : center(0.0f, 0.0f)
            , resolution(800.0f, 600.0f)
            , initialZoom(1.0f)
            , minZoom(0.1f)
            , maxZoom(10.0f)
            , smoothingFactor(0.85f)
            , maintainResolution(false)
        {
        }

        // Custom constructor
        explicit Config(sf::Vector2f customCenter,
            sf::Vector2f customResolution = sf::Vector2f(800.0f, 600.0f))
            : center(customCenter)
            , resolution(customResolution)
            , initialZoom(1.0f)
            , minZoom(0.1f)
            , maxZoom(10.0f)
            , smoothingFactor(0.85f)
            , maintainResolution(false)
        {
        }
    };

private:
    // Core camera properties
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

    // Movement and effects
    sf::Vector2f targetPosition;
    float smoothingFactor;

    // Enhanced functionality
    float lastDeltaTime;
    std::vector<std::pair<float, sf::Vector2f>> positionHistory;
    static constexpr size_t MAX_HISTORY_SIZE = 10;

public:
    explicit Camera2D(const Config& config = Config{})
        : position(config.center)
        , baseResolution(config.resolution)
        , zoomLevel(config.initialZoom)
        , rotation(0.0f)
        , maintainResolution(config.maintainResolution)
        , minZoom(config.minZoom)
        , maxZoom(config.maxZoom)
        , boundingEnabled(false)
        , targetPosition(config.center)
        , smoothingFactor(config.smoothingFactor)
        , lastDeltaTime(0.0f)
    {
        view.setCenter(config.center);
        view.setSize(config.resolution);
        validateConfig(config);
    }

    // Rule of five implementation
    Camera2D(const Camera2D&) = delete;
    Camera2D& operator=(const Camera2D&) = delete;
    Camera2D(Camera2D&&) noexcept = default;
    Camera2D& operator=(Camera2D&&) noexcept = default;
    ~Camera2D() = default;

    // Core camera control methods
    void setPosition(const sf::Vector2f& newPosition) {
        position = boundingEnabled ? clampToBounds(newPosition) : newPosition;
        targetPosition = position;
        updateView();
    }

    void move(const sf::Vector2f& offset) noexcept {
        setTargetPosition(targetPosition + offset);
    }

    void setZoom(float zoom) {
        zoomLevel = std::clamp(zoom, minZoom, maxZoom);
        updateView();
    }

    void zoom(float factor) noexcept {
        setZoom(zoomLevel * factor);
    }

    void setRotation(float angle) noexcept {
        rotation = std::fmod(angle, 360.0f);
        if (rotation < 0) rotation += 360.0f;
        updateView();
    }

    void rotate(float angle) noexcept {
        setRotation(rotation + angle);
    }

    void focusOn(const sf::Vector2f& target, float duration = 1.0f) {
        auto distance = target - position;
        auto speed = distance / duration;
        targetPosition = target;
        smoothingFactor = calculateSmoothingForSpeed(speed.x, speed.y);
    }

    // Bounds management
    void setBounds(const sf::FloatRect& newBounds) {
        if (newBounds.width <= 0 || newBounds.height <= 0) {
            throw std::invalid_argument("Bounds dimensions must be positive");
        }
        bounds = newBounds;
        boundingEnabled = true;
        setPosition(position);
    }

    void clearBounds() noexcept {
        boundingEnabled = false;
    }

    // Resolution management
    void setMaintainResolution(bool maintain) noexcept {
        maintainResolution = maintain;
        updateViewport();
    }

    void setBaseResolution(const sf::Vector2f& resolution) {
        if (resolution.x <= 0 || resolution.y <= 0) {
            throw std::invalid_argument("Resolution components must be positive");
        }
        baseResolution = resolution;
        updateViewport();
    }

    // Update and application
    void update(float deltaTime) {
        lastDeltaTime = deltaTime;
        updateSmoothMovement(deltaTime);
        updatePositionHistory();
        updateView();
    }

    void applyTo(sf::RenderTarget& target) {
        if (maintainResolution) {
            updateViewport(target.getSize());
        }
        target.setView(view);
    }

    // Coordinate conversion
    [[nodiscard]] sf::Vector2f screenToWorldCoords(const sf::Vector2f& screenPos,
        const sf::RenderTarget& target) const {
        return target.mapPixelToCoords(
            sf::Vector2i(static_cast<int>(screenPos.x),
                static_cast<int>(screenPos.y)), view);
    }

    [[nodiscard]] sf::Vector2f worldToScreenCoords(const sf::Vector2f& worldPos,
        const sf::RenderTarget& target) const {
        return sf::Vector2f(target.mapCoordsToPixel(worldPos, view));
    }

    // Getters
    [[nodiscard]] const sf::Vector2f& getPosition() const noexcept { return position; }
    [[nodiscard]] float getZoomLevel() const noexcept { return zoomLevel; }
    [[nodiscard]] float getRotation() const noexcept { return rotation; }
    [[nodiscard]] sf::FloatRect getViewport() const noexcept { return view.getViewport(); }
    [[nodiscard]] const sf::Vector2f& getBaseResolution() const noexcept { return baseResolution; }
    [[nodiscard]] bool isMaintainingResolution() const noexcept { return maintainResolution; }
    [[nodiscard]] const std::vector<std::pair<float, sf::Vector2f>>& getMotionTrail() const noexcept {
        return positionHistory;
    }

    [[nodiscard]] sf::FloatRect getVisibleArea() const noexcept {
        auto size = view.getSize();
        auto center = view.getCenter();
        return sf::FloatRect(
            center.x - size.x / 2,
            center.y - size.y / 2,
            size.x,
            size.y
        );
    }

private:
    void validateConfig(const Config& config) {
        if (config.resolution.x <= 0 || config.resolution.y <= 0) {
            throw std::invalid_argument("Resolution must be positive");
        }
        if (config.minZoom <= 0 || config.maxZoom <= config.minZoom) {
            throw std::invalid_argument("Invalid zoom limits");
        }
        if (config.smoothingFactor < 0 || config.smoothingFactor >= 1) {
            throw std::invalid_argument("Smoothing factor must be in [0, 1)");
        }
    }

    void updateView() noexcept {
        view.setCenter(position);
        view.setSize(baseResolution / zoomLevel);
        view.setRotation(rotation);
    }

    void updateViewport(const sf::Vector2u& actualWindowSize) noexcept {
        if (!maintainResolution) return;

        float windowAspectRatio = static_cast<float>(actualWindowSize.x) / static_cast<float>(actualWindowSize.y);
        float targetAspectRatio = baseResolution.x / baseResolution.y;

        sf::FloatRect viewport(0.f, 0.f, 1.f, 1.f);

        if (std::abs(windowAspectRatio - targetAspectRatio) > 0.001f) {
            if (windowAspectRatio > targetAspectRatio) {
                // Ultrawide case - add black bars on the sides (pillarboxing)
                float ratio = targetAspectRatio / windowAspectRatio;
                viewport.left = (1.f - ratio) / 2.f;
                viewport.width = ratio;
            }
            else {
                // Tall case - add black bars on top/bottom (letterboxing)
                float ratio = windowAspectRatio / targetAspectRatio;
                viewport.top = (1.f - ratio) / 2.f;
                viewport.height = ratio;
            }
        }

        view.setViewport(viewport);
    }

	void updateViewport() noexcept { // this is for backwards compatibility
        if (!maintainResolution) return;

        // This is a fallback that uses view size rather than actual window size
        // For proper black borders, use the overloaded version with window size
        sf::Vector2f viewSize = view.getSize();
        updateViewport(sf::Vector2u(static_cast<unsigned int>(viewSize.x),
            static_cast<unsigned int>(viewSize.y)));
    }

    void setTargetPosition(const sf::Vector2f& target) noexcept {
        targetPosition = boundingEnabled ? clampToBounds(target) : target;
    }

    [[nodiscard]] sf::Vector2f clampToBounds(const sf::Vector2f& pos) const noexcept {
        return sf::Vector2f(
            std::clamp(pos.x, bounds.left, bounds.left + bounds.width),
            std::clamp(pos.y, bounds.top, bounds.top + bounds.height)
        );
    }

    void updateSmoothMovement(float deltaTime) noexcept {
        if (smoothingFactor > 0) {
            float smoothing = std::pow(smoothingFactor, deltaTime * 60.0f);
            position = position * smoothing + targetPosition * (1.0f - smoothing);
        }
        else {
            position = targetPosition;
        }
    }

    void updatePositionHistory() {
        positionHistory.emplace_back(lastDeltaTime, position);
        while (positionHistory.size() > MAX_HISTORY_SIZE) {
            positionHistory.erase(positionHistory.begin());
        }
    }

    [[nodiscard]] float calculateSmoothingForSpeed(float dx, float dy) const noexcept {
        float speed = std::sqrt(dx * dx + dy * dy);
        return std::clamp(1.0f - (speed * 0.01f), 0.0f, 0.99f);
    }
};