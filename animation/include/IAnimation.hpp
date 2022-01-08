#pragma once
#include <chrono>
#include <cstdint>
#include <functional>

using AnimationId = std::uint32_t;

class IAnimation
{
public:
    virtual void play(std::chrono::milliseconds ms) = 0;
    virtual void invokeFinishedCallback() const = 0;
    virtual bool isFinished() const = 0;
    virtual void setFinishedCallback(std::function<void()> callback) = 0;

    IAnimation(const IAnimation&) = delete;
    void operator=(const IAnimation&) = delete;
    IAnimation() = default;
    virtual ~IAnimation() = default;
};