#pragma once
#include <chrono>
#include <cstdint>
#include <functional>

using AnimationId = std::uint32_t;

template <typename DurationType = std::chrono::milliseconds>
class IAnimation
{
public:
    virtual void play(DurationType t) = 0;
    virtual void invokeFinishedCallback() const = 0;
    virtual bool isFinished() const = 0;
    virtual void setFinishedCallback(std::function<void()> callback) = 0;

    IAnimation(const IAnimation&) = delete;
    void operator=(const IAnimation&) = delete;
    IAnimation() = default;
    virtual ~IAnimation() = default;
};