#pragma once
#include "IAnimation.hpp"
#include <cassert>

class IAnimationManager;
class ScopedAnimationId
{
public:
    ScopedAnimationId(AnimationId id, IAnimationManager* manager);
    ScopedAnimationId(const ScopedAnimationId& other);
    ScopedAnimationId& operator=(const ScopedAnimationId& other);
    ~ScopedAnimationId();
    operator AnimationId();

private:
    AnimationId m_AnimationId;
    IAnimationManager* m_Manager;
};