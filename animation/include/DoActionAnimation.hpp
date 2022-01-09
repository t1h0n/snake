#pragma once
#include "CAnimation.hpp"

template <typename DurationType = std::chrono::milliseconds>
class DoActionImpl : public CAnimation<DurationType>
{
public:
    explicit DoActionImpl(const std::function<void()>& callback) : m_StartedCallback{callback} { assert(callback); }
    explicit DoActionImpl(std::function<void()>&& callback) : m_StartedCallback{std::move(callback)} { assert(m_StartedCallback); }
    void play_impl(DurationType)
    {
        m_StartedCallback();
        CAnimation<DurationType>::m_Finished = true;
    }

protected:
    std::function<void()> m_StartedCallback;
};
using DoActionAnimation = DoActionImpl<>;
