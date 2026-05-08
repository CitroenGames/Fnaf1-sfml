#include "SFML/PaingineSFML.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>

#include <SDL3/SDL.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <imstb_truetype.h>

namespace {
    constexpr float FallbackGlyphWidth = 0.55f;

    [[nodiscard]] double NowSeconds() {
        return static_cast<double>(SDL_GetPerformanceCounter()) /
               static_cast<double>(SDL_GetPerformanceFrequency());
    }

    [[nodiscard]] SDL_Rect ToSDLRect(const sf::IntRect &rect) {
        return SDL_Rect{rect.left, rect.top, rect.width, rect.height};
    }

    [[nodiscard]] SDL_FRect ToSDLFRect(const sf::FloatRect &rect) {
        return SDL_FRect{rect.left, rect.top, rect.width, rect.height};
    }

    [[nodiscard]] SDL_FRect ToSDLFRect(const sf::IntRect &rect) {
        return SDL_FRect{
            static_cast<float>(rect.left),
            static_cast<float>(rect.top),
            static_cast<float>(rect.width),
            static_cast<float>(rect.height)};
    }

    [[nodiscard]] unsigned char ClampToByte(int value) {
        return static_cast<unsigned char>(std::clamp(value, 0, 255));
    }

    [[nodiscard]] SDL_Scancode ToScancode(sf::Keyboard::Key key) {
        switch (key) {
            case sf::Keyboard::A:
                return SDL_SCANCODE_A;
            case sf::Keyboard::B:
                return SDL_SCANCODE_B;
            case sf::Keyboard::C:
                return SDL_SCANCODE_C;
            case sf::Keyboard::D:
                return SDL_SCANCODE_D;
            case sf::Keyboard::E:
                return SDL_SCANCODE_E;
            case sf::Keyboard::F:
                return SDL_SCANCODE_F;
            case sf::Keyboard::G:
                return SDL_SCANCODE_G;
            case sf::Keyboard::H:
                return SDL_SCANCODE_H;
            case sf::Keyboard::I:
                return SDL_SCANCODE_I;
            case sf::Keyboard::J:
                return SDL_SCANCODE_J;
            case sf::Keyboard::K:
                return SDL_SCANCODE_K;
            case sf::Keyboard::L:
                return SDL_SCANCODE_L;
            case sf::Keyboard::M:
                return SDL_SCANCODE_M;
            case sf::Keyboard::N:
                return SDL_SCANCODE_N;
            case sf::Keyboard::O:
                return SDL_SCANCODE_O;
            case sf::Keyboard::P:
                return SDL_SCANCODE_P;
            case sf::Keyboard::Q:
                return SDL_SCANCODE_Q;
            case sf::Keyboard::R:
                return SDL_SCANCODE_R;
            case sf::Keyboard::S:
                return SDL_SCANCODE_S;
            case sf::Keyboard::T:
                return SDL_SCANCODE_T;
            case sf::Keyboard::U:
                return SDL_SCANCODE_U;
            case sf::Keyboard::V:
                return SDL_SCANCODE_V;
            case sf::Keyboard::W:
                return SDL_SCANCODE_W;
            case sf::Keyboard::X:
                return SDL_SCANCODE_X;
            case sf::Keyboard::Y:
                return SDL_SCANCODE_Y;
            case sf::Keyboard::Z:
                return SDL_SCANCODE_Z;
            case sf::Keyboard::Num0:
                return SDL_SCANCODE_0;
            case sf::Keyboard::Num1:
                return SDL_SCANCODE_1;
            case sf::Keyboard::Num2:
                return SDL_SCANCODE_2;
            case sf::Keyboard::Num3:
                return SDL_SCANCODE_3;
            case sf::Keyboard::Num4:
                return SDL_SCANCODE_4;
            case sf::Keyboard::Num5:
                return SDL_SCANCODE_5;
            case sf::Keyboard::Num6:
                return SDL_SCANCODE_6;
            case sf::Keyboard::Num7:
                return SDL_SCANCODE_7;
            case sf::Keyboard::Num8:
                return SDL_SCANCODE_8;
            case sf::Keyboard::Num9:
                return SDL_SCANCODE_9;
            case sf::Keyboard::Left:
                return SDL_SCANCODE_LEFT;
            case sf::Keyboard::Right:
                return SDL_SCANCODE_RIGHT;
            case sf::Keyboard::Up:
                return SDL_SCANCODE_UP;
            case sf::Keyboard::Down:
                return SDL_SCANCODE_DOWN;
            case sf::Keyboard::Enter:
                return SDL_SCANCODE_RETURN;
            case sf::Keyboard::Escape:
                return SDL_SCANCODE_ESCAPE;
            case sf::Keyboard::Space:
                return SDL_SCANCODE_SPACE;
            case sf::Keyboard::Tab:
                return SDL_SCANCODE_TAB;
            case sf::Keyboard::Backspace:
                return SDL_SCANCODE_BACKSPACE;
            case sf::Keyboard::LShift:
                return SDL_SCANCODE_LSHIFT;
            case sf::Keyboard::RShift:
                return SDL_SCANCODE_RSHIFT;
            case sf::Keyboard::LControl:
                return SDL_SCANCODE_LCTRL;
            case sf::Keyboard::RControl:
                return SDL_SCANCODE_RCTRL;
            case sf::Keyboard::F1:
                return SDL_SCANCODE_F1;
            case sf::Keyboard::F2:
                return SDL_SCANCODE_F2;
            default:
                return SDL_SCANCODE_UNKNOWN;
        }
    }
}

namespace sf {
const Color Color::Black{0, 0, 0, 255};
const Color Color::White{255, 255, 255, 255};
const Color Color::Transparent{0, 0, 0, 0};
const Time Time::Zero{0.0f};

Time seconds(float value) {
    return Time(value);
}

Clock::Clock()
    : m_LastSeconds(NowSeconds()) {
}

Time Clock::restart() {
    const double current = NowSeconds();
    const double elapsed = current - m_LastSeconds;
    m_LastSeconds = current;
    return Time(static_cast<float>(elapsed));
}

bool Image::loadFromMemory(const void *data, std::size_t size) {
    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char *pixels = stbi_load_from_memory(
        static_cast<const stbi_uc *>(data),
        static_cast<int>(size),
        &width,
        &height,
        &channels,
        4);

    if (pixels == nullptr || width <= 0 || height <= 0) {
        stbi_image_free(pixels);
        return false;
    }

    m_Size = Vector2u(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
    const std::size_t byteCount = static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4;
    m_Pixels.assign(pixels, pixels + byteCount);
    stbi_image_free(pixels);
    return true;
}

void Image::create(unsigned int width, unsigned int height, const Color &color) {
    m_Size = Vector2u(width, height);
    m_Pixels.resize(static_cast<std::size_t>(width) * height * 4);

    for (std::size_t i = 0; i < m_Pixels.size(); i += 4) {
        m_Pixels[i + 0] = color.r;
        m_Pixels[i + 1] = color.g;
        m_Pixels[i + 2] = color.b;
        m_Pixels[i + 3] = color.a;
    }
}

Vector2u Image::getSize() const {
    return m_Size;
}

Color Image::getPixel(unsigned int x, unsigned int y) const {
    if (x >= m_Size.x || y >= m_Size.y || m_Pixels.empty()) {
        return Color::Transparent;
    }

    const std::size_t index = (static_cast<std::size_t>(y) * m_Size.x + x) * 4;
    return Color(m_Pixels[index + 0], m_Pixels[index + 1], m_Pixels[index + 2], m_Pixels[index + 3]);
}

void Image::setPixel(unsigned int x, unsigned int y, const Color &color) {
    if (x >= m_Size.x || y >= m_Size.y || m_Pixels.empty()) {
        return;
    }

    const std::size_t index = (static_cast<std::size_t>(y) * m_Size.x + x) * 4;
    m_Pixels[index + 0] = color.r;
    m_Pixels[index + 1] = color.g;
    m_Pixels[index + 2] = color.b;
    m_Pixels[index + 3] = color.a;
}

Texture::Texture(const Texture &other)
    : m_Image(other.m_Image),
      m_Smooth(other.m_Smooth),
      m_Repeated(other.m_Repeated) {
}

Texture &Texture::operator=(const Texture &other) {
    if (this != &other) {
        releaseNative();
        m_Image = other.m_Image;
        m_Smooth = other.m_Smooth;
        m_Repeated = other.m_Repeated;
    }
    return *this;
}

Texture::Texture(Texture &&other) noexcept
    : m_Image(std::move(other.m_Image)),
      m_Smooth(other.m_Smooth),
      m_Repeated(other.m_Repeated),
      m_Texture(other.m_Texture),
      m_TextureRenderer(other.m_TextureRenderer) {
    other.m_Texture = nullptr;
    other.m_TextureRenderer = nullptr;
}

Texture &Texture::operator=(Texture &&other) noexcept {
    if (this != &other) {
        releaseNative();
        m_Image = std::move(other.m_Image);
        m_Smooth = other.m_Smooth;
        m_Repeated = other.m_Repeated;
        m_Texture = other.m_Texture;
        m_TextureRenderer = other.m_TextureRenderer;
        other.m_Texture = nullptr;
        other.m_TextureRenderer = nullptr;
    }
    return *this;
}

Texture::~Texture() {
    releaseNative();
}

bool Texture::loadFromMemory(const void *data, std::size_t size) {
    Image image;
    if (!image.loadFromMemory(data, size)) {
        return false;
    }

    return loadFromImage(image);
}

bool Texture::loadFromImage(const Image &image) {
    if (image.getSize().x == 0 || image.getSize().y == 0 || image.pixels().empty()) {
        return false;
    }

    releaseNative();
    m_Image = image;
    return true;
}

Image Texture::copyToImage() const {
    return m_Image;
}

Vector2u Texture::getSize() const {
    return m_Image.getSize();
}

void Texture::setSmooth(bool smooth) {
    m_Smooth = smooth;
}

bool Texture::isSmooth() const {
    return m_Smooth;
}

void Texture::setRepeated(bool repeated) {
    m_Repeated = repeated;
}

bool Texture::isRepeated() const {
    return m_Repeated;
}

SDL_Texture *Texture::native(SDL_Renderer *renderer) const {
    if (renderer == nullptr || m_Image.getSize().x == 0 || m_Image.getSize().y == 0 || m_Image.pixels().empty()) {
        return nullptr;
    }

    if (m_Texture != nullptr && m_TextureRenderer == renderer) {
        return m_Texture;
    }

    releaseNative();
    m_Texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STATIC,
        static_cast<int>(m_Image.getSize().x),
        static_cast<int>(m_Image.getSize().y));

    if (m_Texture == nullptr) {
        std::cerr << "SDL texture creation failed: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    SDL_SetTextureBlendMode(m_Texture, SDL_BLENDMODE_BLEND);

    const SDL_Rect rect{0, 0, static_cast<int>(m_Image.getSize().x), static_cast<int>(m_Image.getSize().y)};
    if (!SDL_UpdateTexture(m_Texture, &rect, m_Image.pixels().data(), static_cast<int>(m_Image.getSize().x * 4))) {
        std::cerr << "SDL texture upload failed: " << SDL_GetError() << std::endl;
        releaseNative();
        return nullptr;
    }

    m_TextureRenderer = renderer;
    return m_Texture;
}

void Texture::releaseNative() const {
    if (m_Texture != nullptr) {
        SDL_DestroyTexture(m_Texture);
        m_Texture = nullptr;
        m_TextureRenderer = nullptr;
    }
}

View::View()
    : m_Center(0.0f, 0.0f),
      m_Size(0.0f, 0.0f) {
}

View::View(const FloatRect &rect)
    : m_Center(rect.left + rect.width * 0.5f, rect.top + rect.height * 0.5f),
      m_Size(rect.width, rect.height) {
}

void View::setCenter(const Vector2f &center) {
    m_Center = center;
}

void View::setCenter(float x, float y) {
    m_Center = Vector2f(x, y);
}

Vector2f View::getCenter() const {
    return m_Center;
}

void View::setSize(const Vector2f &size) {
    m_Size = size;
}

void View::setSize(float width, float height) {
    m_Size = Vector2f(width, height);
}

Vector2f View::getSize() const {
    return m_Size;
}

void View::setViewport(const FloatRect &viewport) {
    m_Viewport = viewport;
}

FloatRect View::getViewport() const {
    return m_Viewport;
}

void View::setRotation(float angle) {
    m_Rotation = angle;
}

float View::getRotation() const {
    return m_Rotation;
}

RenderTarget::RenderTarget(const Vector2u &size)
    : m_View(FloatRect(0.0f, 0.0f, static_cast<float>(size.x), static_cast<float>(size.y))),
      m_DefaultView(m_View),
      m_Size(size) {
}

void RenderTarget::draw(const Drawable &drawable) {
    drawable.draw(*this);
}

void RenderTarget::setView(const View &view) {
    m_View = view;
}

const View &RenderTarget::getView() const {
    return m_View;
}

const View &RenderTarget::getDefaultView() const {
    return m_DefaultView;
}

Vector2u RenderTarget::getSize() const {
    return m_Size;
}

Vector2f RenderTarget::mapPixelToCoords(const Vector2i &point) const {
    return mapPixelToCoords(point, m_View);
}

Vector2f RenderTarget::mapPixelToCoords(const Vector2i &point, const View &view) const {
    const FloatRect viewport = viewportPixels(view);
    if (viewport.width <= 0.0f || viewport.height <= 0.0f) {
        return Vector2f();
    }

    const Vector2f size = view.getSize();
    const Vector2f topLeft = view.getCenter() - size / 2.0f;
    const float normalizedX = (static_cast<float>(point.x) - viewport.left) / viewport.width;
    const float normalizedY = (static_cast<float>(point.y) - viewport.top) / viewport.height;
    return Vector2f(topLeft.x + normalizedX * size.x, topLeft.y + normalizedY * size.y);
}

Vector2i RenderTarget::mapCoordsToPixel(const Vector2f &point, const View &view) const {
    const FloatRect viewport = viewportPixels(view);
    const Vector2f size = view.getSize();
    if (size.x == 0.0f || size.y == 0.0f) {
        return Vector2i();
    }

    const Vector2f topLeft = view.getCenter() - size / 2.0f;
    const float normalizedX = (point.x - topLeft.x) / size.x;
    const float normalizedY = (point.y - topLeft.y) / size.y;
    return Vector2i(
        static_cast<int>(viewport.left + normalizedX * viewport.width),
        static_cast<int>(viewport.top + normalizedY * viewport.height));
}

void RenderTarget::setTargetSize(const Vector2u &size) {
    m_Size = size;
    m_DefaultView = View(FloatRect(0.0f, 0.0f, static_cast<float>(size.x), static_cast<float>(size.y)));
}

SDL_Renderer *RenderTarget::renderer() const {
    return m_Renderer;
}

void RenderTarget::setRenderer(SDL_Renderer *renderer) {
    m_Renderer = renderer;
}

FloatRect RenderTarget::worldToTargetRect(const FloatRect &worldRect) const {
    const Vector2f topLeft = mapPixelToCoords(Vector2i(0, 0), m_DefaultView);
    (void) topLeft;

    const FloatRect viewport = viewportPixels(m_View);
    const Vector2f viewSize = m_View.getSize();
    const Vector2f viewTopLeft = m_View.getCenter() - viewSize / 2.0f;

    if (viewSize.x == 0.0f || viewSize.y == 0.0f) {
        return FloatRect();
    }

    return FloatRect(
        viewport.left + ((worldRect.left - viewTopLeft.x) / viewSize.x) * viewport.width,
        viewport.top + ((worldRect.top - viewTopLeft.y) / viewSize.y) * viewport.height,
        (worldRect.width / viewSize.x) * viewport.width,
        (worldRect.height / viewSize.y) * viewport.height);
}

FloatRect RenderTarget::viewportPixels(const View &view) const {
    const FloatRect viewport = view.getViewport();
    return FloatRect(
        viewport.left * static_cast<float>(m_Size.x),
        viewport.top * static_cast<float>(m_Size.y),
        viewport.width * static_cast<float>(m_Size.x),
        viewport.height * static_cast<float>(m_Size.y));
}

Sprite::Sprite(const Texture &texture) {
    setTexture(texture, true);
}

void Sprite::setTexture(const Texture &texture, bool resetRect) {
    m_Texture = &texture;
    if (resetRect || m_TextureRect.width == 0 || m_TextureRect.height == 0) {
        const Vector2u size = texture.getSize();
        m_TextureRect = IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y));
    }
}

const Texture *Sprite::getTexture() const {
    return m_Texture;
}

void Sprite::setTextureRect(const IntRect &rect) {
    m_TextureRect = rect;
}

IntRect Sprite::getTextureRect() const {
    return m_TextureRect;
}

void Sprite::setPosition(float x, float y) {
    m_Position = Vector2f(x, y);
}

void Sprite::setPosition(const Vector2f &position) {
    m_Position = position;
}

Vector2f Sprite::getPosition() const {
    return m_Position;
}

void Sprite::setOrigin(float x, float y) {
    m_Origin = Vector2f(x, y);
}

void Sprite::setOrigin(const Vector2f &origin) {
    m_Origin = origin;
}

Vector2f Sprite::getOrigin() const {
    return m_Origin;
}

void Sprite::setScale(float x, float y) {
    m_Scale = Vector2f(x, y);
}

void Sprite::setScale(const Vector2f &scale) {
    m_Scale = scale;
}

Vector2f Sprite::getScale() const {
    return m_Scale;
}

void Sprite::setRotation(float angle) {
    m_Rotation = angle;
}

float Sprite::getRotation() const {
    return m_Rotation;
}

void Sprite::setColor(const Color &color) {
    m_Color = color;
}

Color Sprite::getColor() const {
    return m_Color;
}

FloatRect Sprite::getLocalBounds() const {
    return FloatRect(
        0.0f,
        0.0f,
        static_cast<float>(m_TextureRect.width),
        static_cast<float>(m_TextureRect.height));
}

FloatRect Sprite::getGlobalBounds() const {
    return FloatRect(
        m_Position.x - m_Origin.x * m_Scale.x,
        m_Position.y - m_Origin.y * m_Scale.y,
        static_cast<float>(m_TextureRect.width) * m_Scale.x,
        static_cast<float>(m_TextureRect.height) * m_Scale.y);
}

void Sprite::draw(RenderTarget &target) const {
    if (m_Texture == nullptr || target.renderer() == nullptr) {
        return;
    }

    SDL_Texture *texture = m_Texture->native(target.renderer());
    if (texture == nullptr) {
        return;
    }

    SDL_SetTextureColorMod(texture, m_Color.r, m_Color.g, m_Color.b);
    SDL_SetTextureAlphaMod(texture, m_Color.a);

    const SDL_FRect source = ToSDLFRect(m_TextureRect);
    const SDL_FRect destination = ToSDLFRect(target.worldToTargetRect(getGlobalBounds()));
    const SDL_FPoint center{m_Origin.x * m_Scale.x, m_Origin.y * m_Scale.y};
    SDL_RenderTextureRotated(
        target.renderer(),
        texture,
        &source,
        &destination,
        static_cast<double>(m_Rotation),
        &center,
        SDL_FLIP_NONE);
}

void Shape::setFillColor(const Color &color) {
    m_FillColor = color;
}

Color Shape::getFillColor() const {
    return m_FillColor;
}

RectangleShape::RectangleShape(const Vector2f &size)
    : m_Size(size) {
}

void RectangleShape::setSize(const Vector2f &size) {
    m_Size = size;
}

Vector2f RectangleShape::getSize() const {
    return m_Size;
}

void RectangleShape::setPosition(float x, float y) {
    m_Position = Vector2f(x, y);
}

void RectangleShape::setPosition(const Vector2f &position) {
    m_Position = position;
}

Vector2f RectangleShape::getPosition() const {
    return m_Position;
}

FloatRect RectangleShape::getLocalBounds() const {
    return FloatRect(0.0f, 0.0f, m_Size.x, m_Size.y);
}

FloatRect RectangleShape::getGlobalBounds() const {
    return FloatRect(m_Position.x, m_Position.y, m_Size.x, m_Size.y);
}

void RectangleShape::draw(RenderTarget &target) const {
    if (target.renderer() == nullptr) {
        return;
    }

    SDL_SetRenderDrawBlendMode(target.renderer(), SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(target.renderer(), m_FillColor.r, m_FillColor.g, m_FillColor.b, m_FillColor.a);
    const SDL_FRect rect = ToSDLFRect(target.worldToTargetRect(getGlobalBounds()));
    SDL_RenderFillRect(target.renderer(), &rect);
}

struct Font::Impl {
    std::vector<unsigned char> data;
    stbtt_fontinfo info{};
    bool loaded = false;
};

Font::Font()
    : m_Impl(std::make_unique<Impl>()) {
}

Font::~Font() = default;
Font::Font(Font &&) noexcept = default;
Font &Font::operator=(Font &&) noexcept = default;

bool Font::loadFromMemory(const void *data, std::size_t size) {
    if (data == nullptr || size == 0) {
        return false;
    }

    m_Impl->data.assign(static_cast<const unsigned char *>(data), static_cast<const unsigned char *>(data) + size);
    const int offset = stbtt_GetFontOffsetForIndex(m_Impl->data.data(), 0);
    if (offset < 0 || !stbtt_InitFont(&m_Impl->info, m_Impl->data.data(), offset)) {
        m_Impl->data.clear();
        m_Impl->loaded = false;
        return false;
    }

    m_Impl->loaded = true;
    return true;
}

bool Font::isLoaded() const {
    return m_Impl && m_Impl->loaded;
}

FloatRect Font::measure(const std::string &text, unsigned int characterSize) const {
    if (!isLoaded() || text.empty()) {
        return FloatRect(0.0f, 0.0f, static_cast<float>(text.size()) * characterSize * FallbackGlyphWidth,
                         static_cast<float>(characterSize));
    }

    const float scale = stbtt_ScaleForPixelHeight(&m_Impl->info, static_cast<float>(characterSize));
    int ascent = 0;
    int descent = 0;
    int lineGap = 0;
    stbtt_GetFontVMetrics(&m_Impl->info, &ascent, &descent, &lineGap);

    float cursor = 0.0f;
    for (std::size_t i = 0; i < text.size(); ++i) {
        const int codepoint = static_cast<unsigned char>(text[i]);
        int advance = 0;
        int lsb = 0;
        stbtt_GetCodepointHMetrics(&m_Impl->info, codepoint, &advance, &lsb);
        cursor += static_cast<float>(advance) * scale;
        if (i + 1 < text.size()) {
            cursor += static_cast<float>(
                          stbtt_GetCodepointKernAdvance(
                              &m_Impl->info,
                              codepoint,
                              static_cast<unsigned char>(text[i + 1]))) *
                      scale;
        }
    }

    const float height = static_cast<float>(ascent - descent + lineGap) * scale;
    return FloatRect(0.0f, 0.0f, std::max(cursor, 1.0f), std::max(height, static_cast<float>(characterSize)));
}

Image Font::rasterize(const std::string &text, unsigned int characterSize) const {
    const FloatRect metrics = measure(text, characterSize);
    const unsigned int width = std::max(1u, static_cast<unsigned int>(std::ceil(metrics.width + 4.0f)));
    const unsigned int height = std::max(1u, static_cast<unsigned int>(std::ceil(metrics.height + 4.0f)));

    Image image;
    image.create(width, height, Color::Transparent);

    if (!isLoaded() || text.empty()) {
        return image;
    }

    std::vector<std::uint8_t> pixels = image.pixels();
    const float scale = stbtt_ScaleForPixelHeight(&m_Impl->info, static_cast<float>(characterSize));
    int ascent = 0;
    int descent = 0;
    int lineGap = 0;
    stbtt_GetFontVMetrics(&m_Impl->info, &ascent, &descent, &lineGap);

    float cursorX = 2.0f;
    const int baseline = static_cast<int>(2.0f + ascent * scale);

    for (std::size_t i = 0; i < text.size(); ++i) {
        const int codepoint = static_cast<unsigned char>(text[i]);
        int advance = 0;
        int lsb = 0;
        stbtt_GetCodepointHMetrics(&m_Impl->info, codepoint, &advance, &lsb);

        int glyphWidth = 0;
        int glyphHeight = 0;
        int offsetX = 0;
        int offsetY = 0;
        unsigned char *bitmap = stbtt_GetCodepointBitmap(
            &m_Impl->info,
            0.0f,
            scale,
            codepoint,
            &glyphWidth,
            &glyphHeight,
            &offsetX,
            &offsetY);

        const int dstX = static_cast<int>(cursorX) + offsetX;
        const int dstY = baseline + offsetY;
        for (int y = 0; y < glyphHeight; ++y) {
            const int targetY = dstY + y;
            if (targetY < 0 || targetY >= static_cast<int>(height)) {
                continue;
            }

            for (int x = 0; x < glyphWidth; ++x) {
                const int targetX = dstX + x;
                if (targetX < 0 || targetX >= static_cast<int>(width)) {
                    continue;
                }

                const unsigned char alpha = bitmap[y * glyphWidth + x];
                const std::size_t pixelIndex =
                    (static_cast<std::size_t>(targetY) * width + static_cast<std::size_t>(targetX)) * 4;
                pixels[pixelIndex + 0] = 255;
                pixels[pixelIndex + 1] = 255;
                pixels[pixelIndex + 2] = 255;
                pixels[pixelIndex + 3] = std::max(pixels[pixelIndex + 3], alpha);
            }
        }

        stbtt_FreeBitmap(bitmap, nullptr);

        cursorX += static_cast<float>(advance) * scale;
        if (i + 1 < text.size()) {
            cursorX += static_cast<float>(
                           stbtt_GetCodepointKernAdvance(
                               &m_Impl->info,
                               codepoint,
                               static_cast<unsigned char>(text[i + 1]))) *
                       scale;
        }
    }

    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            const std::size_t pixelIndex = (static_cast<std::size_t>(y) * width + x) * 4;
            image.setPixel(
                x,
                y,
                Color(
                    pixels[pixelIndex + 0],
                    pixels[pixelIndex + 1],
                    pixels[pixelIndex + 2],
                    pixels[pixelIndex + 3]));
        }
    }

    return image;
}

Text::Text() = default;

void Text::setFont(const Font &font) {
    m_Font = &font;
    m_TextureDirty = true;
}

void Text::setString(const std::string &text) {
    m_String = text;
    m_TextureDirty = true;
}

void Text::setCharacterSize(unsigned int size) {
    m_CharacterSize = size;
    m_TextureDirty = true;
}

void Text::setFillColor(const Color &color) {
    m_Color = color;
}

Color Text::getFillColor() const {
    return m_Color;
}

void Text::setPosition(float x, float y) {
    m_Position = Vector2f(x, y);
}

void Text::setPosition(const Vector2f &position) {
    m_Position = position;
}

Vector2f Text::getPosition() const {
    return m_Position;
}

FloatRect Text::getLocalBounds() const {
    if (m_Font != nullptr) {
        return m_Font->measure(m_String, m_CharacterSize);
    }

    return FloatRect(0.0f, 0.0f, static_cast<float>(m_String.size()) * m_CharacterSize * FallbackGlyphWidth,
                     static_cast<float>(m_CharacterSize));
}

FloatRect Text::getGlobalBounds() const {
    const FloatRect local = getLocalBounds();
    return FloatRect(m_Position.x + local.left, m_Position.y + local.top, local.width, local.height);
}

void Text::draw(RenderTarget &target) const {
    if (target.renderer() == nullptr || m_String.empty() || m_Font == nullptr) {
        return;
    }

    refreshTexture();
    SDL_Texture *texture = m_Texture.native(target.renderer());
    if (texture == nullptr) {
        return;
    }

    SDL_SetTextureColorMod(texture, m_Color.r, m_Color.g, m_Color.b);
    SDL_SetTextureAlphaMod(texture, m_Color.a);
    const Vector2u textureSize = m_Texture.getSize();
    const SDL_FRect source{0.0f, 0.0f, static_cast<float>(textureSize.x), static_cast<float>(textureSize.y)};
    const SDL_FRect destination = ToSDLFRect(target.worldToTargetRect(
        FloatRect(m_Position.x, m_Position.y, source.w, source.h)));
    SDL_RenderTexture(target.renderer(), texture, &source, &destination);
}

void Text::refreshTexture() const {
    if (!m_TextureDirty || m_Font == nullptr) {
        return;
    }

    m_Texture.loadFromImage(m_Font->rasterize(m_String, m_CharacterSize));
    m_TextureDirty = false;
}

RenderTexture::RenderTexture()
    : RenderTarget(Vector2u(0, 0)) {
}

RenderWindow::RenderWindow()
    : RenderTarget(Vector2u(0, 0)) {
}

RenderWindow::RenderWindow(VideoMode mode, const std::string &title)
    : RenderWindow() {
    create(mode, title);
}

RenderWindow::~RenderWindow() {
    close();
    if (renderer() != nullptr) {
        SDL_DestroyRenderer(renderer());
        setRenderer(nullptr);
    }

    if (m_Window != nullptr) {
        SDL_DestroyWindow(m_Window);
        m_Window = nullptr;
    }
}

bool RenderWindow::create(VideoMode mode, const std::string &title) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    m_Window = SDL_CreateWindow(title.c_str(), static_cast<int>(mode.width), static_cast<int>(mode.height), SDL_WINDOW_RESIZABLE);
    if (m_Window == nullptr) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_Renderer *createdRenderer = SDL_CreateRenderer(m_Window, nullptr);
    if (createdRenderer == nullptr) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(m_Window);
        m_Window = nullptr;
        return false;
    }

    setRenderer(createdRenderer);
    setTargetSize(Vector2u(mode.width, mode.height));
    setView(getDefaultView());
    SDL_SetRenderDrawBlendMode(createdRenderer, SDL_BLENDMODE_BLEND);
    m_Open = true;
    return true;
}

bool RenderWindow::isOpen() const {
    return m_Open;
}

void RenderWindow::close() {
    m_Open = false;
}

bool RenderWindow::pollEvent(Event &event) {
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        event.native = sdlEvent;
        switch (sdlEvent.type) {
            case SDL_EVENT_QUIT:
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                event.type = Event::Closed;
                return true;
            case SDL_EVENT_WINDOW_RESIZED: {
                int width = 0;
                int height = 0;
                SDL_GetWindowSize(m_Window, &width, &height);
                setTargetSize(Vector2u(static_cast<unsigned int>(std::max(width, 1)),
                                       static_cast<unsigned int>(std::max(height, 1))));
                event.type = Event::Resized;
                return true;
            }
            default:
                event.type = Event::Other;
                return true;
        }
    }

    return false;
}

void RenderWindow::clear(const Color &color) {
    if (renderer() == nullptr) {
        return;
    }

    SDL_SetRenderDrawColor(renderer(), color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer());
}

void RenderWindow::display() {
    if (renderer() != nullptr) {
        SDL_RenderPresent(renderer());
    }
}

void RenderWindow::setVerticalSyncEnabled(bool enabled) {
    if (renderer() != nullptr) {
        SDL_SetRenderVSync(renderer(), enabled ? 1 : 0);
    }
}

SDL_Window *RenderWindow::nativeWindow() const {
    return m_Window;
}

SDL_Renderer *RenderWindow::nativeRenderer() const {
    return renderer();
}

bool Keyboard::isKeyPressed(Key key) {
    int keyCount = 0;
    const bool *state = SDL_GetKeyboardState(&keyCount);
    const SDL_Scancode scancode = ToScancode(key);
    return scancode != SDL_SCANCODE_UNKNOWN && scancode < keyCount && state[scancode];
}

bool Mouse::isButtonPressed(Button button) {
    if (button != Left) {
        return false;
    }

    return (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_LMASK) != 0;
}

Vector2i Mouse::getPosition(const RenderWindow &window) {
    (void) window;
    float x = 0.0f;
    float y = 0.0f;
    SDL_GetMouseState(&x, &y);
    return Vector2i(static_cast<int>(std::round(x)), static_cast<int>(std::round(y)));
}
} // namespace sf
