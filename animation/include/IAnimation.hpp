#pragma once
#include <chrono>
#include <cstdint>
#include <functional>

using AnimationId = std::uint32_t;

template <typename DurationType = std::chrono::milliseconds>
class IAnimationImpl
{
protected:
    static constexpr auto ZERO_DURATION = DurationType{static_cast<typename DurationType::rep>(0)};

public:
    virtual void play(DurationType t) = 0;
    virtual void invokeFinishedCallback() const = 0;
    virtual bool isFinished() const = 0;
    virtual void setFinishedCallback(std::function<void()> callback) = 0;

    IAnimationImpl() = default;
    virtual ~IAnimationImpl() = default;
};

using IAnimation = IAnimationImpl<>;