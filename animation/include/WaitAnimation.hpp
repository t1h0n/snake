#pragma once
#include "CAnimation.hpp"

template <typename DurationType = std::chrono::milliseconds>
class WaitAnimationImpl : public CAnimation<DurationType>
{
public:
    explicit WaitAnimationImpl(DurationType const& duration)
        : m_Duration{duration}, m_CurrentTime{static_cast<typename DurationType::rep>(0)}
    {
        assert(m_Duration > IAnimationImpl<DurationType>::ZERO_DURATION);
    }

    void play_impl(DurationType const& t)
    {
        m_CurrentTime += t;
        CAnimation<DurationType>::m_Finished = m_CurrentTime >= m_Duration;
    }

protected:
    DurationType m_Duration;
    DurationType m_CurrentTime;
};

using WaitAnimation = WaitAnimationImpl<>;