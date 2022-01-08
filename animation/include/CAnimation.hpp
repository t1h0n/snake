#pragma once
#include "IAnimation.hpp"

template <typename DurationType = std::chrono::milliseconds>
class CAnimation : public IAnimationImpl<DurationType>
{
public:
    CAnimation() : m_Finished{false} {}
    void invokeFinishedCallback() const override
    {
        if (m_FinishedCallback)
        {
            m_FinishedCallback();
        }
    }
    bool isFinished() const override { return m_Finished; }
    void setFinishedCallback(std::function<void()> callback) override
    {
        assert(callback);
        m_FinishedCallback = std::move(callback);
    }
    void play(DurationType t) override
    {
        assert(t.count() > static_cast<typename DurationType::rep>(0));
        if (!m_Finished)
        {
            play_impl(t);
        }
    }

protected:
    virtual void play_impl(std::chrono::milliseconds t) = 0;
    std::function<void()> m_FinishedCallback;
    bool m_Finished;
};