#include "ScopedAnimationId.hpp"
#include "IAnimationManager.hpp"

ScopedAnimationId::ScopedAnimationId(AnimationId id, IAnimationManager* manager) : m_AnimationId{id}, m_Manager{manager}
{
    assert(manager != nullptr);
}
ScopedAnimationId::ScopedAnimationId(const ScopedAnimationId& other)
    : m_AnimationId{other.m_AnimationId}, m_Manager{other.m_Manager}
{
}
ScopedAnimationId& ScopedAnimationId::operator=(const ScopedAnimationId& other)
{
    if (this != &other)
    {
        m_Manager->cancelAnimationById(m_AnimationId);
        m_Manager = other.m_Manager;
        m_AnimationId = other.m_AnimationId;
    }
    return *this;
}
ScopedAnimationId::~ScopedAnimationId() { m_Manager->cancelAnimationById(m_AnimationId); }
ScopedAnimationId::operator AnimationId() { return m_AnimationId; }