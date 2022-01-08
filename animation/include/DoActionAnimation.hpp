#pragma once
#include "CAnimation.hpp"
#include <cassert>

template <typename DurationType = std::chrono::milliseconds>
class DoAction : public CAnimation<DurationType>
{
public:
    DoAction(const std::function<void()>& callback) : m_StartedCallback{callback} { assert(callback); }
    DoAction(std::function<void()>&& callback) : m_StartedCallback{std::move(callback)} { assert(m_StartedCallback); }
    void play_impl(DurationType)
    {
        m_StartedCallback();
        CAnimation<DurationType>::m_Finished = true;
    }

protected:
    std::function<void()> m_StartedCallback;
};
