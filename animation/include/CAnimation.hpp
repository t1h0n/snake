#pragma once
#include "IAnimation.hpp"
#include <cassert>

template <typename DurationType = std::chrono::milliseconds>
class CAnimation : public IAnimationImpl<DurationType>
{
public:
    CAnimation() : m_Finished{false} {}
    virtual void invokeFinishedCallback() const override
    {
        if (m_FinishedCallback)
        {
            m_FinishedCallback();
        }
    }
    virtual bool isFinished() const override { return m_Finished; }
    virtual void setFinishedCallback(std::function<void()> callback) override
    {
        assert(callback);
        m_FinishedCallback = std::move(callback);
    }
    virtual void play(DurationType t) override
    {
        assert(t >= IAnimationImpl<DurationType>::ZERO_DURATION);
        if (!m_Finished)
        {
            play_impl(t);
        }
    }

protected:
    virtual void play_impl(DurationType t) = 0;
    std::function<void()> m_FinishedCallback;
    bool m_Finished;
};