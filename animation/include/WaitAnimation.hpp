#pragma once
#include "CAnimation.hpp"

template <typename DurationType = std::chrono::milliseconds>
class WaitAnimation : public CAnimation<DurationType>
{
public:
    WaitAnimation(DurationType duration) : m_Duration{duration} {}

    void play_impl(DurationType ms)
    {
        m_CurrentTime += ms;
        CAnimation<DurationType>::m_Finished = m_CurrentTime >= m_Duration;
    }

protected:
    DurationType m_Duration;
    DurationType m_CurrentTime;
};