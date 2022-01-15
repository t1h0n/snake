#pragma once
#include "IAnimation.hpp"
#include <cassert>
namespace animation
{
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
    virtual void setFinishedCallback(const std::function<void()>& callback) override
    {
        assert(callback);
        m_FinishedCallback = callback;
    }
    virtual void play(DurationType const& t) override
    {
        assert(t > IAnimationImpl<DurationType>::ZERO_DURATION);
        if (!m_Finished)
        {
            play_impl(t);
        }
    }

    virtual void reset() override
    {
        m_Finished = false;
        reset_impl();
    }

protected: // methods
    virtual void play_impl(DurationType const& t) = 0;
    virtual void reset_impl() = 0;

protected: // members
    std::function<void()> m_FinishedCallback;
    bool m_Finished;
};
} // namespace animation