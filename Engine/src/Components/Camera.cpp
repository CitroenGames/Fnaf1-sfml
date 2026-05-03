#include "Components/Camera.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

Camera2D::Config::Config()
    : center(0.0f, 0.0f)
    , resolution(800.0f, 600.0f)
    , initialZoom(1.0f)
    , minZoom(0.1f)
    , maxZoom(10.0f)
    , smoothingFactor(0.85f)
    , maintainResolution(false) {
}

Camera2D::Config::Config(sf::Vector2f customCenter, sf::Vector2f customResolution)
    : center(customCenter)
    , resolution(customResolution)
    , initialZoom(1.0f)
    , minZoom(0.1f)
    , maxZoom(10.0f)
    , smoothingFactor(0.85f)
    , maintainResolution(false) {
}

Camera2D::Camera2D(const Config &config)
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
    , lastDeltaTime(0.0f) {
    view.setCenter(config.center);
    view.setSize(config.resolution);
    validateConfig(config);
}

void Camera2D::setPosition(const sf::Vector2f &newPosition) {
    position = boundingEnabled ? clampToBounds(newPosition) : newPosition;
    targetPosition = position;
    updateView();
}

void Camera2D::move(const sf::Vector2f &offset) noexcept {
    setTargetPosition(targetPosition + offset);
}

void Camera2D::setZoom(float zoom) {
    zoomLevel = std::clamp(zoom, minZoom, maxZoom);
    updateView();
}

void Camera2D::zoom(float factor) noexcept {
    setZoom(zoomLevel * factor);
}

void Camera2D::setRotation(float angle) noexcept {
    rotation = std::fmod(angle, 360.0f);
    if (rotation < 0) {
        rotation += 360.0f;
    }
    updateView();
}

void Camera2D::rotate(float angle) noexcept {
    setRotation(rotation + angle);
}

void Camera2D::focusOn(const sf::Vector2f &target, float duration) {
    auto distance = target - position;
    auto speed = distance / duration;
    targetPosition = target;
    smoothingFactor = calculateSmoothingForSpeed(speed.x, speed.y);
}

void Camera2D::setBounds(const sf::FloatRect &newBounds) {
    if (newBounds.width <= 0 || newBounds.height <= 0) {
        throw std::invalid_argument("Bounds dimensions must be positive");
    }
    bounds = newBounds;
    boundingEnabled = true;
    setPosition(position);
}

void Camera2D::clearBounds() noexcept {
    boundingEnabled = false;
}

void Camera2D::setMaintainResolution(bool maintain) noexcept {
    maintainResolution = maintain;
    updateViewport();
}

void Camera2D::setBaseResolution(const sf::Vector2f &resolution) {
    if (resolution.x <= 0 || resolution.y <= 0) {
        throw std::invalid_argument("Resolution components must be positive");
    }
    baseResolution = resolution;
    updateViewport();
}

void Camera2D::update(float deltaTime) {
    lastDeltaTime = deltaTime;
    updateSmoothMovement(deltaTime);
    updatePositionHistory();
    updateView();
}

void Camera2D::applyTo(sf::RenderTarget &target) {
    if (maintainResolution) {
        updateViewport(target.getSize());
    }
    target.setView(view);
}

sf::Vector2f Camera2D::screenToWorldCoords(const sf::Vector2f &screenPos,
                                           const sf::RenderTarget &target) const {
    return target.mapPixelToCoords(
        sf::Vector2i(static_cast<int>(screenPos.x),
                     static_cast<int>(screenPos.y)), view);
}

sf::Vector2f Camera2D::worldToScreenCoords(const sf::Vector2f &worldPos,
                                           const sf::RenderTarget &target) const {
    return sf::Vector2f(target.mapCoordsToPixel(worldPos, view));
}

const sf::Vector2f &Camera2D::getPosition() const noexcept {
    return position;
}

float Camera2D::getZoomLevel() const noexcept {
    return zoomLevel;
}

float Camera2D::getRotation() const noexcept {
    return rotation;
}

sf::FloatRect Camera2D::getViewport() const noexcept {
    return view.getViewport();
}

const sf::Vector2f &Camera2D::getBaseResolution() const noexcept {
    return baseResolution;
}

bool Camera2D::isMaintainingResolution() const noexcept {
    return maintainResolution;
}

const std::vector<std::pair<float, sf::Vector2f> > &Camera2D::getMotionTrail() const noexcept {
    return positionHistory;
}

sf::FloatRect Camera2D::getVisibleArea() const noexcept {
    auto size = view.getSize();
    auto center = view.getCenter();
    return sf::FloatRect(
        center.x - size.x / 2,
        center.y - size.y / 2,
        size.x,
        size.y
    );
}

void Camera2D::validateConfig(const Config &config) {
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

void Camera2D::updateView() noexcept {
    view.setCenter(position);
    view.setSize(baseResolution / zoomLevel);
    view.setRotation(rotation);
}

void Camera2D::updateViewport(const sf::Vector2u &actualWindowSize) noexcept {
    if (!maintainResolution) {
        return;
    }

    float windowAspectRatio = static_cast<float>(actualWindowSize.x) / static_cast<float>(actualWindowSize.y);
    float targetAspectRatio = baseResolution.x / baseResolution.y;

    sf::FloatRect viewport(0.f, 0.f, 1.f, 1.f);
    if (std::abs(windowAspectRatio - targetAspectRatio) > 0.001f) {
        if (windowAspectRatio > targetAspectRatio) {
            float ratio = targetAspectRatio / windowAspectRatio;
            viewport.left = (1.f - ratio) / 2.f;
            viewport.width = ratio;
        } else {
            float ratio = windowAspectRatio / targetAspectRatio;
            viewport.top = (1.f - ratio) / 2.f;
            viewport.height = ratio;
        }
    }

    view.setViewport(viewport);
}

void Camera2D::updateViewport() noexcept {
    if (!maintainResolution) {
        return;
    }

    sf::Vector2f viewSize = view.getSize();
    updateViewport(sf::Vector2u(static_cast<unsigned int>(viewSize.x),
                                static_cast<unsigned int>(viewSize.y)));
}

void Camera2D::setTargetPosition(const sf::Vector2f &target) noexcept {
    targetPosition = boundingEnabled ? clampToBounds(target) : target;
}

sf::Vector2f Camera2D::clampToBounds(const sf::Vector2f &pos) const noexcept {
    return sf::Vector2f(
        std::clamp(pos.x, bounds.left, bounds.left + bounds.width),
        std::clamp(pos.y, bounds.top, bounds.top + bounds.height)
    );
}

void Camera2D::updateSmoothMovement(float deltaTime) noexcept {
    if (smoothingFactor > 0) {
        float smoothing = std::pow(smoothingFactor, deltaTime * 60.0f);
        position = position * smoothing + targetPosition * (1.0f - smoothing);
    } else {
        position = targetPosition;
    }
}

void Camera2D::updatePositionHistory() {
    positionHistory.emplace_back(lastDeltaTime, position);
    while (positionHistory.size() > MAX_HISTORY_SIZE) {
        positionHistory.erase(positionHistory.begin());
    }
}

float Camera2D::calculateSmoothingForSpeed(float dx, float dy) const noexcept {
    float speed = std::sqrt(dx * dx + dy * dy);
    return std::clamp(1.0f - (speed * 0.01f), 0.0f, 0.99f);
}
