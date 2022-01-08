#pragma once
#include "CAnimation.hpp"

class DoAction : public CAnimation
{
public:
    DoAction(const std::function<void()>& callback) : m_StartedCallback{callback} {}
    DoAction(std::function<void()>&& callback) : m_StartedCallback{std::move(callback)} {}
    void play_impl(std::chrono::milliseconds ms)
    {
        m_StartedCallback();
        m_Finished = true;
    }

protected:
    std::function<void()> m_StartedCallback;
};
