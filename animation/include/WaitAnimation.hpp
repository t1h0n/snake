#pragma once
#include "CAnimation.hpp"
namespace animation
{
template <typename DurationType = std::chrono::milliseconds>
class WaitAnimationImpl : public CAnimation<DurationType>
{
public:
    explicit WaitAnimationImpl(DurationType const& duration)
        : m_Duration{duration}, m_CurrentTime{IAnimationImpl<DurationType>::ZERO_DURATION}
    {
        assert(m_Duration > IAnimationImpl<DurationType>::ZERO_DURATION);
    }

protected: // methods
    virtual void play_impl(DurationType const& t) override
    {
        m_CurrentTime += t;
        CAnimation<DurationType>::m_Finished = m_CurrentTime >= m_Duration;
    }
    virtual void reset_impl() override { m_CurrentTime = IAnimationImpl<DurationType>::ZERO_DURATION; }

protected: // members
    DurationType m_Duration;
    DurationType m_CurrentTime;
};

using WaitAnimation = WaitAnimationImpl<>;
} // namespace animation