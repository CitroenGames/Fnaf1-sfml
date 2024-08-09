#include "Flipbook.h"

Flipbook::Flipbook() : layer(layer), frameDuration(0), elapsedTime(0.f), currentFrame(0), isPlayingFlag(false), loop(false) {}

Flipbook::Flipbook(int layer, float frameDuration, bool loop)
    : layer(layer), frameDuration(frameDuration), elapsedTime(0.f), currentFrame(0), isPlayingFlag(false), loop(loop) {}

void Flipbook::addFrame(const sf::Texture& texture) {
    frames.push_back(sf::Sprite(texture));
}

void Flipbook::addFrame(const sf::Sprite& sprite) {
	frames.push_back(sprite);
}

void Flipbook::addFrames(const std::vector<sf::Sprite>& sprites)
{
	for (auto sprite : sprites)
	{
		frames.push_back(sprite);
	}
}

void Flipbook::addFrames(const std::vector<sf::Texture>& Textures)
{
	for (auto Texture : Textures)
	{
		frames.push_back(sf::Sprite(Texture));
	}
}

void Flipbook::update(float deltaTime) {
    if (!isPlayingFlag || frames.empty()) return;

    elapsedTime += deltaTime;
    if (elapsedTime >= frameDuration) {
        elapsedTime = 0.f;
        if (currentFrame + 1 < frames.size()) {
            currentFrame++;
        }
        else if (loop) {
            currentFrame = 0;  // Reset to the first frame if looping is enabled
        }
        else {
            isPlayingFlag = false;  // Stop the animation if not looping
        }
    }
}

void Flipbook::registerToLayerManager() {
    if (!frames.empty()) {
        LayerManager::RemoveDrawable(frames[currentFrame]);
        LayerManager::AddDrawable(layer, frames[currentFrame]);
    }
}

void Flipbook::play() {
    isPlayingFlag = true;
}

void Flipbook::pause() {
    isPlayingFlag = false;
}

void Flipbook::stop() {
    isPlayingFlag = false;
    currentFrame = 0;
    elapsedTime = 0.f;
}

void Flipbook::setFrameDuration(float duration) {
    frameDuration = duration;
}

void Flipbook::setLoop(bool shouldLoop) {
    loop = shouldLoop;
}

bool Flipbook::isPlaying() const {
    return isPlayingFlag;
}

