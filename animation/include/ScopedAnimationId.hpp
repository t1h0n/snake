#pragma once
#include "IAnimation.hpp"
#include <cassert>
#include <memory>

template <typename DurationType, typename AnimationPointerType>
class IAnimationManager;

template <typename DurationType = std::chrono::milliseconds,
          typename AnimationPointerType = std::unique_ptr<IAnimation<DurationType>>>
class ScopedAnimationId
{
public:
    ScopedAnimationId(AnimationId id, IAnimationManager<DurationType, AnimationPointerType>* manager);
    ScopedAnimationId(const ScopedAnimationId& other);
    ScopedAnimationId& operator=(const ScopedAnimationId& other);
    ~ScopedAnimationId();
    operator AnimationId();

private:
    AnimationId m_AnimationId;
    IAnimationManager<DurationType, AnimationPointerType>* m_Manager;
};

template <typename DurationType, typename AnimationPointerType>
inline ScopedAnimationId<DurationType, AnimationPointerType>::ScopedAnimationId(
    AnimationId id, IAnimationManager<DurationType, AnimationPointerType>* manager)
    : m_AnimationId{id}, m_Manager{manager}
{
    assert(manager != nullptr);
}
template <typename DurationType, typename AnimationPointerType>
inline ScopedAnimationId<DurationType, AnimationPointerType>::ScopedAnimationId(const ScopedAnimationId& other)
    : m_AnimationId{other.m_AnimationId}, m_Manager{other.m_Manager}
{
}
template <typename DurationType, typename AnimationPointerType>
inline ScopedAnimationId<DurationType, AnimationPointerType>& ScopedAnimationId<DurationType, AnimationPointerType>::operator=(
    const ScopedAnimationId<DurationType, AnimationPointerType>& other)
{
    if (this != &other)
    {
        m_Manager->cancelAnimationById(m_AnimationId);
        m_Manager = other.m_Manager;
        m_AnimationId = other.m_AnimationId;
    }
    return *this;
}
template <typename DurationType, typename AnimationPointerType>
inline ScopedAnimationId<DurationType, AnimationPointerType>::~ScopedAnimationId()
{
    m_Manager->cancelAnimationById(m_AnimationId);
}
template <typename DurationType, typename AnimationPointerType>
inline ScopedAnimationId<DurationType, AnimationPointerType>::operator AnimationId()
{
    return m_AnimationId;
}
