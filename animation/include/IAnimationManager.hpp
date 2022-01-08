#pragma once
#include "IAnimation.hpp"
#include "ScopedAnimationId.hpp"
#include <memory>

template <typename DurationType = std::chrono::milliseconds, typename AnimationPointerType = std::unique_ptr<IAnimation<DurationType>>>
class IAnimationManager
{
public:
    virtual void cancelAnimationById(AnimationId id) = 0;
    virtual ScopedAnimationId<DurationType, AnimationPointerType> addAnimation(AnimationPointerType animation) = 0;
    virtual AnimationId addAnimationRawId(AnimationPointerType animation) = 0;
    virtual void play(DurationType t) = 0;
    virtual ~IAnimationManager() = default;
};