#pragma once
#include "IAnimation.hpp"
#include "ScopedAnimationId.hpp"
#include <memory>

class IAnimationManager
{
public:
    virtual void cancelAnimationById(AnimationId id) = 0;
    virtual ScopedAnimationId addAnimation(std::unique_ptr<IAnimation> animation) = 0;
    virtual AnimationId addAnimationRawId(std::unique_ptr<IAnimation> animation) = 0;
    virtual void play(std::chrono::milliseconds ms) = 0;
    virtual ~IAnimationManager() = default;
};