#pragma once
#include "CAnimation.hpp"
#include <deque>
#include <memory>

template <typename DurationType = std::chrono::milliseconds,
          typename AnimationPointerType = std::unique_ptr<IAnimation<DurationType>>>
class SequenceAnimationImpl : public CAnimation<DurationType>
{
public:
    template <typename... Args>
    SequenceAnimationImpl(Args&&... args)
    {
        fillContainerFromVariadic(std::forward<Args>(args)...);
    }
    virtual void play_impl(DurationType ms) override
    {
        auto& top_animation = m_AnimationList.front();
        top_animation->play(ms);
        if (top_animation->isFinished())
        {
            top_animation->invokeFinishedCallback();
            m_AnimationList.pop_front();
            CAnimation<DurationType>::m_Finished = m_AnimationList.empty();
        }
    }

protected:
    template <typename T, typename... Args>
    void fillContainerFromVariadic(T&& first, Args&&... args)
    {
        m_AnimationList.push_back(std::move(first));
        if constexpr (sizeof...(Args) > 0)
        {
            fillContainerFromVariadic(std::forward<Args>(args)...);
        }
    }
    std::deque<AnimationPointerType> m_AnimationList;
};

using SequenceAnimation = SequenceAnimationImpl<>;