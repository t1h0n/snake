#pragma once
#include "IAnimation.hpp"

class CAnimation : public IAnimation
{
public:
    CAnimation() : m_Finished{false} {}
    void invokeFinishedCallback() const override { m_FinishedCallback(); }
    bool isFinished() const override { return m_Finished; }
    void setFinishedCallback(std::function<void()> callback) override { m_FinishedCallback = std::move(callback); }
    void play(std::chrono::milliseconds ms) override
    {
        if (!m_Finished)
        {
            play_impl(ms);
        }
    }

protected:
    virtual void play_impl(std::chrono::milliseconds ms) = 0;
    std::function<void()> m_FinishedCallback;
    bool m_Finished;
};