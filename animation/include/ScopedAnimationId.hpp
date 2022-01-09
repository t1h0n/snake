#pragma once
#include "IAnimation.hpp"
#include <cassert>
#include <memory>

template <typename DurationType, typename AnimationPointerType>
class IAnimationManagerImpl;

template <typename DurationType = std::chrono::milliseconds, typename AnimationPointerType = std::unique_ptr<IAnimationImpl<DurationType>>>
class ScopedAnimationIdImpl
{
public:
    ScopedAnimationIdImpl() : m_AnimationId{static_cast<AnimationId>(0)}, m_Manager{nullptr} {}
    ScopedAnimationIdImpl(AnimationId id, IAnimationManagerImpl<DurationType, AnimationPointerType>* manager);
    ScopedAnimationIdImpl(ScopedAnimationIdImpl& other);
    ScopedAnimationIdImpl(ScopedAnimationIdImpl&& other);
    ScopedAnimationIdImpl& operator=(ScopedAnimationIdImpl& other);
    ScopedAnimationIdImpl& operator=(ScopedAnimationIdImpl&& other);
    ~ScopedAnimationIdImpl();
    operator AnimationId();

private:
    AnimationId m_AnimationId;
    IAnimationManagerImpl<DurationType, AnimationPointerType>* m_Manager;
};

template <typename DurationType, typename AnimationPointerType>
inline ScopedAnimationIdImpl<DurationType, AnimationPointerType>::ScopedAnimationIdImpl(
    AnimationId id, IAnimationManagerImpl<DurationType, AnimationPointerType>* manager)
    : m_AnimationId{id}, m_Manager{manager}
{
    assert(manager != nullptr);
}
template <typename DurationType, typename AnimationPointerType>
inline ScopedAnimationIdImpl<DurationType, AnimationPointerType>::ScopedAnimationIdImpl(ScopedAnimationIdImpl& other)
    : m_AnimationId{other.m_AnimationId}, m_Manager{other.m_Manager}
{
    other.m_Manager = nullptr;
}
template <typename DurationType, typename AnimationPointerType>
inline ScopedAnimationIdImpl<DurationType, AnimationPointerType>::ScopedAnimationIdImpl(ScopedAnimationIdImpl&& other)
    : m_AnimationId{other.m_AnimationId}, m_Manager{other.m_Manager}
{
    other.m_Manager = nullptr;
}
template <typename DurationType, typename AnimationPointerType>
inline ScopedAnimationIdImpl<DurationType, AnimationPointerType>&
ScopedAnimationIdImpl<DurationType, AnimationPointerType>::operator=(ScopedAnimationIdImpl<DurationType, AnimationPointerType>& other)
{
    if (m_Manager)
    {
        m_Manager->cancelAnimationById(m_AnimationId);
    }
    m_Manager = other.m_Manager;
    m_AnimationId = other.m_AnimationId;
    other.m_Manager = nullptr;
    return *this;
}
template <typename DurationType, typename AnimationPointerType>
inline ScopedAnimationIdImpl<DurationType, AnimationPointerType>&
ScopedAnimationIdImpl<DurationType, AnimationPointerType>::operator=(ScopedAnimationIdImpl<DurationType, AnimationPointerType>&& other)
{
    if (m_Manager)
    {
        m_Manager->cancelAnimationById(m_AnimationId);
    }
    m_Manager = other.m_Manager;
    m_AnimationId = other.m_AnimationId;
    other.m_Manager = nullptr;
    return *this;
}
template <typename DurationType, typename AnimationPointerType>
inline ScopedAnimationIdImpl<DurationType, AnimationPointerType>::~ScopedAnimationIdImpl()
{
    if (m_Manager)
    {
        m_Manager->cancelAnimationById(m_AnimationId);
    }
}
template <typename DurationType, typename AnimationPointerType>
inline ScopedAnimationIdImpl<DurationType, AnimationPointerType>::operator AnimationId()
{
    return m_AnimationId;
}

using ScopedAnimationId = ScopedAnimationIdImpl<>;