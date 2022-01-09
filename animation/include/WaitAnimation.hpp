#pragma once
#include "CAnimation.hpp"

template <typename DurationType = std::chrono::milliseconds>
class WaitAnimationImpl : public CAnimation<DurationType>
{
public:
    explicit WaitAnimationImpl(DurationType duration) : m_Duration{duration} {}

    void play_impl(DurationType t)
    {
        m_CurrentTime += t;
        CAnimation<DurationType>::m_Finished = m_CurrentTime >= m_Duration;
    }

protected:
    DurationType m_Duration;
    DurationType m_CurrentTime;
};

using WaitAnimation = WaitAnimationImpl<>;