#pragma once
#include "IAnimation.hpp"
#include "ScopedAnimationId.hpp"

template <typename DurationType = std::chrono::milliseconds, typename AnimationPointerType = std::unique_ptr<IAnimationImpl<DurationType>>>
class IAnimationManagerImpl
{
public:
    virtual void cancelAnimationById(AnimationId id) = 0;
    [[nodiscard]] virtual ScopedAnimationIdImpl<DurationType, AnimationPointerType> addAnimation(AnimationPointerType animation) = 0;
    virtual AnimationId addAnimationRawId(AnimationPointerType animation) = 0;
    virtual void play(DurationType t) = 0;
    virtual ~IAnimationManagerImpl() = default;
    IAnimationManagerImpl() = default;
    IAnimationManagerImpl(const IAnimationManagerImpl&) = delete;
    void operator=(const IAnimationManagerImpl&) = delete;
};

using IAnimationManager = IAnimationManagerImpl<>;