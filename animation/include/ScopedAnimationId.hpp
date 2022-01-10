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
    ScopedAnimationIdImpl(AnimationId id, IAnimationManagerImpl<DurationType, AnimationPointerType>* manager)
        : m_AnimationId{id}, m_Manager{manager}
    {
        assert(manager != nullptr);
    }
    ScopedAnimationIdImpl(ScopedAnimationIdImpl& other) : m_AnimationId{other.m_AnimationId}, m_Manager{other.m_Manager}
    {
        other.m_Manager = nullptr;
    }
    ScopedAnimationIdImpl(ScopedAnimationIdImpl&& other)
        : m_AnimationId{std::move(other).m_AnimationId}, m_Manager{std::move(other).m_Manager}
    {
        other.m_Manager = nullptr;
    }
    ScopedAnimationIdImpl& operator=(ScopedAnimationIdImpl& other)
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
    ScopedAnimationIdImpl& operator=(ScopedAnimationIdImpl&& other)
    {
        if (m_Manager)
        {
            m_Manager->cancelAnimationById(m_AnimationId);
        }
        m_Manager = std::move(other).m_Manager;
        m_AnimationId = std::move(other).m_AnimationId;
        other.m_Manager = nullptr;
        return *this;
    }
    ~ScopedAnimationIdImpl()
    {
        if (m_Manager)
        {
            m_Manager->cancelAnimationById(m_AnimationId);
        }
    }
    operator const AnimationId&() const { return m_AnimationId; }

private:
    AnimationId m_AnimationId;
    IAnimationManagerImpl<DurationType, AnimationPointerType>* m_Manager;
};

using ScopedAnimationId = ScopedAnimationIdImpl<>;
