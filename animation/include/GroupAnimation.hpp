#pragma once
#include "CAnimation.hpp"
#include <deque>
#include <memory>

template <typename DurationType = std::chrono::milliseconds,
          typename AnimationPointerType = std::unique_ptr<IAnimation<DurationType>>>
class GroupAnimation : public CAnimation<DurationType>
{
public:
    template <typename... Args>
    GroupAnimation(Args&&... args)
    {
        fillContainerFromVariadic(std::forward<Args>(args)...);
    }
    virtual void play_impl(DurationType ms) override
    {
        CAnimation<DurationType>::m_Finished = true;
        for (auto& animation : m_AnimationList)
        {
            animation->play(ms);
            CAnimation<DurationType>::m_Finished = CAnimation<DurationType>::m_Finished && animation->isFinished();
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