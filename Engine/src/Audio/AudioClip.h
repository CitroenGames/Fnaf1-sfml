#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class AudioBuffer {
public:
    static std::shared_ptr<AudioBuffer> CreateFromMemory(const void *data,
                                                         std::size_t size,
                                                         const std::string &debugName);

    ~AudioBuffer();

    AudioBuffer(const AudioBuffer &) = delete;
    AudioBuffer &operator=(const AudioBuffer &) = delete;
    AudioBuffer(AudioBuffer &&) noexcept;
    AudioBuffer &operator=(AudioBuffer &&) noexcept;

    bool IsValid() const;

private:
    struct Impl;

    explicit AudioBuffer(std::unique_ptr<Impl> impl);

    std::unique_ptr<Impl> m_Impl;

    friend class AudioClip;
};

class AudioClip {
public:
    enum class Status {
        Stopped,
        Playing
    };

    static std::shared_ptr<AudioClip> Create(std::shared_ptr<AudioBuffer> buffer,
                                             const std::string &debugName);

    ~AudioClip();

    AudioClip(const AudioClip &) = delete;
    AudioClip &operator=(const AudioClip &) = delete;
    AudioClip(AudioClip &&) noexcept;
    AudioClip &operator=(AudioClip &&) noexcept;

    void play();
    void stop();
    void setLoop(bool loop);
    void setVolume(float volume);

    Status getStatus() const;
    bool IsValid() const;

private:
    struct Impl;

    AudioClip(std::shared_ptr<AudioBuffer> buffer, std::unique_ptr<Impl> impl);

    std::shared_ptr<AudioBuffer> m_Buffer;
    std::unique_ptr<Impl> m_Impl;
};
