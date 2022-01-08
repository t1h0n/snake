#pragma once
#include "CAnimation.hpp"

class WaitAnimation : public CAnimation
{
public:
    WaitAnimation(std::chrono::milliseconds duration) : m_Duration{duration} {}

    void play_impl(std::chrono::milliseconds ms)
    {
        m_CurrentTime += ms;
        m_Finished = m_CurrentTime >= m_Duration;
    }

protected:
    std::chrono::milliseconds m_Duration;
    std::chrono::milliseconds m_CurrentTime;
};