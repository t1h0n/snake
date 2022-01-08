#pragma once
#include "CAnimation.hpp"
#include <deque>
#include <memory>

class SequenceAnimation : public CAnimation
{
public:
    template <typename... Args>
    SequenceAnimation(std::unique_ptr<Args>... args)
    {
        fillContainerFromVariadic(std::move(args)...);
    }
    virtual void play_impl(std::chrono::milliseconds ms) override
    {
        auto& top_animation = m_AnimationList.front();
        top_animation->play(ms);
        if (top_animation->isFinished())
        {
            top_animation->invokeFinishedCallback();
            m_AnimationList.pop_front();
            m_Finished = m_AnimationList.empty();
        }
    }

protected:
    template <typename T, typename... Args>
    void fillContainerFromVariadic(std::unique_ptr<T> first, std::unique_ptr<Args>... args)
    {
        m_AnimationList.push_back(std::move(first));
        if constexpr (sizeof...(Args) > 0)
        {
            fillContainerFromVariadic(std::move(Args)...);
        }
    }
    std::deque<std::unique_ptr<IAnimation>> m_AnimationList;
};
